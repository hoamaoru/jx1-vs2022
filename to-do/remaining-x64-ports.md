# Các project chưa port lên x64

## ✅ KẾT QUẢ TRIỂN KHAI — 5 project server (2026-07-20)

**Đã port xong cả 5 project trong nhóm "server còn lại"**: `ExpandPackage`, `ExpandPackageStaticLib`, `FilterText_StaticLib`, `Sword3PaySys` (S3AccServer), `S3RelayServer`. Đúng như dự đoán ban đầu — không có asm x86, không dùng Berkeley DB, chỉ là công việc cơ khí thêm config x64 theo đúng pattern đã dùng cho Bishop/Goddess/S3Relay.

**Verify bằng build thật**: cả 5 project build sạch ở cả `Debug|x64`/`Release|x64`, build lại toàn bộ `JXAll.sln` ở `Server Debug|x64`/`Server Release|x64` (nay có **17 project x64**, tăng từ 12) sạch hoàn toàn, regression-check `Server Debug|Win32`/`Client Debug|Win32` cũng sạch.

**Phát hiện phát sinh (đã fix chủ động, không phải sửa lỗi sau khi build fail)**: `Sword3PaySys.cpp` dùng `int main()` (console) nhưng Release config gốc set `SubSystem=Windows` — cùng bug pattern đã gặp ở Bishop trước đó. Lần này áp dụng bài học cũ, đặt `SubSystem=Console` cho `Release|x64` ngay từ đầu (không copy y nguyên từ Win32 Release), nên không phải fix lại sau khi build fail như lần Bishop.

**Ghi chú kỹ thuật khác**:
- `Sword3PaySys` link `engine.lib`/`LuaLibDll.lib` không điều kiện (luôn bản debug, giống quirk của Goddess) — giữ nguyên quirk này cho x64.
- `S3RelayServer` không có Library dependency nào ngoài hệ thống (odbc32/odbccp32/Ws2_32) — đơn giản nhất trong 5 project.
- Cả `Sword3PaySys` và `S3RelayServer` dùng `msado15.tlh`/`.tli` (ADO type library đã generate sẵn, không phải `#import` động) — không có vấn đề gì trên x64.
- `ExpandPackage`/`ExpandPackageStaticLib`/`FilterText_StaticLib` vẫn **không rõ có server nào thực sự cần dùng lúc runtime hay không** (đã note từ trước) — port xong để có sẵn nếu cần, không có ảnh hưởng gì tới cụm server đang chạy vì không ai phụ thuộc chúng.

**Phạm vi x64 hiện tại: 17/20 project** — chỉ còn 3 project client-only (`S3Client`, `Represent2`, `Represent3`) cố ý giữ 32-bit, không có kế hoạch port (rào cản DirectDraw).

## 🔧 Fix build-system: shared IntDir gây lỗi `StdAfx.sbr` khi build song song trong Visual Studio (2026-07-20)

User build "Server Debug x64" trực tiếp trong VS 2022 (không qua CLI) gặp lỗi `C1083: Cannot open compiler generated file: '..\..\..\..\bin\server\x64\Debug\StdAfx.sbr': Permission denied` cho cả `S3Relay`, `GameServer`, `Goddess`.

**Nguyên nhân**: `IntDir` của config `Debug|x64` ở 6 project (`GameServer`, `Bishop`, `Goddess`, `S3Relay`, `Sword3PaySys`, `S3RelayServer`) vẫn trỏ vào thư mục dùng chung `bin\server\x64\Debug\` — kế thừa y nguyên từ pattern Win32 gốc. Dù `.obj`/`.pch`/`.exe` đã được override riêng cho từng project (`.\x64\Debug\`), file browse-info `.sbr` lại đi theo `IntDir` mặc định chứ không theo override — nên khi VS build nhiều project song song (mặc định của VS, khác với CLI msbuild không dùng `/m`), nhiều `cl.exe` cùng ghi `StdAfx.sbr` (mọi project đều có file `StdAfx.cpp`) vào đúng 1 thư mục cùng lúc.

**Đã fix**: đổi `IntDir`/`OutDir` của `Debug|x64` ở cả 6 project thành thư mục riêng `.\x64\Debug\` — khớp với các setting khác (ObjectFileName, PCH, Link output) vốn đã dùng sẵn, và khớp đúng pattern `Release|x64` đã đúng từ đầu. Verify bằng `msbuild /m` (mô phỏng build song song của VS) nhiều lần, kể cả `-t:Rebuild` — không còn lỗi `StdAfx.sbr`.

## 🐛 Bug thật: `ADO_LONGPTR` hardcode 32-bit trong `msado15.tlh` gây crash x64 (2026-07-20)

Sau khi fix build, khởi động thử `S3RelayServer.exe` và `Sword3PaySys.exe` (x64) đều báo lỗi runtime **"Run-Time Check Failure #2 - Stack around the variable '_result' was corrupted"** ngay khi khởi động — trong khi bản 32-bit chạy bình thường.

**Chẩn đoán**: attach WinDbg trực tiếp vào tiến trình đang treo ở dialog lỗi (không cần đợi crash dump), lấy được stack trace chính xác: `S3PAccount::GetServerID → S3P_MSSQLServer_Result::num_rows → Recordset15::GetRecordCount` (hàm wrapper do `#import "msado15.dll"` tự sinh ra trong `msado15.tli`).

**Nguyên nhân gốc**: `msado15.tlh` (file header sinh tự động bởi `#import`, được **checkin sẵn trong repo** thay vì để MSVC tự generate lại mỗi lần build — sinh ra một lần trên máy 32-bit từ rất lâu) khai báo cứng:
```cpp
typedef long ADO_LONGPTR;   // luôn 4 byte, không phân biệt platform
```
Nhưng property `RecordCount`/`MaxRecords`/`PageCount` của ADO Recordset trên **thực tế** dùng kiểu pointer-sized (`ADO_LONGPTR`) — trên x64, `msado15.dll` thật sự cài trên máy ghi **8 byte** vào con trỏ `&_result` khi gọi `get_RecordCount()`, nhưng code của ta khai báo `_result` chỉ 4 byte (theo typedef cứng trong file .tlh cũ) → COM method ghi đè 4 byte ra ngoài biên biến cục bộ → RTC bắt được ngay ở lần gọi `GetRecordCount()` đầu tiên lúc khởi động, đúng như header Microsoft chính chủ định nghĩa (`#if defined(_WIN64) typedef __int64 ADO_LONGPTR; #else typedef long ADO_LONGPTR; #endif`).

**Đã fix**: sửa `typedef long ADO_LONGPTR;` thành khai báo có điều kiện theo `_WIN64` (khớp đúng cách Microsoft tự định nghĩa) ở **cả 2 file** `Sword3PaySys/S3AccServer/msado15.tlh` và `Sword3PaySys/S3RELAYSERVER/msado15.tlh` (mỗi project có 1 bản riêng, không dùng chung). Build lại + chạy thử lại: cả 2 server khởi động sạch, listening đúng port (`S3RelayServer` port 7777, `Sword3PaySys` port 5002), có kết nối ESTABLISHED tới SQL Server (`127.0.0.1:1433`) — không còn lỗi runtime.

**Bài học chung cho tương lai nếu port thêm project dùng `#import` COM type library đã checkin sẵn `.tlh`/`.tli`** (thay vì để MSVC tự generate lại): kiểm tra kỹ mọi typedef pointer-sized trong file `.tlh` cũ — rất có thể được sinh ra trên máy 32-bit và hardcode sai kích thước cho x64.

## Thông tin khảo sát

| | |
|---|---|
| Branch khảo sát | `port_x64` |
| Commit ID | `fd11db37d48afa3aaa07f6a51d7d7657654a1c28` |
| Commit đó | `Port GameServer/Bishop/Goddess/S3Relay build chain to x64` |
| Ngày khảo sát | 2026-07-20 |

## Bối cảnh

`JXAll.sln` có tổng cộng 20 project. Tính tới commit trên, **12 project đã có config x64** (`Debug|x64`/`Release|x64`, riêng Core dùng `Server Debug|x64`/`Server Release|x64`):

- **Support libs**: `Common`, `Engine`, `Core`, `LuaLibDll`, `JpgLib`, `KMp3Lib`
- **Cụm server game**: `Heaven`, `Rainbow`, `GameServer`, `Bishop`, `Goddess`, `S3Relay`
- **Khác**: `FilterText` (DLL lọc tên nhân vật, Goddess cần lúc runtime)

Chi tiết đầy đủ về quá trình port 12 project trên: `to-do/gameserver-x64-migration.md` và `to-do/goddess-bishop-s3relay-x64-migration.md`.

**8 project còn lại chưa port** — liệt kê ở đây để làm sau này nếu cần.

## Danh sách project chưa port

### Nhóm client — KHÔNG nên port (out of scope theo quyết định từ đầu)

| Project | File | Lý do giữ 32-bit |
|---|---|---|
| `S3Client` (tên hiển thị trong .sln: `Game`) | `S3Client/S3Client.vcxproj` | Game client chính — phụ thuộc DirectDraw, không có bản x64 chính thức trong Windows SDK hiện đại. Đây là rào cản đã xác định từ đầu, lý do cả dự án port x64 lần này chỉ nhắm vào phía server. |
| `Represent2` | `Represent/Represent2/Represent2.vcxproj` | Rendering 2D, client-only, phụ thuộc S3Client |
| `Represent3` | `Represent/Represent3/Represent3.vcxproj` | Rendering 3D, client-only, phụ thuộc S3Client |

### Nhóm server còn lại — CÓ THỂ port sau này nếu cần, chưa có rào cản kỹ thuật rõ ràng

Đã khảo sát nhanh cả 5 project dưới đây: **không có asm x86**, **không dùng Berkeley DB**, **không project nào trong cụm x64 hiện tại (12 project đã port) phụ thuộc build-time hay runtime vào chúng** — Bishop chỉ include header `Sword3PaySys\S3AccServer` cho struct dùng chung, không link binary. Nếu port, nhiều khả năng sẽ đơn giản như Bishop/Goddess/S3Relay — thuần cơ khí thêm config x64 + fix vài chỗ kiểu dữ liệu nếu có, không có rào cản kỹ thuật lớn đã biết trước. Chưa khảo sát sâu (chưa đọc hết source), chỉ mới xác nhận không có blocker rõ ràng như asm hay Berkeley DB.

| Project | File | Vai trò | Ghi chú |
|---|---|---|---|
| `Sword3PaySys` (tên hiển thị: `S3AccServer`) | `Sword3PaySys/S3AccServer/Sword3PaySys.vcxproj` | Server tài khoản / mua VIP item | Thuộc nhóm "Payment System" theo CLAUDE.md, tách biệt cụm GameServer/Bishop/Goddess/S3Relay |
| `S3RelayServer` | `Sword3PaySys/S3RELAYSERVER/S3RelayServer.vcxproj` | Relay thanh toán | Cùng nhóm Payment System với Sword3PaySys |
| `ExpandPackage` | `ExpandPackage2.0/ExpandPackage.vcxproj` | DLL đóng gói asset game (nén/mã hoá) | Chưa xác nhận có server nào cần lúc runtime — grep không thấy `LoadLibrary("ExpandPackage.dll")` trong `MultiServer/`hay `Core/`, cũng không project nào link tĩnh. `ExpandPackage.dll` (32-bit) có mặt trong `bin/server/debug/` nhưng chưa rõ có thực sự được dùng hay chỉ là leftover copy |
| `ExpandPackageStaticLib` | `ExpandPackageStaticLib/ExpandPackageStaticLib.vcxproj` | Bản static lib của ExpandPackage | Không project nào link tới — có vẻ không dùng, cần xác nhận lại trước khi quyết định port |
| `FilterText_StaticLib` | `FilterText/FilterText_StaticLib.vcxproj` | Bản static lib của FilterText (khác bản DLL `FilterText` đã port ở đợt trước) | Không project nào link tới — có vẻ không dùng, cần xác nhận lại trước khi quyết định port |

## Việc cần làm khi port tiếp (nếu có nhu cầu)

1. Xác nhận lại xem `ExpandPackage`, `ExpandPackageStaticLib`, `FilterText_StaticLib` có thực sự cần thiết cho server không trước khi bỏ công port (có vẻ dead weight, nhưng chưa khảo sát kỹ 100%).
2. Nếu `Sword3PaySys`/`S3RelayServer` cần port — làm theo đúng pattern đã dùng cho Bishop/Goddess/S3Relay: thêm `Debug|x64`/`Release|x64` vào `.vcxproj`, map GUID vào `JXAll.sln`, build thử, fix lỗi phát sinh (nếu có, dự kiến ít vì không asm/không BDB), thêm mapping Server Debug/Release|x64 vào `JXAll.sln`.
3. Vẫn cần regression-check Win32 sau mỗi lần sửa, theo đúng quy trình đã áp dụng xuyên suốt các đợt port trước.
