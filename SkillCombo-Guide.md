# Skill Combo Guide — Hệ thống combo skill theo nhánh vũ khí

> Mục đích: Hướng dẫn cách xem, chỉnh sửa và thêm mới các chuỗi combo skill (tăng % sát thương khi thi triển đúng thứ tự) mà không cần sửa code C++.

---

## 1. Tính năng này làm gì

Khi người chơi thi triển đúng thứ tự các skill trong 1 chuỗi (thường là 5 bậc: 10 → 30 → 60 → 80 → 150 của cùng 1 nhánh vũ khí), mỗi skill trong chuỗi sẽ được cộng thêm % sát thương riêng, tăng dần theo từng bậc.

Ví dụ nhánh Đao của Thiên Vương:

| Bậc | Skill | Bonus |
|---|---|---|
| 10 | Kinh Lôi Trảm | +30% |
| 30 | Bát Phong Trảm | +50% |
| 60 | Vô Tâm Trảm | +100% |
| 80 | Phá Thiên Trảm | +500% |
| 150 | Hào Hùng Trảm | +800% |

## 2. Luật chơi (game rule)

- **Đúng thứ tự, liên tiếp**: phải thi triển đúng skill kế tiếp trong chuỗi thì mới lên bậc và tăng bonus. Đánh sai skill (không thuộc chuỗi) → combo kết thúc ngay (`Combo ended`).
- **3 giây để lên bậc kế tiếp**: từ lúc đạt 1 bậc, có 3 giây để thi triển skill kế tiếp trong chuỗi. Quá 3 giây mà chưa lên bậc mới → combo kết thúc.
- **3 giây để "giữ" bonus ở bậc hiện tại**: khi đạt 1 bậc, đồng hồ 3s để **lặp lại chính skill đó** cũng được tính. Lặp lại đúng skill trong 3s vẫn giữ nguyên % bonus của bậc đó (không lên bậc, không reset đồng hồ — đồng hồ này là cố định từ lúc đạt bậc, lặp lại không kéo dài thêm). Sau 3s kể từ lúc đạt bậc, combo kết thúc dù có lặp lại hay không.
- Skill mở đầu chuỗi (bậc 10) luôn dùng độc lập được — thi triển nó bất cứ lúc nào cũng tự động bắt đầu 1 lượt combo mới (+30%), kể cả khi vừa kết thúc 1 combo khác.

## 3. File dữ liệu: `settings/SkillCombos.txt`

Đây là file dạng bảng, tab-delimited, giống hệt `Skills.txt` — sửa file này là đủ, **không cần build lại code**.

### Vị trí file (phải sửa đồng thời ở tất cả các nơi đang chạy)

| Thư mục | Dùng cho |
|---|---|
| `bin/server/settings/SkillCombos.txt` | GameServer (bản gốc) |
| `bin/server/debug/settings/SkillCombos.txt` | GameServer (Server Debug) |
| `bin/server/x64/debug/settings/SkillCombos.txt` | GameServer (x64 Debug) |
| `bin/client/debug/settings/SkillCombos.txt` | Game.exe (Client Debug) |
| `bin/client/release/settings/SkillCombos.txt` | Game.exe (Client Release) |

> Lưu ý: thư mục nào có tên kèm `- Copy` (ví dụ `bin/server/debug - Copy/`) là bản sao lưu thủ công, **không track vào git** và không bắt buộc phải đồng bộ, nhưng nếu bạn chạy server/client từ đó thì vẫn phải tự copy file qua.

### Cấu trúc cột

| Cột | Ý nghĩa |
|---|---|
| `ComboId` | Số thứ tự, chỉ để dễ đọc, không dùng trong logic |
| `ComboName` | Tên gợi nhớ (nên viết không dấu để tránh lỗi encoding) |
| `Step1SkillId` / `Step1Bonus` | SkillId + % bonus của bậc 1 (thấp nhất) |
| `Step2SkillId` / `Step2Bonus` | Bậc 2 |
| `Step3SkillId` / `Step3Bonus` | Bậc 3 |
| `Step4SkillId` / `Step4Bonus` | Bậc 4 |
| `Step5SkillId` / `Step5Bonus` | Bậc 5 (cao nhất) |

Dữ liệu hiện có (3 nhánh Thiên Vương):

```
ComboId	ComboName	Step1SkillId	Step1Bonus	Step2SkillId	Step2Bonus	Step3SkillId	Step3Bonus	Step4SkillId	Step4Bonus	Step5SkillId	Step5Bonus
1	Thien Vuong - Dao	34	30	37	50	32	100	322	500	1058	800
2	Thien Vuong - Thuong	30	30	35	50	41	100	323	500	1060	800
3	Thien Vuong - Chuy	29	30	31	50	324	100	325	500	1059	800
```

## 4. Thêm 1 nhánh combo mới (môn phái khác / vũ khí khác)

1. Tra SkillId của các skill theo từng bậc trong `bin/server/settings/Skills.txt` (cột `CharClass` = môn phái, cột `EqtLimit` = loại vũ khí, cột `ReqLevel` = bậc yêu cầu). File này dùng encoding `TCVN5712-1`, không phải UTF-8 — dùng `iconv -f TCVN5712-1 -t UTF-8` nếu cần đọc tên skill tiếng Việt.
2. Thêm 1 dòng mới vào `SkillCombos.txt` (tăng `ComboId` lên 1), điền đủ 5 cặp SkillId/Bonus theo đúng thứ tự bậc thấp → cao.
3. Copy file đã sửa ra **tất cả** các thư mục ở mục 3.
4. Khởi động lại GameServer.exe (và Game.exe nếu muốn test cả console log phía client).

## 5. Chuỗi combo ngắn hơn 5 bậc (3 hoặc 4 bậc)

Không cần sửa code. Chỉ cần **để trống** các cột `SkillId`/`Bonus` của các bậc không dùng tới (không điền gì, hoặc để 0) — hệ thống tự hiểu SkillId = 0 nghĩa là "hết chuỗi tại đó".

Ví dụ chuỗi chỉ có 3 bậc:

```
4	Ten combo 3 buoc	111	30	222	50	333	100			
```

Sau khi đánh xong bậc 3 (skill `333`), người chơi vẫn được lặp lại skill đó trong 3s để giữ +100%, nhưng thi triển bất kỳ skill nào khác (kể cả đúng bậc 4 lý thuyết) sẽ không được nhận diện là tiếp nối — vì cột đó rỗng — nên combo kết thúc bình thường.

## 6. Xem log combo lúc test

Khi chạy `Game.exe` (mở console tự động kèm theo, xem mục 7), mỗi lần combo được tính sẽ in ra:

```
[22:13:41.191] [COMBO] Damage bonus: +800%
[22:13:44.303] [COMBO] Combo ended.
```

## 7. Console debug tự mở khi chạy Game.exe

`Game.exe` tự động mở kèm 1 cửa sổ console (terminal) để hiện log combo — không cần thêm cờ dòng lệnh. Đóng cửa sổ đó bất cứ lúc nào không ảnh hưởng tới game (không tự tắt game).

## 8. File source liên quan (nếu cần đổi luật chơi, không chỉ đổi số liệu)

| File | Vai trò |
|---|---|
| `SwordOnline/Sources/Core/Src/KNpc.h` | Khai báo state theo dõi combo trên mỗi `KNpc` |
| `SwordOnline/Sources/Core/Src/KNpc.cpp` | `LoadSkillComboSetting()` (đọc file), `UpdateComboBonus()` (luật combo), áp dụng bonus trong `AppendSkillEffect()` |
| `SwordOnline/Sources/Core/Src/KCore.cpp` | Gọi `LoadSkillComboSetting()` lúc khởi động |
| `SwordOnline/Sources/Core/Src/CoreUseNameDef.h` | Đường dẫn file (`SKILLCOMBO_SETTING_FILE`), mã thông báo (`enumMSG_ID_COMBO_BONUS`, `enumMSG_ID_COMBO_END`) |
| `SwordOnline/Sources/Core/Src/KProtocolProcess.cpp` | Client nhận thông báo, in ra console kèm timestamp |
| `SwordOnline/Sources/S3Client/S3Client.cpp` | Client tự mở console khi khởi động |

Hai hằng số điều chỉnh nhịp độ combo nằm trong `KNpc.cpp` (gần đầu hàm `UpdateComboBonus`):

```cpp
static const DWORD COMBO_TIME_WINDOW = 3 * GAME_FPS;   // thời gian để lên bậc kế tiếp
static const DWORD COMBO_REPEAT_WINDOW = 3 * GAME_FPS;  // thời gian giữ bonus khi lặp lại skill hiện tại
```

Đổi số `3` thành số giây khác nếu muốn chỉnh nhịp độ (cả 2 hằng số này đang cùng là 3s).
