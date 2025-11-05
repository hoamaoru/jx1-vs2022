# Jian Xia Qing Yuan (Sword Online) - Game Source Code

## Tổng quan dự án

Đây là source code của game MMORPG **Jian Xia Qing Yuan (Sword Online)** - một game online kiếm hiệp cổ điển. Dự án được phát triển bằng C++ với Visual Studio và sử dụng kiến trúc client-server phân tán.

**Đặc điểm kỹ thuật:**
- Ngôn ngữ: C++14
- IDE: Visual Studio 2022
- Kiến trúc: Client-Server (Multi-Server)
- Tổng số projects: **20 modules**

---

## Cấu trúc dự án

### 🎮 **Core Modules (Các module cốt lõi)**

#### **Core** (`Core\Core.vcxproj`)
- **Chức năng**: Engine cốt lõi của game, chứa logic game chính
- **Bao gồm**: 
  - Hệ thống nhân vật (KPlayer)
  - NPC (KNpc)
  - Kỹ năng (KSkills, KSkillManager)
  - Vật phẩm (KItemList, KInventory)
  - Nhiệm vụ (KPlayerTask)
  - Thuộc tính ma pháp (KMagicAttrib)
- **Vai trò**: Xử lý tất cả logic gameplay, chiến đấu, tương tác

#### **Engine** (`Engine\Engine.vcxproj`)
- **Chức năng**: Engine đồ họa và hệ thống cơ bản
- **Bao gồm**: 
  - Xử lý file (KFile)
  - Hệ thống cửa sổ (KWin32Wnd)
  - Rendering, âm thanh, input
- **Vai trò**: Cung cấp nền tảng kỹ thuật cho game

---

### 💻 **Client Side (Phía Client)**

#### **S3Client** (`S3Client\S3Client.vcxproj`)
- **Chức năng**: Game client chính - giao diện người chơi
- **Bao gồm**: 
  - UI System (UiCase, UiElem)
  - Login system (Login.cpp)
  - Chat room (UiChatRoom)
  - Server selection (UiSelServer)
  - Player control bar (UiPlayerControlBar)
- **Vai trò**: Hiển thị game và nhận input từ người chơi

#### **Represent2 & Represent3** 
- **Chức năng**: Hệ thống hiển thị đồ họa 2D/3D
- **Vai trò**: Render nhân vật, môi trường, hiệu ứng

---

### 🖥️ **Server Side (Phía Server)**

#### **GameServer** (`MultiServer\GameServer\GameServer.vcxproj`)
- **Chức năng**: Server game chính xử lý logic gameplay
- **Vai trò**: Quản lý người chơi trong game, chiến đấu, tương tác

#### **Goddess** (`MultiServer\Goddess\Goddess.vcxproj`)
- **Chức năng**: Database server - quản lý dữ liệu nhân vật
- **Bao gồm**: IDBRoleServer - interface database
- **Vai trò**: Lưu trữ thông tin player, vật phẩm, tiến độ

#### **Heaven** (`MultiServer\Heaven\Heaven.vcxproj`)
- **Chức năng**: Login server - xác thực đăng nhập
- **Vai trò**: Kiểm tra tài khoản, phân phối server

#### **Rainbow** (`MultiServer\Rainbow\Rainbow.vcxproj`)
- **Chức năng**: Chat server hoặc social system
- **Vai trò**: Xử lý chat, bang hội, tương tác xã hội

#### **Bishop** (`MultiServer\Bishop\Bishop.vcxproj`)
- **Chức năng**: Manager server - giám sát các server khác
- **Bao gồm**: GamePlayer - quản lý người chơi
- **Vai trò**: Load balancing, monitoring, quản lý cluster

#### **S3Relay** (`MultiServer\S3Relay\S3Relay.vcxproj`)
- **Chức năng**: Relay server - chuyển tiếp dữ liệu
- **Vai trò**: Proxy giữa client và game server

#### **Common** (`MultiServer\Common\Common.vcxproj`)
- **Chức năng**: Thư viện chung cho các server
- **Vai trò**: Code dùng chung, protocol, utility functions

---

### 💳 **Payment System (Hệ thống thanh toán)**

#### **Sword3PaySys**
- **S3AccServer** (`Sword3PaySys\S3AccServer\Sword3PaySys.vcxproj`): Account server
- **S3RelayServer** (`Sword3PaySys\S3RELAYSERVER\S3RelayServer.vcxproj`): Payment relay
- **Chức năng**: Xử lý nạp thẻ, mua vật phẩm, quản lý tài khoản VIP

---

### 📚 **Support Libraries (Thư viện hỗ trợ)**

#### **LuaLib** (`Library\LuaLib\LuaLibDll.vcxproj`)
- **Chức năng**: Thư viện Lua scripting
- **Vai trò**: Cho phép viết script, quest, AI bằng Lua

#### **JpgLib** (`JpgLib\JpgLib.vcxproj`)
- **Chức năng**: Thư viện xử lý ảnh JPEG
- **Vai trò**: Load và hiển thị texture, UI graphics

#### **KMp3LibClass** (`KMp3LibClass\KMp3LibClass.vcxproj`)
- **Chức năng**: Thư viện phát nhạc MP3
- **Vai trò**: Background music, sound effects

#### **FilterText** (`FilterText\FilterText.vcxproj`)
- **Chức năng**: Bộ lọc từ ngữ
- **Vai trò**: Kiểm duyệt chat, tên nhân vật, chống spam

#### **ExpandPackage** (`ExpandPackage2.0\ExpandPackage.vcxproj`)
- **Chức năng**: Hệ thống mở rộng gói dữ liệu
- **Vai trò**: Nén/giải nén, mã hóa game assets

---

## 🏗️ Kiến trúc hệ thống

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   S3Client  │◄──►│   S3Relay   │◄──►│ GameServer  │
│   (Client)  │    │  (Proxy)    │    │  (Logic)    │
└─────────────┘    └─────────────┘    └─────────────┘
                            │                 ▲
                            ▼                 │
                   ┌─────────────┐    ┌─────────────┐
                   │   Heaven    │    │   Bishop    │
                   │  (Login)    │    │ (Manager)   │
                   └─────────────┘    └─────────────┘
                            │                 │
                            ▼                 ▼
                   ┌─────────────┐    ┌─────────────┐
                   │  Goddess    │    │  Rainbow    │
                   │ (Database)  │    │   (Chat)    │
                   └─────────────┘    └─────────────┘
                            │
                            ▼
                   ┌─────────────┐
                   │Sword3PaySys │
                   │ (Payment)   │
                   └─────────────┘
```

---

## 🔧 Hướng dẫn build

### Yêu cầu hệ thống:
- **Visual Studio 2022** hoặc cao hơn
- **Windows SDK**
- **C++14 Compiler**

### Các bước build:
1. Mở solution trong Visual Studio 2022
2. Restore NuGet packages (nếu có)
3. Build theo thứ tự:
   - Core libraries (Engine, Core)
   - Support libraries (LuaLib, JpgLib, KMp3LibClass)
   - Client (S3Client)
   - Servers (GameServer, Goddess, Heaven, etc.)

### Build order khuyến nghị:
```
1. Engine
2. Core
3. LuaLib, JpgLib, KMp3LibClass, FilterText
4. Common
5. S3Client, Represent2, Represent3
6. GameServer, Goddess, Heaven, Rainbow, Bishop, S3Relay
7. Sword3PaySys modules
```

---

## 📁 Cấu trúc thư mục

```
SwordOnline/Sources/
├── Core/                    # Core game logic
├── Engine/                  # Base engine
├── S3Client/               # Game client
│   ├── Ui/
│   │   ├── UiCase/         # UI screens
│   │   └── Elem/           # UI elements
│   └── Login/              # Login system
├── MultiServer/            # Server modules
│   ├── GameServer/         # Main game server
│   ├── Goddess/            # Database server
│   ├── Heaven/             # Login server
│   ├── Rainbow/            # Chat server
│   ├── Bishop/             # Manager server
│   ├── S3Relay/            # Relay server
│   └── Common/             # Shared server code
├── Sword3PaySys/          # Payment system
├── Library/               # External libraries
├── Represent/             # Graphics rendering
├── JpgLib/               # Image processing
├── KMp3LibClass/         # Audio processing
├── FilterText/           # Text filtering
└── ExpandPackage2.0/     # Package system
```

---

## 🎯 Các tính năng chính

- **🗡️ Combat System**: Hệ thống chiến đấu real-time với kỹ năng đa dạng
- **👥 Multiplayer**: Hỗ trợ nhiều người chơi online cùng lúc
- **💬 Chat System**: Hệ thống chat đa kênh
- **🏪 Trading**: Hệ thống giao dịch vật phẩm
- **🎪 Guild System**: Hệ thống bang hội
- **💰 Payment**: Tích hợp hệ thống thanh toán
- **🎵 Audio/Video**: Hỗ trợ âm thanh và hình ảnh đa phương tiện
- **📜 Scripting**: Hỗ trợ Lua scripting cho quest và AI

---

## 🤝 Đóng góp

Dự án này là source code mở của game kiếm hiệp cổ điển. Mọi đóng góp để cải thiện, fix bug hoặc thêm tính năng đều được hoan nghênh.

---

## 📄 License

[Thêm thông tin license tại đây]

---

## 📞 Liên hệ

[Thêm thông tin liên hệ tại đây]

---

**⚔️ Chúc các hiệp sĩ có những trải nghiệm tuyệt vời trong thế giới giang hồ! ⚔️**
