# Gán kỹ năng từ bảng F5 vào ô đồ dùng nhanh (1-9)

Commit: [`d1168c5`](../../commit/d1168c5) — *"Cho phep gan ky nang tu bang F5 vao o do dung nhanh (1-9)"*

## Bối cảnh / yêu cầu

Bảng **Kỹ năng võ công** (phím F5, `KUiSkills` / `KUiFightSkillSubPage` trong [UiSkills.cpp](SwordOnline/Sources/S3Client/Ui/UiCase/UiSkills.cpp)) hiển thị toàn bộ kỹ năng người chơi đã học, nhưng không có cách nào gán một kỹ năng vào phím tắt để cast nhanh. Thanh đồ dùng nhanh 9 ô (phím 1-9, `KUiPlayerBar::m_ImmediaItem[9]` trong [UiPlayerBar.cpp](SwordOnline/Sources/S3Client/Ui/UiCase/UiPlayerBar.cpp)) vốn chỉ nhận **item** (`SetObjectGenre(CGOG_ITEM)`).

Yêu cầu: chuột phải vào 1 kỹ năng trong bảng F5 → nhấc icon ra, kéo theo chuột → thả (click trái) vào 1 trong 9 ô đồ dùng nhanh → kỹ năng được gán vào đúng ô/phím đó, thay thế item đang hiển thị trong ô.

## Ràng buộc quan trọng đã xác định trước khi code

- Vị trí item trong ô đồ dùng nhanh (`pos_immediacy`) là dữ liệu **server quản lý thật** — di chuyển/gán item ở đó đi qua `GOI_SWITCH_OBJECT` → `KPlayer::MoveItem()` → gửi packet lên server (`SendClientCmdMoveItem`, [KPlayer.cpp:3483](SwordOnline/Sources/Core/Src/KPlayer.cpp#L3483)).
- Không có cách nào "gỡ item khỏi ô mà không cần thả vào đâu" qua API đó — nếu gọi thiếu 1 trong 2 đầu Pick/Drop, code tự copy đầu kia vào ("move item to chính vị trí cũ của nó" — vô nghĩa).
- **Quyết định thiết kế:** việc gán kỹ năng vào ô hoàn toàn **thuần client**, không đụng tới Core/server. Kỹ năng chỉ *che* icon item đi (ghi đè hiển thị + hành vi bấm phím), item thật vẫn còn nguyên ở `pos_immediacy` phía server, không bị gỡ/di chuyển.

## Cơ chế hoạt động

### 1. Kho lưu "ô nào đang gán kỹ năng nào" — tái dùng `ms_ShortcutSkills[9]`

`KUiSkillTree` (bảng kỹ năng tả/hữu) đã có sẵn mảng tĩnh `ms_ShortcutSkills[SKILLTREE_SHORTCUT_SKILL_COUNT=9]` dùng cho hệ thống phím tắt cũ (hover + bấm số trong flyout). Tái dùng mảng này, thêm 2 hàm public:

- `KUiSkillTree::SetShortcutSkill(nIndex, uGenre, uId)` — [UiSkillTree.h:29-31](SwordOnline/Sources/S3Client/Ui/UiCase/UiSkillTree.h#L29-L31), [UiSkillTree.cpp:477-495](SwordOnline/Sources/S3Client/Ui/UiCase/UiSkillTree.cpp#L477-L495)
- `KUiSkillTree::GetShortcutSkill(nIndex, uGenre&, uId&)` — trả `false` nếu ô trống.

### 2. Nhấc icon kỹ năng ra khỏi bảng F5 (chuột phải)

[WndObjContainer.cpp](SwordOnline/Sources/S3Client/Ui/Elem/WndObjContainer.cpp) — `KWndObjectBox::WndProc`, case `WM_RBUTTONDOWN`: nếu ô thuộc container `UOC_SKILL_LIST` (bảng F5), gọi `Wnd_DragBegin()` — API kéo-thả có sẵn của engine — để icon bắt đầu bám theo con trỏ chuột (dùng lại đúng cơ chế vẽ icon "đang cầm" mà game có sẵn cho việc kéo item/trang bị).

### 3. Huỷ icon đang cầm — chuột phải (hoặc trái) ở bất kỳ đâu

[Wnds.cpp](SwordOnline/Sources/S3Client/Ui/Elem/Wnds.cpp) — đầu `Wnd_ProcessInput()` (điểm vào xử lý input toàn cục, mọi sự kiện chuột đều qua đây trước khi định tuyến tới cửa sổ cụ thể): nếu đang cầm 1 icon **và** icon đó là kỹ năng (`CGOG_SKILL_FIGHT`), bấm **chuột phải** ở bất kỳ đâu sẽ huỷ ngay (`Wnd_DragFinished()`), sự kiện dừng lại tại đây không đi tiếp xuống cửa sổ khác.

> Lưu ý: chỉ scope theo genre `CGOG_SKILL_FIGHT`, không áp dụng cho các loại kéo-thả khác (item/trang bị thật) — vì với item, **click trái** chính là thao tác thả/đặt vào ô đích, nếu chặn theo click trái sẽ hỏng luôn tính năng thả item có sẵn.

### 4. Thả vào 1 trong 9 ô đồ dùng nhanh (click trái)

- [WndMessage.h](SwordOnline/Sources/S3Client/Ui/Elem/WndMessage.h#L77-L80) — thêm message `WND_N_SKILL_DROP`.
- [WndObjContainer.cpp](SwordOnline/Sources/S3Client/Ui/Elem/WndObjContainer.cpp) — `WM_LBUTTONDOWN`: nếu đang cầm 1 skill và ô đích thuộc `UOC_IMMEDIA_ITEM`, gửi `WND_N_SKILL_DROP` cho cửa sổ cha thay vì đi qua đường thả-item (`DropObject()`) bình thường — vì `DropObject()` sẽ từ chối do genre không khớp (`m_uAcceptableGenre == CGOG_ITEM`).
- [UiPlayerBar.cpp](SwordOnline/Sources/S3Client/Ui/UiCase/UiPlayerBar.cpp) — nhận `WND_N_SKILL_DROP`, xác định đúng chỉ số ô (0-8), gọi `KUiSkillTree::SetShortcutSkill(i, ...)`.

### 5. Hiển thị / hành vi khi ô đã có kỹ năng

Tất cả nằm trong `KImmediaItem` ([UiPlayerBar.cpp](SwordOnline/Sources/S3Client/Ui/UiCase/UiPlayerBar.cpp)):

| Hành động | Xử lý |
|---|---|
| Vẽ ô (`PaintWindow`) | Nếu ô có skill gán → vẽ icon skill **thay** icon item (item thật không đổi, chỉ không hiển thị). |
| Bấm phím 1-9 (`OnUseItem`) | Nếu ô có skill gán → cast skill (`g_pCoreShell->UseSkill`) thay vì dùng item. |
| Click phải vào ô (dùng nhanh) | Đi qua `WND_N_RIGHT_CLICK_ITEM` → `OnUseItem` → cùng logic cast-skill-nếu-có ở trên. |
| Click trái vào ô lúc **không** cầm gì (`KImmediaItem::WndProc`) | Nếu ô có skill gán → **nhấc kỹ năng ra khỏi ô** (xoá gán + bắt đầu kéo lại icon đó theo chuột) — giống hệt cách item được lấy ra khỏi ô đồ dùng nhanh. |
| Hover chuột vào ô (`KImmediaItem::WndProc`, `WM_MOUSEHOVER`/`WM_MOUSEMOVE`) | Nếu ô có skill gán → gọi `SetMouseHoverObjectDesc()` với genre/id của skill (đúng hàm bảng F5 dùng) → tooltip hiện tên/mô tả/thuộc tính giống hệt trong F5. |

## File đã sửa

| File | Nội dung thay đổi |
|---|---|
| [WndMessage.h](SwordOnline/Sources/S3Client/Ui/Elem/WndMessage.h) | Thêm message `WND_N_SKILL_DROP` |
| [WndObjContainer.cpp](SwordOnline/Sources/S3Client/Ui/Elem/WndObjContainer.cpp) | Chuột phải nhấc skill (bảng F5); chuột trái thả skill vào ô đồ dùng nhanh |
| [Wnds.cpp](SwordOnline/Sources/S3Client/Ui/Elem/Wnds.cpp) | Chuột phải bất kỳ đâu huỷ icon skill đang cầm (global input handler) |
| [UiSkillTree.h](SwordOnline/Sources/S3Client/Ui/UiCase/UiSkillTree.h) / [.cpp](SwordOnline/Sources/S3Client/Ui/UiCase/UiSkillTree.cpp) | `SetShortcutSkill` / `GetShortcutSkill` — đọc/ghi kho lưu skill-theo-ô |
| [UiPlayerBar.h](SwordOnline/Sources/S3Client/Ui/UiCase/UiPlayerBar.h) / [.cpp](SwordOnline/Sources/S3Client/Ui/UiCase/UiPlayerBar.cpp) | `KImmediaItem::WndProc` (nhấc ra / hover), `PaintWindow` (vẽ icon skill), nhận `WND_N_SKILL_DROP`, `OnUseItem` cast-skill-nếu-có |

## Lưu ý encoding

Toàn bộ file `.h/.cpp` liên quan (trừ `WndObjContainer.cpp`, đã là UTF-8 sẵn) đang ở **ISO-8859-1**. Mọi chỉnh sửa được chèn bằng `head`/`tail`/`printf` ở mức byte (không dùng tool edit văn bản thông thường) để tránh việc tool tự động re-encode cả file sang UTF-8 làm hỏng các comment tiếng Trung/Việt không dấu ASCII có sẵn trong file gốc.

## Chưa làm / giới hạn hiện tại

- Không có UI để "xoá" gán skill khỏi ô ngoài cách nhấc icon ra rồi huỷ (chuột phải) — chưa có nút xoá nhanh riêng.
- Gán skill không được lưu qua các lần đăng nhập theo cơ chế riêng cho ô đồ dùng nhanh — nó dùng chung cơ chế lưu của `ms_ShortcutSkills` (qua `KUiSkillTree::SaveConfig`), cần kiểm tra lại khi test rằng gán vẫn còn sau khi tắt/mở lại bảng kỹ năng tả/hữu (dùng chung mảng, có thể bị dedupe chéo nếu cùng 1 skill được gán ở cả 2 nơi).
- Chưa build/test được trong game (môi trường này không có Visual Studio) — cần người dùng build (Core → S3Client, Client Debug) và kiểm tra thủ công.
