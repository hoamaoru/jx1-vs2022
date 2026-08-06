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

Dữ liệu hiện có (3 nhánh Thiên Vương + 2 nhánh Cái Bang):

```
ComboId	ComboName	Step1SkillId	Step1Bonus	Step2SkillId	Step2Bonus	Step3SkillId	Step3Bonus	Step4SkillId	Step4Bonus	Step5SkillId	Step5Bonus
1	Thien Vuong - Dao	34	30	37	50	32	100	322	500	1058	800
2	Thien Vuong - Thuong	30	30	35	50	41	100	323	500	1060	800
3	Thien Vuong - Chuy	29	30	31	50	324	100	325	500	1059	800
4	Cai Bang - Chuong	122	30	128	50	357	100	1073	500
5	Cai Bang - Bong	119	30	125	50	359	100	1074	500
```

Combo 4/5 chỉ có 4 bậc (10→50→80→150, không phải 10→30→60→80→150 như Thiên Vương) — cột `Step5SkillId`/`Step5Bonus` để trống, đúng theo mục 5 bên dưới. SkillId của 2 chuỗi này lấy theo đúng chuỗi buff dame nội bộ mà `gaibang.lua` (script kỹ năng Cái Bang) đã tự liên kết sẵn giữa các skill, không phải đoán theo `ReqLevel`/`EqtLimit`.

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

Dòng này chỉ hiện cho **đúng người đang thi triển combo** (client tự lọc theo nhân vật đang điều khiển), không hiện cho combo của người khác.

## 7. Console debug tự mở khi chạy Game.exe

`Game.exe` tự động mở kèm 1 cửa sổ console (terminal) để hiện log combo — không cần thêm cờ dòng lệnh. Đóng cửa sổ đó bất cứ lúc nào không ảnh hưởng tới game (không tự tắt game).

## 8. Bubble tên combo trên đầu nhân vật

Ngay khi đủ chuỗi (đánh xong bậc cuối lần đầu, hoặc lặp lại đúng skill đó trong 3s sau) — game hiện thêm 1 dòng chữ hoạt hình phía trên đầu nhân vật, lấy đúng nội dung cột **`ComboName`** trong `SkillCombos.txt` (không phải tên skill). Tên nhân vật/thanh HP vẫn hiện bình thường bên dưới, không bị che. Khác với console log ở mục 6, bubble này hiện cho **bất kỳ ai gần đó cũng thấy được**, không riêng người thi triển.

Hiệu ứng: chữ bắt đầu màu đỏ, bay vào theo từng đơn vị (ký tự hoặc từ, tuỳ chế độ), khi đủ chữ thì phóng to + chuyển màu theo **hệ ngũ hành** của người thi triển (xem bảng màu bên dưới), giữ 1 khoảng thời gian rồi biến mất.

### File cấu hình: `Ui/ComboNameDisplay.ini`

Toàn bộ animation này đọc từ file ini phía **client** — sửa file, mở lại `Game.exe`, không cần build lại code.

| Vị trí | Dùng cho |
|---|---|
| `bin/client/debug/Ui/ComboNameDisplay.ini` | Game.exe (Client Debug) |
| `bin/client/release/Ui/ComboNameDisplay.ini` | Game.exe (Client Release) |

> Đây là cấu hình phía **client only** — không cần copy vào các thư mục `bin/server/...`, và không cần restart GameServer khi chỉnh sửa.

```ini
[ComboNameBubble]
; RevealMode: 0=CharByChar 1=WordByWord 2=SwordSlash 3=InkBlot
RevealMode=1
; Delay (ms) trước khi đơn vị (chữ/từ) kế tiếp bắt đầu bay vào
CharStaggerMs=40
; Thời gian (ms) để 1 đơn vị bay vào đúng vị trí
CharFlyMs=180
; Số đơn vị world-height mà 1 đơn vị di chuyển lúc bay vào
FlyDistance=15
; Thời gian (ms) chuyển size + màu khi đã đủ chữ
GrowMs=250
; Thời gian (ms) giữ nguyên ở size/màu cuối trước khi biến mất
HoldMs=2000
; Màu chữ lúc bắt đầu hiện (R,G,B)
StartColor=255,0,0
; Màu dự phòng nếu không xác định được hệ ngũ hành của người thi triển (R,G,B)
EndColor=255,255,0
; Khoảng cách (world-height units) phía trên nameplate/HP bar để vẽ bubble
VerticalOffset=50
; Khoảng cách giữa các ký tự/từ, chỉ áp dụng cho phần cỡ chữ TRÊN 16 (cỡ
; 10-16 là font gốc đã đúng khoảng cách sẵn, không đụng tới -- siết vào đó
; sẽ làm chữ đè lên nhau). 100 = giữ nguyên khoảng cách của các cỡ >16,
; số càng nhỏ càng sát lại để bù cho việc bitmap phóng to bị thưa ra.
CharSpacing=70
; Cỡ chữ lúc mới bay vào / lúc đã phóng to xong. Chỉ có 12 cỡ: 10, 12, 13, 14,
; 16, 18, 20, 22, 24, 26, 28, 30 là đã đăng ký sẵn (xem Ui/Ui3/UiPubLicSetting.ini
; [FontList]) -- điền số khác sẽ tự làm tròn về cỡ gần nhất trong danh sách này.
; Các cỡ lớn hơn 16 dùng chung bitmap glyph của cỡ 16 rồi phóng to qua khung vẽ
; (không có file .fnt độ phân giải cao hơn), nên càng to sẽ càng mờ/vỡ nét.
; Có thể để FontSizeStart lớn hơn FontSizeEnd để chữ thu nhỏ dần thay vì phóng to.
FontSizeStart=12
FontSizeEnd=30

; Màu "đủ chữ" theo hệ ngũ hành của người thi triển (ghi đè EndColor ở trên),
; khớp đúng bảng màu <color=Metal>/<color=Wood>/<color=Water>/<color=Fire>/<color=Earth>
; game đã dùng sẵn cho chat (R,G,B)
EndColorMetal=246,255,117
EndColorWood=0,255,120
EndColorWater=78,124,255
EndColorFire=255,90,0
EndColorEarth=254,207,179
```

### 4 kiểu animation (`RevealMode`)

| Giá trị | Tên | Mô tả |
|---|---|---|
| 0 | CharByChar | Từng ký tự bay thẳng lên |
| 1 | WordByWord | Từng từ (cách nhau bởi khoảng trắng) bay thẳng lên — mặc định |
| 2 | SwordSlash | "Kiếm quang" — từng từ lướt chéo từ dưới-trái vào, kèm vệt sáng mờ theo sau |
| 3 | InkBlot | "Bút mực thư pháp" — từng từ nở ra tại chỗ từ 1 chấm mực nhỏ, không di chuyển ngang/dọc |

### Giới hạn kỹ thuật cần biết

- **Không có font đậm (bold) thật, và không giả lập được đẹp.** Engine vẽ chữ bằng bitmap glyph dựng sẵn (không phải font vector như Windows) và mỗi lần gọi vẽ chữ đều tự kèm 1 lớp viền đen riêng — từng thử giả lập đậm bằng cách vẽ đè cùng dòng chữ nhiều lần lệch vài pixel nhưng bị stack nhiều lớp viền đen chồng lên nhau thành hiệu ứng "chữ nổi khối" xấu, nên tính năng này đã bỏ.
- **Cỡ chữ chỉ chọn được trong các cỡ đã đăng ký sẵn.** Engine chỉ vẽ được cỡ chữ đã đăng ký sẵn trong `Ui/Ui3/UiPubLicSetting.ini` → `[FontList]` (hiện có: 10, 12, 13, 14, 16, 18, 20, 22, 24, 26, 28, 30 — 7 cỡ sau cùng là mục mới thêm, dùng chung bitmap của cỡ 16 phóng to lên vì không có file `.fnt` độ phân giải cao hơn nên sẽ hơi mờ/vỡ nét, đặc biệt ở cỡ 30). Cỡ chữ nào không có trong danh sách này sẽ **âm thầm không vẽ gì** — đây là lỗi thật đã gặp phải khi cố phóng to 50% (12→18) lúc mới làm tính năng này. `FontSizeStart`/`FontSizeEnd` trong ini tự làm tròn về cỡ gần nhất trong danh sách đó nên không bị lỗi này nữa. Muốn cỡ khác 30 (to hơn nữa) phải đăng ký thêm 1 mục `[FontList]` mới, tăng `RS2_MAX_FONT_ITEM_NUM` trong `KRepresentShell2.h`/`KRepresentShell3.h` nếu đã dùng hết 12 slot, rồi thêm vào mảng `SKILLNAME_VALID_FONT_SIZES` trong `KNpc.cpp`.
- Đổi sang font chữ (typeface) khác hẳn cần có sẵn 1 file `.fnt` đúng định dạng nhị phân riêng của engine này (không phải `.ttf` thường) — hiện repo không có công cụ tự tạo file đó.

## 9. Kiến trúc: client tự suy luận, server không gửi gì về hiển thị

Server (`GameServer`) **chỉ** tính `% bonus sát thương` để áp vào damage thật — không gửi bất kỳ gói tin nào về console log hay bubble. Mỗi client tự chạy **cùng một** logic đối chiếu chuỗi combo (`KNpc::UpdateComboBonus`) mỗi khi quan sát thấy bất kỳ nhân vật nào (không riêng bản thân) thi triển skill — dữ liệu dùng để đối chiếu (`SkillCombos.txt`, luật thời gian) đều là dữ liệu công khai client đã tự load sẵn, nên không cần server xác nhận thêm.

Nguyên tắc chia việc: **damage/logic ảnh hưởng người khác → server xử lý; hiển thị (bubble, console log, màu sắc, animation) → client tự đọc dữ liệu công khai rồi tự hiển thị.**

## 10. File source liên quan (nếu cần đổi luật chơi/animation, không chỉ đổi số liệu)

| File | Vai trò |
|---|---|
| `SwordOnline/Sources/Core/Src/KNpc.h` | Khai báo state theo dõi combo + bubble trên mỗi `KNpc` |
| `SwordOnline/Sources/Core/Src/KNpc.cpp` | `LoadSkillComboSetting()` (đọc SkillCombos.txt), `UpdateComboBonus()` (luật combo + tự in console log/tự hiện bubble phía client), áp dụng bonus trong `AppendSkillEffect()`, `LoadComboDisplaySetting()` + `PaintSkillNameBubble()` (đọc ComboNameDisplay.ini + vẽ animation) |
| `SwordOnline/Sources/Core/Src/CoreDrawGameObj.cpp` | Gọi `PaintSkillNameBubble()` mỗi frame, độc lập với logic vẽ tên/chat |
| `SwordOnline/Sources/Core/Src/KCore.cpp` | Gọi `LoadSkillComboSetting()` lúc khởi động (cả client lẫn server) |
| `SwordOnline/Sources/Core/Src/CoreUseNameDef.h` | Đường dẫn file (`SKILLCOMBO_SETTING_FILE`, `COMBO_NAME_DISPLAY_SETTING_FILE`) |
| `SwordOnline/Sources/S3Client/S3Client.cpp` | Client tự mở console khi khởi động |

Hai hằng số điều chỉnh nhịp độ combo (không phải animation bubble — xem mục 8 để chỉnh animation) nằm trong `KNpc.cpp` (gần đầu hàm `UpdateComboBonus`):

```cpp
static const DWORD COMBO_TIME_WINDOW = 3 * GAME_FPS;   // thời gian để lên bậc kế tiếp
static const DWORD COMBO_REPEAT_WINDOW = 3 * GAME_FPS;  // thời gian giữ bonus khi lặp lại skill hiện tại
```

Đổi số `3` thành số giây khác nếu muốn chỉnh nhịp độ (cả 2 hằng số này đang cùng là 3s, và hiện chưa đưa vào file cấu hình — nằm trong code vì ảnh hưởng luật chơi/sát thương, không chỉ hiển thị).
