# Nhật Ký Khắc Phục Lỗi & Quy Trình Xử Lý (Cafe TUI)

Tài liệu này ghi lại chi tiết toàn bộ các lỗi đã xảy ra trong phiên bản hiện tại của ứng dụng Cafe TUI, người phát hiện, nguyên nhân gốc rễ, cách thức đã khắc phục, và quá trình xử lý sự cố liên quan đến Git Push / SSH.

---

## 1. Danh Sách Các Lỗi Giao Diện & Logic Ứng Dụng

### Lỗi 1: Hiện tượng nhấp nháy màn hình (Flicker) mỗi khi nhấn phím
* **Người phát hiện**: Người dùng (User).
* **Hiện tượng**: Màn hình ứng dụng TUI bị nhấp nháy (chớp giật đen màn hình liên tục) rất khó chịu mỗi khi có thay đổi trạng thái (di chuyển menu, chuyển tab, cập nhật đơn hàng).
* **Nguyên nhân**: Mã nguồn cũ thực hiện xóa toàn bộ màn hình bằng cách gọi `clear_screen()` (gửi mã ANSI `\033[2J` xóa toàn màn hình) trước mỗi lần render lại giao diện. Việc vẽ lại toàn bộ ký tự từ màn hình trống tạo ra khoảng trễ nháy đen xen kẽ giữa các frame.
* **Cách khắc phục**:
  - **Tệp chỉnh sửa**: [main.cpp](file:///home/viet/Project/main.cpp)
  - **Hành động**:
    1. Thay thế việc xóa màn hình bằng cơ chế di chuyển con trỏ terminal về góc trên bên trái `(0, 0)` bằng mã ANSI `\033[H` (Home) ở mỗi đầu frame render.
    2. Gom toàn bộ nội dung của Sidebar panel và Workspace panel lại, ghép nối nằm ngang bằng hàm `merge_horizontal()`.
    3. Thực hiện xuất ra màn hình (print) toàn bộ chuỗi giao diện tích hợp này trong **một câu lệnh duy nhất** (`std::cout << full_frame`), hoạt động tương tự như cơ chế **Double Buffering** (Virtual DOM). Ký tự cũ bị ký tự mới đè trực tiếp mà không qua bước xóa màn hình, giúp triệt tiêu hoàn toàn lỗi giật nhấp nháy (Flicker).

---

### Lỗi 2: Lệch khung viền Panel do tính sai kích thước Icon và Emojis
* **Người phát hiện**: Người dùng (User).
* **Hiện tượng**: Khung viền bên phải của Workspace hoặc Sidebar bị thụt lùi vào trong hoặc bị đẩy xuống hàng dưới làm vỡ cấu trúc giao diện khi dòng có chứa các icon (như ``, `📊`, `👑`).
* **Nguyên nhân**:
  - Hệ thống hỗ trợ chế độ Nerd Fonts (hiển thị các ký tự PUA đặc biệt) và Standard Text Mode (hiển thị Emojis/Ký tự thường).
  - Terminal hiển thị các emojis (như `📊`, `👑`) dưới dạng ký tự rộng (**Wide Character - chiếm 2 cột**), trong khi các icon Nerd Fonts (trong dải PUA `0xE000` - `0xF8FF`) được hiển thị với độ rộng là **1 cột** trong terminal monospace.
  - Hàm `codepoint_width` cũ không phân biệt được các dải ký tự này, tính toán sai độ rộng hiển thị (mặc định coi emoji rộng là 1 cột). Dẫn tới hàm `str_display_width` tính sai tổng độ rộng của dòng chữ, khiến hàm `pad_right` thêm thừa hoặc thiếu khoảng trắng đệm, phá vỡ khung viền.
* **Cách khắc phục**:
  - **Tệp chỉnh sửa**: [src/ui/ui_renderer.hpp](file:///home/viet/Project/src/ui/ui_renderer.hpp)
  - **Hành động**:
    1. Cấu trúc lại hàm `codepoint_width` để tính toán chính xác độ rộng:
       - Các dải unicode của Emojis & Symbols rộng (`0x2600` đến `0x27BF`, `0x1F300` đến `0x1F6FF`, `0x1F900` đến `0x1FAFF`) được trả về chiều rộng là **2 cột**.
       - Các ký tự thuộc dải Nerd Fonts PUA (`0xE000` đến `0xF8FF`) được trả về chiều rộng là **1 cột**.
    2. Nhờ đó, việc đo đạc chiều rộng dòng chữ chứa cả chữ thường, icon Nerd Fonts và emoji đều chính xác tuyệt đối, khung viền panel được vẽ khít và không còn bị méo mó.

---

### Lỗi 3: Khôi phục cơ chế tăng số lượng món bằng phím tắt
* **Người phát hiện**: Người dùng (User).
* **Hiện tượng**: Tính năng nhập trực tiếp số lượng món bằng bàn phím (mở hộp thoại nhập văn bản khi bấm `Space` hoặc `Enter`) làm chậm thao tác lên đơn và gây rườm rà không cần thiết.
* **Nguyên nhân**: Thiết kế trước đó cố gắng thêm tính năng nhập số lượng tự do qua hộp thoại, nhưng không tối ưu cho tốc độ của người dùng TUI.
* **Cách khắc phục**:
  - **Tệp chỉnh sửa**: [main.cpp](file:///home/viet/Project/main.cpp)
  - **Hành động**:
    1. Loại bỏ trạng thái `InputState::OrderQty` khỏi enum và xóa toàn bộ các khối mã nguồn kết xuất hộp thoại nhập số lượng.
    2. Khôi phục phím tắt `Space` ở trạng thái chọn món:
       - Nếu món ăn chưa có trong giỏ hàng: Thêm món đó vào giỏ hàng với số lượng mặc định là `1`.
       - Nếu món ăn đã có sẵn trong giỏ hàng: Xóa hoàn toàn món đó khỏi giỏ hàng.
    3. Giữ nguyên phím `+` (tăng số lượng thêm 1) và phím `-` (giảm số lượng đi 1, nếu về 0 thì tự động xóa món khỏi giỏ hàng).
    4. Cập nhật nhãn hướng dẫn ở chân trang thành `[Space] Chon/Bo mon`.

---

### Lỗi 4: Loang màu nền Panel do mã Reset màu (ansi_reset)
* **Người phát hiện**: Người dùng (User).
* **Hiện tượng**: Tại màn hình Thống kê (Màn hình 4), vùng trống phân chia giữa các cột và phần tổng hợp doanh thu ở đáy panel bị đổi thành màu đen (mất màu nền xanh đậm đặc trưng `#181825` của theme Catppuccin Mocha), khiến giao diện trông loang lổ và thiếu đồng bộ.
* **Nguyên nhân**:
  - Mã reset ANSI chuẩn (`\033[0m`) được sử dụng ở cuối mỗi chuỗi ký tự định dạng (chữ màu vàng, in đậm, chữ xanh...) để đưa định dạng chữ về mặc định. Tuy nhiên, mã này đồng thời cũng **reset luôn màu nền** về màu mặc định của terminal (thường là màu đen).
  - Khi thực hiện ghép các cột bằng hàm `merge_horizontal`, các khoảng trắng đệm được chèn sau mã reset này nên bị in ra với nền đen.
* **Cách khắc phục**:
  - **Tệp chỉnh sửa**: [src/ui/ui_renderer.hpp](file:///home/viet/Project/src/ui/ui_renderer.hpp)
  - **Hành động**:
    1. Chỉnh sửa hàm `render` của cấu trúc `SectionContent`.
    2. Trước khi in các dòng văn bản ra màn hình, tiến hành duyệt qua nội dung dòng và thay thế tất cả các mã reset `\033[0m` bằng tổ hợp `\033[0m` cộng với mã ANSI thiết lập lại màu nền `bg` của panel hiện tại.
    3. Kết quả: Ngay sau khi màu chữ hoặc hiệu ứng chữ bị reset, màu nền của panel ngay lập tức được thiết lập lại, giúp giữ màu nền `#181825` (hoặc `#11111b` cho Sidebar) đồng bộ cho toàn bộ panel.

---

### Lỗi 5: Lệch vị trí con trỏ nhập liệu ở màn hình Thêm/Sửa món ăn
* **Người phát hiện**: Người dùng (User).
* **Hiện tượng**: Khi bấm phím `A` (Thêm món) hoặc `E` (Sửa món) để nhập tên món mới, nội dung người dùng gõ lại bị hiển thị ở ô **Giá món**, trong khi dòng **Tên món** chỉ hiển thị một con trỏ `_` trống rỗng.
* **Nguyên nhân**:
  - Cả hai ô nhập liệu đều sử dụng chung biến bộ đệm tạm thời `input_buffer`.
  - Trong logic vẽ giao diện cũ, dòng "Giá món" luôn hiển thị trực tiếp `input_buffer`, còn dòng "Tên món" hiển thị biến `temp_name` (biến này chỉ có dữ liệu sau khi nhấn `Enter` để lưu tên món). Điều này khiến trong lúc gõ tên, ô Tên trống còn ô Giá hiện chữ đang gõ.
* **Cách khắc phục**:
  - **Tệp chỉnh sửa**: [main.cpp](file:///home/viet/Project/main.cpp)
  - **Hành động**:
    1. Phân chia rõ ràng biến hiển thị dựa trên trạng thái nhập liệu (`InputState`):
       - Nếu đang ở trạng thái nhập Tên món (`AddName`/`EditName`): Dòng **Tên món** hiển thị `input_buffer + "_"`, còn dòng **Giá món** để trống (hoặc hiển thị giá cũ không có con trỏ khi sửa).
       - Nếu đang ở trạng thái nhập Giá món (`AddPrice`/`EditPrice`): Dòng **Tên món** hiển thị tên đã nhập xong (`temp_name`), còn dòng **Giá món** hiển thị `input_buffer + "_"`.
    2. Nhờ đó, con trỏ nhập liệu và nội dung đang gõ luôn xuất hiện chính xác tại dòng tương ứng với trạng thái thao tác.

---

### Lỗi 6: Sai đường dẫn tương đối khi include các thư viện cấu trúc dữ liệu từ thư mục `lib/`
* **Người phát hiện**: Người dùng (User).
* **Hiện tượng**: Trong file [src/cafe_models.hpp](file:///home/viet/Project/src/cafe_models.hpp), các cấu trúc dữ liệu tùy biến được include bằng đường dẫn dạng `"lib/..."`. Mặc dù trình biên dịch có thể hoạt động nếu thiết lập include path gốc (`-I.`), việc khai báo này không đúng cấu trúc phân cấp thư mục thực tế của dự án.
* **Nguyên nhân**: File `cafe_models.hpp` nằm trong thư mục con `src/`, còn thư mục chứa cấu trúc dữ liệu `lib/` lại nằm ở thư mục cha (ngang hàng với `src/`). Do đó, để include đúng đường dẫn tương đối từ `src/` sang `lib/`, ta cần sử dụng ký hiệu `../` để quay về thư mục cha trước.
* **Cách khắc phục**:
  - **Tệp chỉnh sửa**: [src/cafe_models.hpp](file:///home/viet/Project/src/cafe_models.hpp)
  - **Hành động**:
    1. Sửa toàn bộ 8 dòng khai báo import từ `"lib/..."` thành `"../lib/..."` (ví dụ: `../lib/LinkedList.hpp`, `../lib/Queue.hpp`, v.v.).
    2. Tiến hành biên dịch kiểm tra trên cả hệ thống Linux cục bộ và hệ thống Windows thông qua Docker Cross-compilation để đảm bảo dự án chạy ổn định và không phát sinh lỗi liên kết.
    3. Đóng gói lại toàn bộ ứng dụng sang thư mục [dist/](file:///home/viet/Project/dist/).

---

## 2. Quá Trình Xử Lý Sự Cố Git Push & SSH Key

Quá trình đẩy mã nguồn lên GitHub đã gặp một số sự cố kỹ thuật về cấu hình SSH trên máy khách. Dưới đây là diễn biến và giải pháp đã thực hiện:

### Sự cố 1: Treo tiến trình ngầm và CPU tăng cao đột ngột (98% CPU)
* **Hiện tượng**: Lệnh `git push` thực hiện bởi trợ lý AI bị treo vô hạn, kiểm tra tài nguyên hệ thống thấy tiến trình `ssh-add` chiếm dụng đến 98% CPU.
* **Nguyên nhân**:
  - Cấu hình SSH của hệ thống có tùy chọn `AddKeysToAgent yes`. Khi thực hiện kết nối SSH tới GitHub, hệ thống cố gắng tự động thêm khóa private key vào SSH Agent thông qua lệnh `ssh-add ~/.ssh/id_ed25519`.
  - Do tiến trình được gọi dưới dạng **không tương tác (non-interactive)** trong môi trường của AI, nó không thể hiển thị hộp thoại yêu cầu người dùng nhập mật khẩu giải mã khóa (passphrase), dẫn tới vòng lặp vô hạn gây nghẽn CPU.
* **Cách khắc phục**:
  1. Giải phóng tài nguyên hệ thống bằng cách tắt tất cả các tiến trình SSH/Git bị treo:
     ```bash
     killall -9 ssh-add
     killall -9 ssh
     ```
  2. Cách ly SSH khỏi tác nhân Agent bằng cách tạm thời đặt biến môi trường `SSH_AUTH_SOCK=""` và thêm tham số vô hiệu hóa tự động thêm khóa (`-o AddKeysToAgent=no`):
     ```bash
     SSH_AUTH_SOCK="" GIT_SSH_COMMAND="ssh -o AddKeysToAgent=no -o IdentitiesOnly=yes -i ~/.ssh/id_ed25519" git push origin main
     ```

---

### Sự cố 2: Lỗi xác thực SSH Key (Passphrase & Permission Denied)
* **Hiện tượng**:
  - Khi chạy lệnh đẩy mã nguồn cô lập Agent, hệ thống yêu cầu nhập mật khẩu bảo vệ khóa:
    `Enter passphrase for key '/home/viet/.ssh/id_ed25519':`
  - Người dùng thắc mắc passphrase là gì (liệu có phải mật khẩu tài khoản GitHub không?).
  - Khi không nhập đúng mật khẩu khóa, GitHub trả về lỗi: `git@ssh.github.com: Permission denied (publickey).`
* **Giải thích nguyên nhân**:
  - **Passphrase** là mật khẩu dùng để mã hóa và bảo vệ file private key (`id_ed25519`) ngay trên máy tính của bạn (được thiết lập lúc bạn chạy lệnh `ssh-keygen`). Nó **không phải** mật khẩu tài khoản GitHub.
  - Nếu private key được đặt passphrase thì mỗi lần sử dụng SSH key đó, hệ thống bắt buộc phải giải mã bằng passphrase này. Nếu quên passphrase, bạn sẽ không thể sử dụng khóa đó được nữa.

---

## 3. Hướng Dẫn Các Bước Tiếp Theo Để Hoàn Tất Push Code

Nếu bạn quên passphrase của khóa `id_ed25519` cũ hoặc muốn cấu hình lại bằng một khóa mới sạch sẽ không cần passphrase (tiện lợi và không bị hỏi mật khẩu), hãy làm theo các bước sau:

### Bước 1: Tạo một SSH Key mới không đặt passphrase
Chạy lệnh sau trong terminal của bạn:
```bash
ssh-keygen -t ed25519 -C "tranthaiquocviet2007@gmail.com" -f ~/.ssh/id_ed25519_new
```
> [!IMPORTANT]
> Khi hệ thống hiển thị câu hỏi nhập passphrase:
> `Enter passphrase (empty for no passphrase):`
> Hãy **nhấn phím Enter** (để trống).
> Hệ thống hỏi xác nhận lại `Enter same passphrase again:`, tiếp tục **nhấn Enter** lần nữa.

### Bước 2: Lấy nội dung khóa công khai (Public Key) mới
Hiển thị nội dung khóa public key mới để copy:
```bash
cat ~/.ssh/id_ed25519_new.pub
```
Nội dung in ra sẽ có dạng bắt đầu bằng `ssh-ed25519 AAAAC3... tranthaiquocviet2007@gmail.com`.

### Bước 3: Thêm Public Key mới lên tài khoản GitHub của bạn
1. Truy cập vào trang web GitHub của bạn.
2. Vào **Settings** (Cài đặt tài khoản) -> **SSH and GPG keys**.
3. Nhấn nút **New SSH key**.
4. Đặt tiêu đề (ví dụ: `Cafe-TUI-New`) và dán toàn bộ nội dung bạn vừa copy ở **Bước 2** vào ô **Key**.
5. Nhấn **Add SSH key**.

### Bước 4: Thực hiện đẩy mã nguồn (Git Push) bằng khóa mới
Chạy lệnh sau trong terminal để đẩy code lên GitHub sử dụng file khóa mới:
```bash
SSH_AUTH_SOCK="" GIT_SSH_COMMAND="ssh -o AddKeysToAgent=no -o IdentitiesOnly=yes -i ~/.ssh/id_ed25519_new" git push origin main
```

Từ lần sau, bạn sẽ không bị hỏi passphrase và việc push/pull code sẽ diễn ra hoàn toàn tự động và mượt mà!
