Bạn đang làm việc với module **Bishop** — cluster manager, giám sát và điều phối tất cả servers. Build ra `bin/server/Bishop.exe`.

## Source folders — Tầng 1: Code của Bishop (sửa trực tiếp ở đây)

| Thư mục | Mô tả |
|---------|-------|
| `SwordOnline/Sources/MultiServer/Bishop/` | Root của module — toàn bộ cluster manager logic |
| `SwordOnline/Sources/MultiServer/Common/` | IOCP networking, sockets, threads |

## Source folders — Tầng 2: Dependencies (đọc để hiểu interface/structs)

| Thư mục | Khi nào cần đọc |
|---------|----------------|
| `SwordOnline/Sources/Sword3PaySys/S3AccServer/AccountLoginDef.h` | Khi liên quan đến account auth — data structures dùng chung với Sword3PaySys |
| `SwordOnline/Sources/S3Client/Login/` | Login protocol từ phía client |
| `SwordOnline/Headers/` | Protocol packets, inter-server interfaces |
| `SwordOnline/Sources/MultiServer/Heaven/` | Khi hiểu luồng login qua Heaven |
| `SwordOnline/Sources/MultiServer/Rainbow/` | Khi hiểu luồng chat/social |
| `SwordOnline/Sources/Core/Src/` | Khi cần game constants, player definitions |
| `SwordOnline/Sources/Engine/src/` | Base utilities |

## Libs được link

| Lib | Nằm tại |
|-----|---------|
| `common.lib` | `SwordOnline/Lib/release/common.lib` |
| `engine.lib` | Built từ `Sources/Engine/` |

## Files quan trọng

- `Application.cpp` — **entry point và credential config** — dòng ~78 chứa `g_theSmartClient.Open(IP, port, username, password)` phải set trước khi build
- `SmartClient.cpp` — kết nối tới account server (Sword3PaySys)
- `Intercessor.cpp` — trung gian điều phối giữa các servers
- `GameServer.cpp` (**Bishop's own**) — quản lý danh sách GameServer instances đang chạy (KHÔNG phải GameServer project)
- `GamePlayer.cpp` — quản lý player sessions qua toàn cluster
- `PlayerCreator.cpp` — tạo nhân vật mới qua DB

## Lookup nhanh

| Muốn sửa | File cần xem |
|-----------|-------------|
| Credential kết nối account server | `Application.cpp` dòng ~78 |
| Thêm/sửa server monitoring | `Intercessor.cpp` |
| Quản lý GameServer instances | `GameServer.cpp` (trong Bishop folder) |
| Player session management | `GamePlayer.cpp` |
| Tạo nhân vật | `PlayerCreator.cpp` |
| Account authentication | `SmartClient.cpp` + `AccountLoginDef.h` |
| Login protocol | `S3Client/Login/` headers |

## Build config

Dùng configuration **Server Debug** hoặc **Server Release**.  
Rebuild order: Common → Bishop.

## Lưu ý quan trọng

Bishop có file `GameServer.cpp` riêng để track GameServer instances — ĐỪNG nhầm với `MultiServer/GameServer/GameServer.cpp`.

## Luồng kết nối

```
Bishop ↔ SmartClient ↔ Sword3PaySys (account auth)
Bishop ↔ Intercessor ↔ GameServer instances (monitoring)
Bishop ← Heaven (login notifications)
```
