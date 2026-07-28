# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is **Jian Xia Qing Yuan (Sword Online / Võ Lâm 1)**, a classic MMORPG migrated to Visual Studio 2022. Written in C++14, 32-bit (Win32) targeting Windows. No automated test suite exists.

## Encoding — MANDATORY

Nearly all source files under `SwordOnline/Sources/` (`.cpp`/`.h`) are encoded in **ISO-8859-1 (Latin-1)**, not UTF-8, and carry Chinese/Vietnamese comments as raw high-byte sequences in that single-byte encoding. Standard text-editing tools (including this assistant's file-editing tools) decode/re-encode files as UTF-8 by default, which **silently corrupts every non-ASCII comment byte** in a file the moment any edit touches it — even a one-line change ends up rewriting the whole file as UTF-8.

**Before editing any file under `SwordOnline/Sources/`:**
1. Check its actual encoding first (e.g. `file <path>` — expect "ISO-8859 text"; if it instead reports "UTF-8", that file is already an already-migrated/pre-existing exception — treat as-is, don't "fix" it unprompted).
2. Never retype or paste back comment text that was displayed mojibake/garbled by a tool — that garbled text is a misdecoded read, not the real bytes, and writing it back corrupts the file permanently.
3. Prefer inserting new code near non-ASCII comment lines using an **ASCII-only anchor** (a neighboring line/token with no high-byte characters) rather than matching on the comment text itself.
4. Write any new comments you add in plain ASCII (English is fine) — never copy adjacent-language comments verbatim into a new edit.
5. After editing, verify with `file <path>` that it still reports ISO-8859 (not UTF-8/Unicode), and check `git diff --stat` — a correct surgical edit shows only the lines you intended to change; a diff spanning hundreds of unrelated lines is a sign the whole file got re-encoded and must be redone.
6. If a tool's normal edit path can't preserve the encoding, do the edit at the byte level instead (e.g. read the original blob via `git show HEAD:<path>`, splice in pure-ASCII new lines with `head`/`tail`/`printf`, matching the file's existing line-ending convention — check for CRLF vs LF first, since files are inconsistent) rather than risk a full re-encode.

## Building

Open `SwordOnline\Sources\JXAll.sln` in Visual Studio 2022 and build via the IDE. There is no command-line build script.

**Build configurations** (select from the VS toolbar):
- `Server Debug` / `Server Release` — for all server-side projects
- `Client Debug` / `Client Release` — for the game client (Core, S3Client, Represent2/3)
- `Debug` / `Release` — for support libraries (Common, LuaLib, JpgLib, etc.)

**Required build order** (project dependencies are not fully declared in the solution):
1. Engine
2. Core (Server Debug or Client Debug depending on target)
3. LuaLibDll, JpgLib, KMp3Lib, FilterText / FilterText_StaticLib, ExpandPackage / ExpandPackageStaticLib
4. Common
5. Represent2, Represent3, Game (S3Client)
6. GameServer, Goddess, Heaven, Rainbow, Bishop, S3Relay
7. Sword3PaySys, S3RelayServer

Built binaries go to `SwordOnline/bin/` (server) and the client output directory.

## Initial Setup

Before first build, set Bishop credentials in `SwordOnline/Sources/MultiServer/Bishop/Application.cpp` around line 78 — the `g_theSmartClient.Open(...)` call with the account server IP, port, username, and password.

Server connection endpoints are configured in `SwordOnline/Sources/MultiServer/GameServer/ServerCfg.ini`.

## Architecture

### Shared Foundation

- **`SwordOnline/Headers/`** — protocol definitions shared between client and server:
  - `KProtocol.h` — main game protocol packets
  - `KRelayProtocol.h`, `KTongProtocol.h`, `KGmProtocol.h` — subsystem protocols
  - `HeavenInterface.h`, `RainbowInterface.h`, `S3DBInterface.h` — inter-server interfaces

- **Engine** (`Sources/Engine/`) — base layer: Win32 windowing, file I/O, rendering, audio input. Provides `KWin32Wnd`, `KFile`, and related utilities used by both client and server.

- **Core** (`Sources/Core/Src/`) — all game logic: players (`KPlayer`), NPCs (`KNpc`, `KNpcAI`), skills (`KSkills`, `KSkillManager`), items (`KItem`, `KInventory`, `KItemList`), quests (`KPlayerTask`), magic attributes (`KMagicAttrib`), regions (`KRegion`, `KSubWorld`). Core builds differently for client vs server via preprocessor defines. Lua quest/AI scripting hooks live in `LuaFuns.cpp`.

### Client Side

- **S3Client** (`Sources/S3Client/`) — game client. Main entry `S3Client.cpp`. UI lives in `S3Client/Ui/`:
  - `Ui/*.cpp/.h` — individual UI screens (each paired with a `.ini` layout file): `UiLogin`, `UiSelServer`, `UiSelPlayer`, `UiChatRoom`, `UiPlayerControlBar`, etc.
  - `Ui/Elem/` — reusable UI elements: `WndButton`, `WndEdit`, `WndList`, `WndScrollBar`, etc.
  - `Ui/UiCase/` — complex multi-panel UI cases
  - `Ui/Ui3/` — additional UI screens

- **Represent2 / Represent3** (`Sources/Represent/`) — 2D/3D rendering of characters, maps, and effects. Used only by the client.

### Server Side (Multi-Server)

```
Client ↔ S3Relay ↔ GameServer ↔ Goddess (DB)
                        ↕
                    Bishop (manager)
Heaven (login) ──────────┘
Rainbow (chat) ───────────┘
```

- **Bishop** (`MultiServer/Bishop/`) — cluster manager, monitors all servers, handles load balancing. `Application.cpp` holds startup/credential config.
- **S3Relay** (`MultiServer/S3Relay/`) — proxy between clients and GameServer; handles connection multiplexing.
- **GameServer** (`MultiServer/GameServer/`) — main game server (KSOServer). Config in `settings/` and `ServerCfg.ini`.
- **Goddess** (`MultiServer/Goddess/`) — database server, persists player/item/progress data via `IDBRoleServer`.
- **Heaven** (`MultiServer/Heaven/`) — login/auth server, validates accounts.
- **Rainbow** (`MultiServer/S3Relay/`) — chat and guild social server; handles channels, friends, Tong (guild) data.
- **Common** (`MultiServer/Common/`) — shared networking library: sockets, IOCP, threads, buffers, CRC, encoding.

### Support Libraries

- **LuaLibDll** (`Library/LuaLib/`) — Lua 5.x scripting engine (DLL). Required by Core and GameServer for quest/AI scripts.
- **JpgLib** — JPEG texture loading for client UI.
- **KMp3Lib** — MP3 playback for BGM/sound effects.
- **FilterText / FilterText_StaticLib** — profanity/spam filter for chat and character names.
- **ExpandPackage / ExpandPackageStaticLib** — game asset packaging (compression/encryption) format.

### Payment System

- **Sword3PaySys / S3AccServer** (`Sword3PaySys/S3AccServer/`) — account & VIP item purchase server.
- **S3RelayServer** (`Sword3PaySys/S3RELAYSERVER/`) — payment relay.

## Key Conventions

- Protocol changes must be coordinated across `SwordOnline/Headers/KProtocol.h` and the corresponding handler in `Core/Src/KProtocolProcess.cpp`.
- UI screen layout is defined in `.ini` files alongside the `.cpp`; both must be updated together when modifying a UI panel.
- Core uses `K` prefix for game-logic classes (`KPlayer`, `KNpc`, `KItem`), `Ui` prefix for client UI screens, and `Wnd` prefix for reusable UI element widgets.
- Server-side logging goes to the `logs/` subdirectory inside each server's build output folder.
