# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is **Jian Xia Qing Yuan (Sword Online / Võ Lâm 1)**, a classic MMORPG migrated to Visual Studio 2022. Written in C++14, 32-bit (Win32) targeting Windows. No automated test suite exists.

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
