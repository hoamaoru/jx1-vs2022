# Fix crash GameServer: gọi AddProp / AddMagicPoint (Lua script) làm văng GS không log

## Bối cảnh

Script NPC gọi các hàm sau làm GameServer crash ngay lập tức, không có bất kỳ log nào (kể cả log lỗi trong `logs/`):

```lua
-- bin/.../script/skill/npcchucnang/trogiup.lua
function nhandiemtnkn()
    AddProp(50000);
    AddMagicPoint(100);
    KickOutSelf()
end
```

Chỉ cần gọi `AddProp(50000)` là đủ để crash — 2 dòng sau không kịp chạy.

## Nguyên nhân gốc

`S2C_PLAYER_SYNC` ([SwordOnline/Headers/KProtocol.h:2356-2365](SwordOnline/Headers/KProtocol.h#L2356-L2365)) dùng field `LPVOID m_lpBuf` theo 2 cách khác nhau, xung đột nhau:

1. **Cách dùng chính thức của protocol**: một số hàm nhét thẳng giá trị `int` vào field con trỏ này để "cõng" dữ liệu qua dây (do struct có kích thước cố định, không có field int riêng). Phía client đọc lại đúng bằng cách ép kiểu ngược: `*(int*)(&pSync->m_lpBuf)` ([KProtocolProcess.cpp:4291](SwordOnline/Sources/Core/Src/KProtocolProcess.cpp#L4291)). Đây là chủ đích, không phải bug.
2. **Destructor của struct lại coi field đó là buffer thật do `new[]` cấp phát**:
   ```cpp
   ~tagS2C_PLAYER_SYNC() { Release(); }
   void Release() { if (m_lpBuf) delete [] m_lpBuf; m_lpBuf = NULL; }
   ```

Khi một hàm gán `sMsg.m_lpBuf = (LPVOID)<một giá trị int>;` rồi để biến `sMsg` (khai báo local) ra khỏi scope, destructor sẽ gọi `delete[] (void*)<giá trị int đó>` — free một địa chỉ không phải do `new[]` cấp phát → heap corruption → CRT/OS kill tiến trình ngay lập tức. Lỗi này **không đi qua** try/catch hay logger nào của server (đây là fast-fail cấp OS/CRT khi phát hiện heap corruption), nên không để lại log — khớp đúng triệu chứng đã gặp.

Cụ thể với `AddProp(50000)`: hàm `LuaAddPropPoint` gán `sMsg.m_lpBuf = (LPVOID)Player[nPlayerIndex].m_nAttributePoint;` (giá trị ~50000 sau khi cộng dồn) rồi `return 0;` — sMsg hủy ngay sau đó → `delete[] (void*)50000` → crash.

## Phạm vi lỗi

Đây là bug hệ thống, không chỉ riêng `AddProp`/`AddMagicPoint`. Grep toàn repo tìm thấy 6 vị trí bị cùng lỗi (gán giá trị non-zero vào `m_lpBuf` mà không reset trước khi `sMsg` hủy):

| File | Hàm | Giá trị nhét vào `m_lpBuf` |
|---|---|---|
| `ScriptFuns.cpp` | `LuaAddMagicPoint` (Lua: `AddMagicPoint`) | `m_nSkillPoint` |
| `ScriptFuns.cpp` | `LuaAddPropPoint` (Lua: `AddProp`/`AddPropPoint`) | `m_nAttributePoint` |
| `ScriptFuns.cpp` | `LuaInput` (Lua: `Input`) | `max` |
| `KPlayer.cpp` | `SetLockState` | `bLock` |
| `KPlayer.cpp` | `SetEquipExpandTime` | `dwTime` |
| `KPlayer.cpp` | `SetExpandBoxNum` | `nNum` |

Đáng chú ý: chính tác giả gốc từng gặp và vá đúng bug này một lần ở `KPlayerPK.cpp` (commit [`a1f5453` "fix crash change pk"](https://github.com/duccom0123/jx1-vs2022/commit/a1f54536d17c66b02a2ad8a14e1a09a2dc07205d)), nhưng 6 vị trí trên chưa được vá theo cùng cách.

## Cách fix

Thêm `sMsg.m_lpBuf = 0;` ngay sau lệnh `g_pServer->PackDataToClient(...)`, trước khi biến `sMsg` ra khỏi scope — đúng pattern đã dùng ở `KPlayerPK.cpp`.

```cpp
sMsg.m_lpBuf = (LPVOID)Player[nPlayerIndex].m_nAttributePoint;
g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
sMsg.m_lpBuf = 0;   // <-- dòng thêm: tránh destructor delete[] một địa chỉ rác
return 0;
```

**Vì sao đây là fix đúng, không phải vá tạm:**
- `delete[] nullptr` được chuẩn C++ đảm bảo là no-op tuyệt đối — sau khi reset về 0, không còn đường nào dẫn tới invalid free nữa.
- `PackDataToClient` copy dữ liệu **đồng bộ** vào write-buffer riêng của nó trước khi return (xem `CIOCPServer::PackDataToClient`, [ServerStage.cpp:385-423](SwordOnline/Sources/MultiServer/Heaven/ServerStage.cpp#L385-L423)) — nên reset `m_lpBuf` sau dòng `PackDataToClient` không ảnh hưởng gì tới dữ liệu đã gửi cho client.
- Không đụng tới format protocol (không sửa gì phía client).

## File đã sửa

- `SwordOnline/Sources/Core/Src/ScriptFuns.cpp` — 3 dòng thêm (`LuaAddMagicPoint`, `LuaAddPropPoint`, `LuaInput`). File này encoding ISO-8859-1, phải sửa bằng byte-splice (`head`/`tail`/`printf`) để không làm hỏng comment tiếng Trung/Việt trong file — xem mục Encoding trong `CLAUDE.md`.
- `SwordOnline/Sources/Core/Src/KPlayer.cpp` — 3 dòng thêm (`SetLockState`, `SetEquipExpandTime`, `SetExpandBoxNum`).

## Kiểm chứng

- `git diff --stat` xác nhận mỗi file chỉ thay đổi đúng 3 dòng, không có thay đổi ngoài ý muốn.
- `file <path>` xác nhận `ScriptFuns.cpp` vẫn là ISO-8859 text sau khi sửa (không bị re-encode sang UTF-8).
- Cần build lại Server Debug/Release và gọi lại `nhandiemtnkn()` (hoặc bất kỳ script nào gọi `AddProp`/`AddMagicPoint` với giá trị khác 0) để xác nhận hết crash.
