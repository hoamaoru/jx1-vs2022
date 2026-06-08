# Tutorial: Hiểu về Dự án Jian Xia Qing Yuan (Võ Lâm 1 - JX1)

> Tài liệu này giải thích kiến trúc và cách hoạt động của source code game MMORPG kiếm hiệp cổ điển **Jian Xia Qing Yuan** đã được migrate lên Visual Studio 2022.

---

## Mục lục

1. [Tổng quan dự án](#1-tổng-quan-dự-án)
2. [Kiến trúc hệ thống](#2-kiến-trúc-hệ-thống)
3. [Module Core — Trái tim của game](#3-module-core--trái-tim-của-game)
4. [Hệ thống thế giới game (Map & World)](#4-hệ-thống-thế-giới-game-map--world)
5. [Hệ thống Gameplay](#5-hệ-thống-gameplay)
6. [Giao thức truyền thông (Protocol)](#6-giao-thức-truyền-thông-protocol)
7. [Phía Client — S3Client](#7-phía-client--s3client)
8. [Hệ thống Server đa tầng](#8-hệ-thống-server-đa-tầng)
9. [Lua Scripting](#9-lua-scripting)
10. [Luồng hoạt động từ Đăng nhập đến Vào game](#10-luồng-hoạt-động-từ-đăng-nhập-đến-vào-game)
11. [Hướng dẫn Build](#11-hướng-dẫn-build)
12. [Branch hiện tại: ui_lien_tram](#12-branch-hiện-tại-ui_lien_tram)

---

## 1. Tổng quan dự án

**Jian Xia Qing Yuan** (JX1, hay còn gọi là Võ Lâm 1) là game MMORPG 2D/3D kiếm hiệp cổ điển, được phát triển bởi Kingsoft (Trung Quốc) vào đầu những năm 2000. Đây là source code của game đó, được fork và chỉnh sửa để build được trên Visual Studio 2022.

**Công nghệ sử dụng:**
- Ngôn ngữ: C++14, 32-bit (Win32)
- IDE: Visual Studio 2022
- Đồ họa: DirectX 9 (D3D9)
- Scripting: Lua 5.x
- Cơ sở dữ liệu: Berkeley DB (libdb41s)
- Mạng: Windows IOCP (I/O Completion Port) — mô hình async hiệu năng cao
- Tổng số project trong solution: **20 modules**

**Đặc điểm quan trọng:**  
Module `Core` được chia sẻ giữa client và server — cùng một source code nhưng compile khác nhau thông qua preprocessor define `_SERVER`. Khi có `_SERVER`, code chạy là server-side; khi không có, code chạy là client-side.

---

## 2. Kiến trúc hệ thống

### Sơ đồ tổng thể

```
┌──────────────────────────────────────────────────────┐
│                    PHÍA CLIENT                       │
│                                                      │
│  S3Client.exe                                        │
│  ┌──────────┐  ┌──────────┐  ┌───────────────────┐  │
│  │  Core    │  │ Represent│  │   UI System        │  │
│  │ (client) │  │  2 / 3   │  │ (UiCase + Wnd*)   │  │
│  └──────────┘  └──────────┘  └───────────────────┘  │
└──────────────────────────┬───────────────────────────┘
                           │ TCP (qua S3Relay)
┌──────────────────────────▼───────────────────────────┐
│                    PHÍA SERVER                       │
│                                                      │
│  ┌──────────┐   ┌──────────┐   ┌──────────────────┐ │
│  │  Heaven  │   │  Bishop  │   │    S3Relay       │ │
│  │ (Login)  │   │(Manager) │   │   (Proxy)        │ │
│  └────┬─────┘   └────┬─────┘   └────────┬─────────┘ │
│       │              │                  │            │
│  ┌────▼─────┐   ┌────▼─────┐   ┌────────▼─────────┐ │
│  │ Goddess  │   │ Rainbow  │   │   GameServer     │ │
│  │  (DB)    │   │  (Chat)  │   │ (Logic - Core)   │ │
│  └──────────┘   └──────────┘   └──────────────────┘ │
│                                                      │
│  ┌──────────────────────────────────────────────┐    │
│  │           Sword3PaySys (Payment)             │    │
│  └──────────────────────────────────────────────┘    │
└──────────────────────────────────────────────────────┘
```

### Vai trò từng thành phần

| Thành phần | Loại | Vai trò |
|---|---|---|
| **S3Client** | Client EXE | Giao diện người chơi, render, input |
| **Core** | Shared lib | Toàn bộ logic game (player, NPC, skill, item...) |
| **Engine** | Static lib | Nền tảng kỹ thuật (Win32, file, DirectX wrapper) |
| **Represent2/3** | DLL | Render nhân vật, map, hiệu ứng |
| **S3Relay** | Server EXE | Proxy TCP giữa client và GameServer |
| **GameServer** | Server EXE | Xử lý logic game, dùng module Core (server build) |
| **Heaven** | Server DLL | Xác thực tài khoản đăng nhập |
| **Goddess** | Server EXE | Lưu trữ dữ liệu nhân vật (Berkeley DB) |
| **Rainbow** | Server DLL | Chat, bang hội (Tông), mạng xã hội |
| **Bishop** | Server EXE | Giám sát và điều phối toàn bộ cụm server |
| **LuaLibDll** | DLL | Engine Lua scripting |
| **ExpandPackage** | Lib | Nén/giải nén file assets game |
| **FilterText** | Lib | Lọc từ ngữ xấu trong chat/tên |
| **Sword3PaySys** | Server | Hệ thống nạp thẻ, tài khoản |

---

## 3. Module Core — Trái tim của game

`SwordOnline/Sources/Core/Src/` là thư mục quan trọng nhất, chứa toàn bộ logic game.

### 3.1 Cơ chế chia sẻ client/server

File `GameDataDef.h` định nghĩa các hằng số quan trọng khác nhau tùy theo build:

```cpp
#ifdef _SERVER
#define MAX_NPC    48000      // Server quản lý tối đa 48000 NPC
#define MAX_PLAYER 1200       // Tối đa 1200 người chơi
#define MAX_ITEM   160000
#else
#define MAX_NPC    256        // Client chỉ cần giữ NPC trong tầm nhìn
#define MAX_PLAYER 2
#define MAX_ITEM   512
#endif

#define GAME_FPS            18    // Game chạy ở 18 FPS
#define MAX_LEVEL          200    // Level tối đa của nhân vật
#define MAX_LEAD_LEVEL     100    // Level thống lĩnh tối đa
#define MAX_TONG_LEVEL      10    // Level bang hội tối đa
```

### 3.2 Class KPlayer — Đối tượng người chơi

`KPlayer` là class trung tâm, tổng hợp hầu hết hệ thống game:

```
KPlayer
├── KInventory          — Túi đồ (lưới 2D chứa vật phẩm)
├── KItemList           — Danh sách trang bị đang mặc
├── KPlayerTask         — Hệ thống nhiệm vụ (quest)
├── KPlayerPK           — Hệ thống PK (Player Kill)
├── KPlayerFaction      — Hệ thống môn phái
├── KPlayerTong         — Thông tin bang hội
├── KPlayerTrade        — Hệ thống giao dịch
├── KPlayerTeam         — Hệ thống nhóm (team)
├── KPlayerChat         — Hệ thống chat
├── KPlayerChatRoom     — Phòng chat
├── KPlayerAI           — AI người chơi (auto-play)
└── KSkills (qua KSkillList) — Danh sách kỹ năng
```

**Các thuộc tính cơ bản:**
- `m_nStrength` / `m_nDexterity` / `m_nVitality` / `m_nEngergy` / `m_nLucky` — 5 chỉ số cơ bản
- `m_nExp` / `m_nNextLevelExp` — Kinh nghiệm
- `m_nLeadExp` / `m_nLeadLevel` — Hệ thống thống lĩnh (lead level riêng biệt)
- `m_nAttributePoint` / `m_nSkillPoint` — Điểm phân bổ chờ dùng

**Phần code chỉ tồn tại khi compile server (`#ifdef _SERVER`):**  
- Buffer lưu dữ liệu để ghi DB (`m_SaveBuffer[64*1024]`)
- Quản lý trạng thái kết nối, thời gian đăng nhập
- Danh sách điểm waypoint/trạm dịch
- Cờ cấm: chat, đổi tên, giao dịch, đổi phe

**Phần code chỉ tồn tại khi compile client (`#ifndef _SERVER`):**  
- Skill trái/phải đang gán vào chuột
- Trạng thái animation chạy
- Dữ liệu bảng xếp hạng nhiệm vụ (mission ladder)

### 3.3 Class KNpc — NPC và quái

`KNpc` đại diện cho mọi nhân vật không phải người chơi, bao gồm cả người chơi khi được render từ góc nhìn người khác.

Các trạng thái hành động NPC (`NPCCMD`):
```
do_none, do_stand, do_walk, do_run, do_jump,
do_skill, do_magic, do_attack, do_sit, do_hurt, do_death,
do_defense, do_idle, do_specialskill, do_runattack,
do_manyattack, do_jumpattack, do_revive ...
```

NPC có AI (`KNpcAI`) với các loại:
- `ProcessAIType01` đến `ProcessAIType06` — các kiểu AI khác nhau (đứng yên, tuần tra, tấn công chủ động, bỏ chạy...)
- AI phía server: tìm đường đi thực (`FindPathNpc`)
- AI phía client: animation theo dõi/đi theo (`ShowNpcType11`~`ShowNpcType14`)

---

## 4. Hệ thống thế giới game (Map & World)

### 4.1 Cấu trúc phân cấp bản đồ

```
KSubWorldSet (quản lý nhiều thế giới)
└── KSubWorld (một bản đồ/zone, tối đa 100 trên server, 1 trên client)
    └── KRegion[] (ô vùng, kích thước 512×1024 pixel)
        ├── KList m_NpcList      — danh sách NPC trong vùng
        ├── KList m_PlayerList   — danh sách người chơi trong vùng
        ├── KList m_ObjList      — danh sách vật phẩm rơi
        ├── KList m_MissleList   — danh sách đạn/chiêu bay
        └── m_Obstacle[][]       — ma trận chướng ngại vật (chỉ server)
```

**Đơn vị đo lường:**
- 1 ô (cell) = 32×32 pixel
- 1 Region = 16×32 ô = 512×1024 pixel
- Toạ độ `MpsX/MpsY` là toạ độ thế giới tính theo đơn vị pixel

**Server kích hoạt Region theo nhu cầu** (`m_nActive`): Region chỉ được cập nhật khi có người chơi ở gần, giúp tiết kiệm CPU.

### 4.2 Hệ thống đồng bộ

Server không gửi toàn bộ bản đồ cho client. Khi người chơi di chuyển, server gửi dữ liệu các Region lân cận. Client chỉ duy trì tối đa 9 Region (`MAX_REGION = 9`) xung quanh người chơi.

---

## 5. Hệ thống Gameplay

### 5.1 Hệ thống Kỹ năng (`KSkill`)

Mỗi kỹ năng có:
- **Loại**: cận chiến (`m_bIsMelee`), AoE, tầm xa qua đạn (`m_bByMissle`), aura, bị động
- **Mục tiêu**: kẻ địch / đồng minh / bản thân / vật thể
- **Chi phí**: mana / stamina / tiền
- **Script**: mỗi kỹ năng có thể gắn script Lua riêng khi lên cấp (`m_dwSkillLevelUpScriptID`)

**Hệ thống đạn (`KMissle`)**: Kỹ năng tầm xa phóng ra đối tượng `KMissle` bay trong không gian, có thể va chạm, nổ vùng. Missle được quản lý trong `m_MissleList` của Region.

### 5.2 Hệ thống Thuộc tính ma pháp (`KMagicAttrib`)

Đây là hệ thống chỉ số mở rộng cho cả kỹ năng, vật phẩm và chiêu thức. Enum `MAGIC_ATTRIB` định nghĩa hàng trăm chỉ số:

- **Damage**: vật lý, ma pháp, lửa, băng, sét, độc
- **Defense**: giáp vật lý, kháng nguyên tố
- **Lifesteal**: hút máu (`magic_steallife_p`), hút mana, hút stamina
- **Critical**: chí mạng thường (`magic_deadlystrike_p`), chí mạng chết người (`magic_fatallystrike_p`)
- **CC**: đóng băng, trúng độc, choáng, tàng hình (`STATE_FREEZE`, `STATE_POISON`, `STATE_STUN`, `STATE_HIDE`)
- **Item**: yêu cầu level, môn phái, giới tính, translife; có thể khóa bán/giao dịch/vứt

### 5.3 Hệ thống Vật phẩm (`KItem` + `KInventory`)

`KInventory` là **lưới 2D** (có chiều rộng và chiều cao), vật phẩm chiếm nhiều ô khác nhau tùy kích thước (`nWidth` × `nHeight`). Không có slot cố định — vật phẩm đặt vào ô trống bất kỳ.

Các loại vật phẩm (`nItemGenre`): vũ khí, giáp, thuốc, đá ma pháp, nhiệm vụ, v.v.  
Vật phẩm có thể **stack** (chồng nhiều cái vào một ô), có thời hạn sử dụng (`nExpireTime`), và có thể **khóa** để không bán/không giao dịch/không rơi.

### 5.4 Hệ thống Nhiệm vụ (`KPlayerTask`)

Quest được lưu trong hai mảng:
- `szSave[MAX_TASK][16]` — biến **được lưu vào database** (tiến độ quest cố định)
- `szClear[MAX_TEMP_TASK][16]` — biến **tạm thời**, reset khi đăng xuất

Hệ thống quest hoạt động dựa trên **Lua script**: NPC gọi script, script kiểm tra/đặt biến `szSave`, rẽ nhánh theo điều kiện. Điều này có nghĩa là muốn tạo quest mới chỉ cần viết script Lua, không cần sửa C++.

Hệ thống **Mission** (`KMission`) là các sự kiện tập thể: nhiều người chơi tham gia vào một Mission, mỗi Mission có danh sách người chơi và NPC tham gia, và có timer riêng.

### 5.5 Hệ thống PK (`KPlayerPK`)

Ba chế độ PK:
- **Normal PK** (`m_nNormalPKFlag`): PK tự do, bật/tắt được
- **Enmity PK** (仇杀/Cừu sát): PK mục tiêu cụ thể, có thời gian hiệu lực (`PK_ANMITY_TIME`, `PK_ANMITY_CLOSE_TIME`)
- **Exercise PK** (切磋/Thủ Xả): PK luyện tập, không có hậu quả

`m_nPKValue`: Chỉ số PK tăng khi giết người, giảm theo thời gian. PKValue cao → bị NPC quan binh truy sát.

### 5.6 Hệ thống Môn phái (`KPlayerFaction`)

- Mỗi nhân vật thuộc một môn phái (faction)
- Môn phái chia theo **Phe/Doanh trại** (Camp): thiện/ác
- Tham gia môn phái mở khóa các kỹ năng đặc thù của môn phái đó
- Nhân vật có thể **chuyển môn phái** nhưng bị giới hạn số lần

### 5.7 Hệ thống Bang hội (`KPlayerTong` + Rainbow server)

Bang hội (Tông/Tong) gồm:
- Tên bang, biệt hiệu trong bang (`m_szAgname`)
- Level bang hội (tối đa 10)
- Chức vụ: Bang chủ (Master), Phó bang (Director), Quản lý (Manager)
- Quỹ bang (`m_dwMoney`), Cống hiến (`m_dwTotalEff`)
- Tuyển thêm thành viên (`m_bRecruit`)

Dữ liệu bang hội được lưu và xử lý bởi **Rainbow server** (không phải GameServer), Rainbow có DB riêng trong thư mục `TongDB/`.

---

## 6. Giao thức truyền thông (Protocol)

### 6.1 Cấu trúc gói tin

File `SwordOnline/Headers/KProtocolDef.h` định nghĩa hai chiều giao tiếp:
- `s2c_PROTOCOL`: **Server → Client** (sync trạng thái game, kết quả action)
- `c2s_PROTOCOL`: **Client → Server** (yêu cầu từ người chơi)

Gói tin nhỏ hơn `g_nGlobalProtocolType = 31` bytes được xử lý đặc biệt (micropacket). Gói lớn hơn có header kèm độ dài (`m_wLength`).

Tất cả struct gói tin đều dùng `#pragma pack(1)` — **không có padding**, mỗi byte đều có ý nghĩa.

### 6.2 Một số giao thức quan trọng

**Đồng bộ trạng thái người chơi:**
```
s2c_synccurplayer      — Sync toàn bộ thông tin nhân vật hiện tại của mình
s2c_syncplayer         — Sync người chơi khác vào tầm nhìn (đầy đủ)
s2c_syncplayermin      — Sync người chơi khác (rút gọn, dùng khi đông người)
s2c_synccurplayerskill — Sync danh sách kỹ năng
```

**Đồng bộ NPC:**
```
s2c_syncnpc            — Sync NPC đầy đủ
s2c_syncnpcmin         — Sync NPC rút gọn
s2c_npcwalk/npcrun     — NPC đang đi/chạy
s2c_npchurt/npcdeath   — NPC bị thương/chết
s2c_npcremove          — NPC ra khỏi tầm nhìn
```

**Đăng nhập:**
```
s2c_gatewayverify      — Server xác nhận kết nối
s2c_accountlogin       — Kết quả đăng nhập tài khoản
s2c_gamelogin          — Vào được game server
s2c_syncend            — Kết thúc đồng bộ ban đầu, bắt đầu chơi
```

### 6.3 Xử lý giao thức phía client

`KProtocolProcess` (`Core/Src/KProtocolProcess.cpp`) là dispatcher trung tâm: một mảng function pointer `ProcessFunc[]` ánh xạ mỗi loại gói tin tới một hàm xử lý cụ thể. Khi client nhận được byte đầu tiên của gói, nó tra bảng này và gọi handler tương ứng.

Ví dụ: `ProcessFunc[s2c_syncplayer] = &KProtocolProcess::SyncPlayer` — mỗi khi có gói sync người chơi đến, hàm `SyncPlayer` được gọi để cập nhật vị trí/trang phục/trạng thái.

---

## 7. Phía Client — S3Client

### 7.1 Luồng khởi động

```
WinMain (S3Client.cpp)
  → Nạp Represent2.dll hoặc Represent3.dll (dynamic link)
  → Khởi tạo g_pRepresentShell (render engine)
  → Khởi tạo g_pCoreShell (Core logic)
  → UiInit() → UiStart() → Game loop (18 FPS)
```

### 7.2 Hệ thống UI

UI được tổ chức thành 3 tầng:

**Tầng 1: Widget nguyên tử (`Wnd*`)** — tái sử dụng được:
```
WndButton, WndEdit, WndList, WndScrollBar,
WndImage, WndText, WndBorder, WndToolBar ...
```

**Tầng 2: Màn hình game (`UiCase/Ui*.cpp`)** — mỗi màn hình là một class:
```
UiLogin          — Màn hình đăng nhập
UiSelServer      — Chọn server
UiSelPlayer      — Chọn nhân vật
UiGame           — HUD trong game
UiPlayerBar      — Thanh máu/mana/stamina
UiSkills         — Cửa sổ kỹ năng
UiItem           — Túi đồ
UiChatRoom       — Chat phòng
UiTrade/UiShop   — Giao dịch / Mua bán NPC
UiTongManager    — Quản lý bang hội
UiMiniMap        — Bản đồ thu nhỏ
UiTaskNote       — Sổ nhiệm vụ
...
```

**Tầng 3: INI layout** — mỗi màn hình có file `.ini` cùng tên định nghĩa vị trí, kích thước, hình ảnh của từng widget. Ví dụ `UiLogin.ini` xác định button đăng nhập ở đâu, textbox tên tài khoản bao nhiêu pixel.

**Ví dụ luồng tạo UI:**
```
UiShell.h khai báo hàm: UiInit(), UiStart(), UiPaint(), UiHeartBeat()
  → UiHeartBeat() gọi tất cả màn hình đang mở
  → Mỗi màn hình có Breathe() (update logic) và PaintWindow() (vẽ)
  → Widget WndProc() xử lý click/keyboard
```

### 7.3 Rendering

`Represent2.dll` / `Represent3.dll` là module render:
- Được load động tại runtime (không link tĩnh)
- Interface `iRepresentShell` tách biệt hoàn toàn logic game và render
- `Represent3` dùng cho render 3D nâng cao; `Represent2` là phiên bản 2D cũ

---

## 8. Hệ thống Server đa tầng

### 8.1 Bishop — Điều phối viên

Bishop là server **đặc biệt**: nó không xử lý game logic mà chỉ giám sát và điều phối. Mọi server khác kết nối ngược lên Bishop để đăng ký tồn tại.

- Kết nối tới Account Server (S3AccServer) bằng username/password cấu hình tại `Application.cpp:78`
- Quản lý danh sách GameServer đang chạy (`GameServer.h`)
- Điều phối người chơi tới GameServer có tải thấp nhất (load balancing)
- Monitor trạng thái tất cả server

### 8.2 S3Relay — Proxy trung gian

S3Relay nhận kết nối TCP từ client rồi chuyển tiếp tới GameServer phù hợp. Lý do có tầng này:
1. Ẩn địa chỉ IP thật của GameServer khỏi client
2. Có thể chuyển người chơi sang GameServer khác (server exchange) mà không ngắt kết nối TCP

Cấu hình `relay_config.ini` định nghĩa các kênh relay và GameServer mà nó proxy tới.

### 8.3 GameServer — Logic chính

`KSwordOnLineSever` (trong `KSOServer.h`) là class chính của GameServer:
- Sử dụng module `Core` (server build) để chạy toàn bộ game logic
- Kết nối tới Heaven (xác thực), Goddess (DB), Rainbow (chat)
- Cấu hình trong `ServerCfg.ini`:
  ```ini
  [Gateway] Ip=127.0.0.1  Port=5632   ; Relay server
  [Database] Ip=127.0.0.1 Port=5001   ; Goddess
  [Chat] Ip=127.0.0.1     Port=5004   ; Rainbow
  [GameServer] Port=6666              ; Cổng lắng nghe của chính nó
  [Overload] MaxPlayer=1000           ; Tối đa người chơi
  ```

`KTransferUnit` xử lý chuyển người chơi giữa các GameServer (ví dụ khi vào bản đồ khác do server khác quản lý).

### 8.4 Goddess — Cơ sở dữ liệu nhân vật

Goddess là database server dựa trên **Berkeley DB** (không phải MySQL/PostgreSQL). Nó cung cấp API đơn giản:
```cpp
void *GetRoleInfo(char* pRoleBuffer, char* strUser, int &nBufLen);
int   SaveRoleInfo(char* pRoleBuffer, const char* strUser, ...);
int   GetRoleListOfAccount(char* szAccountName, ...);
bool  DeleteRole(const char* strUser);
```

Toàn bộ dữ liệu nhân vật được serialize thành byte buffer rồi ghi vào Berkeley DB key-value store. Không có schema SQL — dữ liệu là binary blob với format cố định.

Goddess còn có cơ chế **backup tự động** (`StartBackupTimer`), có thể trigger backup thủ công.

### 8.5 Heaven — Xác thực tài khoản

Heaven là server đăng nhập. Khi người chơi nhập username/password:
1. Client gửi request tới Heaven (qua Relay)
2. Heaven kiểm tra tài khoản trong database riêng
3. Trả về token/ticket xác thực
4. Client dùng ticket này để vào GameServer

Heaven được implement dưới dạng DLL với interface COM (`IServer`, `IIOCPServer`).

### 8.6 Rainbow — Chat và Bang hội

Rainbow quản lý:
- **Kênh chat**: thế giới, khu vực, nhóm, bang hội, riêng tư
- **Bang hội (Tông)**: lưu DB riêng tại `TongDB/`, `TongMemberDB/`
- **Bạn bè**: lưu DB riêng tại `dbfriend/`
- **Relay chat**: nhận từ GameServer rồi broadcast tới các client khác

Khác với GameServer, Rainbow sử dụng multi-thread với mutex (`Lock.h`, `LockMRSW.h`) để xử lý đồng thời.

### 8.7 Mô hình mạng: IOCP

Tất cả server đều dùng **Windows IOCP** (`IOCompletionPort.h`, `SocketServer.h`):
- I/O bất đồng bộ hiệu năng cao
- Thread pool xử lý completion event
- Không tạo thread riêng cho mỗi kết nối

---

## 9. Lua Scripting

### 9.1 Vai trò của Lua

Lua được dùng cho:
- **Quest/Nhiệm vụ**: logic rẽ nhánh, kiểm tra điều kiện, trao thưởng
- **Dialogue NPC**: NPC nói gì, hiện lựa chọn gì, trigger event gì
- **AI NPC đặc biệt**: NPC boss có behavior phức tạp
- **Sự kiện thế giới**: spawn quái định kỳ, mở/đóng dungeon

### 9.2 Cách hoạt động

`LuaFuns.cpp` (bị bọc bởi `#ifdef USEOLD`) đăng ký C function vào Lua runtime:

```cpp
CORE_API TLua_Funcs SysFuns[] = {
    {"Wait",             LuaWait},        // Dừng script N frame
    {"Goto",             LuaGotoLabel},   // Nhảy tới label
    {"MessageBox",       LuaMessageBox},  // Hiện hộp thoại
    {"SendMessage",      LuaSendMessage}, // Gửi message tới game
    {"SetValue",         LuaSetValue},    // Đặt giá trị biến quest
    {"GetValue",         LuaGetValue},    // Đọc biến quest
    ...
};
```

Script Lua chạy kiểu **coroutine** (step-by-step): `KStepLuaScript` cho phép script dừng giữa chừng (`Wait`) rồi tiếp tục frame sau, giống như async/await.

### 9.3 Vị trí script

Script Lua nằm trong thư mục `settings/` của GameServer và trong thư mục `script/` của GameServer. Khi NPC/item/event kích hoạt, engine tìm file script theo ID và chạy hàm tương ứng.

---

## 10. Luồng hoạt động từ Đăng nhập đến Vào game

### Bước 1: Kết nối ban đầu
```
Client mở chương trình
  → Kết nối TCP tới S3Relay
  → Relay gửi: s2c_gatewayverify (xác nhận kết nối)
```

### Bước 2: Đăng nhập tài khoản
```
Client nhập user/pass tại UiLogin
  → c2s_accountlogin gửi tới Relay
  → Relay chuyển tới Heaven
  → Heaven xác thực, trả về s2c_accountlogin (thành công/thất bại)
```

### Bước 3: Chọn nhân vật
```
s2c_syncrolelist → Client nhận danh sách nhân vật → UiSelPlayer hiển thị
Client chọn nhân vật → c2s_gamelogin
  → Relay → Heaven → GameServer
  → s2c_gamelogin thành công
```

### Bước 4: Đồng bộ ban đầu
```
GameServer load dữ liệu nhân vật từ Goddess
GameServer gửi hàng loạt gói:
  s2c_synccurplayer      — Thông tin nhân vật của mình
  s2c_synccurplayerskill — Tất cả kỹ năng
  s2c_syncworld          — Thông tin bản đồ hiện tại
  s2c_syncplayer × N     — Các người chơi trong tầm nhìn
  s2c_syncnpc × N        — Các NPC trong tầm nhìn
  s2c_syncend            — KẾT THÚC đồng bộ → vào game
```

### Bước 5: Gameplay loop
```
Client 18 FPS:
  → Nhận input chuột/bàn phím
  → Gửi c2s_* (di chuyển, tấn công, dùng kỹ năng...)
  → Nhận s2c_* (update trạng thái thế giới)
  → Core cập nhật logic
  → Represent render frame
  → UI Breathe() cập nhật HUD
```

---

## 11. Hướng dẫn Build

### Yêu cầu
- Visual Studio 2022 (bắt buộc)
- Windows SDK (đã kèm trong VS)
- DirectX 9 SDK headers (nằm ở `dx9dsdk/include/`)

### Các bước

**Bước 1**: Mở solution
```
SwordOnline\Sources\JXAll.sln
```

**Bước 2**: Cấu hình credentials Bishop trước khi build server  
Mở `Sources/MultiServer/Bishop/Application.cpp`, tìm dòng khoảng 78:
```cpp
g_theSmartClient.Open(m_theNetwork.GetAccSvrIP(), m_theNetwork.GetAccSvrPort(), 0, "txjx", "1111");
```
Thay `"txjx"` và `"1111"` bằng username/password Account Server của bạn.

**Bước 3**: Chọn build configuration và build theo thứ tự

| Thứ tự | Project | Configuration |
|---|---|---|
| 1 | Engine | Debug |
| 2 | LuaLibDll, JpgLib, KMp3Lib | Debug |
| 3 | FilterText_StaticLib, ExpandPackageStaticLib | Debug |
| 4 | Core | **Server Debug** (cho server) |
| 5 | Common | Debug |
| 6 | GameServer, Goddess, Heaven, Rainbow, Bishop, S3Relay | Debug |
| 7 | Core | **Client Debug** (cho client) |
| 8 | Represent2, Represent3 | Debug |
| 9 | Game (S3Client) | Client Debug |

> **Lưu ý**: Core phải được build 2 lần — một lần **Server Debug** cho các server, một lần **Client Debug** cho client. Hai build này tạo ra lib khác nhau tại thư mục `ClientDebug/` và `ServerDebug/` trong thư mục Core.

**Bước 4**: Output  
File EXE/DLL sau khi build nằm ở `SwordOnline/bin/`:
- Server: `bin/server/` hoặc `bin/multiserver/`
- Client: `bin/client/`

### Thứ tự chạy server
```
1. Goddess.exe    (DB server, cổng 5001)
2. Heaven.exe     (Login server)
3. Rainbow.exe    (Chat server, cổng 5004, 5005)
4. GameServer.exe (Game logic, cổng 6666)
5. S3Relay.exe    (Proxy, cổng 5632)
6. Bishop.exe     (Manager)
```

---

## 12. Branch hiện tại: ui_lien_tram

Branch `ui_lien_tram` đang phát triển tính năng **UI Liên Trạm** (连站/Trạm dịch) — hệ thống hiển thị combo di chuyển liên tiếp qua các trạm.

### Những thay đổi đã có

**`UiRankLienTram`** (`S3Client/Ui/UiCase/UiRankLienTram.cpp/.h`):  
Class UI mới hiển thị số lần liên trạm hiện tại dưới dạng số có 3 chữ số (hàng trăm, hàng chục, hàng đơn vị — mỗi chữ số là một `KWndImage` riêng để dùng ảnh sprite số).

**`S3Client.cpp`**:  
Thêm cơ chế **pending request** thread-safe để gọi `UiRankLienTram::OpenWindow()` từ network thread an toàn:
```cpp
// Pending request value for LienTram UI; LONG_MIN means no request
static volatile LONG g_PendingLienTram = LONG_MIN;
```
`InterlockedExchange` đảm bảo atomic update giữa thread nhận gói tin và game loop thread.

**`KProtocolProcess.cpp`** và **`KNpc.cpp/KNpc.h`**:  
Thêm xử lý giao thức mới để nhận thông tin liên trạm từ server và kích hoạt UI.

### Cách thêm tính năng UI mới (học từ ui_lien_tram)

1. Tạo class mới kế thừa từ `KWndWindow` hoặc `KWndShowAnimate`
2. Tạo file `.ini` cùng tên để định nghĩa layout
3. Thêm `#include` vào `S3Client.cpp` và gọi `OpenWindow()` khi cần
4. Nếu cần gọi từ network thread → dùng pattern `InterlockedExchange` như trong `g_PendingLienTram`
5. Thêm handler vào `KProtocolProcess` nếu cần nhận dữ liệu từ server

---

*Tài liệu này được tổng hợp từ việc đọc trực tiếp source code trong repository. Các comment gốc trong code chủ yếu bằng tiếng Trung, một phần đã được bổ sung bằng tiếng Anh/Việt bởi các contributor sau này.*
