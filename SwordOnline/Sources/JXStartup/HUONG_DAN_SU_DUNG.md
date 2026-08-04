# JXStartup - Huong dan su dung

Cong cu quan ly khoi dong 6 server cua Jian Xia Qing Yuan (Sword Online) qua giao dien do hoa: **Sword3PaySys, S3RelayServer, Goddess, Bishop, S3Relay, GameServer**. Thay vi mo tay 6 cua so terminal, ban dung 1 ung dung duy nhat de bat/tat, xem log va theo doi tinh trang tung server.

## 1. Vi tri dat file

`JXStartup.exe` phai duoc dat **cung thu muc** voi 6 file exe cua server (vi du `bin/server/x64/Debug/`). Ung dung tu dong dung thu muc chua chinh no lam noi chua cac server, khong can cau hinh gi them.

Sau khi build project `JXStartup` trong `JXAll.sln` (cau hinh Debug/Release, Win32/x64), file exe se tu dong duoc copy vao dung vi tri nay.

## 2. Giao dien chinh

```
┌──────────────────────────────────────────────────────────┐
│  Dich vu | Trang thai | PID | Thoi gian chay | CPU% | RAM │  <- Bang danh sach 6 server
├──────────────────────────────────────────────────────────┤
│ [Start][Stop][Restart][Force Stop][Start All][Stop All][Export Log] │
├──────────────────────────────────────────────────────────┤
│ [Sword3PaySys][S3RelayServer][Goddess][Bishop][S3Relay][GameServer] │  <- Tab log
│                                                            │
│   (noi dung log cua server dang chon, nen toi)            │
│                                                            │
├──────────────────────────────────────────────────────────┤
│  San sang.                                                │  <- Status bar
└──────────────────────────────────────────────────────────┘
```

- **Bang danh sach**: click 1 dong de chon server do lam muc tieu cho cac nut Start/Stop/Restart/Force Stop.
  - Cot **Trang thai** to mau: xanh la = dang chay, do = loi/crash, cam = dang khoi dong hoac dang dung, xam = da dung.
  - Cot **CPU%/RAM** to mau do khi vuot nguong canh bao (CPU >= 90%, RAM >= 1GB).
- **Tab log**: moi server co 1 tab rieng, click de xem log cua server do.

## 3. Cac thao tac co ban

| Nut | Y nghia |
|---|---|
| **Start** | Khoi dong server dang duoc chon trong bang |
| **Stop** | Dung nhe nhang (graceful) server dang chon |
| **Force Stop** | Buoc dung ngay lap tuc (kill), dung khi Stop khong phan hoi |
| **Restart** | Dung roi tu dong khoi dong lai server dang chon |
| **Start All** | Khoi dong toan bo 6 server theo dung thu tu phu thuoc, moi server cach nhau 1.5 giay |
| **Stop All** | Dung toan bo theo thu tu nguoc lai |
| **Export Log** | Xuat toan bo log hien tai cua 6 tab ra 1 file `.zip` de gui debug |

**Thu tu khoi dong mac dinh** (dung khi bam Start All): Sword3PaySys -> S3RelayServer -> Goddess -> Bishop -> S3Relay -> GameServer. Day la thu tu phu thuoc thuc te cua he thong (GameServer can Goddess/Bishop san sang truoc), nen luon dung Start All thay vi tu bam tung nut neu khong chac chan ve thu tu.

## 4. Doc mau sac trong log

Log duoc to mau theo muc do de de nhan biet:

- **Xanh duong nhat**: thong bao cua chinh JXStartup (vi du "Da khoi dong, PID = ...")
- **Do**: loi (`[ERROR]`, tien trinh thoat ngoai y muon, khong tim thay file...)
- **Cam**: canh bao (`[WARN ]`)
- **Xam sang**: log binh thuong cua server

### Vi sao co server khong hien log truc tiep?

- **GameServer**: doc log real-time qua pipe (khong co gioi han).
- **Sword3PaySys, Goddess, Bishop, S3Relay**: 4 server nay tu quan ly console rieng (an) nen JXStartup khong redirect stdio truc tiep duoc; thay vao do JXStartup **tu tim va theo doi file log** ma chung tu ghi ra (`logs/<ten>/`). Co the tre vai giay luc moi start (server can thoi gian ghi dong log dau tien).
- **S3RelayServer**: chay Windows subsystem, khong co console va khong ghi file log kieu KGLog, nen JXStartup **khong doc duoc log truc tiep**. Cot "Ghi chu" trong bang se ghi ro dieu nay. Muon xem log cua no thi mo cua so cua no (JXStartup se hien cua so that cua S3RelayServer, khong an).

## 5. Theo doi tai nguyen (CPU/RAM)

Moi giay JXStartup do CPU% va RAM cua tung server dang chay. Neu:
- CPU >= 90% (tinh tren 1 loi, khong chia cho tong so loi CPU)
- RAM >= 1024 MB

thi o tuong ung to do va **1 dong canh bao duoc ghi vao tab log** cua server do (chi ghi 1 lan khi vua vuot nguong, khong lap lai moi giay).

## 6. Thu nho xuong khay he thong (System Tray)

- Bam nut **thu nho** (minimize) tren cua so: JXStartup an khoi taskbar, chi con icon o khay he thong (system tray), **van chay ngam va tiep tuc theo doi/ghi log binh thuong**.
- **Double-click** vao icon khay: mo lai cua so.
- **Click chuot phai** vao icon khay: hien menu nhanh - Mo JXStartup / Start All / Stop All / Thoat.
- Mau icon phan anh trang thai tong the: xanh la (co server dang chay on dinh), vang (dang xu ly start/stop), do (co server loi/crash), xam (tat ca da dung).

Luu y: nut **X** (dong cua so) van la thoat that su (se hoi co dung cac server dang chay truoc khi thoat khong), khac voi nut thu nho.

## 7. Xuat log de debug (Export Log)

Bam **Export Log**: JXStartup gom noi dung hien tai cua ca 6 tab log + 1 file `summary.txt` (trang thai, PID, CPU, RAM tung server tai thoi diem xuat) thanh 1 file `JXStartup_Logs_<ngay_gio>.zip` luu canh file exe, roi tu mo Explorer tro toi file do. Gui file zip nay cho nguoi ho tro ky thuat la du thong tin can thiet.

## 8. Cac co che bao ve

- **Chi mo duoc 1 JXStartup cung luc**: mo lan 2 se tu dong dong lai va focus vao cua so dang chay san, khong tao instance moi.
- **Chan start server da chay san ben ngoai**: neu 1 file .exe server (vi du GameServer.exe) da dang chay (do tu mo tay, hoac con sot lai tu lan truoc), JXStartup se **tu choi start** va ghi ro ly do trong log, tranh 2 tien trinh tranh chap cong ket noi. Trong truong hop nay hay dung tien trinh cu (qua Task Manager hoac chinh JXStartup neu no dang duoc theo doi) roi thu lai.

## 9. Xu ly su co thuong gap

**Server bam Start xong bien mat/khong thay gi trong log?**
Kiem tra dong "Ghi chu" trong bang - neu ghi "Kiem tra tab log de biet chi tiet" nghia la server da thoat ngay sau khi khoi dong (crash). Mo tab log cua no de xem dong loi cuoi cung.

**Bam Start All nhung GameServer khong ket noi duoc Database?**
GameServer can Goddess (va thuong ca Bishop) da san sang truoc. Neu chay tung nut rieng le, hay dam bao khoi dong dung thu tu o Muc 3. Neu dung Start All thi thu tu da tu dong dam bao, chi can doi du thoi gian (moi buoc cach nhau 1.5s, cong them thoi gian tung server tu ket noi).

**Log server dung KGLog (Sword3PaySys/Goddess/Bishop/S3Relay) khong hien gi ngay sau khi Start?**
Binh thuong - server can vai giay de ghi dong log dau tien, JXStartup se tu dong tim thay file va bat dau hien. Neu doi qua 10-15s van khong co gi, kiem tra thu muc `logs/<ten server>/` xem file log co duoc tao khong.

**Ly do khong thay log cua S3RelayServer?**
Xem giai thich o Muc 4 - day la gioi han ky thuat (Windows subsystem, khong co console/khong ghi file log), khong phai loi.

## 10. Cau truc source

```
SwordOnline/Sources/JXStartup/
├── JXStartup.cpp        - entry point (WinMain), single-instance guard
├── MainWnd.h/.cpp        - toan bo giao dien (Win32 API thuan), tray icon, custom draw
├── ServiceManager.h/.cpp - quan ly tien trinh: start/stop/tick, doc log (pipe/file-tail),
│                           theo doi CPU/RAM, chan trung tien trinh
├── ZipWriter.h/.cpp      - tu viet file .zip (STORED, khong nen), khong can thu vien ngoai
├── StdAfx.h/.cpp         - precompiled header
├── resource.h            - dinh nghia ID control, message
└── JXStartup.manifest    - khai bao Common Controls v6 + DPI awareness
```

Build bang Visual Studio 2022 (mo `JXAll.sln`, chon project `JXStartup`), khong can cai dat gi them (khong MFC, khong .NET, CRT tinh - chi can copy exe la chay duoc tren may khac).
