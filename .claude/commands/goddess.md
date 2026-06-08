---
description: Làm việc với module Goddess — database server, lưu trữ nhân vật/item/tiến độ
---
Bạn đang làm việc với module **Goddess** — database server, lưu trữ toàn bộ dữ liệu nhân vật/item/tiến độ. Build ra `bin/server/Goddess.exe`.

## Source folders — Tầng 1: Code của Goddess (sửa trực tiếp ở đây)

| Thư mục | Mô tả |
|---------|-------|
| `SwordOnline/Sources/MultiServer/Goddess/` | Root của module — toàn bộ DB logic |
| `SwordOnline/Sources/MultiServer/Common/` | IOCP networking, sockets, threads |

## Source folders — Tầng 2: Dependencies (đọc để hiểu contract, không sửa trực tiếp)

| Thư mục | Khi nào cần đọc |
|---------|----------------|
| `SwordOnline/Headers/S3DBInterface.h` | **Luôn luôn** — đây là contract giữa GameServer và Goddess. Mọi function Goddess implement đều được khai báo ở đây |
| `SwordOnline/Sources/Engine/src/` | Khi cần base utilities: `KFile`, buffer helpers |
| `SwordOnline/Sources/Core/Src/` | Khi cần hiểu data structures của `KPlayer`, `KItem` để biết cần lưu trường gì vào DB |

## Libs được link

| Lib | Nằm tại |
|-----|---------|
| `common.lib` | `SwordOnline/Lib/release/common.lib` |
| `engine.lib` | Built từ `Sources/Engine/` |
| `libdb62d.lib` (debug) / `libdb62.lib` (release) | Berkeley DB — embedded database engine |

## Files quan trọng

- `Goddess.cpp` — entry point, khởi tạo DB server
- `ClientNode.cpp` — quản lý connections từ GameServer
- `IDBRoleServer.cpp` — implementation của `S3DBInterface.h`, tất cả DB operations
- `DBTable.cpp` — định nghĩa và truy cập các DB tables (role, item, skill, task, ...)
- `DBBackup.cpp` — tự động backup database
- `DBDumpLoad.cpp` — export/import dữ liệu
- `FilterTextLib.cpp` — tích hợp filter tên nhân vật
- `RoleNameFilter.cpp` — kiểm tra tên nhân vật hợp lệ

## Database engine

Goddess dùng **Berkeley DB** (BDB) — embedded key/value database, KHÔNG phải SQL Server.
- BDB files thường ở thư mục `db/` hoặc `data/` trong thư mục output
- Mỗi "table" là một BDB file riêng

## Lookup nhanh

| Muốn sửa | File cần xem |
|-----------|-------------|
| Thêm DB operation mới | `IDBRoleServer.cpp` + thêm declaration vào `S3DBInterface.h` |
| Schema bảng dữ liệu | `DBTable.cpp` |
| Lưu/load nhân vật | `IDBRoleServer.cpp` — functions `SaveRole`, `LoadRole` |
| Lưu/load item | `IDBRoleServer.cpp` — functions item-related |
| Backup DB | `DBBackup.cpp` |
| Connection từ GameServer | `ClientNode.cpp` |
| Interface với GameServer | `SwordOnline/Headers/S3DBInterface.h` |

## Build config

Dùng configuration **Server Debug** hoặc **Server Release**.  
Rebuild order: Common → Goddess.

## Lưu ý

Berkeley DB là embedded — không cần cài DB server riêng. Dữ liệu lưu trực tiếp trong files trên disk.
