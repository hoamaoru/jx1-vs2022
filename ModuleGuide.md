# Module Guide — Tài liệu tham chiếu cho 6 module chính

> Mục đích: Khi cần chỉnh sửa một module cụ thể, tài liệu này cho bạn biết ngay **thư mục source nào cần đọc**, **lib/header nào được link**, và **file config nào ảnh hưởng** — không cần scan toàn bộ project.

---

## Sơ đồ phụ thuộc nhanh

```
Sword3PaySys (login/account)
    ↑ xác thực
Bishop (manager) ──── S3Relay (proxy + chat)
    ↑ điều phối          ↑
    └──────── GameServer ┘
                 ↑
              Goddess (database)

S3Client ──→ S3Relay ──→ GameServer
```

---

## 1. S3Client → `Game.exe` (Client game)

### Thông tin build
| Mục | Giá trị |
|---|---|
| Project | `Sources/S3Client/S3Client.vcxproj` |
| Output Debug | `bin/client/Game.exe` |
| Build config | `Client Debug` / `Client Release` |
| Precompiled header | `KWin32.h` |

### Thư mục source chính
```
Sources/S3Client/
├── S3Client.cpp / S3Client.h    ← Entry point, game loop chính
├── S3Config.h                   ← Cấu hình compile-time
├── ErrorCode.cpp / .h           ← Mã lỗi
│
├── Login/
│   ├── Login.cpp / Login.h      ← Logic đăng nhập tài khoản
│   └── LoginDef.h               ← Struct gói tin login
│
├── NetConnect/
│   └── NetConnectAgent.cpp / .h ← Quản lý kết nối TCP tới server
│
├── Ui/                          ← Toàn bộ giao diện
│   ├── UiShell.h                ← Entry point UI (UiInit, UiPaint, UiHeartBeat)
│   ├── UiCase/                  ← Từng màn hình game (UiLogin, UiGame, UiItem...)
│   ├── Elem/                    ← Widget tái sử dụng (WndButton, WndEdit, WndList...)
│   └── Ui3/                     ← Màn hình bổ sung
│
├── TextCtrlCmd/                 ← Xử lý lệnh text (GM command từ client)
│
└── settings/                   ← File config chạy cùng Game.exe
    ├── ServerList.ini           ← Danh sách server hiển thị ở UiSelServer
    ├── GameData.ini             ← Thông số game cơ bản
    ├── GameSetting.ini          ← Cài đặt đồ họa/âm thanh
    ├── MapList.ini              ← Danh sách bản đồ
    ├── npc/                     ← Data NPC cho client render
    ├── item/                    ← Data vật phẩm cho client
    ├── Music/                   ← Danh sách nhạc nền
    └── faction/                 ← Data môn phái
```

### Include paths (từ vcxproj)
| Path | Lấy gì |
|---|---|
| `Sources/Engine/src` | `KWin32.h`, `KEngine.h`, `KFile.h`, DirectX wrappers |
| `Sources/Engine/include` | Header bổ sung của Engine |
| `Sources/Core/src` | `CoreShell.h`, `KPlayer.h`, `KNpc.h`, toàn bộ game logic |
| `SwordOnline/Headers` | `KProtocol.h`, `KProtocolDef.h`, `IClient.h` |
| `Sources/KMp3LibClass/src` | `KMp3Music.h` để phát nhạc |

### Static libs link (tìm trong `SwordOnline/Lib/`)
```
lualibdll.lib    ← Lua scripting engine
Winmm.lib        ← Windows multimedia (âm thanh)
shlwapi.lib      ← Shell API
odbc32.lib       ← ODBC (hệ thống)
```

### DLL cần có cạnh Game.exe lúc chạy
```
CoreClient.dll   ← Đã có sẵn trong Sources/S3Client/ (Core build Client Debug)
Represent2.dll   ← Render 2D (build từ Sources/Represent/Represent2/)
Represent3.dll   ← Render 3D (build từ Sources/Represent/Represent3/)
lualibdll.dll    ← Đã có trong SwordOnline/Lib/
```

### Khi cần sửa S3Client, tham chiếu ở đây

| Muốn sửa | Xem file/thư mục |
|---|---|
| Màn hình UI mới hoặc chỉnh layout | `Ui/UiCase/Ui*.cpp` + file `.ini` cùng tên |
| Widget button, textbox, list | `Ui/Elem/Wnd*.cpp` |
| Logic đăng nhập | `Login/Login.cpp`, `Login/LoginDef.h` |
| Kết nối tới server | `NetConnect/NetConnectAgent.cpp` |
| Nhận/xử lý gói tin từ server | `Sources/Core/Src/KProtocolProcess.cpp` (client build) |
| Định nghĩa gói tin | `SwordOnline/Headers/KProtocol.h` |
| Dữ liệu NPC/item hiển thị | `settings/npc/`, `settings/item/` |
| Danh sách server | `settings/ServerList.ini` |
| Logic game (player, skill, item) | `Sources/Core/Src/` (đọc header, sửa phải rebuild Core) |

---

## 2. Sword3PaySys → `Sword3PaySys.exe` (Account / Login server)

### Thông tin build
| Mục | Giá trị |
|---|---|
| Project | `Sources/Sword3PaySys/S3AccServer/Sword3PaySys.vcxproj` |
| Output Debug | `bin/server/Sword3PaySys.exe` |
| Build config | `Debug` / `Release` |
| Database | **MS SQL Server** (qua ODBC / ADO — khác với các server khác dùng Berkeley DB) |

### Thư mục source chính
```
Sources/Sword3PaySys/S3AccServer/
├── main.cpp                     ← Entry point
├── S3PAccount.cpp / .h          ← Logic tài khoản (đăng nhập, đăng ký)
├── AccountLoginDef.h            ← Struct giao thức login (QUAN TRỌNG — Bishop và GameServer cũng include file này)
├── S3PDB_MSSQLServer_Connection ← Kết nối SQL Server
├── S3PDBConnectionPool          ← Pool kết nối DB
├── S3PDBSocketPool / ConVBC     ← Socket pool
├── S3PRole*.cpp                 ← Thông tin nhân vật (role)
├── S3PCard*.cpp                 ← Thẻ nạp/card
├── S3PTask*.cpp                 ← Nhiệm vụ tài khoản
├── S3PEquipment*.cpp            ← Trang bị tài khoản
├── S3PFriend*.cpp               ← Danh sách bạn bè
├── GlobalDTD.h / GlobalFun.cpp  ← Global definitions
└── KThread.cpp                  ← Thread wrapper
```

### Include paths (từ vcxproj)
| Path | Lấy gì |
|---|---|
| `SwordOnline/Headers` | `KProtocolDef.h` (Debug build thêm `Sources/Core/Src`) |

### Static libs link
```
engine.lib       ← SwordOnline/Lib/debug/engine.lib
LuaLibDll.lib    ← SwordOnline/Lib/debug/ (chỉ debug build)
Ws2_32.lib       ← Winsock
```

### Khi cần sửa Sword3PaySys, tham chiếu ở đây

| Muốn sửa | Xem file/thư mục |
|---|---|
| Logic đăng nhập/xác thực tài khoản | `S3PAccount.cpp`, `S3PAccountInfoDAO.cpp` |
| Giao thức trao đổi với Bishop | `AccountLoginDef.h` (cũng dùng bởi Bishop) |
| Kết nối SQL Server | `S3PDB_MSSQLServer_Connection.cpp`, `S3PDBConnectionPool.cpp` |
| Thông tin nhân vật từ góc độ account | `S3PRole*.cpp`, `S3PGamerIDDAO.cpp` |
| Hệ thống thẻ nạp | `S3PCard*.cpp`, `S3PAccCardHistoryDAO.cpp` |

> **Lưu ý**: Sword3PaySys dùng **MS SQL Server**, không phải Berkeley DB như Goddess. Cần có SQL Server instance chạy trước khi khởi động Sword3PaySys.

---

## 3. Goddess → `Goddess.exe` (Database server nhân vật)

### Thông tin build
| Mục | Giá trị |
|---|---|
| Project | `Sources/MultiServer/Goddess/Goddess.vcxproj` |
| Output Debug | `bin/server/Goddess.exe` |
| Build config | `Debug` / `Release` |
| Database | **Berkeley DB** (`libdb62d.lib` debug / `libdb62.lib` release) |
| Port mặc định | `5001` (cấu hình trong ServerCfg.ini của GameServer) |

### Thư mục source chính
```
Sources/MultiServer/Goddess/
├── Goddess.cpp                  ← Entry point, main server loop
├── ClientNode.cpp / .h          ← Xử lý kết nối từ GameServer
├── IDBRoleServer.cpp / .h       ← API chính: GetRoleInfo, SaveRoleInfo, GetRoleListOfAccount
├── DBTable.cpp / .h             ← Abstraction layer cho Berkeley DB
├── db.h                         ← Header Berkeley DB
├── DBBackup.cpp / .h            ← Backup tự động dữ liệu
├── DBDumpLoad.cpp / .h          ← Import/export dữ liệu DB
├── FilterTextLib.cpp / .h       ← Lọc tên nhân vật xấu
└── RoleNameFilter.cpp / .h      ← Filter tên nhân vật

Thư mục dữ liệu (runtime):
Sources/MultiServer/Goddess/database/  ← Files Berkeley DB thực tế
Sources/MultiServer/Goddess/logs/      ← Log file
```

### Include paths (từ vcxproj)
| Path | Lấy gì |
|---|---|
| `MultiServer/Common/` | Network, thread, buffer utilities |
| `MultiServer/Rainbow/Interface` | `RainbowInterface.h` |
| `MultiServer/Heaven/Interface` | `HeavenInterface.h` |
| `SwordOnline/lib` | Header libs |
| `SwordOnline/Headers` | `KProtocol.h`, `S3DBInterface.h` |

### Static libs link
```
debug/common.lib    ← MultiServer networking library
debug/engine.lib    ← Base engine
libdb62d.lib        ← Berkeley DB (debug), libdb62.lib (release) — từ SwordOnline/Lib/
```

### Khi cần sửa Goddess, tham chiếu ở đây

| Muốn sửa | Xem file/thư mục |
|---|---|
| API lưu/đọc dữ liệu nhân vật | `IDBRoleServer.cpp` — đây là file quan trọng nhất |
| Schema dữ liệu nhân vật (format binary) | `SwordOnline/Headers/S3DBInterface.h` |
| Cơ chế backup tự động | `DBBackup.cpp` |
| Import/export database | `DBDumpLoad.cpp` |
| Kết nối từ GameServer | `ClientNode.cpp` |
| Lọc tên nhân vật | `RoleNameFilter.cpp`, `FilterTextLib.cpp` |
| Thao tác Berkeley DB | `DBTable.cpp`, `db.h` |

> **Lưu ý**: Toàn bộ dữ liệu nhân vật là **binary blob** lưu trong Berkeley DB key-value. Format được định nghĩa ở `S3DBInterface.h`. Muốn thêm field vào nhân vật phải sửa cả `S3DBInterface.h` + `Core/Src/KPlayer*` (logic save/load).

---

## 4. Bishop → `Bishop.exe` (Quản lý multi-server + điều phối xác thực)

### Thông tin build
| Mục | Giá trị |
|---|---|
| Project | `Sources/MultiServer/Bishop/Bishop.vcxproj` |
| Output Debug | `bin/server/Bishop.exe` |
| Build config | `Debug` / `Release` |
| Cấu hình credential | `Application.cpp` dòng ~78 |

### Thư mục source chính
```
Sources/MultiServer/Bishop/
├── Application.cpp / .h         ← Entry point — cấu hình credentials tài khoản
├── Bishop.cpp                   ← Main server logic
├── SmartClient.cpp / .h         ← Kết nối tới Sword3PaySys (account server)
├── Intercessor.cpp / .h         ← Điều phối giữa các server
├── Network.cpp / .h             ← Mạng nội bộ
├── GameServer.cpp / .h          ← Quản lý danh sách GameServer đang chạy (Bishop's own)
├── GamePlayer.cpp / .h          ← Theo dõi player đang online
├── PlayerCreator.cpp / .h       ← Tạo/phân bổ player vào server
├── ICommand.h / IGServer.h / IPlayer.h  ← Interface definitions
└── msg_define.h                 ← Định nghĩa message nội bộ
```

### Include paths (từ vcxproj)
| Path | Lấy gì |
|---|---|
| `MultiServer/Common/` | Network, socket, thread utilities |
| `MultiServer/Rainbow/Interface` | `RainbowInterface.h` |
| `MultiServer/Heaven/Interface` | `HeavenInterface.h` |
| `SwordOnline/Headers` | `KProtocolDef.h`, `KProtocol.h` |
| `Sword3PaySys/S3AccServer` | **`AccountLoginDef.h`** — struct giao thức với account server |
| `S3Client/Login` | `LoginDef.h` — giao thức login client |
| `Core/src` | Core headers (dùng một phần) |

### Static libs link
```
debug/common.lib    ← MultiServer networking
debug/engine.lib    ← Base engine
```

### Khi cần sửa Bishop, tham chiếu ở đây

| Muốn sửa | Xem file/thư mục |
|---|---|
| Thay đổi credentials Account Server | `Application.cpp` ~dòng 78: `g_theSmartClient.Open(...)` |
| Cách kết nối tới Sword3PaySys | `SmartClient.cpp` |
| Giao thức xác thực login | `Sword3PaySys/S3AccServer/AccountLoginDef.h` |
| Quản lý danh sách GameServer | `GameServer.cpp` (Bishop's own — không nhầm với GameServer project) |
| Phân bổ player vào server | `PlayerCreator.cpp`, `Intercessor.cpp` |
| Theo dõi player online | `GamePlayer.cpp` |

---

## 5. S3Relay → `S3Relay.exe` (Proxy kết nối + Chat + Bang hội)

### Thông tin build
| Mục | Giá trị |
|---|---|
| Project | `Sources/MultiServer/S3Relay/S3Relay.vcxproj` |
| Output Debug | `bin/server/S3Relay.exe` |
| Build config | `Debug` / `Release` |
| Libs | `engine.lib` + `libdb62d.lib` (Berkeley DB cho Tong/Friend DB) |

> **Lưu ý quan trọng**: S3Relay không chỉ là proxy — nó là server kết hợp xử lý cả **relay TCP**, **chat theo kênh**, **bang hội (Tông)** và **danh sách bạn bè**. Đây là module lớn nhất trong MultiServer.

### Thư mục source chính
```
Sources/MultiServer/S3Relay/
│
├── [Relay / Proxy core]
│   ├── S3Relay.cpp / .h         ← Entry point
│   ├── RelayCenter.cpp / .h     ← Trung tâm điều phối relay
│   ├── RelayServer.cpp / .h     ← Lắng nghe kết nối từ GameServer
│   ├── RelayClient.cpp / .h     ← Kết nối ngược lên GameServer
│   ├── RelayConnect.cpp / .h    ← Xử lý từng kết nối relay
│   ├── GatewayCenter.cpp / .h   ← Gateway cho client
│   ├── GatewayClient.cpp / .h   ← Kết nối từ game client
│   ├── HostServer.cpp / .h      ← Host server
│   ├── HostConnect.cpp / .h     ← Kết nối host
│   ├── RootCenter / RootClient  ← Root server management
│   ├── NetSockDupEx.cpp         ← Socket duplication (chuyển kết nối giữa server)
│   └── DealRelay.cpp / .h       ← Xử lý gói tin relay
│
├── [Chat system]
│   ├── ChatServer.cpp / .h      ← Server chat chính
│   ├── ChatConnect.cpp / .h     ← Kết nối chat
│   ├── ChannelMgr.cpp / .h      ← Quản lý kênh chat (thế giới, khu vực, nhóm...)
│   ├── NetCenter / NetServer / NetClient / NetConnect ← Mạng chat
│   └── DoScript.cpp / .h        ← Xử lý script chat (filter, lệnh đặc biệt)
│
├── [Bang hội — Tông]
│   ├── TongServer.cpp / .h      ← Server bang hội
│   ├── TongConnect.cpp / .h     ← Kết nối bang hội
│   ├── KTongControl.cpp / .h    ← Điều khiển logic bang hội
│   ├── KTongSet.cpp / .h        ← Tập hợp bang hội
│   └── TONGDB.CPP / .H          ← Lưu trữ Berkeley DB cho bang hội
│
├── [Danh sách bạn bè]
│   └── FriendMgr.cpp / .h       ← Quản lý bạn bè
│
├── [Interface với Heaven / Rainbow]
│   ├── HeavenLib.cpp / .h       ← Client kết nối tới Heaven (xác thực)
│   └── RainbowLib.cpp / .h      ← Client kết nối ngược tới Rainbow (nếu có)
│
├── [Utilities]
│   ├── SockThread.cpp / .h      ← Thread cho socket
│   ├── Lock.cpp / LockMRSW      ← Multi-reader/single-writer lock
│   ├── LogFile.cpp / .h         ← Ghi log
│   ├── Memory.cpp / .h          ← Custom memory management
│   └── KThread.cpp / .h         ← Thread wrapper
│
└── [Config files — chạy cùng S3Relay.exe]
    ├── relay_config.ini          ← Cấu hình chính: IP/port relay, danh sách GameServer
    ├── relay_channcfg.ini        ← Cấu hình kênh chat
    ├── relay_channel.ini         ← Dữ liệu kênh chat
    └── relay_friendcfg.ini       ← Cấu hình danh sách bạn bè
```

### Include paths (từ vcxproj — không khai báo `AdditionalIncludeDirectories` tường minh)

S3Relay tự chứa hầu hết code — không depend nhiều vào Core hoặc Engine headers. Nó chỉ include:
- `SwordOnline/Headers/KRelayProtocol.h` — giao thức relay
- `SwordOnline/Headers/KTongProtocol.h` — giao thức bang hội

### Static libs link
```
debug/engine.lib    ← Base engine (file I/O, string utils)
libdb62d.lib        ← Berkeley DB — lưu dữ liệu Tông và bạn bè
lualibdll.lib       ← Lua (qua Ws2_32 + lualibdll dependency)
Ws2_32.lib          ← Winsock
```

### Khi cần sửa S3Relay, tham chiếu ở đây

| Muốn sửa | Xem file/thư mục |
|---|---|
| Cấu hình port, IP GameServer cần relay | `relay_config.ini` |
| Cấu hình kênh chat | `relay_channcfg.ini`, `relay_channel.ini` |
| Logic relay gói tin client ↔ GameServer | `RelayCenter.cpp`, `DealRelay.cpp` |
| Kênh chat và broadcast | `ChannelMgr.cpp`, `ChatServer.cpp` |
| Xử lý bang hội (Tông) | `KTongControl.cpp`, `TONGDB.CPP` |
| Quản lý bạn bè | `FriendMgr.cpp` |
| Giao thức relay | `SwordOnline/Headers/KRelayProtocol.h` |
| Giao thức bang hội | `SwordOnline/Headers/KTongProtocol.h` |
| Chuyển kết nối client sang GameServer khác | `NetSockDupEx.cpp` |

---

## 6. GameServer → `GameServer.exe` (Game server chính)

### Thông tin build
| Mục | Giá trị |
|---|---|
| Project | `Sources/MultiServer/GameServer/GameServer.vcxproj` |
| Output Debug | `bin/server/GameServer.exe` |
| Build config | `Debug` / `Release` |
| Core build | Dùng `CoreServer.lib` (Core build với `_SERVER` define) |

### Thư mục source chính
```
Sources/MultiServer/GameServer/
├── GameServer.cpp               ← Entry point, main loop
├── KSOServer.cpp / .h           ← Class chính KSwordOnLineSever — khởi tạo toàn bộ
├── KTransferUnit.cpp / .h       ← Chuyển player giữa các GameServer instance
├── CRC32.C / .h                 ← Checksum (copy local)
│
├── settings/                   ← Game data — QUAN TRỌNG
│   ├── GameData.ini             ← Thông số game (exp, drop rate...)
│   ├── GameSetting.ini          ← Cài đặt server
│   ├── MapList.ini              ← Danh sách bản đồ
│   ├── NativePlaceList.ini      ← Danh sách quê quán nhân vật
│   ├── RevivePos.ini            ← Vị trí hồi sinh
│   ├── MagicDesc.ini            ← Mô tả ma pháp/skill
│   ├── MapTraffic.ini           ← Cấu hình giao thông bản đồ
│   ├── npc/                     ← Template NPC (spawn, stats, AI type)
│   ├── item/                    ← Data vật phẩm (stats, giá trị)
│   ├── faction/                 ← Data môn phái
│   ├── maps/                    ← Bản đồ game
│   ├── obj/                     ← Object definitions
│   ├── Tong/                    ← Dữ liệu bang hội
│   └── Weather/                 ← Cấu hình thời tiết
│
├── maps/                       ← File bản đồ binary
├── script/                     ← Script Lua cho quest/NPC/AI
└── ServerCfg.ini               ← Cấu hình kết nối servers (IP/port của Relay, DB, Chat, Tong)
```

### Include paths (từ vcxproj)
| Path | Lấy gì |
|---|---|
| `MultiServer/Common/` | Socket, IOCP, thread, buffer, CRC |
| `MultiServer/Rainbow/Interface` | `RainbowInterface.h` — gửi chat |
| `MultiServer/Heaven/Interface` | `HeavenInterface.h` — xác thực |
| `SwordOnline/Headers` | `KProtocol.h`, `KProtocolDef.h`, `S3DBInterface.h` |
| `Sources/Engine/src` & `include` | Engine base headers |

### Static libs link
```
debug/CoreServer.lib ← Core game logic (build từ Core project với _SERVER) — ĐÂY LÀ LIB QUAN TRỌNG NHẤT
debug/engine.lib     ← Base engine
debug/common.lib     ← MultiServer networking (IOCP, Socket, Thread, Buffer)
```

### Khi cần sửa GameServer, tham chiếu ở đây

| Muốn sửa | Xem file/thư mục |
|---|---|
| Logic game (player, NPC, skill, item) | **`Sources/Core/Src/`** — sửa rồi rebuild Core → GameServer |
| Giao thức client ↔ server | `SwordOnline/Headers/KProtocol.h` + `Core/Src/KProtocolProcess.cpp` |
| Kết nối tới các server khác | `ServerCfg.ini` (IP/port) |
| Spawn NPC, stats NPC | `settings/npc/` |
| Data vật phẩm | `settings/item/` |
| Script quest/AI | `script/` (Lua files) |
| Cấu hình game (exp, drop) | `settings/GameData.ini` |
| Bản đồ | `maps/` + `settings/maps/` |
| Chuyển player giữa server | `KTransferUnit.cpp` |
| Main server loop | `KSOServer.cpp` |

---

## Shared resources — dùng bởi nhiều module

### `SwordOnline/Headers/` — Protocol headers dùng chung

| File | Dùng bởi |
|---|---|
| `KProtocol.h` | S3Client, GameServer, S3Relay, Bishop |
| `KProtocolDef.h` | S3Client, GameServer, Bishop, Sword3PaySys |
| `KRelayProtocol.h` | GameServer, S3Relay |
| `KTongProtocol.h` | GameServer, S3Relay |
| `HeavenInterface.h` | GameServer, S3Relay, Bishop |
| `RainbowInterface.h` | GameServer, S3Relay |
| `S3DBInterface.h` | GameServer, Goddess |
| `IClient.h` | S3Client, Core |
| `IServer.h` | GameServer, Heaven |

### `SwordOnline/Lib/` — Pre-compiled libs

```
Lib/
├── debug/
│   ├── CoreClient.lib      ← Core (Client Debug) → dùng bởi S3Client
│   ├── CoreServer.lib      ← Core (Server Debug) → dùng bởi GameServer
│   ├── engine.lib          ← Engine (Debug) → tất cả server modules
│   └── common.lib          ← Common network (Debug) → GameServer, Goddess, Bishop
├── release/
│   ├── CoreClient.lib      ← Client Release
│   ├── CoreServer.lib      ← Server Release
│   ├── engine.lib
│   └── common.lib
├── lualibdll.lib           ← Lua DLL import lib → S3Client, S3Relay, Sword3PaySys
├── lualibdll.dll           ← Lua runtime DLL (cần đặt cạnh exe)
├── libdb62d.lib            ← Berkeley DB Debug → Goddess, S3Relay
└── libdb62.lib             ← Berkeley DB Release
```

### `Sources/MultiServer/Common/` — Network library (source)

Dùng bởi: **GameServer, Goddess, Bishop** (link qua `common.lib`)

| File | Cung cấp |
|---|---|
| `IOCompletionPort.cpp` | IOCP async I/O |
| `SocketServer.cpp` | TCP server lắng nghe |
| `SocketClient.cpp` | TCP client kết nối |
| `Thread.cpp` | Thread wrapper |
| `Buffer.cpp` / `IOBuffer.cpp` | Packet buffer |
| `IniFile.cpp` | Đọc file .ini |
| `KSG_EncodeDecode.cpp` | Mã hóa/giải mã gói tin |
| `CRC32.c` | Checksum |

---

## Bảng tóm tắt — Lib nào cần rebuild khi sửa source

| Sửa source | Rebuild project này | Ảnh hưởng tới |
|---|---|---|
| `Sources/Core/Src/` + `_SERVER` | `Core` (Server Debug) → `CoreServer.lib` | GameServer |
| `Sources/Core/Src/` + client | `Core` (Client Debug) → `CoreClient.lib` | S3Client |
| `Sources/Engine/` | `Engine` → `engine.lib` | Tất cả |
| `Sources/MultiServer/Common/` | `Common` → `common.lib` | GameServer, Goddess, Bishop |
| `Sources/Library/LuaLib/` | `LuaLibDll` → `lualibdll.dll` | S3Client, S3Relay, Sword3PaySys |
| `SwordOnline/Headers/KProtocol.h` | Rebuild tất cả module sử dụng | S3Client + GameServer |
