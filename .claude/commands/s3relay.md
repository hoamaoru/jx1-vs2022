---
description: Làm việc với module S3Relay — relay+social server, proxy giữa client và GameServer
---
Bạn đang làm việc với module **S3Relay** — combined relay+social server. Build ra `bin/server/S3Relay.exe`.

> **Quan trọng:** S3Relay KHÔNG chỉ là TCP proxy. Nó là server lớn tích hợp: relay proxy + chat channels + Tong (bang hội) management + friend list + Berkeley DB storage.

## Source folders — Tầng 1: Code của S3Relay (sửa trực tiếp ở đây)

| Thư mục | Mô tả |
|---------|-------|
| `SwordOnline/Sources/MultiServer/S3Relay/` | Root của module — relay, chat, Tong, Friend |
| `SwordOnline/Sources/MultiServer/Common/` | IOCP networking, sockets, threads |

## Source folders — Tầng 2: Dependencies (đọc để hiểu protocol/interface)

| Thư mục | Khi nào cần đọc |
|---------|----------------|
| `SwordOnline/Headers/KRelayProtocol.h` | **Luôn luôn khi làm relay** — định nghĩa toàn bộ packet giữa Client ↔ S3Relay ↔ GameServer |
| `SwordOnline/Headers/KTongProtocol.h` | Khi làm tính năng Tong (bang hội) |
| `SwordOnline/Headers/RainbowInterface.h` | Khi làm chat/channel qua Rainbow |
| `SwordOnline/Headers/HeavenInterface.h` | Khi liên quan đến login authentication flow |
| `SwordOnline/Sources/Engine/src/` | Base utilities |

## Libs được link

| Lib | Nằm tại |
|-----|---------|
| `engine.lib` | Built từ `Sources/Engine/` |
| `libdb62d.lib` (debug) / `libdb62.lib` (release) | Berkeley DB — lưu Tong và Friend data |
| `lualibdll.lib` | Lua scripting |

## Files quan trọng theo sub-system

### Relay (proxy Client ↔ GameServer)
- `RelayCenter.cpp` — trung tâm relay, routing packets
- `RelayServer.cpp` / `RelayClient.cpp` / `RelayConnect.cpp`
- `GatewayCenter.cpp` / `GatewayClient.cpp` — gateway management

### Chat Channels
- `ChannelMgr.cpp` — quản lý chat channels
- `ChatServer.cpp` — xử lý chat messages

### Tong (Bang hội / Guild)
- `KTongControl.cpp` / `KTongSet.cpp` — Tong logic
- `TONGDB.cpp` — lưu/load Tong data vào Berkeley DB
- `TongConnect.cpp` / `TongServer.cpp` — Tong networking

### Friend System
- `FriendMgr.cpp` — quản lý friend list, lưu vào Berkeley DB

### Auth Bridges
- `HeavenLib.cpp` — bridge tới Heaven login server
- `RainbowLib.cpp` — bridge tới Rainbow chat

### Networking
- `NetCenter.cpp` / `NetServer.cpp` / `NetClient.cpp` / `NetConnect.cpp`

## Lookup nhanh

| Muốn sửa | File cần xem |
|-----------|-------------|
| Relay routing logic | `RelayCenter.cpp` |
| Chat channel management | `ChannelMgr.cpp`, `ChatServer.cpp` |
| Bang hội (Tong) feature | `KTongControl.cpp`, `KTongSet.cpp` |
| Tong data persistence | `TONGDB.cpp` (Berkeley DB) |
| Friend list | `FriendMgr.cpp` (Berkeley DB) |
| Relay protocol packets | `SwordOnline/Headers/KRelayProtocol.h` |
| Tong protocol packets | `SwordOnline/Headers/KTongProtocol.h` |
| Gateway connections | `GatewayCenter.cpp` |

## Build config

Dùng configuration **Server Debug** hoặc **Server Release**.  
Rebuild order: Common → S3Relay.

## Luồng kết nối

```
S3Client ↔ S3Relay (GatewayCenter) ↔ GameServer (RelayCenter)
              ↕
          ChannelMgr (chat)
          KTongControl (bang hội, lưu BDB)
          FriendMgr (bạn bè, lưu BDB)
              ↕
          HeavenLib ↔ Heaven
          RainbowLib ↔ Rainbow
```
