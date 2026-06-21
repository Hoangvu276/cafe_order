# Báo Cáo Kết Quả Kiểm Thử (Test Report) - Cafe TUI

**Thời gian kiểm thử**: 2026-06-21 10:42:02
**Người thực hiện**: AI QA Agent (Antigravity)
**Trạng thái tổng quan**: Tất cả các kịch bản kiểm thử đã được chạy tự động thông qua giả lập PTY.

| Nhóm Kiểm Thử | Tên Kịch Bản | Trạng Thái | Chi Tiết Xác Minh |
| :--- | :--- | :---: | :--- |
| Giao diện TUI | TC 1.1: Chống giật màn hình | ✅ PASS | Di chuyển sidebar mượt mà, nội dung vẽ đè đầy đủ, không gây lỗi treo hoặc nhấp nháy. |
| Quản lý Thực đơn | TC 2.1: Thêm món hợp lệ | ✅ PASS | Món 'Sinh to Bo' (40.000đ) được thêm vào Vector và HashTable thành công, hiển thị đúng dòng cột. |
| Quản lý Thực đơn | TC 2.2: Chặn giá không hợp lệ | ✅ PASS | Hệ thống chặn giá chứa chữ, hiển thị thông báo lỗi 'Gia tien khong hop le!'. |
| Tạo Đơn hàng | TC 3.1: Tăng giảm số lượng | ✅ PASS | Chọn món bằng Space và tăng giảm số lượng (+/-) hoạt động chính xác. Giỏ hàng cập nhật số lượng. |
| Tạo Đơn hàng | TC 3.2: Hàng đợi ưu tiên VIP | ✅ PASS | Đơn hàng VIP C tự động vượt lên trước Khach B trong hàng đợi nhờ PriorityQueue Max-Heap. |
| Xử lý Đơn hàng | TC 4.1: Thăng cấp đơn tiếp theo | ✅ PASS | Xử lý đơn (P) thành công. Đơn VIP C tự động được đẩy lên làm Đơn hàng đang thực hiện. |
| Xử lý Đơn hàng | TC 4.2: Hoàn tác chế biến (Undo) | ✅ PASS | Hoàn tác thành công. Khôi phục đơn Khach A về vị trí 'Đang thực hiện' bằng Stack. |
| Thống kê | TC 5.1: Đồng bộ doanh thu | ✅ PASS | Đồng bộ doanh thu thành công. Cây AVL doanh thu theo ngày và bảng băm mặt hàng hoạt động chính xác. |

### Kết quả chụp màn hình kiểm thử (Evidence):
Dưới đây là nội dung văn bản ghi nhận từ bộ đệm màn hình terminal (đã loại bỏ ANSI escape):

```text
;247m│  1. Banh mi thit -> 22 luot  2026-06-21: 116.000đ        │
│ > 📊 Thong ke & b││  2. Ca phe sua -> 19 luot                                │
│    Thoat chuong ││  3. Bac xiu -> 12 luot                                   │
│                  ││  4. Tra dao cam x -> 8 luot                              │
│                  ││  5. Tra xanh Latt -> 5 luot                              │
│                  ││                                                          │
│                  ││==========================================================│
│                  ││                                                          │
│                  ││  Tong doanh thu: 116.000đ                                │
│                  ││  Tong so don da xu ly: 2 don                             │
│                  ││                                                          │
│                  ││                                                          │
│                  ││                                                          │
╰─ v1.0.0 ─────────╯╰─ Report & Analytics ─────────────────────────────────────╯
================================================================================
  MESSAGE: Da xu ly xong don #3 cho khach: Khach VIP C
  Phim tat: [ESC/←] Ve menu chinh

```
