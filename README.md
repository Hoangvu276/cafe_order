# Ứng dụng TUI Quản lý Quán Cà Phê
**Nhóm CSC10004** — Môn Cấu trúc Dữ liệu và Giải thuật

---

## Yêu cầu hệ thống

- Linux / macOS (có terminal hỗ trợ ncurses)
- g++ >= C++17
- libncurses: `sudo apt install libncurses-dev` (Ubuntu) hoặc `brew install ncurses` (macOS)

---

## Biên dịch & Chạy

```bash
make          # biên dịch
make run      # chạy ngay
./cafe_app    # hoặc chạy trực tiếp
```

---

## Cấu trúc thư mục

```
.
├── main.cpp          # Toàn bộ ứng dụng TUI
├── Makefile
├── menu.txt          # Dữ liệu menu (tự sinh khi chạy)
├── history.txt       # Lịch sử đơn hàng (tự sinh khi chạy)
└── lib/
    ├── LinkedList.hpp
    ├── Queue.hpp
    ├── PriorityQueue.hpp
    ├── Stack.hpp
    ├── HashTable.hpp
    ├── BST.hpp
    ├── AVL.hpp
    └── Algorithms.hpp
```

---

## Điều hướng TUI

| Phím | Chức năng |
|------|-----------|
| ↑ / ↓ | Di chuyển cursor |
| ENTER | Xác nhận / Chọn |
| SPACE | Toggle (VIP, chọn món) |
| U | Undo thao tác cuối |
| B / ESC | Quay lại menu trước |
| PgUp / PgDn | Chuyển trang menu |

---

## Chức năng chính

### 1. Quản lý Menu
- **[A]** Thêm món: nhập tên, giá → ID tự sinh (M01, M02...)
- **[E]** Sửa món: chỉnh tên và giá của món đang chọn
- **[D]** Xóa món: xóa với xác nhận
- **[S]** Sắp xếp: theo giá tăng/giảm, theo tên A-Z/Z-A (dùng `mergeSort`)

### 2. Tạo đơn hàng
- Nhập tên khách → toggle VIP → chọn món bằng SPACE → [T] Thanh toán
- Đơn VIP vào `PriorityQueue`, đơn thường vào `Queue`

### 3. Xử lý đơn hàng
- **[P]** Lấy đơn tiếp theo (VIP ưu tiên trước) → hiển thị chi tiết → lưu lịch sử

### 4. Thống kê & Báo cáo
- Top 5 món bán chạy (sort bằng `mergeSort`)
- Doanh thu theo ngày (lưu trong `AVL`, inorder = tăng dần theo ngày)

### 5. Undo
- Nhấn **[U]** bất cứ lúc nào để hoàn tác thao tác cuối:
  - Undo thêm món → xóa lại
  - Undo sửa món → khôi phục về cũ
  - Undo xóa món → thêm lại

---

## Cấu trúc dữ liệu được sử dụng

| Cấu trúc | Dùng cho |
|----------|----------|
| `LinkedList<MenuItem>` | Danh sách menu (có thứ tự thêm vào) |
| `HashTable<string,int>` | Tra cứu món theo ID O(1), sold count |
| `Queue<Order>` | Hàng đợi đơn thường (FIFO) |
| `PriorityQueue<Order>` | Hàng đợi VIP (max-heap, VIP trước) |
| `Stack<Action>` | Lưu lịch sử thao tác để Undo |
| `LinkedList<Order>` | Lịch sử đơn đã xử lý |
| `AVL<DailyRevenue>` | Doanh thu theo ngày (inorder = thứ tự ngày) |
| `mergeSort` / `bubbleSort` | Sắp xếp menu, top món bán chạy |

---

## Lưu trữ dữ liệu

- `menu.txt`: lưu menu khi thoát, load lại khi khởi động
- `history.txt`: lưu lịch sử đơn hàng đã xử lý
