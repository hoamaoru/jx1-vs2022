# Build GameServer thành bản 64-bit (x64)

## ✅ KẾT QUẢ TRIỂN KHAI (2026-07-20)

**Đã triển khai xong và verify bằng build thật** (không phải mô phỏng) — `GameServer.exe` (x64) build thành công cho cả Debug và Release, xác nhận qua `dumpbin /headers` là PE thật `machine (x64)`. Build sạch qua cả msbuild trực tiếp từng project lẫn qua toàn bộ `JXAll.sln` (`Server Debug|x64` và `Server Release|x64`). Regression-check Win32 sau khi sửa: `Server Debug|Win32` và `Client Debug|Win32` build sạch toàn solution — không có gì bị hỏng.

### Phạm vi thực tế: 9 project (không phải 7 như dự kiến ban đầu)

Khi build thử phát sinh 2 dependency ẩn không thấy trong khảo sát ban đầu:
- **JpgLib** và **KMp3LibClass** — `Engine.vcxproj` link tĩnh 2 lib này (`..\Lib\{debug|release}\JpgLib.lib`, `KMp3Lib.lib`) không điều kiện Client/Server, nên build x64 của Engine bắt buộc cần bản x64 của cả hai.

### Rủi ro asm x86 — nghiêm trọng hơn dự kiến, nhưng đã xử lý xong

Khảo sát ban đầu chỉ tìm thấy asm thật ở `Common` (2 hàm). Khi thực sự bật x64 và build, phát hiện thêm rất nhiều:

| File | Asm gì | Cách xử lý |
|---|---|---|
| `Common/Macro.h` | `ASSERT`/`VERIFY` macro dùng `_asm{int 0x03}` | Đổi sang `__debugbreak()` (intrinsic, tương đương, chạy được cả x86/x64) |
| `Common/KSG_EncodeDecode.cpp` | Cipher XOR packet (asm thật) | Đã có sẵn bản C tương đương (`KSG_DecodeEncode`) — route sang bản đó cho x64 qua `#if !defined(_M_IX86)` |
| `Common/CRC32.C` | CRC32 tối ưu tay | Có sẵn bản C tương đương (`CRC32_C`) — route sang cho non-x86 |
| `JpgLib/KJpegIdct.c` | MMX IDCT (`jpeg_idct_mmx`) | Route sang bản C có sẵn `jpeg_idct_2d` cho x64 |
| `JpgLib/KJpegColor.c` | MMX color convert (`Y2RGB`, `RGB16_555/565`, `YCbCr411_*`) | Port thủ công sang C cho `Y2RGB`/`RGB16_555`/`RGB16_565` (đã trace từng lệnh asm); `YCbCr411_565/555` route sang `YCbCr411_nommx` có sẵn (không port vì quá phức tạp và chỉ dùng khi bật MMX) |
| `JpgLib/KJpegDecode.c` | `jpeg_preprocess` (byte un-stuffing) | Port sang C, đã trace từng lệnh |
| `JpgLib/KJpegDecode.c` | `jpeg_decode_DU` (bộ giải Huffman/bitstream JPEG đầy đủ, ~240 dòng asm) | **Không port** — quá phức tạp, đây là code giải mã JPEG hoàn toàn client-only, GameServer không bao giờ gọi tới. Thay bằng stub an toàn (`memset` buffer về 0) cho x64, kèm comment giải thích rõ |
| `JpgLib/KJpegLib.h` | Macro `READ_WORD` dùng asm đọc big-endian word | Viết lại bằng C thuần (đơn giản, áp dụng cho mọi platform luôn, không cần `#if`) |
| `Engine/KMemBase.cpp` | 8 hàm memcpy/memset/memcmp/memxor tối ưu tay qua `#ifdef WIN32` | **Không dùng `#else` có sẵn** — đã kiểm chứng bằng cách đọc kỹ asm rằng `g_MemFill(WORD)` và `g_MemFill(DWORD)` coi `dwLen` là đơn vị WORD/DWORD (không phải byte) trong khi `#else` gốc coi là byte — **là bug tiềm ẩn có sẵn, chưa từng chạy vì `WIN32` luôn được định nghĩa**. Đã viết lại C thủ công bám sát đúng semantics của asm gốc cho `g_MemFill(WORD)`, `g_MemFill(DWORD)`, `g_MemXore` (thay vì tin `#else`); 5 hàm còn lại (`g_MemCopy`, `g_MemCopyMmx`, `g_MemComp`, `g_MemFill(BYTE)`, `g_MemZero`) đã verify khớp `#else` nên chỉ đổi guard `#ifdef WIN32` → `#if defined(WIN32) && defined(_M_IX86)` |
| `Engine/KStrBase.cpp` | Tương tự — 6 hàm string qua `#ifdef WIN32` | `g_StrCpyLen`: asm có semantics khác `strncpy` (không pad, luôn null-terminate) — viết C thủ công bám asm; 5 hàm còn lại verify khớp `#else`, chỉ đổi guard |
| `Engine/KColors.cpp` | 7 hàm RGB565/555 convert | Port toàn bộ sang C (trace từng lệnh, kể cả 1 chỗ có vẻ là bug gốc ở `g_Red` — giữ nguyên hành vi, không "sửa") |

**Cụm rendering/audio/video/input trong Engine bị loại khỏi build x64** (giữ nguyên Win32, không đổi gì) vì client-only và có quá nhiều asm/DirectX phụ thuộc lẫn nhau để port hết trong phạm vi này — loại theo từng vòng build-thử-xem-lỗi-thiếu-symbol-loại-tiếp: `KBitmap(16).cpp, KBmpFile(24).cpp, KBmp2Spr.cpp, KCanvas.cpp` (giữ), `KDrawBase/Bitmap/Bitmap16/Fade/Font/Sprite/SpriteAlpha.cpp, KFont.cpp, KGifFile.cpp, KJpgFile.cpp, KMp4Movie.cpp, KMp4Video.cpp, KPalette.cpp, KSprite.cpp, KSpriteCache.cpp, KSpriteCodec.cpp, KSpriteMaker.cpp`. `KDDraw.cpp` và `KColors.cpp` **không loại** — không có asm thật, chỉ cần port (KColors) hoặc build thẳng (KDDraw, DirectDraw x64 import lib có sẵn trong Windows SDK hiện tại).

### Sửa khác ngoài asm

- **`KProtocol.h`**: đổi `size_t` → `DWORD` ở 4 struct (`TProcessData`, `tagGuidableInfo`, `tagDBSyncPlayerInfo`, `tagGS2GWSaveRole`) như kế hoạch — verify build sạch trên Common/Core/GameServer/Goddess/Bishop/S3Relay (Win32).
- **`Common/OpaqueUserData.h`**: đổi `GetUserData/SetUserData` từ `unsigned long` → `ULONG_PTR`, xoá macro `InterlockedExchangePointer` fallback nguy hiểm — như kế hoạch.
- **`Core.vcxproj`**: loại `EP1KDL20.LIB`/`EpsForJoLib.lib` (USBKey dongle, chỉ 32-bit) khỏi build x64 — code gọi chúng nằm sau `#ifdef _USBKEY` chưa từng định nghĩa, xác nhận dead code.
- **`GameServer/KSOServer.cpp`**: 2 biến cục bộ `uSize` khai báo `unsigned int` nhưng truyền vào tham chiếu `size_t&` của `IClient::GetPackFromServer`/`IServer::GetPackFromClient` (API của Rainbow.dll) — lỗi biên dịch trên x64 vì `size_t` (8 byte) không thể bind vào biến 4-byte qua reference. Đổi cả 2 khai báo sang `size_t` (khớp đúng kiểu API, an toàn, không đổi logic).
- **Build system**: mỗi trong 9 project (`Common`, `JpgLib`, `KMp3LibClass`, `LuaLibDll`, `Engine`, `Core`, `Heaven`, `Rainbow`, `GameServer`) được thêm config x64 riêng (Core dùng tên `Server Debug|x64`/`Server Release|x64` khớp quy ước sẵn có, còn lại dùng `Debug|x64`/`Release|x64`), output tách thư mục hoàn toàn riêng (`x64\Debug\`, `SwordOnline\Lib\x64\...`, `bin\server\x64\...`) — không đụng tới bất kỳ artefact Win32 nào. `JXAll.sln` được thêm 2 solution-config mới (`Server Debug|x64`, `Server Release|x64`) map đúng cho 9 project này, các project khác (Goddess/Bishop/S3Relay/S3Client/Sword3PaySys/Represent2/3) không có mapping cho 2 config này (MSBuild tự bỏ qua, đúng ý đồ).
- **Lưu ý khi build qua msbuild trực tiếp** (không qua IDE): với Core, phải chỉ đường dẫn tường minh tới `lualibdll.lib` trong `AdditionalDependencies` (`..\..\Lib\x64\debug\lualibdll.lib`) thay vì chỉ tên file + `/LIBPATH` — vì lý do chưa rõ, `/LIBPATH` tương đối không resolve đúng trong môi trường build này dù cú pháp đúng chuẩn.

## ✅ Test tích hợp thật + 2 bug runtime phát hiện và đã fix (2026-07-20)

Sau khi Goddess/Bishop/S3Relay cũng xong x64 (xem `to-do/goddess-bishop-s3relay-x64-migration.md`) và có Berkeley DB x64 thật, đã **khởi động thật cả 4 server theo đúng thứ tự Goddess → Bishop → S3Relay → GameServer** từ `bin\server\x64\Debug` (qua `Start-Process` để có console thật, tránh nhầm với lỗi môi trường không-console khi launch qua pipe). Quá trình test lộ ra 2 bug runtime thật — cả 2 đều đã xác nhận bằng crash dump (cài WinDbg/`cdb.exe` qua winget để phân tích `.dmp` do Windows Error Reporting tự sinh) và đã fix + verify lại bằng build + chạy thật:

**Bug 1 — `long dir = _findfirst(...)` cắt cụt handle** (`Core/Src/KSortScript.cpp:168`, hàm `LoadScriptInDirectory`): `_findfirst` trả về `intptr_t` (8 byte trên x64), gán vào biến `long` (Windows LLP64: `long` luôn 4 byte kể cả x64) làm mất nửa trên handle. Gọi lại `_findnext`/`_findclose` với handle hỏng → heap corruption bên trong UCRT, crash access-violation ngay lúc GameServer quét thư mục `script/` lúc khởi tạo Lua script (stack trace: `GameServer!main → KSwordOnLineSever::Init → CoreServer!g_InitCore → g_IniScriptEngine → LoadAllScript → LoadScriptInDirectory → _findnext64i32 → common_find_next_narrow/wide`). Sửa: `long dir` → `intptr_t dir`. Cùng lỗi (nhưng chưa từng gây crash vì không dùng lại handle) tìm thấy thêm ở `MultiServer/Goddess/DBDumpLoad.cpp:21,86` (`long aFileFound`) — sửa luôn `intptr_t` cho nhất quán/phòng ngừa.

**Bug 2 — `GetGameData`/`OperationRequest` dùng `unsigned int` để truyền con trỏ** (`Core/Src/CoreServerShell.h`, `CoreServerShell.cpp`): 2 hàm generic dispatcher trong `iCoreServerShell` khai báo `unsigned int uParam` nhưng **body và caller đều dùng làm con trỏ** — bên trong hàm cast `(char*)uParam` để ghi buffer, còn ~70 call site trong `GameServer/KSOServer.cpp` truyền vào kiểu `(unsigned int)&sStruct`/`(unsigned int)szBuffer`. Trên x64, mỗi cast như vậy cắt cụt con trỏ 8-byte xuống 4-byte → con trỏ rác khi hàm dùng lại bên trong, crash access-violation khi GameServer xử lý gói tin từ Gateway lúc đang chạy bình thường (stack trace: `GatewayMessageProcess → GatewaySmallPackProcess → CoreServer!CoreServerShell::GetGameData`, ghi đè qua một con trỏ rõ ràng bị cắt cụt, ví dụ `rdx=0x0f16f830`). Sửa: widen `uParam`→`UINT_PTR`, `nParam`→`INT_PTR` ở **cả 3 chỗ khai báo phải khớp nhau** (interface `struct iCoreServerShell` trong `.h`, khai báo member trong `class CoreServerShell` — nằm ngay trong `.cpp`, và định nghĩa out-of-line) + đổi toàn bộ ~70 cast `(unsigned int)` → `(UINT_PTR)` tại call site trong `KSOServer.cpp`. Interface phía client (`iCoreShell::GetGameData` trong `CoreShell.h`) **không đụng tới** — client vẫn 32-bit, không cần.

**Bug phụ (không phải x64, phát hiện tình cờ)**: `Goddess.exe` (x64) lúc khởi động gọi `LoadLibrary("FilterText.dll")` để lọc tên nhân vật (`Goddess/FilterTextLib.cpp`) nhưng `FilterText.dll` trước đó chỉ có bản 32-bit trong repo → `LoadLibrary` fail → hiện `MessageBox` "text filter's initing has failed" rồi thoát. Đã port `FilterText.vcxproj` sang x64 (không asm, đơn giản) — chi tiết đầy đủ nằm ở `to-do/goddess-bishop-s3relay-x64-migration.md` vì đây là fix riêng cho Goddess.

**Kết quả sau khi fix cả 2 bug + FilterText**: build sạch toàn bộ `JXAll.sln` (`Server Debug|x64`), khởi động lại đủ 4 server — **sống ổn định >25 giây** (trước đó GameServer luôn crash trong 10-15 giây), có kết nối ESTABLISHED đầy đủ hai chiều giữa cả 4 server (Goddess↔Bishop↔S3Relay↔GameServer), không sinh crash dump mới. Đây là lần đầu tiên toàn bộ cụm server x64 chạy được thật, không chỉ build được.

## Việc CHƯA làm

- **Test tích hợp với client thật** (S3Client 32-bit kết nối vào cụm x64, test đủ luồng login → chọn nhân vật → vào game → chat/tong → giao dịch/lưu tiến độ) — mới test được 4 server nói chuyện với nhau, **chưa có client thật kết nối vào**. Cần người dùng tự làm (ngoài khả năng môi trường CLI).
- **`Server Release|Win32` xác nhận hỏng sẵn từ trước** (không phải do phiên làm việc này gây ra) — thư mục `SwordOnline/Lib/release/` trống hoàn toàn, `Engine` Release|Win32 tự nó lỗi biên dịch vì xung đột `winsock2.h`/`windows.h` không liên quan gì tới x64. Nằm ngoài phạm vi.
- **File build output (`.lib`/`.dll`/`.exp`) bị ghi đè** do các lần build verify trong phiên này (ví dụ `SwordOnline/Lib/debug/*.lib`, `SwordOnline/Lib/JpgLib.lib`...) — đây là artefact build, không phải thay đổi source, nhưng đang bị git track nên sẽ hiện trong `git status`. Nên cân nhắc gitignore thư mục `Lib`/`bin` build output nếu chưa có.

## Thông tin khảo sát

| | |
|---|---|
| Branch khảo sát | `main` |
| Commit ID | `b59ac49ebcd44715ff5622e5ffd3b38eb42fb32a` |
| Commit đó | `Update config Client Debug` (2026-06-08 22:34:43 +0700) |
| Ngày khảo sát/lên kế hoạch | 2026-07-20 |
| Working tree lúc khảo sát | có 1 file chưa commit: `SwordOnline/Sources/S3Client/S3Client.vcxproj.filters` (không liên quan tới kế hoạch này) |

> Toàn bộ đường dẫn file/dòng trích dẫn bên dưới được xác nhận đúng tại commit trên. Nếu code đã thay đổi nhiều so với commit này, nên khảo sát lại trước khi triển khai.

## Bối cảnh

Toàn bộ solution `JXAll.sln` hiện chỉ có platform **Win32** — không có cấu hình x64 nào tồn tại ở bất kỳ project nào. User muốn build riêng **GameServer** (main game server, xử lý toàn bộ game logic) thành bản 64-bit, trong khi các thành phần còn lại của cụm server (Goddess, Bishop, S3Relay, Sword3PaySys) và game client (S3Client) **vẫn giữ nguyên 32-bit**.

Đây là bước port hẹp hơn nhiều so với kế hoạch "toàn bộ game 32→64" đã từng bàn trước đây (plan cũ không còn tồn tại trên đĩa). Vì chỉ port GameServer, không port client, nên rào cản lớn nhất của kế hoạch cũ (DirectDraw không có bản 64-bit) không áp dụng ở đây — nhưng phát sinh một ràng buộc mới: GameServer 64-bit phải **tiếp tục nói chuyện qua mạng đúng byte-layout** với các server 32-bit khác.

Kế hoạch này dựa trên khảo sát trực tiếp source (không suy đoán) qua 3 hướng: build/dependency graph, rủi ro pointer-truncation, và tính tương thích wire-protocol.

## Phạm vi: 7 project phải build x64 đồng bộ

GameServer.exe không tự đứng một mình — nó nạp các DLL sau **ngay khi khởi động** (implicit load qua import lib, hoặc `LoadLibrary` tường minh):

| Project | Loại | Vì sao bắt buộc |
|---|---|---|
| `GameServer` (`MultiServer/GameServer/GameServer.vcxproj`) | Application | Chính nó |
| `Core` — config **Server Debug/Release** (`Sources/Core/Core.vcxproj`) | DynamicLibrary → xuất `CoreServer.dll` | GameServer link `CoreServer.lib` (import lib, không phải static lib thật) |
| `Common` (`MultiServer/Common/`) | StaticLibrary | GameServer + Core + Heaven + Rainbow đều link `common.lib` |
| `Engine` (`Sources/Engine/`) | DynamicLibrary → `engine.dll` | GameServer link `engine.lib` (import lib) |
| `LuaLibDll` (`Sources/Library/LuaLib/`) | DynamicLibrary | `CoreServer.dll` link `lualibdll.lib` → nạp ngầm `LuaLibDll.dll` |
| `Heaven` (`MultiServer/Heaven/`) | DynamicLibrary | GameServer `LoadLibrary("heaven.dll")` runtime, cùng process → phải cùng bitness |
| `Rainbow` (`MultiServer/Rainbow/`) | DynamicLibrary | GameServer `LoadLibrary("rainbow.dll")` runtime, cùng process → phải cùng bitness |

**Không cần đụng tới**: Goddess, Bishop, S3Relay, S3Client, Sword3PaySys (S3AccServer/S3RELAYSERVER), DBTranslate — các module này giữ nguyên Win32. Cũng **không có Berkeley DB hay lib 32-bit-only nào** trong chuỗi phụ thuộc GameServer (các lib này chỉ nằm ở Goddess/S3Relay/Sword3PaySys) — đây là điểm khác biệt tích cực so với lo ngại trong plan cũ.

### Vì sao Goddess/Bishop/S3Relay KHÔNG cần port dù cũng nạp Heaven.dll/Rainbow.dll/common.lib

Đây là điểm dễ hiểu nhầm nhất — cần phân biệt rõ 2 kiểu "dùng chung":

1. **Nạp DLL trong cùng process (in-process)** — chỗ duy nhất bắt buộc khớp bitness. Goddess/Bishop/S3Relay/S3AccServer cũng `LoadLibrary("heaven.dll")`/`rainbow.dll` **vào process riêng của chúng**, độc lập với GameServer. Plan **không** đè bản build 32-bit hiện có — mà **thêm config x64 song song** trong cùng `.vcxproj`, xuất ra thư mục output riêng (`bin\server\x64\heaven.dll`), trong khi bản Win32 cũ (`bin\server\heaven.dll`) giữ nguyên. Goddess/Bishop/S3Relay (không đổi) tiếp tục nạp bản 32-bit nằm cạnh chúng; chỉ GameServer.exe bản x64 (thư mục riêng) mới nạp bản 64-bit nằm cạnh nó. Đây là lý do bước "Tách output path theo platform" ở dưới là bắt buộc, không phải tuỳ chọn.
   `Common` còn đơn giản hơn vì là **static lib**, biên dịch thẳng vào từng .exe/.dll lúc build — không phải file runtime dùng chung, nên 2 bản (32-bit và x64) hoàn toàn độc lập, không có khái niệm "đè lên nhau" lúc chạy.
2. **Giao tiếp qua mạng (socket, giữa các process/máy khác nhau)** — đây là kênh thật sự nối GameServer với Goddess/Bishop/S3Relay, và **không yêu cầu cùng bitness**. `heaven.dll`/`rainbow.dll` trong process GameServer (x64) và trong process Goddess (32-bit) là 2 instance độc lập, chỉ cần gửi/nhận byte đúng layout struct đã thoả thuận (`KProtocol.h`...) — đây chính là lý do fix `size_t → UINT32` là bắt buộc (đảm bảo layout gói tin giống nhau dù 2 bên khác bitness), còn bitness của tiến trình bên kia thì không liên quan.

Kết luận: Goddess/Bishop/S3Relay giữ nguyên 32-bit, không đổi code — điều kiện đủ là (a) tách thư mục output x64/Win32 riêng biệt, và (b) fix 2 struct `size_t` trong `KProtocol.h`.

Tất cả lib bên thứ ba còn lại trong chuỗi (`odbc32.lib`, `odbccp32.lib`, `Ws2_32.lib`, `Winmm.lib`, `Shlwapi.lib`, `wsock32.lib`) đều là Windows SDK system lib — VS2022 tự chọn bản x64 tương ứng, không phải rào cản.

**Rủi ro build cần xác minh sớm**: `Engine.vcxproj` **chưa tách config Client/Server** như Core — nó link cứng `ddraw.lib;dsound.lib;dxguid.lib;dinput8.lib` cho mọi config. DirectDraw không có bản 64-bit chính thức trong SDK hiện đại; cần thử build thử Engine ở x64 trước để biết `ddraw.lib` (x64) có tồn tại trong Windows SDK đi kèm VS2022 hay không. Nếu không có, phải tách riêng một Engine-x64 config chỉ chứa phần Server thực sự cần (KFile, logging, Win32 helpers) và bỏ các dependency Direct* — cần điều tra thêm ngay khi bắt đầu code, vì hiện chưa biết chắc.

## Vấn đề cốt lõi: 2 struct trong `KProtocol.h` chứa `size_t`

Đây là thứ **bắt buộc phải sửa trước khi build x64**, không phải tuỳ chọn — nếu bỏ qua, GameServer x64 sẽ nói chuyện sai layout với mọi server 32-bit khác và crash/hỏng dữ liệu ngay khi có traffic.

- `TProcessData` (`SwordOnline/Headers/KProtocol.h:1140`) — field `size_t nDataLen`. Struct này được gửi thẳng qua socket bằng `sizeof(TProcessData)` ở rất nhiều nơi: `GameServer/KSOServer.cpp` (giao tiếp GameServer↔Goddess, GameServer↔S3Relay), `Goddess/ClientNode.cpp`, `Bishop/GamePlayer.cpp`, `S3Relay/RootClient.cpp`, `S3Client/Login/Login.cpp`. Trên x64, `size_t` = 8 byte thay vì 4 byte → mọi field sau nó (`ulIdentity`, `bLeave`, `szName`, `pDataBuffer`) sẽ lệch offset so với bên 32-bit.
- `tagGuidableInfo` (`KProtocol.h:884`) — field `size_t datalength`, dùng `sizeof()` để gửi ở `Bishop/GameServer.cpp:780` và `GameServer/KSOServer.cpp:2206` (giao tiếp GameServer↔Bishop).
- `tagDBSyncPlayerInfo` (`KProtocol.h:849`) và `tagGS2GWSaveRole` (`KProtocol.h:1113`) — cùng lỗi `size_t`, nhưng grep không thấy nơi nào dùng `sizeof()` để gửi mạng (có vẻ dead code). Sửa luôn cho nhất quán/an toàn, ưu tiên thấp hơn 2 struct trên.

**Cách sửa**: đổi `size_t` → kiểu cố định 4-byte (`UINT32` hoặc `DWORD`, theo quy ước đang dùng trong cùng file) ở cả 4 struct. Trên Win32 hiện tại `size_t` vốn đã là 4 byte nên đây là thay đổi **không đổi hành vi** cho mọi build 32-bit hiện có — chỉ cần rebuild lại toàn bộ solution Win32 để xác nhận không có chỗ nào phụ thuộc ngầm vào kiểu `size_t` (ví dụ so sánh với `std::string::size()`, hoặc dùng `%zu`).

`KRelayProtocol.h`, `KTongProtocol.h`, `KGmProtocol.h`, `KProtocolDef.h` đã kiểm tra sạch — không có `size_t`/con trỏ/`HANDLE` nào trong struct mạng. `HeavenInterface.h`/`RainbowInterface.h`/`S3DBInterface.h` có con trỏ/callback nhưng chỉ dùng in-process (không qua mạng) nên an toàn nếu Heaven/Rainbow build x64 đồng bộ.

## Vấn đề phụ: API pointer-unsafe trong `Common/OpaqueUserData.h`

Lớp IOCP chính (`IOCompletionPort`, `SocketServer`) đã viết đúng chuẩn 64-bit từ đầu (`ULONG_PTR`/`PDWORD_PTR` cho completion key) — không cần sửa. Chỉ có 1 chỗ cần vá phòng ngừa:

- `Common/OpaqueUserData.h:55-63` — `GetUserData()`/`SetUserData()` ép con trỏ qua `unsigned long` (cắt còn 4 byte trên x64). Hiện tại grep toàn bộ call site (`Buffer.cpp`, `SocketServer.cpp`, `Heaven/ServerStage.cpp`) xác nhận **chưa ai dùng để lưu con trỏ thật** (chỉ dùng cho số liệu), nhưng đây là API "bẫy" — nên đổi kiểu về `ULONG_PTR`/`DWORD_PTR` để chặn lỗi tương lai.
- `Common/OpaqueUserData.h:23-26` — macro fallback tự định nghĩa `InterlockedExchangePointer` ép qua `PLONG` 32-bit. Nên xoá hẳn (không dựa vào `#ifndef` may rủi với thứ tự include).

Thay đổi này nằm trong `Common` — ảnh hưởng build lại mọi module link `common.lib` (GameServer, Bishop, Goddess, S3Relay, Heaven, Rainbow, Sword3PaySys...), nhưng vì hành vi không đổi (chưa ai lưu con trỏ thật), rủi ro thấp.

## Các bước thực hiện

1. **Sửa `KProtocol.h`** — đổi `size_t` → `UINT32`/`DWORD` ở 4 struct nêu trên. Rebuild toàn bộ solution Win32 hiện có, xác nhận không lỗi biên dịch/không có warning liên quan (đây là thay đổi ảnh hưởng rộng nên làm và verify riêng, tách khỏi các bước x64 sau).
2. **Sửa `Common/OpaqueUserData.h`** — đổi kiểu `GetUserData`/`SetUserData`, xoá macro fallback. Rebuild Win32 lại lần nữa để xác nhận.
3. **Thêm platform x64 vào solution** cho 7 project: `JXAll.sln` + `GameServer.vcxproj`, `Core.vcxproj` (thêm "Server Debug|x64", "Server Release|x64" — **không** đụng tới Client configs vì client chưa port), `Common`, `Engine`, `LuaLibDll`, `Heaven`, `Rainbow.vcxproj`. Copy nguyên preprocessor definitions/lib dependencies từ config Win32 tương ứng sang x64.
4. **Tách output path theo platform** — hiện tại `OutDir`/`PostBuildEvent` của cả 7 project đều dùng đường dẫn cố định không chứa `$(Platform)` (ví dụ `SwordOnline\Lib\release\CoreServer.lib`, `bin\server\GameServer.exe`) → build x64 sẽ **ghi đè lên artefact Win32**. Cần thêm thư mục con `x64\` (ví dụ `SwordOnline\Lib\x64\release\`, `bin\server\x64\`) cho các config x64 mới, giữ nguyên đường dẫn Win32 cũ không đổi. Tiện thể sửa luôn bug có sẵn của `LuaLibDll` (Debug/Release ghi đè cùng 1 file output) khi đụng tới project này.
5. **Xử lý Engine/DirectX** — thử build `Engine` x64 trước tiên (đứng độc lập nhất trong 6 dependency). Nếu thiếu `ddraw.lib`/`dsound.lib`/`dxguid.lib`/`dinput8.lib` bản x64 trong SDK, tách một biến thể Engine-Server không phụ thuộc Direct* (chỉ cần cho tới khi rõ scope thật sự dùng trong GameServer — `KFile`, logging, Win32 helpers).
6. **Build theo đúng thứ tự dependency**: `Common` → `Engine` → `LuaLibDll` → `Core` (Server x64) → `Heaven` → `Rainbow` → `GameServer`.
7. **Kiểm thử tích hợp** cụm hỗn hợp: GameServer (x64) + Goddess/Bishop/S3Relay (giữ 32-bit) + 1 client S3Client (32-bit). Test luồng: GameServer khởi động, kết nối Goddess (chạm `TProcessData`), kết nối Bishop (chạm `tagGuidableInfo`), client login → chọn nhân vật → vào game → di chuyển/tương tác NPC → nhặt item → chat/tong (chạm `KProtocolProcess` dispatch + Lua script qua `LuaLibDll` x64) → thoát sạch. Theo dõi log GameServer để bắt sớm bất kỳ lỗi lệch struct nào còn sót.

## Phạm vi file dự kiến chỉnh sửa

- `SwordOnline/Headers/KProtocol.h` — 4 struct (`TProcessData`, `tagGuidableInfo`, `tagDBSyncPlayerInfo`, `tagGS2GWSaveRole`)
- `SwordOnline/Sources/MultiServer/Common/OpaqueUserData.h` — 2 chỗ
- `SwordOnline/Sources/JXAll.sln` — thêm solution config x64
- `SwordOnline/Sources/MultiServer/GameServer/GameServer.vcxproj`
- `SwordOnline/Sources/Core/Core.vcxproj`
- `SwordOnline/Sources/MultiServer/Common/*.vcxproj`
- `SwordOnline/Sources/Engine/Engine.vcxproj`
- `SwordOnline/Sources/Library/LuaLib/LuaLibDll.vcxproj`
- `SwordOnline/Sources/MultiServer/Heaven/Heaven.vcxproj`
- `SwordOnline/Sources/MultiServer/Rainbow/Rainbow.vcxproj`

## Phạm vi ảnh hưởng

- `KProtocol.h` và `OpaqueUserData.h` được include bởi **toàn bộ** project trong solution (kể cả client, Goddess, Bishop, S3Relay, Sword3PaySys) → mọi thứ phải **rebuild lại Win32** sau bước 1-2, dù thay đổi được thiết kế để không đổi hành vi 32-bit.
- 6 project phụ thuộc (Common/Engine/LuaLibDll/Core-Server/Heaven/Rainbow) build ra output dùng chung bởi cả server lẫn (một phần) client — cần cẩn thận không phá cấu hình Win32 hiện có khi thêm config x64 song song.
- Goddess/Bishop/S3Relay/S3Client/Sword3PaySys: không đổi code, nhưng phụ thuộc gián tiếp vào 2 file header sửa ở bước 1-2 nên cần rebuild + smoke-test lại.

## Xác minh

- Build solution ở cấu hình Win32 hiện có (Server Debug/Release, Client Debug/Release) sau bước 1-2 — phải xanh 100%, không warning mới liên quan `size_t`/pointer truncation.
- Build 7 project ở x64 theo đúng thứ tự — mỗi project phải link thành công trước khi sang project kế tiếp.
- Chạy cụm hỗn hợp (bước 7) và xác nhận không crash/desync khi GameServer x64 giao tiếp với Goddess/Bishop/S3Relay/client 32-bit qua các struct đã sửa.
