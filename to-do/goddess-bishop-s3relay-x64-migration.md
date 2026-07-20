# Build Goddess, Bishop, S3Relay thành bản 64-bit (x64)

## ✅ KẾT QUẢ TRIỂN KHAI — Giai đoạn 1: Bishop (2026-07-20)

**Đã build thành công thật** — `Bishop.exe` (x64) verify bằng build log MSBuild (không phải suy đoán), cả `Debug|x64` lẫn `Release|x64`, cộng với build lại toàn bộ `JXAll.sln` ở `Server Debug|x64`/`Server Release|x64` (bao gồm Bishop + 8 project x64 khác từ đợt GameServer) và regression-check `Server Debug|Win32`/`Client Debug|Win32` — cả hai đều sạch, không lỗi mới.

**Thay đổi thực hiện** (đúng như kế hoạch, cộng thêm 1 phát hiện phát sinh):

1. `Bishop.vcxproj` — thêm `Debug|x64`/`Release|x64`: ProjectConfiguration, PropertyGroup, PropertySheets, OutDir/IntDir (`Release|x64` → `.\x64\Release\`, `Debug|x64` → `..\..\..\..\bin\server\x64\Debug\`), đầy đủ `ItemDefinitionGroup` (AdditionalIncludeDirectories giống Win32), fix PCH per-file condition cho `StdAfx.cpp`, sửa 4 `<Library>` entry cũ (`common.lib`/`engine.lib` × debug/release) thêm `ExcludedFromBuild` cho x64 + thêm 4 entry mới trỏ `..\..\..\Lib\x64\{debug,release}\{common,engine}.lib`.
2. `GamePlayer.cpp:1594` — **phát sinh ngoài kế hoạch**: local `size_t datalength` không bind được vào tham số `unsigned int &uDataLength` của `CPlayerCreator::GetRoleData()` (lỗi C2664) trên x64 (Win32 thì `size_t`==`unsigned int` nên không lộ ra). Đổi khai báo local thành `unsigned int datalength = 0;` để khớp đúng kiểu tham số thật của hàm — cùng pattern đã dùng ở `KSOServer.cpp` cho GameServer, chỉ khác chiều (ở đây hàm dùng kiểu hẹp hơn, không phải kiểu rộng hơn).
3. `Bishop.vcxproj` (`Release|x64` `<Link>`) — **phát sinh ngoài kế hoạch**: `SubSystem` copy nguyên từ Win32 Release là `Windows`, nhưng `Bishop.cpp` định nghĩa `int main()` (console entry point) chứ không phải `WinMain` → lỗi link `LNK2019 unresolved external symbol WinMain`. Đây là bug có sẵn từ trước ở **Win32 Release** (không phải do đợt port này gây ra — xác nhận qua so sánh với `Debug|Win32`/`Debug|x64` đều đúng là `Console`), nhưng trước giờ không lộ ra vì Win32 Release của Bishop **chưa từng link được** (thiếu `common.lib` do thư mục `Lib/release/` rỗng — vấn đề pre-existing khác, đã ghi nhận từ đợt port GameServer). Sửa `SubSystem` của `Release|x64` thành `Console` (khớp `Debug|x64`, khớp source thật) — **chỉ sửa cho `Release|x64`, giữ nguyên `Release|Win32` không đổi** vì đó là state cũ ngoài phạm vi.
4. `JXAll.sln` — thêm mapping `Server Debug|x64`/`Server Release|x64` cho GUID Bishop (`{B697C402-667F-5BCC-E459-156F0C8F6125}`), theo đúng pattern các project khác (map vào `Debug|x64`/`Release|x64` của chính project).

**Output verify**: `bin/Server/x64/Bishop.exe` + `bin/Server/x64/release/Bishop.exe` (Release), `bin/server/x64/Debug/Bishop.exe` (Debug).

**Việc chưa làm**: Giai đoạn 2 (Goddess + S3Relay) vẫn bị chặn bởi Berkeley DB x64 như phân tích bên dưới — chưa có gì thay đổi ở đó. Test tích hợp thật (Giai đoạn 3) chưa chạy — cần môi trường ngoài CLI.

## 🔧 Dọn dẹp thư mục runtime `bin\server\x64\Debug` (2026-07-20)

Sau khi build xong, phát hiện `bin\server\x64\Debug` (thư mục chạy thật, không phải chỉ build output) đã bị **copy nguyên khối** từ `bin\server\Debug` (32-bit) vào trước đó (thủ công, ngoài phạm vi các lệnh MSBuild/PostBuildEvent của session này) — trộn lẫn exe/dll 32-bit và 64-bit trong cùng 1 thư mục. Đây chính xác là nguyên nhân "sẽ không khởi động được" mà người dùng nêu ra.

**Xác minh bằng PE header (machine type) thật, không suy đoán theo tên file** — dùng `xxd` đọc trực tiếp offset `e_lfanew` + Machine field của từng file:

- Đúng x64 (do build session này tạo ra): `GameServer.exe`, `Bishop.exe`, `Coreserver.dll`, `engine.dll`, `heaven.dll`, `Rainbow.dll`.
- **Phát hiện lỗi thật sự nghiêm trọng**: `LuaLibDll.dll` trong `bin\server\x64\Debug` là **bản 32-bit** (bị đè bởi thao tác copy nguyên khối, đè lên bản x64 đúng mà PostBuildEvent của `LuaLibDll.vcxproj` từng copy vào đây lúc port GameServer). `Coreserver.dll` (x64) LoadLibrary `LuaLibDll.dll` lúc runtime để chạy Lua quest/AI script → nếu không fix, GameServer/Bishop x64 sẽ lỗi ngay khi khởi tạo Lua (crash hoặc mất toàn bộ tính năng script). **Đã fix**: copy lại bản đúng từ `SwordOnline\Sources\Library\LuaLib\x64\Debug\LuaLibDll.dll`.
- Các file 32-bit bị copy nhầm khác (không thuộc GameServer/Bishop, không cần trong thư mục x64): `Goddess.exe`, `S3Relay.exe`, `S3RelayServer.exe`, `Sword3PaySys.exe` (+ toàn bộ `.ilk/.pdb/.log/.res/.tlog/.sbr` đi kèm), `libdb62.dll`/`libdb62d.dll` (Berkeley DB 32-bit, GameServer/Bishop không dùng BDB), `Represent2.dll`/`Represent3.dll` (rendering client-only, không liên quan server), `S3DBInterface.dll`, `ExpandPackage.dll`, `FilterText.dll`, `UpdateDLL.dll`, các exe tiện ích không liên quan (`Ham.exe`, `JxStartup*.exe`, `JxStudio.exe`, `DebugWin.exe`), và 4 DLL runtime VC6 cổ 32-bit (`MSVCP60(D).DLL`, `MSVCRT(D).DLL`, không liên quan gì tới toolchain VS2022 dùng để build GameServer/Bishop). **Đã xoá toàn bộ** — bản gốc 32-bit vẫn còn nguyên trong `bin\server\Debug`, không mất gì.
- Ghi chú phụ (không phải bug mới, chỉ là điểm lạ đã ghi nhận): `libdb62.dll` (bản Release) hoá ra là x64 **ở cả 2 thư mục** kể cả thư mục 32-bit gốc, nhưng vô hại vì Goddess luôn link bản debug (`libdb62d.dll`, đúng 32-bit) bất kể cấu hình Debug/Release — quirk này đã ghi nhận từ trước (mục Giai đoạn 2 bên dưới).

**Nguyên tắc cốt lõi (lý do tại sao không thể "dùng chung" 1 thư mục)**: Windows bắt buộc khớp bitness khi `LoadLibrary` — exe 64-bit không thể load DLL 32-bit và ngược lại. GameServer(x64)/Bishop(x64) không cần chung thư mục với Goddess/S3Relay(32-bit) để giao tiếp — chúng nói chuyện qua TCP/socket (đã đảm bảo tương thích wire-protocol từ đợt port GameServer), không qua chia sẻ DLL trong cùng thư mục.

**Quyết định của người dùng về data/config dùng chung** (`.ini`, `maps/`, `pak/`, `script/`, `settings/`, `database/`, `db/`, `dbfriend/`, `TongDB/`, `TongMemberDB/`, `logs/`): giữ **bản copy vật lý riêng** cho từng thư mục runtime (không dùng NTFS junction) — đơn giản hơn, chấp nhận rủi ro lệch cấu hình nếu sửa 1 bên mà quên sửa bên kia.

## ✅ KẾT QUẢ TRIỂN KHAI — Giai đoạn 2: Goddess + S3Relay (2026-07-20)

**Đã hoàn thành toàn bộ phần build-system** (giống hệt kế hoạch Giai đoạn 2) cho cả `Goddess.vcxproj` và `S3Relay.vcxproj`: thêm `Debug|x64`/`Release|x64` (ProjectConfiguration, PropertyGroup, PropertySheets, OutDir/IntDir, đầy đủ `ItemDefinitionGroup`, fix PCH per-file cho `StdAfx.cpp`, sửa `<Library>` entries), thêm mapping GUID vào `JXAll.sln`.

**Xác nhận bằng build thật (không suy đoán)**: cả 4 tổ hợp (Goddess Debug/Release|x64, S3Relay Debug/Release|x64) đều **compile sạch 100%** — chỉ có 1 lỗi duy nhất, xảy ra ở bước link cuối cùng: `LNK1104/LNK1181: cannot open file '..\..\..\Lib\x64\libdb62d.lib'` — đúng như dự đoán, đây là blocker Berkeley DB x64 đã biết trước, không phải lỗi phát sinh mới. Điều này chứng minh: **toàn bộ source code của Goddess/S3Relay không có gì cản trở x64** (đúng như khảo sát ban đầu — không asm, không API pointer-unsafe) — chỉ còn thiếu duy nhất file `.lib` Berkeley DB bản 64-bit.

**Riêng cho Goddess/S3Relay Release|x64**: đã chủ động đặt `SubSystem=Console` ngay từ đầu (không copy `Windows` từ Win32 Release) — rút kinh nghiệm từ bug đã gặp ở Bishop (`Goddess.cpp`/`S3Relay.cpp` đều dùng `int main()`, không phải `WinMain`) — tránh lặp lại lỗi tương tự.

**Riêng cho S3Relay**: `lualibdll.lib` trong Win32 vốn được tham chiếu bằng tên trần (`lualibdll.lib` qua `AdditionalDependencies`, không có `AdditionalLibraryDirectories` tường minh trong `.vcxproj`) — áp dụng luôn workaround đã dùng cho Core.vcxproj (đường dẫn tương đối tường minh `..\..\..\Lib\x64\{debug,release}\lualibdll.lib`) thay vì bare name, để tránh lặp lại lỗi resolve `/LIBPATH` đã gặp trước đó. Build thử xác nhận không có lỗi liên quan lualibdll.

**Phát sinh ngoài kế hoạch — race condition khi build song song (`-m`) do IntDir dùng chung**: `Goddess.vcxproj`, `Bishop.vcxproj`, `S3Relay.vcxproj` (và cả `GameServer.vcxproj`, `Sword3PaySys.vcxproj`) đều dùng chung 1 thư mục `IntDir`/`OutDir` (`bin\server\Debug\` cho Win32, `bin\server\x64\Debug\` cho x64) — đây là quirk có sẵn từ trước (đã cảnh báo qua warning MSB8028 từ đợt port GameServer/Bishop). Khi nhiều project trong số này **đồng thời cần recompile** (ví dụ sau khi sửa `.vcxproj`) và build solution bằng `msbuild /m` (multi-proc), các file trung gian trùng tên — đặc biệt `StdAfx.sbr`/`StdAfx.pch` vì **project nào cũng có file nguồn tên `StdAfx.cpp`** — bị nhiều tiến trình `cl.exe` ghi đè cùng lúc → lỗi `C1083`/`BK1506: Permission denied`. Đây **không phải lỗi do đợt port này gây ra** (cấu trúc IntDir dùng chung đã có từ trước), nhưng **dễ bị kích hoạt hơn** khi càng nhiều project cùng có config x64 mới (càng nhiều project "dirty" cùng lúc). Build lại **tuần tự** (bỏ `/m`) xác nhận sạch hoàn toàn — không phải lỗi thật. Ghi chú cho tương lai: nếu build solution đầy đủ bằng `msbuild /m` từ command line và gặp lỗi `Permission denied` liên quan `StdAfx.sbr`/`.pch`, thử lại tuần tự (bỏ `/m`) hoặc build qua Visual Studio IDE (IDE tự tuần tự hoá theo dependency graph, ít gặp race hơn CLI `/m` thô).

**Đã fix 1 lỗi phát sinh khác ngoài phạm vi trực tiếp**: lúc dọn dẹp `bin\server\x64\Debug` trước đó (xem mục "Dọn dẹp thư mục runtime" ở trên), lệnh xoá `*.sbr` hàng loạt đã lỡ xoá luôn cả `.sbr` hợp lệ của `GameServer`/`Bishop` (do dùng chung thư mục) — khiến `bscmake` báo lỗi `BK1506` khi build lại solution x64. Fix bằng cách `-t:Rebuild` lại `GameServer.vcxproj` và `Bishop.vcxproj` (Debug|x64) để sinh lại đầy đủ `.sbr`.

## ✅ KẾT QUẢ TRIỂN KHAI — Blocker Berkeley DB x64: ĐÃ GỠ (2026-07-20)

Người dùng tự build Berkeley DB 6.2.32 x64 từ source Oracle (project VS đi kèm sẵn trong `build_windows/`), output tại `db-6.2.32/build_windows/{Win32,x64}/{Debug,Release}/`. Đã verify bằng PE header (`machine=8664`) rằng cả `libdb62d.dll`/`.lib` (x64/Debug) và `libdb62.dll`/`.lib` (x64/Release) đúng là bản 64-bit thật, không phải copy nhầm.

**Đã tích hợp**:
- Copy `libdb62d.lib`/`libdb62.lib` (import lib, dùng lúc link) vào `SwordOnline/Lib/x64/` — đúng vị trí đã cấu hình sẵn trong `Goddess.vcxproj`/`S3Relay.vcxproj` từ trước, không cần sửa gì thêm trong `.vcxproj`.
- Copy `libdb62d.dll`/`libdb62.dll` (runtime DLL, cần lúc chạy) vào cả 3 thư mục output x64 dùng để launch server (`bin/server/x64/`, `bin/server/x64/Debug/`, `bin/server/x64/release/`) — **lưu ý đây là bước KHÔNG có trong plan gốc và KHÔNG được tự động hoá bởi `.vcxproj`/PostBuildEvent nào** (giống hệt cách Win32 vốn cũng chỉ có DLL nằm sẵn trong `bin\server\debug\` do đặt tay, không qua build system) — nếu thiếu bước này, `Goddess.exe`/`S3Relay.exe` x64 sẽ **link được nhưng không khởi động được** (thiếu DLL runtime).

**Xác nhận build thật**: cả 4 tổ hợp (`Goddess`/`S3Relay` × `Debug|x64`/`Release|x64`) đều **link thành công hoàn toàn**, không còn lỗi `LNK1104`/`LNK1181` nào. Build lại toàn bộ `JXAll.sln` ở `Server Debug|x64` — **sạch 100%, không còn lỗi nào** (lần đầu tiên toàn bộ 11 project x64 — Common/Engine/LuaLibDll/Core/Heaven/Rainbow/JpgLib/KMp3LibClass/GameServer/Bishop/Goddess/S3Relay — build thành công cùng lúc). Regression-check `Server Debug|Win32` cũng sạch.

**Một lỗi nhỏ phát sinh và đã fix**: build S3Relay Debug|x64 lần đầu (không phải rebuild) báo `BSCMAKE: error BK1513: nonincremental update requires all .SBR files` — cùng họ với race/quirk IntDir-dùng-chung đã ghi nhận ở trên, không phải lỗi link/code. Fix bằng `-t:Rebuild` cho S3Relay, sạch ngay.

**Giai đoạn 2 (Goddess + S3Relay): HOÀN TẤT TOÀN BỘ** — cả build-system lẫn dependency Berkeley DB đều đã xong và verify bằng build thật.

## ✅ KẾT QUẢ TRIỂN KHAI — Giai đoạn 3: Khởi động thật cả cụm 4 server (2026-07-20)

**Đã khởi động thật** cả 4 server theo đúng thứ tự Goddess → Bishop → S3Relay → GameServer từ `bin\server\x64\Debug` (dùng `Start-Process` để có console thật thay vì pipe — launch qua pipe khiến `GetConsoleMode` fail và Goddess tự thoát sớm dù không phải bug, chỉ là môi trường launch không có console đính kèm).

**2 bug runtime thật phát hiện và đã fix** trong quá trình test (chi tiết đầy đủ ở `to-do/gameserver-x64-migration.md` mục "Test tích hợp thật + 2 bug runtime"):
1. `FilterText.dll` thiếu bản x64 → Goddess hiện `MessageBox` lỗi rồi thoát ngay khi khởi động (đã port `FilterText.vcxproj` sang x64).
2. `long dir = _findfirst(...)` cắt cụt handle trong `Core/Src/KSortScript.cpp` → GameServer crash lúc quét thư mục `script/`; tìm thấy đồng thời lỗi tương tự `GetGameData`/`OperationRequest` truyền con trỏ qua `unsigned int` (cắt cụt trên x64) → GameServer crash lúc xử lý gói tin Gateway. Cả hai đều đã fix + verify qua crash dump (WinDbg).

**Kết quả sau fix**: cả 4 server chạy ổn định >25 giây (mốc trước đây GameServer luôn crash trong 10-15s), có kết nối ESTABLISHED đầy đủ giữa Goddess↔Bishop↔S3Relay↔GameServer, không sinh crash dump mới, đọc log xác nhận `[Goddess] start success`/`[Bishop] start success` sạch.

## Việc còn lại

**Test tích hợp với client thật** (S3Client 32-bit kết nối vào cụm x64) vẫn chưa chạy — mới xác nhận 4 server nói chuyện được với nhau, chưa có client thật vào game. Cần người dùng tự làm (ngoài khả năng môi trường CLI): test đủ luồng login → chọn nhân vật → vào game → chat/tong → thao tác DB qua Berkeley DB x64. Đặc biệt cần chú ý test kỹ thao tác đọc/ghi Berkeley DB (tạo/xoá nhân vật, lưu tiến độ) vì đây là lần đầu chạy engine lưu trữ ở bản 64-bit — dù Berkeley DB nhìn chung portable giữa các kiến trúc cùng version, vẫn nên test với 1 bản copy dữ liệu trước khi dùng production, chưa giả định an toàn 100%.

## Thông tin khảo sát

| | |
|---|---|
| Branch khảo sát | `main` |
| Commit ID | `9f24f9861c66e7e4379da68ce2fcdc8f9e07a68f` |
| Commit đó | `update To-Do` (2026-07-20 10:49:57 +0700) |
| Ngày khảo sát/lên kế hoạch | 2026-07-20 |
| Working tree lúc khảo sát | Có nhiều thay đổi chưa commit từ phiên port GameServer trước đó (`KProtocol.h`, `Common/*`, `Engine/*`, `Core.vcxproj`, `JXAll.sln`, các `.vcxproj` của Common/JpgLib/KMp3LibClass/LuaLibDll/Engine/Core/Heaven/Rainbow/GameServer, và nhiều file `.lib` build-output) — **kế hoạch này giả định các thay đổi đó đã có sẵn trong cây nguồn** (chưa cần commit, nhưng phải tồn tại trên máy build). |

> Toàn bộ đường dẫn file/dòng trích dẫn bên dưới được xác nhận đúng tại thời điểm khảo sát trên. Nếu code đã thay đổi nhiều, nên khảo sát lại trước khi triển khai.

## Bối cảnh

Đây là phần tiếp theo của việc port GameServer sang x64 (xem `to-do/gameserver-x64-migration.md` — **đã triển khai xong**, verify bằng build thật). Giờ mở rộng sang 3 server còn lại trong cụm cluster: **Goddess** (DB server), **Bishop** (cluster manager), **S3Relay** (relay/social proxy).

**Tin rất tốt**: nhờ phiên port GameServer trước, **8 trong 9 dependency dùng chung đã có sẵn bản x64 và đã build thành công**: `Common`, `Engine`, `LuaLibDll`, `Core` (Server Debug/Release|x64), `Heaven`, `Rainbow` (JpgLib/KMp3LibClass cũng có nhưng Goddess/Bishop/S3Relay không cần tới chúng). Goddess/Bishop/S3Relay chỉ cần thêm config x64 cho **chính project của chúng**, không cần đụng lại các dependency đã xong.

**Tin xấu**: phát hiện 1 blocker nghiêm trọng — **Berkeley DB** (thư viện thứ 3, không có source trong repo) được **Goddess và S3Relay** link trực tiếp, và chỉ có bản 32-bit.

## Khảo sát nhanh: cả 3 đều KHÔNG có asm x86

Đã grep `__asm`/`_asm\b` trên toàn bộ `Goddess/`, `Bishop/`, `S3Relay/` — **0 kết quả cho cả 3 project**. Đây là khác biệt lớn so với GameServer (phải port hàng chục hàm asm trong Common/Engine/JpgLib) — vì Goddess/Bishop/S3Relay không dùng chung code với Engine ở mức sâu như vậy, và bản thân code riêng của chúng thuần logic nghiệp vụ, không có tối ưu tay bằng asm. **Đây là phần việc nặng nhất của GameServer đã không lặp lại ở đây.**

## Từng project

### Bishop — không có blocker, giống hệt pattern GameServer

- **Dependency**: `common.lib`, `engine.lib` (cả debug/release, đã excluded đúng theo cấu hình) — **cả hai đã có bản x64 sẵn**.
- **Không có** Berkeley DB, không có `esclient.lib` độc lập. `SmartClient.cpp`/`.h` (class `CSmartClient`) dùng `IClient` — chính là interface của **Rainbow.dll** (đã có bản x64) — không phải file `esclient.lib` prebuilt nằm trong `SwordOnline/Lib/` (lo ngại trong memory cũ về `esclient.lib`/`iocpserver.lib` **không áp dụng cho Bishop**, tương tự kết luận đã có với GameServer).
- **Không có asm.**
- Link cứng `..\Lib\debug\engine.lib` (Debug) / `..\Lib\release\engine.lib` (Release) — cần thêm 2 entry x64 tương ứng trỏ `..\Lib\x64\debug\engine.lib` / `..\Lib\x64\release\engine.lib`, y hệt cách đã làm cho GameServer.
- **Kết luận: port Bishop gần như chỉ là công việc cơ khí (thêm config x64 vào .vcxproj) — không có rủi ro kỹ thuật mới.**

### Goddess — có blocker Berkeley DB

- **Dependency**: `common.lib`, `engine.lib` (đã có x64), và **`libdb62d.lib`** (Berkeley DB, **link không điều kiện** — dùng cho cả Debug lẫn Release, không có bản `libdb62.lib` release riêng dù file đó tồn tại trong `Lib/` — nghĩa là **Goddess luôn link bản debug của Berkeley DB, kể cả ở Release build** — quirk có sẵn từ trước, không phải lỗi cần sửa, chỉ cần lặp lại đúng hành vi này cho x64).
- **Dùng Berkeley DB thật sự** (không chỉ link cho có): `DBTable.cpp`, `DBBackup.cpp`, `DBDumpLoad.cpp`, `db.h` (define `DB_VERSION_MAJOR 6`, `DB_VERSION_MINOR 2`, `DB_VERSION_PATCH 32` → **Oracle Berkeley DB 12c Release 1, phiên bản 6.2.32**) — đọc/ghi dữ liệu nhân vật/role trực tiếp qua Berkeley DB.
- **`SwordOnline/Lib/` chỉ có file `.lib` 32-bit đã build sẵn** (`libdb41s.lib`, `libdb62.lib`, `libdb62d.lib`) — **không có source Berkeley DB trong repo**, nên không thể "port" như đã làm với asm — phải có bản x64 thật.
- Không có asm trong Goddess tự thân.
- **Kết luận: Goddess KHÔNG THỂ build x64 cho tới khi có `libdb62(d).lib` bản x64.**

### S3Relay — cũng dính blocker Berkeley DB, nhưng ít dependency hơn Goddess

- **Dependency**: chỉ `engine.lib` (đã có x64) + **`libdb62d.lib`** (Berkeley DB, cũng link không điều kiện cho cả Debug/Release, cùng quirk như Goddess) + `lualibdll.lib` (khai trong `AdditionalDependencies`, đã có bản x64).
- **Không link `common.lib`** — S3Relay có bộ networking/threading riêng của nó (`NetServer.cpp`, `NetClient.cpp`, `SockThread.cpp`, `KThread.cpp`, `Memory.cpp`, `Lock.cpp`...), không dùng chung `Common/` với các server khác. Dùng Heaven/Rainbow qua wrapper riêng (`HeavenLib.cpp`, `RainbowLib.cpp`) — load `heaven.dll`/`rainbow.dll` lúc runtime giống các server khác, không cần link tĩnh gì thêm.
- `DBTable.cpp`, `db.h` cũng xuất hiện — S3Relay cũng đọc/ghi Berkeley DB trực tiếp (không chỉ Goddess mới dùng).
- Không có asm.
- **Kết luận: cùng blocker Berkeley DB như Goddess, nhưng về mặt build-system thì đơn giản hơn (ít Library entry hơn, không phụ thuộc `common.lib`).**

## 🚧 Blocker chính: Berkeley DB 6.2.32 chưa có bản x64

Đây là điều kiện tiên quyết phải giải quyết **trước khi** làm Goddess và S3Relay (Bishop thì không bị ảnh hưởng, có thể làm độc lập ngay).

**3 phương án** (không phương án nào có thể tự làm trong môi trường CLI hiện tại — cần quyết định/thao tác từ người dùng):

1. **Tải Oracle Berkeley DB 12c Release 1 (6.2.32) source** từ Oracle (cần đăng ký tài khoản Oracle, chấp nhận license — bản cũ như 6.2.32 vẫn tải được qua Oracle Software Delivery Cloud hoặc archive). Bộ source đi kèm sẵn solution/project file cho Visual Studio (`build_windows/`), build ra `libdb62.lib`/`libdb62d.lib` bản x64 bằng chính VS2022 đang dùng cho repo này. Đây là phương án **chính xác nhất về mặt tương thích** (cùng version, cùng toolchain).
2. **Tìm bản prebuilt x64 Berkeley DB 6.2.x** từ nguồn thứ 3 (vd. vcpkg có gói `berkeleydb` nhưng thường là bản mới hơn nhiều — **rủi ro tương thích on-disk format** nếu khác major/minor version, cần test kỹ với dữ liệu DB thật trước khi dùng production).
3. **Đổi sang engine lưu trữ khác** (di dời dữ liệu sang SQLite/khác) — phương án lớn nhất, ngoài phạm vi "port x64", chỉ nên cân nhắc nếu build BDB x64 thực sự bế tắc.

**Khuyến nghị: phương án 1** (tự build từ source Oracle) — rủi ro thấp nhất, giữ nguyên format DB hiện có, không cần di dời dữ liệu.

**Việc cần làm trước khi tôi (hoặc ai đó) code tiếp Goddess/S3Relay**: người dùng tải & build `libdb62.lib` + `libdb62d.lib` bản x64, đặt vào `SwordOnline/Lib/x64/{debug,release}/` (theo đúng quy ước thư mục đã dùng cho các lib khác trong đợt port GameServer).

## Kế hoạch triển khai

### Giai đoạn 1 — Bishop (làm ngay, không phụ thuộc gì)

1. Thêm `Debug|x64`/`Release|x64` vào `Bishop.vcxproj` — sao chép cấu trúc `ItemDefinitionGroup` từ Debug|Win32/Release|Win32, đổi output path sang `x64\Debug\`/`x64\Release\`, output copy đích `bin\server\x64\...` (giống hệt pattern đã áp dụng cho `GameServer.vcxproj`, `Heaven.vcxproj`... trong đợt trước).
2. Sửa 4 `<Library>` entry (`common.lib`, `engine.lib` × debug/release) — thêm `ExcludedFromBuild Condition="'$(Platform)'=='x64'"` cho 4 entry Win32 cũ, thêm 4 entry mới trỏ `..\Lib\x64\{debug,release}\{common,engine}.lib`.
3. Sửa PCH per-file condition cho `StdAfx.cpp` — thêm điều kiện `Debug|x64`/`Release|x64` (bài học từ GameServer: quên bước này sẽ ra lỗi `C1083 Cannot open precompiled header`).
4. Build thử `msbuild Bishop.vcxproj /p:Configuration=Debug /p:Platform=x64`, rồi `Release`, fix lỗi phát sinh (dự kiến rất ít vì không có asm/lib lạ).
5. Thêm `Debug|x64`/`Release|x64` cho GUID Bishop (`{B697C402-667F-5BCC-E459-156F0C8F6125}`) vào `JXAll.sln` (map trực tiếp `Debug|x64`→`Debug|x64`, `Release|x64`→`Release|x64`, không có tiền tố "Server" như Core).
6. Build lại toàn solution ở `Debug|x64`/`Release|x64` để xác nhận không phá gì, và build lại Win32 (`Debug`, `Release`) để regression-check.

### Giai đoạn 2 — Goddess + S3Relay (BỊ CHẶN bởi Berkeley DB x64)

Sau khi có `libdb62.lib`/`libdb62d.lib` bản x64 (xem mục Blocker ở trên):

1. Copy `libdb62.lib` → `SwordOnline/Lib/x64/release/`, `libdb62d.lib` → `SwordOnline/Lib/x64/debug/` (hoặc theo đúng vị trí thật mà 2 project này reference — cần double-check đường dẫn tương đối trong `.vcxproj`, vì hiện tại cả hai đều reference `..\..\..\Lib\libdb62d.lib` — **không nằm trong thư mục con `debug\`/`release\` như common.lib/engine.lib** — tức là bản x64 nên đặt tại `SwordOnline/Lib/x64/libdb62d.lib` để giữ đúng cấu trúc tương đối, KHÔNG phải `Lib/x64/debug/`. Cần xác nhận lại khi thực sự có file để tránh sai đường dẫn).
2. **Goddess.vcxproj**: thêm `Debug|x64`/`Release|x64`, sửa `<Library>` (common.lib, engine.lib × debug/release + `libdb62d.lib` trỏ bản x64 — lưu ý libdb62d.lib link **không điều kiện** cho cả 2 config, giữ nguyên quirk này), fix PCH per-file cho `StdAfx.cpp`, build thử, fix lỗi.
3. **S3Relay.vcxproj**: thêm `Debug|x64`/`Release|x64`, sửa `<Library>` (engine.lib × debug/release + `libdb62d.lib`), fix PCH cho `StdAfx.cpp`, build thử, fix lỗi. Không cần sửa gì liên quan `common.lib` (không dùng).
4. Thêm 2 GUID vào `JXAll.sln` (Goddess `{E673C82E-ECBE-1ADA-F51A-05631F772A69}`, S3Relay `{A27D6D86-FBD5-1992-0ABC-0675DA76CFBE}`).
5. Build lại toàn solution x64 + regression-check Win32.

### Giai đoạn 3 — Kiểm thử tích hợp (ngoài khả năng CLI, cần người dùng tự chạy)

Chạy cụm server hỗn hợp: Bishop/Goddess/S3Relay (x64) + GameServer (x64, đã xong từ trước) + client 32-bit thật — xác nhận toàn bộ luồng login → chọn nhân vật → vào game → chat/tong → thao tác DB (tạo/xoá nhân vật, lưu tiến độ) hoạt động đúng qua Berkeley DB x64.

## Phạm vi file dự kiến chỉnh sửa

- `SwordOnline/Sources/MultiServer/Bishop/Bishop.vcxproj`
- `SwordOnline/Sources/MultiServer/Goddess/Goddess.vcxproj`
- `SwordOnline/Sources/MultiServer/S3Relay/S3Relay.vcxproj`
- `SwordOnline/Sources/JXAll.sln` (thêm mapping cho 3 GUID trên)
- (Nếu build BDB x64 từ source) — không đụng gì trong repo, chỉ thêm file `.lib` mới vào `SwordOnline/Lib/x64/`

## Rủi ro / điểm cần chú ý khi triển khai thật

- **Quên PCH per-file condition cho `StdAfx.cpp`** — lỗi phổ biến nhất đã gặp nhiều lần khi port GameServer, luôn phải nhớ thêm điều kiện `Debug|x64`/`Release|x64` song song với `Debug|Win32`/`Release|Win32`.
- **`/LIBPATH` tương đối không resolve đúng** khi build qua msbuild trực tiếp (đã gặp với `lualibdll.lib` ở Core) — nếu gặp lỗi "unresolved external" dù file `.lib` tồn tại đúng chỗ, thử chỉ đường dẫn tường minh trong `AdditionalDependencies` thay vì chỉ tên file + `AdditionalLibraryDirectories`.
- **Tương thích on-disk format của Berkeley DB** giữa bản 32-bit hiện có và bản 64-bit mới build — Berkeley DB nhìn chung portable giữa các kiến trúc cùng version nhờ page format riêng, nhưng **bắt buộc phải test đọc/ghi thật với 1 bản copy dữ liệu** trước khi đưa vào production, không giả định an toàn 100%.
- Goddess link **debug** `engine.lib`/Berkeley DB **cho cả Release build** (quirk có sẵn) — khi thêm x64 phải lặp lại đúng quirk này (không "sửa" thành đúng chuẩn, để tránh thay đổi hành vi ngoài phạm vi yêu cầu).
