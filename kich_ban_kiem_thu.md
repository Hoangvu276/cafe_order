# Cẩm Nang Kịch Bản Kiểm Thử (Test Cases) - Cafe TUI

Tài liệu này cung cấp các kịch bản kiểm thử (Test Cases) chi tiết để lập trình viên và người vận hành (Barista/Thu ngân) kiểm tra tính đúng đắn về giao diện, logic nghiệp vụ, và các cấu trúc dữ liệu tùy chỉnh của dự án Cafe TUI.

---

## 🖥️ Nhóm 1: Kiểm Thử Giao Diện & Tính Ổn Định Terminal

### **Kịch Bản 1.1: Kiểm tra chống giật màn hình (Flicker-Free)**
* **Mục đích**: Xác nhận màn hình không bị nháy chớp đen mỗi khi thao tác bàn phím (di chuyển menu, đổi màn hình).
* **Các bước thực hiện**:
  1. Khởi chạy ứng dụng bằng lệnh: `./build/cafe_tui`
  2. Nhấn giữ phím `Down` hoặc `Up` liên tục để di chuyển qua lại thanh sáng trên Sidebar.
  3. Nhấn phím `Tab` liên tục để chuyển nhanh giữa các màn hình hiển thị.
* **Kết quả mong đợi**: Giao diện cập nhật trơn tru, không có khoảng trễ nhấp nháy đen xen kẽ giữa các frame. 
* **Cấu trúc dữ liệu & Kỹ thuật liên quan**: Cơ chế gom frame vẽ đè bằng mã ANSI `\033[H` tại tệp tin [main.cpp](file:///home/viet/Project/main.cpp).

### **Kịch Bản 1.2: Co giãn kích thước Terminal (Window Resize)**
* **Mục đích**: Kiểm tra khả năng tự động căn chỉnh và vẽ lại giao diện theo độ phân giải mới của terminal.
* **Các bước thực hiện**:
  1. Kéo giãn rộng hoặc thu hẹp cửa sổ ứng dụng terminal đang chạy.
  2. Bấm phím bất kỳ (trên Windows để nhận diện resize) hoặc xem tự động căn chỉnh (trên Linux).
* **Kết quả mong đợi**: Khung viền panel tự động co giãn khớp kích thước mới, các cột thông tin giữ nguyên tỷ lệ vuông vắn, không bị vỡ hoặc đẩy dòng xuống dưới.
* **Cấu trúc dữ liệu & Kỹ thuật liên quan**: Hàm `get_terminal_size()` và bộ lọc chiều rộng ký tự đa byte UTF-8 trong [ui_renderer.hpp](file:///home/viet/Project/src/ui/ui_renderer.hpp).

---

## 📋 Nhóm 2: Quản Lý Thực Đơn (Menu Management)

### **Kịch Bản 2.1: Thêm món ăn mới với dữ liệu hợp lệ**
* **Mục đích**: Kiểm tra hoạt động thêm phần tử thực đơn và độ nhạy của con trỏ nhập liệu.
* **Các bước thực hiện**:
  1. Tại Sidebar, chọn mục **Quản lý Menu** (Màn hình 1).
  2. Nhấn phím `A` (Add).
  3. Nhập tên món: `Sinh to Bo` $\rightarrow$ Nhấn `Enter`.
  4. Nhập giá món: `40000` $\rightarrow$ Nhấn `Enter`.
* **Kết quả mong đợi**: 
  * Khi đang gõ Tên món, con trỏ nhấp nháy `_` nằm đúng dòng **Tên món**, dòng **Giá** trống.
  * Khi chuyển sang gõ Giá, tên món hiển thị tĩnh, con trỏ hiển thị ở dòng **Giá món**.
  * Sau khi nhấn `Enter` lần 2, món `Sinh to Bo` xuất hiện ở cuối thực đơn với giá hiển thị `40.000đ`.
* **Cấu trúc dữ liệu & Kỹ thuật liên quan**: Lưu trữ phần tử mới vào [Vector.hpp](file:///home/viet/Project/lib/Vector.hpp) và đồng bộ vào bảng băm [HashTable.hpp](file:///home/viet/Project/lib/HashTable.hpp).

### **Kịch Bản 2.2: Ngăn chặn dữ liệu giá không hợp lệ (Edge Case)**
* **Mục đích**: Đảm bảo hệ thống không nhận dữ liệu sai định dạng gây crash hoặc lỗi tính toán doanh thu.
* **Các bước thực hiện**:
  1. Tại màn hình Quản lý Menu, nhấn phím `A`.
  2. Nhập tên món: `Tra Chanh` $\rightarrow$ Nhấn `Enter`.
  3. Nhập giá món chứa chữ: `abc10` hoặc để trống $\rightarrow$ Nhấn `Enter`.
* **Kết quả mong đợi**: Hệ thống bỏ qua giá trị lỗi, giữ nguyên thực đơn cũ hoặc hiển thị thông báo yêu cầu nhập lại số nguyên dương hợp lệ.

---

## 🛒 Nhóm 3: Lên Đơn & Thanh Toán (Order Creation)

### **Kịch Bản 3.1: Thêm món & Điều chỉnh số lượng bằng phím tắt nhanh**
* **Mục đích**: Kiểm tra tốc độ lên đơn bằng phím tắt tối giản phục vụ việc checkout nhanh.
* **Các bước thực hiện**:
  1. Chuyển sang màn hình **Tạo đơn hàng** (Màn hình 2).
  2. Di chuyển thanh sáng đến món `Ca phe sua`.
  3. Nhấn phím `Space` $\rightarrow$ Nhấn phím `+` (3 lần) $\rightarrow$ Nhấn phím `-` (1 lần).
* **Kết quả mong đợi**: 
  * Nhấn `Space` lần đầu: Món `Ca phe sua` được thêm vào hóa đơn tạm tính với số lượng `x1`.
  * Nhấn `+` liên tục: Số lượng tăng lên `x4`, tổng tiền hóa đơn tự động cập nhật.
  * Nhấn `-`: Số lượng giảm về `x3`. Nếu nhấn tiếp phím `-` khi số lượng là `1`, món ăn sẽ tự động bị xóa khỏi giỏ hàng.
* **Cấu trúc dữ liệu & Kỹ thuật liên quan**: Thao tác cập nhật phần tử trên giỏ hàng [Vector.hpp](file:///home/viet/Project/lib/Vector.hpp).

### **Kịch Bản 3.2: Ưu tiên đơn hàng VIP so với đơn hàng thường**
* **Mục đích**: Kiểm tra thuật toán sắp xếp độ ưu tiên của hàng đợi pha chế.
* **Các bước thực hiện**:
  1. Tạo đơn thứ 1: Tên khách `Khach Thuong A`, **không** tích chọn VIP. Chọn 1 món $\rightarrow$ Nhấn `T` (Thanh toán).
  2. Tạo đơn thứ 2: Tên khách `Khach Thuong B`, **không** tích chọn VIP. Chọn 1 món $\rightarrow$ Nhấn `T`.
  3. Tạo đơn thứ 3: Tên khách `Khach VIP C`, **có** tích chọn VIP (hiển thị `[x] VIP`). Chọn 1 món $\rightarrow$ Nhấn `T`.
* **Kết quả mong đợi**: Di chuyển sang màn hình **Xử lý đơn hàng** (Màn hình 3):
  * Đơn hàng của `Khach VIP C` phải tự động đứng trước `Khach Thuong B` trong hàng đợi pha chế (mặc dù VIP C thanh toán sau cùng).
* **Cấu trúc dữ liệu & Kỹ thuật liên quan**: Đẩy đơn hàng vào hàng đợi ưu tiên [PriorityQueue.hpp](file:///home/viet/Project/lib/PriorityQueue.hpp) hoạt động bằng cơ chế Max-Heap.

---

## ☕ Nhóm 4: Xử Lý Đơn Chế Biến (Kitchen System)

### **Kịch Bản 4.1: Xử lý đơn hàng và Tự động thăng cấp**
* **Mục đích**: Kiểm tra quy trình pha chế của Barista và hiển thị KDS (Kitchen Display System).
* **Các bước thực hiện**:
  1. Vào màn hình **Xử lý đơn hàng** (Màn hình 3).
  2. Quan sát phần **ĐƠN HÀNG ĐANG THỰC HIỆN** ở trên cùng.
  3. Nhấn phím `P` (Process) từ xa ngay tại Sidebar.
* **Kết quả mong đợi**: 
  * Đơn hàng hiện tại biến mất khỏi mục "Đang thực hiện" và lưu vào lịch sử đã hoàn thành.
  * Đơn hàng tiếp theo có độ ưu tiên cao nhất (`Khach VIP C` hoặc `Khach Thuong A`) **tự động nhảy lên** thế chỗ tại mục "Đang thực hiện".
* **Cấu trúc dữ liệu & Kỹ thuật liên quan**: Gọi hàm `dequeue()` của [Queue.hpp](file:///home/viet/Project/lib/Queue.hpp) / Heap để lấy đơn hàng tiếp theo.

### **Kịch Bản 4.2: Hoàn tác tác vụ chế biến (Undo)**
* **Mục đích**: Hỗ trợ Barista kéo lại đơn hàng vừa vô tình nhấn hoàn thành nhầm.
* **Các bước thực hiện**:
  1. Nhấn phím `U` (Undo) ngay sau khi thực hiện Kịch Bản 4.1.
* **Kết quả mong đợi**: Đơn hàng vừa chế biến xong lập tức được khôi phục về mục **ĐƠN HÀNG ĐANG THỰC HIỆN**, đơn hàng đang làm dở bị đẩy ngược lại hàng đợi chờ.
* **Cấu trúc dữ liệu & Kỹ thuật liên quan**: Thu hồi phần tử từ ngăn xếp lưu vết [Stack.hpp](file:///home/viet/Project/lib/Stack.hpp).

---

## 📊 Nhóm 5: Thống Kê & Báo Cáo Doanh Thu (Statistics)

### **Kịch Bản 5.1: Đồng bộ doanh thu bán hàng**
* **Mục đích**: Xác nhận dữ liệu doanh thu được ghi nhận đúng sau khi đơn hàng được chế biến thành công.
* **Các bước thực hiện**:
  1. Ghi nhận doanh thu hiện tại ở màn hình **Báo cáo thống kê** (Màn hình 4).
  2. Sang màn hình Tạo đơn hàng, lên 1 đơn trị giá `30.000đ` và bấm thanh toán `T`.
  3. Sang màn hình Xử lý đơn hàng, nhấn `P` để hoàn thành đơn hàng này.
  4. Quay lại màn hình Thống kê kiểm tra.
* **Kết quả mong đợi**: 
  * Tổng doanh thu tăng chính xác `30.000đ`.
  * Cây AVL tự động cập nhật doanh thu và sắp xếp danh sách các món theo doanh thu từ thấp đến cao.
  * Vùng trống phân chia các cột giữ nguyên màu nền `#181825` đồng bộ, không bị lỗi loang màu đen.
* **Cấu trúc dữ liệu & Kỹ thuật liên quan**: Tự động cân bằng và sắp xếp dữ liệu thông qua cấu trúc cây [AVL.hpp](file:///home/viet/Project/lib/AVL.hpp).
