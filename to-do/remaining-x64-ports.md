# Các project chưa port lên x64

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
