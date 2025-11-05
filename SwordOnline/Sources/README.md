# Jian Xia Qing Yuan (Sword Online) - Game Source Code

## T?ng quan d? án

?ây là source code c?a game MMORPG **Jian Xia Qing Yuan (Sword Online)** - m?t game online ki?m hi?p c? ?i?n. D? án ???c phát tri?n b?ng C++ v?i Visual Studio và s? d?ng ki?n trúc client-server phân tán.

**??c ?i?m k? thu?t:**
- Ngôn ng?: C++14
- IDE: Visual Studio 2022
- Ki?n trúc: Client-Server (Multi-Server)
- T?ng s? projects: **20 modules**

---

## C?u trúc d? án

### ?? **Core Modules (Các module c?t lõi)**

#### **Core** (`Core\Core.vcxproj`)
- **Ch?c n?ng**: Engine c?t lõi c?a game, ch?a logic game chính
- **Bao g?m**: 
  - H? th?ng nhân v?t (KPlayer)
  - NPC (KNpc)
  - K? n?ng (KSkills, KSkillManager)
  - V?t ph?m (KItemList, KInventory)
  - Nhi?m v? (KPlayerTask)
  - Thu?c tính ma pháp (KMagicAttrib)
- **Vai trò**: X? lý t?t c? logic gameplay, chi?n ??u, t??ng tác

#### **Engine** (`Engine\Engine.vcxproj`)
- **Ch?c n?ng**: Engine ?? h?a và h? th?ng c? b?n
- **Bao g?m**: 
  - X? lý file (KFile)
  - H? th?ng c?a s? (KWin32Wnd)
  - Rendering, âm thanh, input
- **Vai trò**: Cung c?p n?n t?ng k? thu?t cho game

---

### ?? **Client Side (Phía Client)**

#### **S3Client** (`S3Client\S3Client.vcxproj`)
- **Ch?c n?ng**: Game client chính - giao di?n ng??i ch?i
- **Bao g?m**: 
  - UI System (UiCase, UiElem)
  - Login system (Login.cpp)
  - Chat room (UiChatRoom)
  - Server selection (UiSelServer)
  - Player control bar (UiPlayerControlBar)
- **Vai trò**: Hi?n th? game và nh?n input t? ng??i ch?i

#### **Represent2 & Represent3** 
- **Ch?c n?ng**: H? th?ng hi?n th? ?? h?a 2D/3D
- **Vai trò**: Render nhân v?t, môi tr??ng, hi?u ?ng

---

### ??? **Server Side (Phía Server)**

#### **GameServer** (`MultiServer\GameServer\GameServer.vcxproj`)
- **Ch?c n?ng**: Server game chính x? lý logic gameplay
- **Vai trò**: Qu?n lý ng??i ch?i trong game, chi?n ??u, t??ng tác

#### **Goddess** (`MultiServer\Goddess\Goddess.vcxproj`)
- **Ch?c n?ng**: Database server - qu?n lý d? li?u nhân v?t
- **Bao g?m**: IDBRoleServer - interface database
- **Vai trò**: L?u tr? thông tin player, v?t ph?m, ti?n ??

#### **Heaven** (`MultiServer\Heaven\Heaven.vcxproj`)
- **Ch?c n?ng**: Login server - xác th?c ??ng nh?p
- **Vai trò**: Ki?m tra tài kho?n, phân ph?i server

#### **Rainbow** (`MultiServer\Rainbow\Rainbow.vcxproj`)
- **Ch?c n?ng**: Chat server ho?c social system
- **Vai trò**: X? lý chat, bang h?i, t??ng tác xã h?i

#### **Bishop** (`MultiServer\Bishop\Bishop.vcxproj`)
- **Ch?c n?ng**: Manager server - giám sát các server khác
- **Bao g?m**: GamePlayer - qu?n lý ng??i ch?i
- **Vai trò**: Load balancing, monitoring, qu?n lý cluster

#### **S3Relay** (`MultiServer\S3Relay\S3Relay.vcxproj`)
- **Ch?c n?ng**: Relay server - chuy?n ti?p d? li?u
- **Vai trò**: Proxy gi?a client và game server

#### **Common** (`MultiServer\Common\Common.vcxproj`)
- **Ch?c n?ng**: Th? vi?n chung cho các server
- **Vai trò**: Code dùng chung, protocol, utility functions

---

### ?? **Payment System (H? th?ng thanh toán)**

#### **Sword3PaySys**
- **S3AccServer** (`Sword3PaySys\S3AccServer\Sword3PaySys.vcxproj`): Account server
- **S3RelayServer** (`Sword3PaySys\S3RELAYSERVER\S3RelayServer.vcxproj`): Payment relay
- **Ch?c n?ng**: X? lý n?p th?, mua v?t ph?m, qu?n lý tài kho?n VIP

---

### ?? **Support Libraries (Th? vi?n h? tr?)**

#### **LuaLib** (`Library\LuaLib\LuaLibDll.vcxproj`)
- **Ch?c n?ng**: Th? vi?n Lua scripting
- **Vai trò**: Cho phép vi?t script, quest, AI b?ng Lua

#### **JpgLib** (`JpgLib\JpgLib.vcxproj`)
- **Ch?c n?ng**: Th? vi?n x? lý ?nh JPEG
- **Vai trò**: Load và hi?n th? texture, UI graphics

#### **KMp3LibClass** (`KMp3LibClass\KMp3LibClass.vcxproj`)
- **Ch?c n?ng**: Th? vi?n phát nh?c MP3
- **Vai trò**: Background music, sound effects

#### **FilterText** (`FilterText\FilterText.vcxproj`)
- **Ch?c n?ng**: B? l?c t? ng?
- **Vai trò**: Ki?m duy?t chat, tên nhân v?t, ch?ng spam

#### **ExpandPackage** (`ExpandPackage2.0\ExpandPackage.vcxproj`)
- **Ch?c n?ng**: H? th?ng m? r?ng gói d? li?u
- **Vai trò**: Nén/gi?i nén, mã hóa game assets

---

## ??? Ki?n trúc h? th?ng

```
???????????????    ???????????????    ???????????????
?   S3Client  ??????   S3Relay   ?????? GameServer  ?
?   (Client)  ?    ?  (Proxy)    ?    ?  (Logic)    ?
???????????????    ???????????????    ???????????????
                            ?                 ?
                            ?                 ?
                   ???????????????    ???????????????
                   ?   Heaven    ?    ?   Bishop    ?
                   ?  (Login)    ?    ? (Manager)   ?
                   ???????????????    ???????????????
                            ?                 ?
                            ?                 ?
                   ???????????????    ???????????????
                   ?  Goddess    ?    ?  Rainbow    ?
                   ? (Database)  ?    ?   (Chat)    ?
                   ???????????????    ???????????????
                            ?
                            ?
                   ???????????????
                   ?Sword3PaySys ?
                   ? (Payment)   ?
                   ???????????????
```

---

## ?? H??ng d?n build

### Yêu c?u h? th?ng:
- **Visual Studio 2022** ho?c cao h?n
- **Windows SDK**
- **C++14 Compiler**

### Các b??c build:
1. M? solution trong Visual Studio 2022
2. Restore NuGet packages (n?u có)
3. Build theo th? t?:
   - Core libraries (Engine, Core)
   - Support libraries (LuaLib, JpgLib, KMp3LibClass)
   - Client (S3Client)
   - Servers (GameServer, Goddess, Heaven, etc.)

### Build order khuy?n ngh?:
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

## ?? C?u trúc th? m?c

```
SwordOnline/Sources/
??? Core/                    # Core game logic
??? Engine/                  # Base engine
??? S3Client/               # Game client
?   ??? Ui/
?   ?   ??? UiCase/         # UI screens
?   ?   ??? Elem/           # UI elements
?   ??? Login/              # Login system
??? MultiServer/            # Server modules
?   ??? GameServer/         # Main game server
?   ??? Goddess/            # Database server
?   ??? Heaven/             # Login server
?   ??? Rainbow/            # Chat server
?   ??? Bishop/             # Manager server
?   ??? S3Relay/            # Relay server
?   ??? Common/             # Shared server code
??? Sword3PaySys/          # Payment system
??? Library/               # External libraries
??? Represent/             # Graphics rendering
??? JpgLib/               # Image processing
??? KMp3LibClass/         # Audio processing
??? FilterText/           # Text filtering
??? ExpandPackage2.0/     # Package system
```

---

## ?? Các tính n?ng chính

- **??? Combat System**: H? th?ng chi?n ??u real-time v?i k? n?ng ?a d?ng
- **?? Multiplayer**: H? tr? nhi?u ng??i ch?i online cùng lúc
- **?? Chat System**: H? th?ng chat ?a kênh
- **?? Trading**: H? th?ng giao d?ch v?t ph?m
- **?? Guild System**: H? th?ng bang h?i
- **?? Payment**: Tích h?p h? th?ng thanh toán
- **?? Audio/Video**: H? tr? âm thanh và hình ?nh ?a ph??ng ti?n
- **?? Scripting**: H? tr? Lua scripting cho quest và AI

---

## ?? ?óng góp

D? án này là source code m? c?a game ki?m hi?p c? ?i?n. M?i ?óng góp ?? c?i thi?n, fix bug ho?c thêm tính n?ng ??u ???c hoan nghênh.

---

## ?? License

[Thêm thông tin license t?i ?ây]

---

## ?? Liên h?

[Thêm thông tin liên h? t?i ?ây]

---

**?? Chúc các hi?p s? có nh?ng tr?i nghi?m tuy?t v?i trong th? gi?i giang h?! ??**