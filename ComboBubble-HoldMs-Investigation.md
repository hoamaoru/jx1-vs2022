# Combo bubble: `HoldMs` trong ComboNameDisplay.ini không có tác dụng — chưa giải quyết được

Trạng thái: **CHƯA XONG**. Đã thử 2 hướng sửa, người dùng xác nhận cả 2 đều không giải quyết được vấn đề. Ghi lại đây để xử lý tiếp ở phiên làm việc sau, tránh lặp lại các hướng đã thử.

## Triệu chứng

Sửa `HoldMs` trong `bin/client/debug/Ui/ComboNameDisplay.ini` (ví dụ 2000 → 3000) không làm thay đổi thời gian giữ (hold) của bubble tên combo trước khi nó biến mất. Đã xác nhận với người dùng: **các phần khác của bubble vẫn chạy đúng** (hiện ra, animation, vị trí, màu sắc...) — chỉ riêng `HoldMs` là không ăn.

## Bối cảnh — cơ chế liên quan

- File cấu hình: `bin/client/debug/Ui/ComboNameDisplay.ini` (không có bản Source, chỉ tồn tại trong `bin/`, đường dẫn cứng `COMBO_NAME_DISPLAY_SETTING_FILE = "\\Ui\\ComboNameDisplay.ini"` ở [CoreUseNameDef.h:97](SwordOnline/Sources/Core/Src/CoreUseNameDef.h#L97)).
- Đọc bởi `LoadComboDisplaySetting()` trong [KNpc.cpp](SwordOnline/Sources/Core/Src/KNpc.cpp) (quanh dòng 2145-2184).
- `HoldMs` → biến static `SKILLNAME_HOLD_MS`, dùng trong `GetSkillNameBubbleTotalDuration()`:
  ```cpp
  static DWORD GetSkillNameBubbleTotalDuration(int nUnitCount)
  {
      DWORD dwRevealEnd = (DWORD)((nUnitCount - 1) * SKILLNAME_CHAR_STAGGER_MS + SKILLNAME_CHAR_FLY_MS);
      return dwRevealEnd + SKILLNAME_GROW_MS + SKILLNAME_HOLD_MS;
  }
  ```
- Bubble bị tắt trong `PaintSkillNameBubble()` khi `dwElapsed > dwTotalShowTime` (`dwTotalShowTime` = kết quả hàm trên).
- Bubble được khởi tạo (trigger) ở `UpdateComboBonus()`, quanh dòng 2380-2413 — nơi set `m_nSkillNameBubbleLen`/`m_dwSkillNameBubbleTime`, có kiểm tra `bBubbleStillActive` để không restart animation nếu đang chạy dở (spam finisher liên tục).

## Đã thử (theo thứ tự, cả 2 đều KHÔNG giải quyết được)

**Thử 1:** Phát hiện `LoadComboDisplaySetting()` gốc có cache `static bool bLoaded` — chỉ đọc ini **đúng 1 lần** trong suốt vòng đời client (lần combo đầu tiên sau khi mở game), sau đó luôn bỏ qua file dù có sửa. Đã bỏ cache này, chuyển sang đọc lại mỗi khi có bubble mới trigger (gọi ở `UpdateComboBonus`), bỏ luôn lệnh gọi (giờ dư thừa) trong `PaintSkillNameBubble` (chạy mỗi frame) để đỡ tốn.

→ Người dùng test: **vẫn không tác dụng**.

**Thử 2:** Nghi ngờ giả định "chỗ trigger chắc chắn đã load trước khi bubble hiện" ở Thử 1 sai trong 1 số trường hợp — phục hồi lại lệnh gọi `LoadComboDisplaySetting()` trong `PaintSkillNameBubble` (chạy mỗi frame lúc bubble hiển thị), giữ nguyên phần bỏ cache. Giờ ini được đọc lại ở cả 2 nơi (trigger + mỗi frame).

→ Người dùng test: **vẫn không tác dụng**. Đây là lần cuối trước khi dừng lại để ghi báo cáo này.

## Đã loại trừ / không phải nguyên nhân

- Không phải do cache 1-lần-duy-nhất (`bLoaded`) — đã bỏ hoàn toàn ở Thử 1, vẫn không tác dụng.
- Không phải do thứ tự gọi giữa trigger-site và paint-loop — đã đảm bảo gọi ở cả 2 nơi ở Thử 2, vẫn không tác dụng.
- Không phải bug tổng quát của cả tính năng bubble — animation/vị trí/màu sắc/CharSpacing (sau khi sửa riêng ở phiên trước) đều hoạt động đúng, xác nhận qua người dùng.
- Đã đọc kỹ `KPakFile::Open()` ([KPakFile.cpp:318-346](SwordOnline/Sources/Engine/Src/KPakFile.cpp#L318-L346)): mode mặc định (`m_nPakFileMode == 0`) ưu tiên đọc từ đĩa trước, chỉ fallback vào file .pak nếu không có file rời trên đĩa — nên về lý thuyết sửa file `.ini` rời trên đĩa phải được ưu tiên đọc, không phải đọc nhầm bản cũ trong .pak. Chưa xác minh được `m_nPakFileMode` thực tế là bao nhiêu trong build hiện tại.

## Hướng nghi ngờ chưa kiểm tra (gợi ý cho lần sau)

1. **Xác nhận build thực sự nhận code mới**: build có sạch (full rebuild) hay incremental? Có khả năng object file/`CoreClient.dll` không được recompile đúng cách (link lỗi thời) dù báo build thành công. Nên thử full rebuild (Rebuild Solution, không chỉ Build) cho Core rồi build lại S3Client.
2. **`m_nPakFileMode`**: tìm xem biến này được set ở đâu, giá trị thực tế trong bản debug đang chạy là gì — nếu là `1` (ưu tiên đọc .pak trước), rất có thể có 1 bản `ComboNameDisplay.ini` cũ đã đóng gói sẵn trong file .pak nào đó, đọc trước bản rời trên đĩa mà không ai để ý.
3. **Log debug trực tiếp giá trị `SKILLNAME_HOLD_MS`**: thêm tạm 1 dòng `g_DebugLog` hoặc tương tự ngay sau khi đọc `HoldMs` trong `LoadComboDisplaySetting()`, in ra giá trị vừa đọc được — xác nhận có thực sự đọc được `3000` từ ini hay không. Nếu log vẫn ra `2000`, vấn đề nằm ở khâu đọc file (path/pak/quyền ghi đè...). Nếu log ra đúng `3000` nhưng bubble vẫn tắt sớm, vấn đề nằm ở phần dùng giá trị đó (`GetSkillNameBubbleTotalDuration`/so sánh `dwElapsed`).
4. **Kiểm tra lại đúng cách người dùng đang test**: đánh 1 combo rồi bấm giờ tay xem thời gian hiện thực tế của bubble là bao lâu (so với 2000ms cũ và 3000ms mới) — có thể sự khác biệt quá nhỏ (1 giây) để nhận ra bằng mắt giữa các lần test, dễ nhầm là "không tác dụng" trong khi thực ra có tác dụng nhưng khó cảm nhận.
5. Có khả năng **2 client debug/release** dùng chung 1 tiến trình Core.dll đã load sẵn trong bộ nhớ từ trước (game chưa thực sự tắt hẳn giữa các lần test) — kiểm tra Task Manager xem có tiến trình `Game.exe` cũ còn treo lại không trước khi mở bản mới.

## File liên quan

| File | Vai trò |
|---|---|
| [KNpc.cpp](SwordOnline/Sources/Core/Src/KNpc.cpp) | `LoadComboDisplaySetting`, `PaintSkillNameBubble`, `UpdateComboBonus` — toàn bộ logic |
| `bin/client/debug/Ui/ComboNameDisplay.ini` | File cấu hình test |
| [CoreUseNameDef.h:97](SwordOnline/Sources/Core/Src/CoreUseNameDef.h#L97) | Đường dẫn cứng tới file ini |
| [KPakFile.cpp](SwordOnline/Sources/Engine/Src/KPakFile.cpp) | Cơ chế đọc file (đĩa vs .pak) — nghi ngờ #2 |

Commit liên quan: `6bdd597` ("Them nut cong diem F5, sua luu skill o phim tat, tang co font combo bubble") — chứa cả 2 lần thử sửa HoldMs (đã gộp chung, không tách commit riêng vì cùng 1 phiên làm việc).
