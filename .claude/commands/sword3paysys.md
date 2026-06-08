---
description: Làm việc với module Sword3PaySys — account server và payment system
---
Bạn đang làm việc với module **Sword3PaySys (S3AccServer)** — account server và payment system. Build ra `bin/server/Sword3PaySys.exe`.

## Source folders — Tầng 1: Code của Sword3PaySys (sửa trực tiếp ở đây)

| Thư mục | Mô tả |
|---------|-------|
| `SwordOnline/Sources/Sword3PaySys/S3AccServer/` | Root của module — account, payment, SQL logic |

## Source folders — Tầng 2: Dependencies (đọc để hiểu interface)

| Thư mục | Khi nào cần đọc |
|---------|----------------|
| `SwordOnline/Sources/MultiServer/Bishop/AccountLoginDef.h` hoặc `S3AccServer/AccountLoginDef.h` | Khi liên quan đến login structs dùng chung với Bishop |
| `SwordOnline/Headers/` | Shared protocol headers |
| `SwordOnline/Sources/Engine/src/` | Base utilities |
| `SwordOnline/Sources/Core/Src/` | Chỉ khi build Debug — một số game constants |

## Libs được link

| Lib | Nằm tại |
|-----|---------|
| `engine.lib` | Built từ `Sources/Engine/` |
| `LuaLibDll.lib` | Lua scripting |
| **msado15.tlh / msado15.tli** | **MS SQL Server ADO** — auto-generated từ COM type library |

## Database engine

Sword3PaySys dùng **Microsoft SQL Server** qua ADO/ODBC — KHÁC HOÀN TOÀN với Goddess (Berkeley DB).
- Connection string cấu hình trong source hoặc config file
- Cần SQL Server instance chạy sẵn

## Files quan trọng

| File | Mô tả |
|------|-------|
| `S3PAccount.cpp` | Account management — create, validate, login |
| `AccountLoginDef.h` | Login data structures, shared với Bishop |
| `S3PDB_MSSQLServer_Connection.cpp` | SQL Server connection management |
| `S3PRole*.cpp` | Role (nhân vật) operations trong SQL |
| `S3PCard*.cpp` | Card/payment operations |
| `S3PTask*.cpp` | Task/mission data trong payment system |
| `S3PEquipment*.cpp` | Equipment/item data liên quan payment |

## Lookup nhanh

| Muốn sửa | File cần xem |
|-----------|-------------|
| Login/account validation | `S3PAccount.cpp` |
| SQL Server connection | `S3PDB_MSSQLServer_Connection.cpp` |
| Account data structures | `AccountLoginDef.h` |
| Role/nhân vật data | `S3PRole*.cpp` files |
| Payment/card operations | `S3PCard*.cpp` files |
| Thêm SQL query mới | `S3PDB_MSSQLServer_Connection.cpp` + relevant `S3P*.cpp` |

## Build config

Dùng configuration **Debug** hoặc **Release** (không phải Server Debug).  
Rebuild order: Engine → Sword3PaySys.

## Lưu ý quan trọng

- Cần có **SQL Server** instance, không phải file-based DB
- `msado15.tlh/.tli` là generated files từ COM type library — không sửa trực tiếp
- `AccountLoginDef.h` được dùng cả trong **Bishop** — thay đổi ở đây ảnh hưởng Bishop
- Đây là điểm xác thực ban đầu: Client → Heaven → Bishop → Sword3PaySys

## Luồng xác thực

```
Client login → Heaven (validate) → Bishop → SmartClient → Sword3PaySys (SQL Server)
                                                               ↓
                                                     Kiểm tra account/password
                                                     Trả kết quả về Bishop
```
