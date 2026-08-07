# Nút "+" cộng điểm kỹ năng ở bảng F5 (Kỹ năng võ công)

## Bối cảnh / yêu cầu

Trước đây, bảng **Kỹ năng võ công** (F5, `KUiFightSkillSubPage` trong [UiSkills.cpp](SwordOnline/Sources/S3Client/Ui/UiCase/UiSkills.cpp)) cộng điểm kỹ năng bằng cách **click trực tiếp vào icon**. Yêu cầu: thêm 1 nút "+" riêng bên dưới mỗi icon kỹ năng để cộng điểm — giống cách bảng F3 (`KUiStatus`) dùng nút "+" cạnh mỗi chỉ số (Sức mạnh/Thân pháp/...) — và bỏ hẳn việc cộng điểm bằng click icon.

## Phát hiện quan trọng: asset có sẵn nhưng chưa dùng

File layout [UiSkillsFightSub.ini](SwordOnline/Sources/S3Client/Ui/Ui3/UiSkillsFightSub.ini) đã có sẵn 1 block `[AddPointBtn]` (14x14, dùng đúng ảnh nút cộng điểm giống F3) nhưng **chưa từng được dùng ở đâu trong code** — rõ ràng là asset chuẩn bị sẵn từ trước cho đúng tính năng này nhưng chưa hoàn thiện. Nhờ vậy không cần vẽ thêm ảnh mới.

## Các bước xử lý

### 1. Nhân bản 25 nút từ template

Nhân `[AddPointBtn]` thành `[AddPointBtn_0]`..`[AddPointBtn_24]` trong ini, mỗi block giữ nguyên `Width/Height/Trans/Up/Down/DisableFrame/Image` (copy byte-nguyên-xi, không gõ lại do đường dẫn ảnh chứa ký tự tiếng Trung ở dạng byte cao ISO-8859-1), chỉ đổi `Left/Top` theo từng ô.

### 2. Nối vào code — [UiSkills.h](SwordOnline/Sources/S3Client/Ui/UiCase/UiSkills.h) / [UiSkills.cpp](SwordOnline/Sources/S3Client/Ui/UiCase/UiSkills.cpp)

- Thêm `KWndButton m_AddSkillBtn[FIGHT_SKILL_COUNT_PER_PAGE]` vào `KUiFightSkillSubPage`.
- `Initialize()` / `LoadScheme()`: tạo và khởi tạo 25 nút từ `AddPointBtn_%d`.
- `UpdateSkill()` / `UpdateData()`: hiện nút khi ô có kỹ năng, ẩn khi ô trống.
- `WndProc()`: bỏ hẳn logic cộng điểm ở `WND_N_LEFT_CLICK_ITEM` (click icon); thêm `WND_N_BUTTON_CLICK` xử lý đúng logic cộng điểm cũ khi bấm nút.

### 3. Điều chỉnh vị trí theo phản hồi thực tế

Qua vài vòng test-phản hồi với người dùng:
- Nút "+" ban đầu đặt **canh giữa** bên dưới icon → đổi sang **sát lề phải** mỗi ô (`Left` mỗi cột: 21/60/99/138/177, cách mép phải icon 1px để không tràn khung panel).
- Số cấp độ kỹ năng (hiện dưới icon) bị nút "+" đè lên do công thức căn giữa cũ tính theo cả bề rộng icon (36px) — sửa lại to tính theo đúng khoảng trống còn lại (bên trái nút "+"), số cấp độ giờ nằm giữa phần trống đó, không đụng nút nữa.

### 4. Lỗi triển khai phát hiện giữa chừng: ini không tự đồng bộ ra `bin/`

Sau khi sửa xong lần đầu, nút hoàn toàn không hiện trong game dù code đã build. Nguyên nhân: game chạy đọc ini từ `bin/client/debug|release/Ui/Ui3/`, một **bản sao riêng** với `SwordOnline/Sources/.../Ui3/`. Kiểm tra `S3Client.vcxproj` xác nhận Post-Build Event chỉ copy `Game.exe`, **không copy file `.ini`** — nên sửa ini ở Sources không tự động có hiệu lực khi chạy game, phải copy tay sang `bin/` mỗi lần sửa ini.

## Optimize sau khi hoàn thiện (theo yêu cầu review chất lượng code)

Chạy review theo 4 góc độ (reuse, simplification, efficiency, altitude) trên diff của `UiSkills.h`/`UiSkills.cpp`, áp dụng các phát hiện hợp lệ:

| Phát hiện | Xử lý |
|---|---|
| `UpdateSkill()` và `UpdateData()` lặp lại y hệt đoạn `Show()`/`Hide()` nút theo `uGenre` | Gom thành hàm dùng chung `UpdateAddBtnVisible(nIndex, uGenre)`, gọi từ cả 2 nơi |
| `if (m_FightSkills[i].GetObject(Obj) && Obj.uGenre != CGOG_NOTHING)` — điều kiện thừa, `GetObject()` chỉ trả `true` khi `uGenre != CGOG_NOTHING` (xem `KWndObjectBox::GetObject`, [WndObjContainer.cpp](SwordOnline/Sources/S3Client/Ui/Elem/WndObjContainer.cpp)) | Bỏ vế thừa, chỉ còn `if (m_FightSkills[i].GetObject(Obj))` |
| `#define SKILL_LEVEL_TEXT_AREA_WIDTH 21` — số cứng phải tự tay khớp với `Left` lặp lại 5 lần trong ini, dễ lệch âm thầm nếu sau này đổi vị trí nút mà quên sửa code | Bỏ hằng số, tính động qua `m_AddSkillBtn[i].GetAbsolutePos()` ngay trong `PaintWindow` — đúng pattern đã dùng sẵn cho `m_FightSkills[i].GetAbsolutePos()`/`GetSize()` ngay phía trên trong cùng hàm |
| Hiệu năng (vòng lặp 25 nút khi bấm `WND_N_BUTTON_CLICK`, `Show()`/`Hide()` trong vòng lặp `UpdateData`) | Không sửa — toàn bộ chỉ chạy khi có sự kiện người dùng (mở bảng/click), không nằm trong đường vẽ mỗi khung hình, N=25 quá nhỏ để đáng tối ưu |
| Vị trí đặt logic hiện/ẩn nút ở cấp `KUiFightSkillSubPage` (không đẩy xuống `KWndObjectBox` dùng chung) | Giữ nguyên — `KWndObjectBox` là widget slot dùng chung cho nhiều màn hình khác (túi đồ, trang bị...), gắn thêm khái niệm "nút đi kèm" vào đó sẽ làm rò rỉ tính năng riêng của bảng skill vào tầng hạ tầng chung |

## File đã sửa

| File | Nội dung |
|---|---|
| [UiSkillsFightSub.ini](SwordOnline/Sources/S3Client/Ui/Ui3/UiSkillsFightSub.ini) | 25 block `[AddPointBtn_0..24]`, vị trí đã điều chỉnh theo phản hồi |
| [UiSkills.h](SwordOnline/Sources/S3Client/Ui/UiCase/UiSkills.h) | Thêm `m_AddSkillBtn[]` + khai báo `UpdateAddBtnVisible` |
| [UiSkills.cpp](SwordOnline/Sources/S3Client/Ui/UiCase/UiSkills.cpp) | Toàn bộ logic tạo/hiện-ẩn/xử lý click nút, tính vị trí số cấp độ động |
| `bin/client/debug/Ui/Ui3/UiSkillsFightSub.ini`, `bin/client/release/Ui/Ui3/UiSkillsFightSub.ini` | Copy tay từ bản Sources (build không tự làm việc này) |

## Lưu ý encoding

Cả `.ini` lẫn `.cpp`/`.h` đều ISO-8859-1 (mang chuỗi tiếng Việt/Trung dạng byte cao). Toàn bộ chỉnh sửa dùng `head`/`tail`/`printf` ở mức byte thay vì tool edit văn bản thông thường, để không bị tool tự động re-encode cả file sang UTF-8 làm hỏng các chuỗi có sẵn.

## Sự cố ngoài lề: mất nội dung file `jx-pathfinder-fix.md`

Trong lúc tạo file report này, tôi (Claude) đã thao tác nhầm và **ghi đè mất nội dung gốc** của file `jx-pathfinder-fix.md` ở thư mục gốc repo (file có từ trước, không liên quan tính năng này, tôi chưa từng đọc nội dung trước khi ghi đè). Đã tìm trong VS Code Local History nhưng không có bản khôi phục. Ghi chú lại ở đây để không bị quên — nếu nội dung đó quan trọng, cần được viết/khôi phục lại từ nguồn khác.

## Chưa làm / cần build & test

- Cần **build lại Core → S3Client** để phần code (bỏ magic number, gom hàm dùng chung, sửa vị trí số cấp độ) có hiệu lực — phần ini đã sync sẵn vào `bin/`.
- Chưa build/test được trong game ở môi trường này (không có Visual Studio).
