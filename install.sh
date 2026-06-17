#!/bin/bash
# Script cài đặt ứng dụng Cafe TUI (Superfile Style) cho Linux

echo "=== CAI DAT CAFE TUI SYSTEM ==="

# 1. Bien dich du an
echo "[+] Dang bien dich ung dung voi lien ket tinh (Static build)..."
mkdir -p build
cd build
cmake -DSTATIC_BUILD=ON ..
make -j$(nproc)
if [ $? -ne 0 ]; then
    echo "[-] Loi: Bien dich that bai!"
    exit 1
fi
cd ..

# 2. Cai dat file thuc thi vao he thong
echo "[+] Dang sao chep file chay vao thu muc he thong..."
if [ -w "/usr/local/bin" ]; then
    cp build/cafe_tui /usr/local/bin/
else
    echo "[!] Can quyen root de sao chep vao /usr/local/bin..."
    sudo cp build/cafe_tui /usr/local/bin/
fi

if [ $? -eq 0 ]; then
    echo "[+] Cai dat thanh cong! Ban co the chay ung dung bang cach go lenh: cafe_tui"
else
    echo "[-] Loi: Khong the cai dat file thuc thi!"
    exit 1
fi

# 3. Khoi tao file menu mac dinh
if [ ! -f "menu.txt" ]; then
    echo "[+] Dang tao file du lieu menu mac dinh (menu.txt)..."
    echo "M01|Ca phe sua|29000|15" > menu.txt
    echo "M02|Tra dao cam xa|39000|8" >> menu.txt
    echo "M03|Banh mi thit|25000|22" >> menu.txt
    echo "M04|Bac xiu|29000|12" >> menu.txt
    echo "M05|Tra xanh Latte|45000|5" >> menu.txt
fi

echo "==============================="
echo "LUU Y: De hien thi giao dien dep nhat:"
echo "1. Cai dat mot font chu Nerd Font (vi du: FiraCode Nerd Font)."
echo "2. Dat font chu do lam font mac dinh cho terminal cua ban."
echo "3. Khi chay ung dung lan dau, hay chon 'Y' de kich hoat che do Nerd Fonts."
echo "==============================="
