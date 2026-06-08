Bạn đang làm việc với module **GameServer** — main game server, build ra `bin/server/GameServer.exe`.

## Source folders — Tầng 1: Code của GameServer (sửa trực tiếp ở đây)

| Thư mục | Mô tả |
|---------|-------|
| `SwordOnline/Sources/MultiServer/GameServer/` | Root của module — `GameServer.cpp`, `KSOServer.cpp`, `KTransferUnit.cpp` |
| `SwordOnline/Sources/MultiServer/Common/` | IOCP networking, sockets, threads, buffers — dùng chung toàn server |

## Source folders — Tầng 2: Dependencies (đọc để hiểu, không sửa trực tiếp)

| Thư mục | Khi nào cần đọc |
|---------|----------------|
| `SwordOnline/Sources/Core/Src/` | **Thường xuyên** — toàn bộ game logic: `KPlayer`, `KNpc`, `KSkills`, `KItem`, `KRegion`, `KSubWorld`, `KPlayerTask`, `KProtocolProcess` — build thành `CoreServer.lib` |
| `SwordOnline/Headers/` | Protocol packets: `KProtocol.h`, `KRelayProtocol.h`, inter-server interfaces `HeavenInterface.h`, `RainbowInterface.h`, `S3DBInterface.h` |
| `SwordOnline/Sources/Engine/src/` | Base utilities: `KFile`, logging, Win32 helpers |
| `SwordOnline/Sources/MultiServer/Heaven/` | Khi liên quan đến login/auth flow |
| `SwordOnline/Sources/MultiServer/Rainbow/` | Khi liên quan đến chat channels |

## Libs được link

| Lib | Nằm tại |
|-----|---------|
| `CoreServer.lib` | `SwordOnline/Lib/release/CoreServer.lib` |
| `common.lib` | `SwordOnline/Lib/release/common.lib` |
| `engine.lib` | Built từ `Sources/Engine/` |

## Files quan trọng

- `GameServer.cpp` — entry point, khởi tạo server
- `KSOServer.cpp` — main server class, quản lý connections và game loop
- `KTransferUnit.cpp` — xử lý data transfer giữa GameServer ↔ S3Relay
- `ServerCfg.ini` — cấu hình IP/port kết nối tới Goddess, Heaven, Bishop, S3Relay
- `settings/` — cấu hình runtime server (world settings, rates, ...)
- `maps/` — dữ liệu bản đồ
- `script/` — Lua scripts cho quest, NPC AI, events

## Lookup nhanh

| Muốn sửa | File cần xem |
|-----------|-------------|
| Xử lý packet từ client | `Core/Src/KProtocolProcess.cpp` (dispatch table) |
| Logic nhân vật | `Core/Src/KPlayer.cpp`, `KPlayerPK.cpp` |
| NPC behavior | `Core/Src/KNpc.cpp`, `KNpcAI.cpp` |
| Skill system | `Core/Src/KSkills.cpp`, `KSkillManager.cpp` |
| Item system | `Core/Src/KItem.cpp`, `KInventory.cpp` |
| Quest system | `Core/Src/KPlayerTask.cpp`, `LuaFuns.cpp` |
| World/region | `Core/Src/KRegion.cpp`, `KSubWorld.cpp` |
| Server → DB interface | `SwordOnline/Headers/S3DBInterface.h` |
| Server config | `GameServer/ServerCfg.ini` |
| Kết nối với S3Relay | `KTransferUnit.cpp` |

## Build config

Dùng configuration **Server Debug** hoặc **Server Release**.  
Rebuild order nếu thay đổi Core: Core (Server Debug) → Common → GameServer.

## Luồng kết nối

```
S3Relay → GameServer (KSOServer) → Goddess (qua S3DBInterface)
                                 → Heaven (xác thực)
                                 → Bishop (báo cáo trạng thái)
```
