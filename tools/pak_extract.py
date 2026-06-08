"""
pak_extract.py — Đọc/extract file từ .pak dựa trên manifest do pak_lister.py tạo ra

Cách dùng:
    # Extract một file theo tên (cần đã có filename matching trong manifest)
    python pak_extract.py manifest.json --get "ui\\map\\world.spr" --out world.spr

    # Extract theo hash ID (luôn hoạt động, kể cả không có tên file)
    python pak_extract.py manifest.json --hash 0xDEADBEEF --pak vng.pak --out output.bin

    # List tất cả file (tên hoặc hash) trong một pak
    python pak_extract.py manifest.json --list vng.pak

    # List tất cả file (có tên) từ mọi pak
    python pak_extract.py manifest.json --list-named

    # Tính hash của một tên file (để tìm trong pak)
    python pak_extract.py --compute-hash "ui\\map\\world.spr"

Decompress:
    Tool này chỉ extract raw (nếu TYPE_NONE) hoặc decompress UCL (nếu có thư viện ucl).
    Với TYPE_FRAME (.spr), data được extract nguyên block, không tách từng frame.
    Với TYPE_BZIP2, cần Python bzip2 stdlib (tự động).
"""

import struct
import os
import sys
import json
import argparse
import bz2
from pathlib import Path


TYPE_FILTER    = 0xFF000000
TYPE_NONE      = 0x00
TYPE_UCL       = 0x01
TYPE_BZIP2     = 0x02
TYPE_FRAME     = 0x10
TYPE_UCL_NEW   = 0x20


def filename_to_id(path: str) -> int:
    """Compute XPackFile/KPakList hash. Path must start with '\\' and be lowercase."""
    id_val = 0
    index = 0
    for c in path:
        index += 1
        id_val = ((id_val + index * ord(c)) % 0x8000000b * 0xffffffef) & 0xFFFFFFFF
    return (id_val ^ 0x12345678) & 0xFFFFFFFF


def normalize_and_hash(filepath: str) -> int:
    """Normalize a filename and compute its hash (same as game engine)."""
    path = filepath.replace('/', '\\').lower().lstrip('\\')
    path = '\\' + path
    return filename_to_id(path)


def decompress_ucl(compressed: bytes, orig_size: int) -> bytes:
    """Decompress UCL nrv2b. Requires ucl Python binding or falls back to raw bytes."""
    try:
        import ucl
        return ucl.decompress(compressed, orig_size)
    except ImportError:
        pass

    # Fallback: try to find ucl DLL from game directory and use ctypes
    try:
        import ctypes
        ucl_dll = None
        search_paths = [
            os.path.join(os.path.dirname(__file__), '..', 'SwordOnline', 'bin', 'client'),
            os.path.join(os.path.dirname(__file__), '..', 'bin', 'client'),
            os.getcwd(),
        ]
        for sp in search_paths:
            candidate = os.path.join(sp, 'ucl.dll')
            if os.path.exists(candidate):
                ucl_dll = ctypes.CDLL(candidate)
                break

        if ucl_dll:
            out_buf = (ctypes.c_uint8 * orig_size)()
            out_len = ctypes.c_uint(orig_size)
            in_buf = (ctypes.c_uint8 * len(compressed))(*compressed)
            ret = ucl_dll.ucl_nrv2b_decompress_8(in_buf, len(compressed), out_buf, ctypes.byref(out_len), None)
            if ret == 0:
                return bytes(out_buf[:out_len.value])
    except Exception:
        pass

    print("WARNING: UCL decompression not available — returning compressed bytes", file=sys.stderr)
    return compressed


def extract_entry(pak_path: str, entry: dict, decompress: bool = True) -> bytes:
    """Extract raw bytes of a single entry from a pak file."""
    with open(pak_path, 'rb') as f:
        f.seek(entry["offset"])
        comp_size = entry["compressed_size"]
        raw = f.read(comp_size)

    if not decompress:
        return raw

    comp = entry["compression"]
    orig_size = entry["original_size"]

    if comp == "NONE":
        return raw
    elif comp in ("UCL", "UCL_NEW"):
        return decompress_ucl(raw, orig_size)
    elif comp == "BZIP2":
        return bz2.decompress(raw)
    elif comp == "FRAME":
        # FRAME-compressed .spr: the raw block contains the full sprite with per-frame compression.
        # Return the entire block; use a .spr parser to read individual frames.
        return raw
    else:
        print(f"WARNING: Unknown compression '{comp}', returning raw bytes", file=sys.stderr)
        return raw


def load_manifest(manifest_path: str) -> dict:
    with open(manifest_path, 'r', encoding='utf-8') as f:
        return json.load(f)


def find_entry_by_hash(manifest: dict, hash_id: int, pak_name: str | None = None):
    """Find an entry by hash_id. Returns (pak, entry) or (None, None)."""
    for pak in manifest["pak_files"]:
        if pak_name and pak["pak_name"] != pak_name:
            continue
        for entry in pak["entries"]:
            if entry["hash_id"] == hash_id:
                return pak, entry
    return None, None


def find_entry_by_name(manifest: dict, filename: str):
    """Find an entry by filename. Returns (pak, entry) or (None, None)."""
    filename_norm = filename.replace('/', '\\').lower().lstrip('\\')
    for pak in manifest["pak_files"]:
        for entry in pak["entries"]:
            fn = entry.get("filename")
            if fn and fn.replace('/', '\\').lower().lstrip('\\') == filename_norm:
                return pak, entry
    # Try hash fallback
    hash_id = normalize_and_hash(filename)
    return find_entry_by_hash(manifest, hash_id)


def cmd_list(manifest: dict, pak_name: str):
    """List all entries in a pak."""
    for pak in manifest["pak_files"]:
        if pak["pak_name"] != pak_name:
            continue
        print(f"Pak [{pak['pak_index']}]: {pak['pak_name']}  ({pak['entry_count']} entries, {pak['file_size']:,} bytes)")
        print(f"{'#':>5}  {'Hash':>12}  {'Offset':>10}  {'Size':>10}  {'CSize':>10}  {'Comp':<10}  Filename")
        print("-" * 100)
        for e in pak["entries"]:
            fn = e.get("filename") or ""
            print(f"{e['entry_index']:5d}  {e['hash_hex']:>12}  {e['offset']:10d}  {e['original_size']:10d}  {e['compressed_size']:10d}  {e['compression']:<10}  {fn}")
        return
    print(f"ERROR: pak '{pak_name}' not found in manifest", file=sys.stderr)


def cmd_list_named(manifest: dict):
    """List all entries that have a known filename."""
    count = 0
    for pak in manifest["pak_files"]:
        pak_printed = False
        for e in pak["entries"]:
            if e.get("filename"):
                if not pak_printed:
                    print(f"\n=== {pak['pak_name']} ===")
                    pak_printed = True
                print(f"  {e['hash_hex']}  {e['filename']}  ({e['original_size']:,} bytes, {e['compression']})")
                count += 1
    print(f"\nTotal: {count} named entries")


def cmd_get(manifest: dict, filename: str, out_path: str, decompress: bool):
    pak, entry = find_entry_by_name(manifest, filename)
    if not pak:
        print(f"ERROR: '{filename}' not found in any pak", file=sys.stderr)
        sys.exit(1)
    print(f"Found in {pak['pak_name']} (entry #{entry['entry_index']}): {entry['original_size']:,} bytes [{entry['compression']}]")
    data = extract_entry(pak["pak_path"], entry, decompress)
    with open(out_path, 'wb') as f:
        f.write(data)
    print(f"Written {len(data):,} bytes -> {out_path}")


def cmd_get_hash(manifest: dict, hash_hex: str, pak_name: str | None, out_path: str, decompress: bool):
    hash_id = int(hash_hex, 16) if hash_hex.startswith('0x') or hash_hex.startswith('0X') else int(hash_hex)
    pak, entry = find_entry_by_hash(manifest, hash_id, pak_name)
    if not pak:
        print(f"ERROR: hash 0x{hash_id:08X} not found", file=sys.stderr)
        sys.exit(1)
    print(f"Found in {pak['pak_name']} (entry #{entry['entry_index']}): {entry['original_size']:,} bytes [{entry['compression']}]")
    data = extract_entry(pak["pak_path"], entry, decompress)
    with open(out_path, 'wb') as f:
        f.write(data)
    print(f"Written {len(data):,} bytes -> {out_path}")


def main():
    parser = argparse.ArgumentParser(description="Extract files from JX1 .pak using manifest")
    parser.add_argument("manifest", nargs="?", help="Path to pak_manifest.json (from pak_lister.py)")

    parser.add_argument("--list", metavar="PAK_NAME",
                        help="List all entries in the given pak (e.g. vng.pak)")
    parser.add_argument("--list-named", action="store_true",
                        help="List all entries with known filenames across all paks")
    parser.add_argument("--get", metavar="FILENAME",
                        help="Extract a file by name (e.g. 'ui\\\\map\\\\world.spr')")
    parser.add_argument("--hash", metavar="HASH",
                        help="Extract a file by hash ID (e.g. 0xDEADBEEF)")
    parser.add_argument("--pak", metavar="PAK_NAME",
                        help="Restrict search to this pak (used with --hash)")
    parser.add_argument("--out", metavar="OUTPUT_PATH",
                        help="Output file path (required for --get and --hash)")
    parser.add_argument("--raw", action="store_true",
                        help="Don't decompress — write raw (compressed) bytes")
    parser.add_argument("--compute-hash", metavar="FILEPATH",
                        help="Compute and print the hash of a given file path")
    args = parser.parse_args()

    # compute-hash doesn't need manifest
    if args.compute_hash:
        path = args.compute_hash.replace('/', '\\').lower().lstrip('\\')
        path = '\\' + path
        h = filename_to_id(path)
        print(f"Path   : {path}")
        print(f"Hash   : 0x{h:08X}  ({h})")
        return

    if not args.manifest:
        parser.print_help()
        sys.exit(1)

    manifest = load_manifest(args.manifest)

    # Resolve pak paths relative to manifest location if needed
    manifest_dir = os.path.dirname(os.path.abspath(args.manifest))
    for pak in manifest["pak_files"]:
        if not os.path.isabs(pak["pak_path"]):
            pak["pak_path"] = os.path.join(manifest_dir, pak["pak_path"])

    if args.list:
        cmd_list(manifest, args.list)
    elif args.list_named:
        cmd_list_named(manifest)
    elif args.get:
        if not args.out:
            print("ERROR: --out is required with --get", file=sys.stderr)
            sys.exit(1)
        cmd_get(manifest, args.get, args.out, not args.raw)
    elif args.hash:
        if not args.out:
            print("ERROR: --out is required with --hash", file=sys.stderr)
            sys.exit(1)
        cmd_get_hash(manifest, args.hash, args.pak, args.out, not args.raw)
    else:
        parser.print_help()


if __name__ == "__main__":
    main()
