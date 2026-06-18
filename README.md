# Ứng dụng TUI Quản lý Quán Cà Phê (Phong cách Superfile)

Ứng dụng quản lý quán cà phê giao diện dòng lệnh hiện đại (TUI - Text-based User Interface) được viết bằng C++17, kế thừa phong cách thiết kế tinh giản, bo tròn và bảng màu Catppuccin Mocha sẫm của công cụ **superfile**.

Ứng dụng sử dụng các cấu trúc dữ liệu tự viết nằm trong thư mục `lib/` gồm: `LinkedList`, `Queue`, `PriorityQueue` (max-heap), `Stack`, `HashTable`, `BST/AVL` và các thuật toán sắp xếp (`mergeSort`).

---

##  Các Tính năng chính

1. **Quản lý thực đơn (Menu Manager)**:
   - Hiển thị danh sách món ăn dưới dạng bảng với viền bo tròn.
   - Thêm món mới (ID tự sinh `M01`, `M02`...), sửa thông tin, xóa món ăn (có xác nhận).
   - Sắp xếp thực đơn theo Giá tăng/giam, Tên A-Z/Z-A sử dụng thuật toán sắp xếp trộn `mergeSort`.
   - Hoàn tác (`Undo`) thao tác thêm/sửa/xóa vừa thực hiện bằng cấu trúc `Stack`.

2. **Tạo đơn hàng (Order Creator)**:
   - Nhập tên khách hàng trực tiếp inline không gây vỡ viền TUI.
   - Tích chọn Khách hàng VIP (VIP được ưu tiên xử lý trước).
   - Chọn món ăn trực quan bằng cách di chuyển thanh sáng và nhấn `Space` để tích chọn `[✓]` hoặc dùng `+`/`-` để thay đổi số lượng.
   - Thanh toán (`T`) đưa đơn hàng vào Hàng đợi.

3. **Xử lý đơn hàng (Order Process)**:
   - Đơn VIP tự động đưa vào `PriorityQueue` (vun đống tối đa - max-heap theo số thứ tự đơn).
   - Đơn thường tự động đưa vào `Queue` (vào trước ra trước - FIFO).
   - Nhấn phím `P` lấy đơn hàng tiếp theo ra xử lý (đơn VIP luôn được xử lý trước đơn thường).
   - Nhấn `U` để hoàn tác đơn hàng vừa xử lý (đưa đơn hàng ngược lại hàng đợi và hoàn lại doanh thu).

4. **Thống kê & Báo cáo (Stats & Reports)**:
   - Top 5 món bán chạy nhất dựa trên số lượt bán (HashTable kết hợp sắp xếp `mergeSort`).
   - Thống kê doanh thu theo ngày từ cây tự cân bằng `AVL` (duyệt inorder tự động hiển thị ngày theo trình tự tăng dần).
   - Tổng quan tổng doanh thu và tổng số đơn đã xử lý.

5. **Cơ chế Fallback Font thông minh**:
   - Khi chạy lần đầu, ứng dụng hiển thị màn hình cài đặt Font để bạn lựa chọn chế độ **Nerd Fonts Mode** (cho giao diện đẹp nhất) hoặc **Standard Text Mode** (tự động chuyển các icon thành text thông thường nếu terminal chưa cài Nerd Fonts). Cấu hình lưu tự động vào file `.cafe_tui.conf`.

---

## 🖥️ Hướng dẫn Điều hướng TUI

* **Sidebar (Cột trái)**: Di chuyển giữa các mục chính bằng phím mũi tên **Up/Down (↑/↓)** hoặc **j/k**. Nhấn **Enter/Tab/→** để chuyển focus vào cột Workspace bên phải.
* **Workspace (Cột phải)**: Nhấn **ESC** hoặc **←** để quay lại Sidebar chọn danh mục khác.
  * **Chế độ xem**: Di chuyển bằng **Up/Down (↑/↓)** hoặc **j/k**. Nhấn các phím chức năng nóng (`A`, `E`, `D`, `S`, `U`, `T`, `C`, `P` tùy theo màn hình).
  * **Chế độ nhập chữ**: Gõ chữ bình thường, nhấn `Backspace` để xóa, nhấn `Enter` để lưu/chuyển trường, nhấn `ESC` để hủy nhập liệu.

---

## 🛠️ Biên dịch & Cài đặt

### 1. Trên Linux (Biên dịch tĩnh hoặc động)

#### Cách nhanh nhất (Dùng Script):
Cấp quyền chạy và chạy script cài đặt tự động (script này biên dịch tĩnh hoàn toàn và cài vào hệ thống):
```bash
chmod +x install.sh
./install.sh
# Chạy chương trình từ bất kỳ đâu bằng lệnh:
cafe_tui
```

#### Biên dịch thủ công bằng CMake:
```bash
mkdir build && cd build
cmake ..
make
./cafe_tui
```

#### Biên dịch tĩnh (Static Build) để phân phối binary độc lập:
```bash
mkdir build && cd build
cmake -DSTATIC_BUILD=ON ..
make
# Binary 'cafe_tui' tạo ra là file biên dịch tĩnh, chạy được trên mọi máy Linux khác.
```

---

### 2. Biên dịch chéo sang Windows (Cross-compile từ Linux)

Bạn có thể tạo ra file chạy `.exe` độc lập cho Windows trực tiếp từ máy Linux thông qua `mingw-w64`:

```bash
# Cài đặt trình biên dịch chéo MinGW trên Ubuntu/Debian
sudo apt install g++-mingw-w64-x86-64-w64-mingw32

# Biên dịch tĩnh tạo file chạy .exe cho Windows
x86_64-w64-mingw32-g++ -O3 -static -o build/cafe_tui.exe main.cpp -std=c++17 -I.
```
Sau đó, bạn chỉ cần nén file `cafe_tui.exe` thành file `.zip` gửi cho người dùng Windows.

---

### 3. Biên dịch cục bộ trên Windows (MSYS2)

1. Tải và cài đặt [MSYS2](https://www.msys2.org/).
2. Mở terminal **MSYS2 MinGW 64-bit** và cài đặt toolchain g++:
   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake make
   ```
3. Di chuyển vào thư mục dự án và biên dịch tĩnh:
   ```bash
   g++ -O3 -static -o cafe_tui.exe main.cpp -std=c++17 -I.
   ```
4. Mở PowerShell hoặc Command Prompt của Windows, di chuyển tới thư mục và chạy file `cafe_tui.exe`.

---

## 🎨 Lưu ý cấu hình hiển thị Nerd Fonts

Để hiển thị trọn vẹn và đẹp mắt các icon:
1. Bạn hãy tải về và cài đặt một font chữ Nerd Font bất kỳ (Ví dụ: **FiraCode Nerd Font** hoặc **Hack Nerd Font**) từ trang chủ [Nerd Fonts](https://www.nerdfonts.com/).
2. Thiết lập font chữ vừa tải làm font hiển thị mặc định của Terminal của bạn (như Windows Terminal, Alacritty, GNOME Terminal, v.v.).
3. Khi khởi động ứng dụng lần đầu, hãy nhấn phím `Y` khi màn hình cấu hình font hiển thị để kích hoạt chế độ **Nerd Fonts**.
