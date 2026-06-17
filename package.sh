#!/bin/bash
# Script đóng gói ứng dụng Cafe TUI cho Linux và Windows

echo "=== DONG GOI UNG DUNG CAFE TUI ==="

# 1. Tạo thư mục dist
mkdir -p dist

# 2. Đóng gói cho Linux
echo "[+] Đang chuẩn bị gói Linux..."
# Đảm bảo đã biên dịch
mkdir -p build
cd build
cmake -DSTATIC_BUILD=ON ..
make -j$(nproc)
if [ $? -eq 0 ]; then
    strip cafe_tui
    cd ..
    # Tạo cấu trúc thư mục đóng gói cho Linux
    mkdir -p dist/cafe_tui_linux
    cp build/cafe_tui dist/cafe_tui_linux/
    cp install.sh dist/cafe_tui_linux/
    # Tạo file menu mặc định trong gói
    echo "M01|Ca phe sua|29000|15" > dist/cafe_tui_linux/menu.txt
    echo "M02|Tra dao cam xa|39000|8" >> dist/cafe_tui_linux/menu.txt
    echo "M03|Banh mi thit|25000|22" >> dist/cafe_tui_linux/menu.txt
    echo "M04|Bac xiu|29000|12" >> dist/cafe_tui_linux/menu.txt
    echo "M05|Tra xanh Latte|45000|5" >> dist/cafe_tui_linux/menu.txt
    
    # Copy README
    cp README.md dist/cafe_tui_linux/HDSD.md
    
    # Nén thành tar.gz
    cd dist
    tar -czf cafe_tui_linux.tar.gz cafe_tui_linux
    rm -rf cafe_tui_linux
    cd ..
    echo "[+] Đóng gói Linux thành công: dist/cafe_tui_linux.tar.gz"
else
    echo "[-] Lỗi: Không thể biên dịch Linux!"
    cd ..
fi

# 3. Đóng gói cho Windows
if [ -f "build/cafe_tui.exe" ]; then
    echo "[+] Đang chuẩn bị gói Windows..."
    mkdir -p dist/cafe_tui_windows
    cp build/cafe_tui.exe dist/cafe_tui_windows/
    
    # Tạo file menu mặc định
    echo "M01|Ca phe sua|29000|15" > dist/cafe_tui_windows/menu.txt
    echo "M02|Tra dao cam xa|39000|8" >> dist/cafe_tui_windows/menu.txt
    echo "M03|Banh mi thit|25000|22" >> dist/cafe_tui_windows/menu.txt
    echo "M04|Bac xiu|29000|12" >> dist/cafe_tui_windows/menu.txt
    echo "M05|Tra xanh Latte|45000|5" >> dist/cafe_tui_windows/menu.txt
    
    # Tạo hướng dẫn sử dụng và cài đặt font cho Windows
    cat << 'EOF' > dist/cafe_tui_windows/HUONG_DAN.txt
============================================================
HUONG DAN CHAY CAFE TUI TREN WINDOWS (PHONG CACH SUPERFILE)
============================================================

1. CAI DAT NERD FONTS (De hien thi icon dep nhu Superfile):
   - Ung dung su dung cac icon dac biet cua bo font Nerd Fonts.
   - Truy cap trang web: https://www.nerdfonts.com/font-downloads
   - Tai xuong font ban thich (Khuyen nghi: "FiraCode Nerd Font", "Hack Nerd Font", hoac "JetBrainsMono Nerd Font").
   - Giai nen file tai ve, nhap chuot phai vao cac file .ttf va chon "Install" hoac "Install for all users".

2. CAU HINH TERMINAL (Sieu quan trong):
   - Tren Windows 10/11, hay su dung ung dung "Windows Terminal" (tai tu Microsoft Store neu chua co). Windows Terminal hien thi mau sac ANSI 24-bit va Nerd Fonts cuc ky chuan va muot.
   - Mo Windows Terminal -> Vao "Settings" (Cai dat) -> Chon Profile Terminal ban su dung (Command Prompt hoac PowerShell).
   - Chon muc "Appearance" (Giao dien) -> Tim phan "Font face" -> Chon ten font Nerd Font ban vua cai dat (vi du: "FiraCode NF" hoac "Hack NF").
   - Nhấn "Save" (Luu).

3. KHOI CHAY UNG DUNG:
   - Nhap dup chuot vao file `cafe_tui.exe` hoac mo Windows Terminal/PowerShell va go:
     .\cafe_tui.exe
   - Khi chay lan dau, ung dung se hoi ban co muon su dung Nerd Fonts hay khong:
     + Neu cac bieu tuong [, 👑, ] hien thi binh thuong (khong bi o vuong/cham hoi): Nhap phím [Y] de kich hoat.
     + Neu bi loi phong chu (o vuong/cham hoi): Nhap phim [N] de ung dung chay o che do Text binh thuong (van rat dep).

============================================================
Nhom CSC10004 - Chuc ban co trai nghiem tuyet voi!
EOF

    # Nén thành file zip
    cd dist
    zip -r cafe_tui_windows.zip cafe_tui_windows
    rm -rf cafe_tui_windows
    cd ..
    echo "[+] Đóng gói Windows thành công: dist/cafe_tui_windows.zip"
else
    echo "[!] Canh bao: Chua co file build/cafe_tui.exe, vui long cho qua trinh bien dich cheo hoan thanh!"
fi

echo "==================================="
