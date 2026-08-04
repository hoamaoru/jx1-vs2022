# Shadow Skill Guide — Skill có hiệu ứng bóng mờ khi di chuyển

> Mục đích: Hướng dẫn tạo skill có bóng mờ (afterimage) đi theo nhân vật khi di chuyển, kèm tuỳ chọn tăng tốc độ (phẳng hoặc tăng dần), chỉ bằng cách sửa data (`.lua` / `.txt` / `.ini`) — không cần đụng code C++, trừ khi muốn thêm một *dạng hiệu ứng hoàn toàn mới*.

---

## 1. Tính năng này làm gì

| Attribute | Hiệu ứng | Trạng thái |
|---|---|---|
| `walkrunshadow` | Bóng mờ (afterimage) theo sau khi đi/chạy/nhảy | Có sẵn trong engine |
| `fastwalkrun_p` | Tăng tốc % **ngay lập tức**, giữ nguyên tới hết thời lượng | Có sẵn trong engine |
| `fastwalkrun_ramp_p` | Tăng tốc **tuyến tính từ 0 lên mức tối đa** trong vài giây đầu, sau đó giữ nguyên | Mới thêm (xem mục 7) |

Ví dụ đã áp dụng cả 3: skill **"Tĩnh Tâm Quyết 2"** (SkillId `1566`, môn Thiên Vương Bang) — kích hoạt thì tốc độ tăng dần trong ~1 giây tới mức tối đa, đồng thời có bóng mờ theo sau trong suốt thời gian hiệu lực.

## 2. Cơ chế hoạt động (tóm tắt kỹ thuật)

**Phía server:**
- Khi skill được kích hoạt, framework buff chung (`KNpc::SetStateSkillEffect`) gọi `KNpcAttribModify` xử lý từng attribute trong danh sách của skill, ghi state lên `KNpc` (`m_WalkRun.nTime` cho `walkrunshadow`, `m_SpeedRush` cho `fastwalkrun_ramp_p`).
- Mỗi tick (`KNpc::ProcessState`), các state này được đếm ngược / cập nhật dần.
- Khi hết thời lượng, engine tự gọi lại `ModifyAttrib` với giá trị **âm** (đúng giá trị lúc apply) để hoàn tác — không cần dọn dẹp thủ công.

**Phía client:**
- Khi `m_WalkRun.nTime > 0`, lúc đi/chạy/nhảy (`OnRun`/`OnWalk`/`DoJump`) client gọi `KNpcRes::SetBlur(TRUE)`.
- Mỗi khung hình vẽ, nếu `m_nBlurState == TRUE`, `KNpcBlur` ghi lại 1 "ảnh chụp" hình dạng hiện tại vào vòng đệm 7 khung (`MAX_BLUR_FRAME`), rồi mờ dần theo thời gian.
- **Khoảng cách giữa các bóng** = số khung hình giữa mỗi lần chụp (`m_dwInterval`) × tốc độ di chuyển thực tế tại thời điểm đó — vì vậy khi skill có `fastwalkrun_ramp_p`, bóng sẽ giãn ra dần đúng theo tốc độ đang tăng.

## 3. File dữ liệu cần sửa & vị trí (phải đồng bộ ở mọi nơi)

| File | Vai trò | Số bản cần đồng bộ |
|---|---|---|
| `script/skill/<mon_phai>.lua` (vd `tianwang.lua`) | Bảng giá trị theo cấp của skill (curve %, thời lượng...) | 7 bản: `bin/server/script`, `bin/server/debug`, `` bin/server/debug - Copy ``, `bin/server/x64/debug`, `bin/client/debug`, `bin/client/release`, `bin/Tools/exported_other` (script client giống nhau, script server giống nhau — 2 nhóm) |
| `settings/Skills.txt` | Khai báo skill (tên, id, icon, cooldown, trỏ tới file .lua ở trên) | 6 bản: `bin/client/debug`, `bin/client/release`, `` bin/server/debug - Copy ``, `bin/server/debug`, `bin/server`, `bin/server/x64/debug` |
| `SwordOnline/Sources/MultiServer/GameServer/settings/MagicDesc.ini`<br>`SwordOnline/Sources/S3Client/settings/MagicDesc.ini` | Mô tả text hiển thị cho từng attribute (tooltip) | 2 bản (đây là bản gốc trong `Sources/`, không phải bản build) |

> Các file `.lua` và `Skills.txt` nằm dưới `bin/` **không được git track** (bị `.gitignore` loại trừ, trừ vài file đã force-add từ trước) — sửa xong nhớ backup thủ công nếu cần lịch sử.
> Toàn bộ các file trên đều là encoding **ISO-8859-1** (không phải UTF-8) — xem cảnh báo encoding trong `CLAUDE.md` trước khi sửa bằng tay, đặc biệt nếu có ký tự tiếng Việt/Trung trong file.

## 4. Các bước thêm 1 skill mới có bóng mờ (+ tuỳ chọn tăng tốc)

1. **Tìm skill mẫu cùng môn phái** trong `Skills.txt` để clone (đảm bảo `SkillStyle`, `CharClass`, cooldown... hợp lý).
2. **Chọn SkillId mới** (số chưa dùng — tra nhanh: `awk -F'\t' 'NR>1{if($3+0>m)m=$3+0}END{print m}' Skills.txt` rồi lấy số tiếp theo).
3. **Thêm entry vào file `.lua` môn phái**, ví dụ trong `tianwang.lua`:
   ```lua
   ten_key_moi={ --Ten skill moi
       attackratingenhance_p={{{1,45},{20,400}},{{1,18*120},{20,18*180}}},
       walkrunshadow={
           [1]={{1,1},{20,1}},              -- luôn bật (1) từ cấp 1-20
           [2]={{1,18*120},{20,18*180}}     -- thời lượng: 120s (cấp1) -> 180s (cấp20)
       },
       fastwalkrun_ramp_p={{{1,9},{20,40}},{{1,18*120},{20,18*180}},{{1,18},{20,18}}},
       -- curve 1: % tối đa (9% -> 40% theo cấp)
       -- curve 2: thời lượng buff (giống walkrunshadow)
       -- curve 3: số tick để đạt max (18 tick = 1 giây, không đổi theo cấp)
       skill_cost_v={{{1,5},{20,20}}}
   },
   ```
   Mỗi curve có dạng `{{cấp_A, giá_trị_A}, {cấp_B, giá_trị_B}}` — engine tự nội suy tuyến tính giữa các mốc cấp đã khai báo. `18` = `GAME_FPS` (số tick/giây của server).
4. **Thêm dòng skill vào `Skills.txt`**: copy dòng mẫu, sửa `SkillName`, `SkillId`, và các cột `LvlSetting/LvlData` (mỗi cặp = tên attribute + tên key vừa tạo ở bước 3):
   ```
   LvlSetting1=attackratingenhance_p   LvlData1=ten_key_moi
   LvlSetting2=walkrunshadow           LvlData2=ten_key_moi
   LvlSetting3=fastwalkrun_ramp_p      LvlData3=ten_key_moi
   LvlSetting4=skill_cost_v            LvlData4=ten_key_moi
   ```
5. **(Tuỳ chọn) thêm mô tả tooltip** cho attribute mới trong `MagicDesc.ini` nếu chưa có (attribute có sẵn như `walkrunshadow`, `fastwalkrun_p` đã có mô tả rồi, không cần thêm lại).
6. **Copy file đã sửa ra tất cả các bản** liệt kê ở mục 3.
7. **Khởi động lại GameServer** (và Game.exe nếu muốn test client) — không cần build lại vì chỉ sửa data.

## 5. Cấp skill tự động cho nhân vật

Dùng hàm Lua **`LearnSkill(skillId, level)`** (mới thêm, wrap `KSkillList::Add` — tạo skill nếu nhân vật chưa có, không hạ cấp nếu đã có cấp cao hơn). Gọi trong bất kỳ script nào chạy đúng lúc bạn muốn cấp skill:

```lua
LearnSkill(1566, 1);   -- cấp skill 1566 ở cấp 1
```

- **Lúc đăng nhập**: thêm vào hàm `BatDauVaoGame1()` trong `bin/server/script/player/playerlogin.lua` (đây là script chạy mỗi lần nhân vật vào game, đã có sẵn logic phát item tân thủ tương tự).
- **Lúc lên cấp / hoàn thành nhiệm vụ**: gọi trong script quest/task tương ứng.

> Lưu ý: `SetSkillLevel()` và `IncSkill()` (2 hàm Lua có sẵn từ trước) **không** cấp được skill hoàn toàn mới — cả hai đều yêu cầu nhân vật đã có skill đó rồi mới chỉnh cấp. Chỉ `LearnSkill()` tạo mới được từ đầu.

## 6. Tinh chỉnh hiệu ứng bóng mờ (số lượng, độ mờ, khoảng cách)

Các hằng số nằm trong `SwordOnline/Sources/Core/Src/KNpcRes.h` (định nghĩa) và `KNpcRes.cpp` (khởi tạo) — **sửa xong phải build lại client (Core + S3Client)**, không áp dụng ngay như sửa data:

| Hằng số | Vị trí | Ý nghĩa | Giá trị hiện tại |
|---|---|---|---|
| `MAX_BLUR_FRAME` | `KNpcRes.h` | Số bóng tối đa hiển thị cùng lúc | 7 |
| `START_BLUR_ALPHA` | `KNpcRes.h` | Độ đậm (alpha) của bóng mới nhất | 128 |
| `BLUR_ALPHA_CHANGE` | `KNpcRes.h` | Tốc độ mờ dần mỗi lần cập nhật | 8 |
| `m_dwInterval` | `KNpcRes.cpp`, `KNpcBlur::KNpcBlur()` | Số khung hình giữa mỗi lần "chụp" bóng mới → quyết định khoảng cách giữa các bóng | 4 (mặc định gốc là 3, đã tăng +30%) |

Đổi `m_dwInterval` sẽ ảnh hưởng **tất cả** skill dùng `walkrunshadow` (không tách riêng theo skill được nếu không sửa sâu hơn vào chuỗi truyền dữ liệu `KMagicAttrib → m_WalkRun → SetBlur → KNpcRes → KNpcBlur`).

## 7. File source liên quan (khi cần sửa code, không chỉ data)

| File | Vai trò |
|---|---|
| `SwordOnline/Sources/Core/Src/KMagicAttrib.h` | Khai báo enum attribute (`magic_walkrunshadow`, `magic_fastwalkrun_p`, `magic_fastwalkrun_ramp_p`...) |
| `SwordOnline/Sources/Core/Src/KMagicDesc.cpp` | Bảng tên chuỗi tương ứng từng attribute (dùng để tra `MagicDesc.ini`) |
| `SwordOnline/Sources/Core/Src/KNpcAttribModify.h/.cpp` | Hàm xử lý khi attribute được áp dụng/thu hồi (`WalkRunShadow`, `FastWalkRunP`, `FastWalkRunRampP`) |
| `SwordOnline/Sources/Core/Src/KNpc.h` | Khai báo state trên mỗi `KNpc` (`m_WalkRun`, `m_SpeedRush`) |
| `SwordOnline/Sources/Core/Src/KNpc.cpp` | Tick state mỗi frame (`ProcessState`), set cờ blur lúc di chuyển (`OnRun`/`OnWalk`/`DoJump`) |
| `SwordOnline/Sources/Core/Src/KNpcRes.h/.cpp` | Vẽ hiệu ứng bóng mờ phía client (`KNpcBlur`, `SetBlur`, `CreateBlur`) |
| `SwordOnline/Sources/Core/Src/ScriptFuns.cpp` | Các hàm Lua-binding (`LearnSkill`, `SetSkillLevel`, `IncSkill`...) |
| `SwordOnline/Sources/Core/Src/KPlayer.cpp` | `LaunchPlayer()` — nơi engine chạy `playerlogin.lua` mỗi lần đăng nhập |

## 8. Build & áp dụng

- **Chỉ sửa `.lua` / `.txt` / `.ini`**: có hiệu lực ngay khi server/client load lại data — thường chỉ cần khởi động lại `GameServer.exe`, không cần mở Visual Studio.
- **Sửa gì trong `SwordOnline/Sources/Core/Src/*.h`/`*.cpp`** (thêm attribute mới, hàm Lua mới, đổi hằng số render...): phải mở `SwordOnline\Sources\JXAll.sln` trong VS2022, build lại **Core** trước, sau đó **GameServer** (server) và **S3Client** (client) mới nhận được thay đổi (xem thứ tự build đầy đủ trong `CLAUDE.md`).
