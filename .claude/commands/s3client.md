Bạn đang làm việc với module **S3Client** — game client chính, build ra `bin/client/Game.exe`.

## Source folders — Tầng 1: Code của S3Client (sửa trực tiếp ở đây)

| Thư mục | Mô tả |
|---------|-------|
| `SwordOnline/Sources/S3Client/` | Root của module — entry point `S3Client.cpp` |
| `SwordOnline/Sources/S3Client/Ui/` | UI screens: `UiLogin`, `UiSelServer`, `UiSelPlayer`, `UiChatRoom`, `UiPlayerControlBar`, ... (mỗi screen có file `.cpp` + `.ini`) |
| `SwordOnline/Sources/S3Client/Ui/Elem/` | Reusable UI widgets: `WndButton`, `WndEdit`, `WndList`, `WndScrollBar`, ... |
| `SwordOnline/Sources/S3Client/Ui/UiCase/` | Complex multi-panel UI cases (ví dụ `UiRankLienTram`) |
| `SwordOnline/Sources/S3Client/Ui/Ui3/` | UI screens bổ sung |
| `SwordOnline/Sources/S3Client/Login/` | Login flow, xác thực kết nối server |
| `SwordOnline/Sources/S3Client/NetConnect/` | Kết nối mạng phía client |

## Source folders — Tầng 2: Dependencies (đọc để hiểu, không sửa trực tiếp)

| Thư mục | Khi nào cần đọc |
|---------|----------------|
| `SwordOnline/Sources/Core/Src/` | Khi cần hiểu game logic: `KPlayer`, `KNpc`, `KSkills`, `KItem`, `KProtocolProcess` — build thành `CoreClient.dll`, S3Client gọi qua DLL |
| `SwordOnline/Headers/` | Protocol packets: `KProtocol.h`, `KProtocolDef.h` — bất cứ khi nào liên quan đến gửi/nhận packet |
| `SwordOnline/Sources/Engine/src/` | Khi cần hiểu file I/O (`KFile`), windowing (`KWin32Wnd`), utilities base |
| `SwordOnline/Sources/Represent/Represent2/` | Khi cần hiểu rendering 2D — nhân vật, map, hiệu ứng (hiếm) |

## Libs được link

| Lib | Nằm tại |
|-----|---------|
| `CoreClient.lib` | `SwordOnline/Lib/release/CoreClient.lib` |
| `lualibdll.lib` | `SwordOnline/Lib/lualibdll.dll` (import lib) |
| `Winmm.lib`, `shlwapi.lib` | Windows SDK |

## Files quan trọng

- `S3Client.cpp` — WinMain entry, game loop, xử lý `g_PendingLienTram` cross-thread
- `Ui/Elem/WndButton.h/.cpp` — base widget cho tất cả buttons
- `Ui/UiCase/` — thêm UI feature mới vào đây
- `settings/` (trong thư mục output) — cấu hình client runtime

## Lookup nhanh

| Muốn sửa | File cần xem |
|-----------|-------------|
| Login screen | `Ui/UiLogin.cpp` + `UiLogin.ini` |
| Server list | `Ui/UiSelServer.cpp` + `UiSelServer.ini` |
| Character select | `Ui/UiSelPlayer.cpp` |
| Chat UI | `Ui/UiChatRoom.cpp` |
| Player HUD | `Ui/UiPlayerControlBar.cpp` |
| Nhận packet từ server | `SwordOnline/Sources/Core/Src/KProtocolProcess.cpp` |
| Protocol định nghĩa | `SwordOnline/Headers/KProtocol.h` |
| Thêm UI feature mới | Tạo class mới trong `Ui/UiCase/`, đăng ký trong `S3Client.cpp` |

## Build config

Dùng configuration **Client Debug** hoặc **Client Release** trong Visual Studio.  
Rebuild order nếu thay đổi Core logic: Core → S3Client.
