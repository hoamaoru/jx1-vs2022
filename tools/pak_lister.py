"""
pak_lister.py — Đọc và liệt kê tất cả file trong .pak của Sword Online (JX1)

Cách dùng:
    python pak_lister.py [--package-ini PATH] [--data-dir DIR] [--scan-src DIR] [--output FILE]

Ví dụ:
    python pak_lister.py
        -> Tự tìm package.ini và data/ trong thư mục hiện tại

    python pak_lister.py --package-ini bin/client/debug/package.ini --output manifest.json

    python pak_lister.py --scan-src SwordOnline/Sources --output manifest.json
        -> Scan source code để build filename dictionary, match hash → tên file

Output (JSON):
    {
      "pak_files": [
        {
          "pak_index": 0,
          "pak_name": "vng.pak",
          "pak_path": "..\\\\data\\\\vng.pak",
          "entry_count": 1234,
          "entries": [
            {
              "entry_index": 0,
              "hash_id": 3735928559,
              "hash_hex": "0xDEADBEEF",
              "offset": 32,
              "original_size": 2048,
              "compressed_size": 1024,
              "compression": "UCL",
              "filename": "ui\\\\map\\\\world.spr"  // chỉ có nếu match được trong dictionary
            }
          ]
        }
      ],
      "stats": { "total_paks": 32, "total_entries": 50000, "matched_filenames": 12000 }
    }

Format binary của .pak (PACK format dùng bởi XPackFile):
    Header (32 bytes):
        [0..3]   signature  = 'PACK' (0x4b434150)
        [4..7]   count      = số lượng file trong pak
        [8..11]  index_off  = offset tới bảng index
        [12..15] data_off   = offset tới vùng data
        [16..19] crc32      = checksum
        [20..31] reserved

    Index entry (16 bytes * count, tại index_off):
        [0..3]   hash_id    = g_FileName2Id(filename)  <- hash, không phải tên
        [4..7]   offset     = offset của data file trong pak
        [8..11]  orig_size  = kích thước gốc (sau decompress)
        [12..15] comp_flag  = byte cao nhất: compression type, 3 byte thấp: compressed size
            0x00xxxxxx = TYPE_NONE  (không nén, compressed_size = orig_size)
            0x01xxxxxx = TYPE_UCL
            0x02xxxxxx = TYPE_BZIP2
            0x10xxxxxx = TYPE_FRAME (chỉ .spr)
            0x20xxxxxx = TYPE_UPL_NEW (UCL variant)

Hash algorithm (g_FileName2Id / KPakList::FileNameToId):
    - Path normalize: prepend '\\', convert uppercase to lowercase
    - id = 0, index = 0
    - for each char c: id = ((id + (++index) * ord(c)) % 0x8000000b * 0xffffffef) & 0xFFFFFFFF
    - return (id ^ 0x12345678) & 0xFFFFFFFF
"""

import struct
import os
import re
import json
import argparse
import sys
from pathlib import Path


PACK_SIGNATURE = b'PACK'
PACK_HEADER_SIZE = 32
INDEX_ENTRY_SIZE = 16

COMPRESSION_TYPES = {
    0x00: "NONE",
    0x01: "UCL",
    0x02: "BZIP2",
    0x10: "FRAME",
    0x20: "UCL_NEW",
}

TYPE_FILTER = 0xFF000000


def filename_to_id(path: str) -> int:
    """Compute XPackFile/KPakList hash for a file path.
    Mirrors FileNameToId() in KPakList.cpp.
    Path must start with '\\' and be lowercased.
    """
    id_val = 0
    index = 0
    for c in path:
        index += 1
        char_val = ord(c)
        id_val = ((id_val + index * char_val) % 0x8000000b * 0xffffffef) & 0xFFFFFFFF
    return (id_val ^ 0x12345678) & 0xFFFFFFFF


def normalize_path_for_hash(filepath: str) -> str:
    """Normalize a file path the same way KPakList::FindElemFile does:
    - Convert to backslash separator
    - Lowercase
    - Ensure starts with single backslash
    """
    path = filepath.replace('/', '\\').lower()
    path = path.lstrip('\\')
    return '\\' + path


def read_pack_header(f) -> dict | None:
    """Read and validate PACK header. Returns None if not a valid pak."""
    data = f.read(PACK_HEADER_SIZE)
    if len(data) < PACK_HEADER_SIZE:
        return None
    sig, count, index_off, data_off, crc32 = struct.unpack_from('<4sIIII', data, 0)
    if sig != PACK_SIGNATURE:
        return None
    if count == 0 or index_off < PACK_HEADER_SIZE or data_off < PACK_HEADER_SIZE:
        return None
    return {
        "count": count,
        "index_offset": index_off,
        "data_offset": data_off,
        "crc32": crc32,
    }


def read_index_table(f, header: dict) -> list[dict]:
    """Read the index table from an open pak file."""
    count = header["count"]
    f.seek(header["index_offset"])
    raw = f.read(count * INDEX_ENTRY_SIZE)
    if len(raw) < count * INDEX_ENTRY_SIZE:
        raise ValueError(f"Short read on index table: expected {count * INDEX_ENTRY_SIZE}, got {len(raw)}")

    entries = []
    for i in range(count):
        off = i * INDEX_ENTRY_SIZE
        hash_id, data_offset, orig_size, comp_flag = struct.unpack_from('<IIII', raw, off)
        comp_type_byte = (comp_flag & TYPE_FILTER) >> 24
        compressed_size = comp_flag & (~TYPE_FILTER & 0xFFFFFFFF)
        if comp_type_byte == 0:
            compressed_size = orig_size

        entries.append({
            "entry_index": i,
            "hash_id": hash_id,
            "hash_hex": f"0x{hash_id:08X}",
            "offset": data_offset,
            "original_size": orig_size,
            "compressed_size": compressed_size,
            "compression": COMPRESSION_TYPES.get(comp_type_byte, f"UNKNOWN_0x{comp_type_byte:02X}"),
            "filename": None,
        })
    return entries


def read_pak_file(pak_path: str, pak_index: int, pak_name: str) -> dict | None:
    """Parse a single .pak file. Returns dict with metadata + entries."""
    try:
        with open(pak_path, 'rb') as f:
            file_size = os.path.getsize(pak_path)
            header = read_pack_header(f)
            if header is None:
                print(f"  [SKIP] {pak_name}: invalid PACK header", file=sys.stderr)
                return None
            entries = read_index_table(f, header)

        return {
            "pak_index": pak_index,
            "pak_name": pak_name,
            "pak_path": pak_path,
            "file_size": file_size,
            "entry_count": len(entries),
            "entries": entries,
        }
    except Exception as e:
        print(f"  [ERROR] {pak_name}: {e}", file=sys.stderr)
        return None


def read_package_ini(ini_path: str) -> tuple[str, list[tuple[int, str]]]:
    """Parse package.ini. Returns (data_path, [(index, pakname), ...]) sorted by index."""
    pak_list = []
    data_path = ""

    with open(ini_path, 'r', encoding='utf-8', errors='ignore') as f:
        in_section = False
        for line in f:
            line = line.strip()
            if line.startswith(';') or not line:
                continue
            if line.lower() == '[package]':
                in_section = True
                continue
            if line.startswith('['):
                in_section = False
                continue
            if not in_section:
                continue

            if '=' not in line:
                continue
            key, _, val = line.partition('=')
            key = key.strip()
            val = val.strip()
            if not val:
                continue

            if key.lower() == 'path':
                data_path = val
            else:
                try:
                    idx = int(key)
                    pak_list.append((idx, val))
                except ValueError:
                    pass

    pak_list.sort(key=lambda x: x[0])
    return data_path, pak_list


def extract_file_paths_from_source(src_dir: str) -> list[str]:
    """Scan C++ source files for string literals that look like game file paths.
    Returns list of candidate filenames (not yet normalized).
    """
    extensions = {'.spr', '.ini', '.txt', '.bin', '.jpg', '.png', '.wav', '.mp3',
                  '.lua', '.scp', '.ani', '.set', '.map', '.rol', '.tab', '.npc',
                  '.cfg', '.dat', '.tga', '.bmp'}

    pattern = re.compile(r'"([^"]{3,200})"')
    candidates = set()

    for root, dirs, files in os.walk(src_dir):
        # Skip build output dirs
        dirs[:] = [d for d in dirs if d not in ('Debug', 'Release', '.git', 'ipch')]
        for fname in files:
            if not fname.endswith(('.cpp', '.h', '.c')):
                continue
            fpath = os.path.join(root, fname)
            try:
                with open(fpath, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                for m in pattern.finditer(content):
                    s = m.group(1)
                    # Must contain a slash or backslash, and end with known extension
                    if ('/' in s or '\\' in s) and any(s.lower().endswith(ext) for ext in extensions):
                        candidates.add(s)
                    # Also pick up bare filenames with known extensions (no path separator)
                    elif any(s.lower().endswith(ext) for ext in extensions) and ' ' not in s:
                        candidates.add(s)
            except Exception:
                pass

    return list(candidates)


def build_filename_dictionary(candidates: list[str]) -> dict[int, str]:
    """Build a hash→filename lookup table from candidate filename strings."""
    dictionary = {}
    for raw in candidates:
        norm = normalize_path_for_hash(raw)
        h = filename_to_id(norm)
        # Keep the most "path-like" one (with slashes) when there's a collision
        if h not in dictionary or '\\' in norm[1:]:
            dictionary[h] = norm.lstrip('\\')
    return dictionary


def load_runtime_log(log_path: str) -> list[str]:
    """Read pak_filenames.log produced by the instrumented game.
    Each line is one filename as passed to KPakList::FindElemFile().
    Returns deduplicated list preserving first-seen order.
    """
    seen = set()
    result = []
    try:
        with open(log_path, 'r', encoding='utf-8', errors='ignore') as f:
            for line in f:
                line = line.strip()
                if line and line not in seen:
                    seen.add(line)
                    result.append(line)
    except FileNotFoundError:
        print(f"ERROR: Log file not found: {log_path}", file=sys.stderr)
        sys.exit(1)
    return result


def apply_dictionary(pak_data_list: list[dict], dictionary: dict[int, str]) -> int:
    """Match hash IDs to known filenames. Returns number of matched entries."""
    matched = 0
    for pak in pak_data_list:
        for entry in pak["entries"]:
            if entry["hash_id"] in dictionary:
                entry["filename"] = dictionary[entry["hash_id"]]
                matched += 1
    return matched


def find_package_ini_and_data() -> tuple[str, str] | tuple[None, None]:
    """Try to auto-detect package.ini and data directory from common locations."""
    candidates = [
        ("bin/client/debug/package.ini", "SwordOnline/bin/client/data"),
        ("bin/client/release/package.ini", "SwordOnline/bin/client/data"),
        ("SwordOnline/bin/client/debug/package.ini", "SwordOnline/bin/client/data"),
    ]
    # Also walk up looking for bin/client/debug/package.ini
    cwd = Path.cwd()
    for _ in range(4):
        for rel_ini, rel_data in candidates:
            ini = cwd / rel_ini
            data = cwd / rel_data
            if ini.exists():
                return str(ini), str(data)
        cwd = cwd.parent

    return None, None


def main():
    parser = argparse.ArgumentParser(
        description="Liệt kê tất cả file trong .pak của Sword Online (JX1)")
    parser.add_argument("--package-ini", help="Đường dẫn tới package.ini")
    parser.add_argument("--data-dir",
                        help="Thư mục chứa .pak files (nếu không chỉ định, lấy từ package.ini)")
    parser.add_argument("--scan-src",
                        help="Thư mục source C++ để scan tìm tên file (build filename dictionary)")
    parser.add_argument("--filename-log",
                        help="File pak_filenames.log từ game instrumented (tốt nhất để match tên)")
    parser.add_argument("--output", default="pak_manifest.json",
                        help="File JSON output (default: pak_manifest.json)")
    parser.add_argument("--no-entries", action="store_true",
                        help="Chỉ in header info, không in từng entry (nhỏ hơn)")
    args = parser.parse_args()

    # --- Locate package.ini ---
    ini_path = args.package_ini
    if not ini_path:
        ini_path, auto_data = find_package_ini_and_data()
        if not ini_path:
            print("ERROR: Không tìm thấy package.ini. Dùng --package-ini để chỉ định.", file=sys.stderr)
            sys.exit(1)
        print(f"Auto-detected package.ini: {ini_path}")
    else:
        auto_data = None

    # --- Parse package.ini ---
    data_rel_path, pak_list = read_package_ini(ini_path)
    ini_dir = os.path.dirname(os.path.abspath(ini_path))

    # --- Resolve data directory ---
    data_dir = args.data_dir
    if not data_dir:
        # data_rel_path from package.ini is relative to the location of the game exe
        # package.ini usually lives in bin/client/debug/, data is in bin/client/data/
        guesses = [
            os.path.normpath(os.path.join(ini_dir, data_rel_path)),
            os.path.normpath(os.path.join(ini_dir, '..', 'data')),
            auto_data or "",
        ]
        data_dir = next((g for g in guesses if g and os.path.isdir(g)), None)
        if not data_dir:
            print(f"ERROR: Không tìm thấy data dir. Thử: {guesses}", file=sys.stderr)
            sys.exit(1)
    print(f"Data dir: {data_dir}")

    # --- Build filename dictionary (optional) ---
    dictionary: dict[int, str] = {}
    if args.filename_log:
        print(f"Loading runtime log: {args.filename_log} ...")
        log_names = load_runtime_log(args.filename_log)
        print(f"  {len(log_names)} unique filenames from game runtime")
        dictionary.update(build_filename_dictionary(log_names))
        print(f"  Dictionary size: {len(dictionary)}")
    if args.scan_src:
        print(f"Scanning source code: {args.scan_src} ...")
        candidates = extract_file_paths_from_source(args.scan_src)
        print(f"  Found {len(candidates)} candidate strings")
        dictionary.update(build_filename_dictionary(candidates))
        print(f"  Dictionary size after source scan: {len(dictionary)}")

    # --- Read each pak ---
    print(f"\nReading {len(pak_list)} pak files...")
    pak_data_list = []
    for pak_idx, pak_name in pak_list:
        pak_path = os.path.join(data_dir, pak_name)
        print(f"  [{pak_idx:2d}] {pak_name}", end="", flush=True)
        if not os.path.exists(pak_path):
            print(f" -> NOT FOUND")
            continue
        pak = read_pak_file(pak_path, pak_idx, pak_name)
        if pak:
            print(f" -> {pak['entry_count']} entries, {pak['file_size']:,} bytes")
            if args.no_entries:
                pak["entries"] = []
            pak_data_list.append(pak)

    # --- Apply dictionary ---
    matched = 0
    if dictionary:
        matched = apply_dictionary(pak_data_list, dictionary)
        print(f"\nFilename matching: {matched} entries matched out of dictionary of {len(dictionary)}")

    # --- Stats ---
    total_entries = sum(p["entry_count"] for p in pak_data_list)
    stats = {
        "total_paks": len(pak_data_list),
        "total_entries": total_entries,
        "matched_filenames": matched,
        "package_ini": os.path.abspath(ini_path),
        "data_dir": os.path.abspath(data_dir),
    }

    # --- Output JSON ---
    output = {
        "format_notes": {
            "hash_algorithm": "((id + index * ord(c)) % 0x8000000b * 0xffffffef) & 0xFFFFFFFF, then XOR 0x12345678",
            "path_normalization": "prepend '\\\\', lowercase, backslash separators",
            "compression_types": COMPRESSION_TYPES,
            "index_entry_layout": "uint32 hash_id, uint32 offset, int32 orig_size, int32 comp_size_flag (high byte=compression, low 3 bytes=compressed_size)",
        },
        "pak_files": pak_data_list,
        "stats": stats,
    }

    with open(args.output, 'w', encoding='utf-8') as f:
        json.dump(output, f, indent=2, ensure_ascii=False)

    print(f"\nOutput: {args.output}")
    print(f"Total: {stats['total_paks']} paks, {stats['total_entries']:,} entries")
    if matched:
        pct = matched / total_entries * 100 if total_entries else 0
        print(f"Matched: {matched:,} filenames ({pct:.1f}%)")


if __name__ == "__main__":
    main()
