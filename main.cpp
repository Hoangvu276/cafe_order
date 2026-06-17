/**
 * cafe_app – Ứng dụng TUI Quản lý Quán Cà Phê
 * Thiết kế theo phong cách giao diện tối giản, bo tròn của Superfile
 * Sử dụng thư viện cấu trúc dữ liệu tự viết trong lib/
 */

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <csignal>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#else
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

#include "src/ui/ui_style.hpp"
#include "src/ui/ui_renderer.hpp"
#include "src/ui/ui_icons.hpp"
#include "src/cafe_models.hpp"

// ── Định nghĩa các phím đặc biệt ─────────────────────────────────────────

constexpr int KEY_UP = 1001;
constexpr int KEY_DOWN = 1002;
constexpr int KEY_LEFT = 1003;
constexpr int KEY_RIGHT = 1004;
constexpr int KEY_ENTER = 1005;
constexpr int KEY_BACKSPACE = 1006;
constexpr int KEY_ESC = 1007;
constexpr int KEY_TAB = 1008;

// ── Thiết lập Terminal Raw Mode ──────────────────────────────────────────

#ifdef _WIN32
static DWORD orig_input_mode;
static DWORD orig_output_mode;

static void disable_raw_mode() {
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), orig_input_mode);
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), orig_output_mode);
    std::cout << "\033[?25h" << "\033[0m" << std::flush;
}

static void enable_raw_mode() {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(hStdin, &orig_input_mode);
    GetConsoleMode(hStdout, &orig_output_mode);
    atexit(disable_raw_mode);
    
    DWORD raw_input = orig_input_mode;
    raw_input &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
    SetConsoleMode(hStdin, raw_input);

    DWORD raw_output = orig_output_mode;
    raw_output |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hStdout, raw_output);
    
    // Set Console Output CodePage to UTF-8 to display borders and icons correctly
    SetConsoleOutputCP(65001);
}
#else
static struct termios orig_termios;

static void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    std::cout << "\033[?25h" << "\033[0m" << std::flush; // Hiện con trỏ & reset
}

static void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    raw.c_iflag &= ~(IXON | ICRNL);
    raw.c_cc[VMIN]  = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
#endif

#ifdef _WIN32
static std::pair<int, int> get_terminal_size() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        return { width > 0 ? width : 80, height > 0 ? height : 24 };
    }
    return { 80, 24 };
}
#else
static std::pair<int, int> get_terminal_size() {
    struct winsize ws{};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    return { ws.ws_col > 0 ? ws.ws_col : 80, ws.ws_row > 0 ? ws.ws_row : 24 };
}
#endif

static void clear_screen() {
    std::cout << "\033[2J\033[H" << std::flush;
}

// ── Bắt tín hiệu Resize ──────────────────────────────────────────────────

static volatile sig_atomic_t g_resized = 0;
#ifndef _WIN32
static void sigwinch_handler(int) { g_resized = 1; }
#endif

// ── Trình đọc phím ───────────────────────────────────────────────────────

#ifdef _WIN32
int get_key() {
    static int last_w = 0, last_h = 0;
    while (!_kbhit()) {
        auto [curr_w, curr_h] = get_terminal_size();
        if (curr_w != last_w || curr_h != last_h) {
            last_w = curr_w;
            last_h = curr_h;
            g_resized = 1;
            return 0; // Return 0 to trigger redrawing
        }
        Sleep(20);
    }
    int c = _getch();
    if (c == 0 || c == 224) {
        int next_c = _getch();
        switch (next_c) {
            case 72: return KEY_UP;
            case 80: return KEY_DOWN;
            case 75: return KEY_LEFT;
            case 77: return KEY_RIGHT;
            default: return 0;
        }
    }
    if (c == 27) return KEY_ESC;
    if (c == 8) return KEY_BACKSPACE;
    if (c == 13 || c == 10) return KEY_ENTER;
    if (c == 9) return KEY_TAB;
    return c;
}
#else
int get_key() {
    char c = 0;
    if (read(STDIN_FILENO, &c, 1) != 1) return 0;
    if (c == '\033') {
        char seq[2];
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
        int r1 = read(STDIN_FILENO, &seq[0], 1);
        int r2 = read(STDIN_FILENO, &seq[1], 1);
        fcntl(STDIN_FILENO, F_SETFL, flags);
        if (r1 == 1 && r2 == 1) {
            if (seq[0] == '[') {
                switch (seq[1]) {
                    case 'A': return KEY_UP;
                    case 'B': return KEY_DOWN;
                    case 'C': return KEY_RIGHT;
                    case 'D': return KEY_LEFT;
                }
            }
        }
        return KEY_ESC;
    }
    if (c == 127 || c == 8) return KEY_BACKSPACE;
    if (c == '\n' || c == '\r') return KEY_ENTER;
    if (c == '\t') return KEY_TAB;
    return c;
}
#endif


// ── Quản lý Cấu hình Font ────────────────────────────────────────────────

bool load_font_config() {
    std::ifstream ifs(".cafe_tui.conf");
    if (!ifs.is_open()) return false;
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.rfind("nerd_fonts=", 0) == 0) {
            EnableNerdFonts = (line.substr(11) == "1");
            return true;
        }
    }
    return false;
}

void save_font_config() {
    std::ofstream ofs(".cafe_tui.conf");
    if (ofs.is_open()) {
        ofs << "nerd_fonts=" << (EnableNerdFonts ? "1" : "0") << "\n";
    }
}

// ── Tạo ID mới tự động cho Menu ──────────────────────────────────────────

std::string generate_new_id(const LinkedList<MenuItem>& list) {
    int max_num = 0;
    for (int i = 0; i < list.size(); i++) {
        std::string id = list.at(i).id;
        if (id.size() >= 2 && id[0] == 'M') {
            try {
                int num = std::stoi(id.substr(1));
                if (num > max_num) max_num = num;
            } catch (...) {}
        }
    }
    char buf[32];
    snprintf(buf, sizeof(buf), "M%02d", max_num + 1);
    return std::string(buf);
}

// ── Các trạng thái giao diện ─────────────────────────────────────────────

enum class FocusPanel { Sidebar, Workspace };
enum class FieldFocus { CustomerName, VipCheck, ItemsList };
enum class InputState { None, AddName, AddPrice, EditName, EditPrice, ConfirmDelete };

int main() {
    // ── Khởi tạo các Manager & Nạp dữ liệu ───────────────────────────────
    MenuManager menu;
    OrderManager order_mgr;

    menu.load_from_file("menu.txt");
    order_mgr.load_history("history.txt", menu);

    // Nếu menu rỗng, thêm dữ liệu mặc định để demo
    if (menu.menu_list.empty()) {
        menu.add_item({"M01", "Ca phe sua", 29000, 15}, false);
        menu.add_item({"M02", "Tra dao cam xa", 39000, 8}, false);
        menu.add_item({"M03", "Banh mi thit", 25000, 22}, false);
        menu.add_item({"M04", "Bac xiu", 29000, 12}, false);
        menu.add_item({"M05", "Tra xanh Latte", 45000, 5}, false);
    }

    init_ui_styles();
    enable_raw_mode();
    std::cout << "\033[?25l"; // Ẩn con trỏ thực của terminal
#ifndef _WIN32
    std::signal(SIGWINCH, sigwinch_handler);
#endif

    // ── Biến trạng thái TUI ──────────────────────────────────────────────
    int term_width, term_height;
    std::tie(term_width, term_height) = get_terminal_size();

    // ── Màn hình Font Setup (Khởi động lần đầu) ──────────────────────────
    bool config_loaded = load_font_config();
    if (!config_loaded) {
        bool setting_font = true;
        while (setting_font) {
            if (g_resized) {
                g_resized = 0;
                std::tie(term_width, term_height) = get_terminal_size();
            }
            clear_screen();
            
            // Vẽ hộp cấu hình font ở chính giữa
            int box_h = 10;
            int box_w = 60;
            int start_row = (term_height - box_h) / 2;
            int start_col = (term_width - box_w) / 2;
            
            Renderer r_font(box_h, box_w, true, Style.panel, Style.border_active, " FONT SETUP ");
            r_font.add_line("  Ban co muon su dung Nerd Fonts cho giao dien dep hon?");
            r_font.add_line("  (Kieu font nhu FiraCode NF, Hack NF, v.v.)");
            r_font.add_line("");
            r_font.add_line("  Neu co: ban se thay cac bieu tuong dep: [  , 👑 ,  ]");
            r_font.add_line("  Neu khong: ung dung se dung ky tu text thong thuong.");
            r_font.add_line("");
            r_font.add_line("  [Y] De bat Nerd Fonts  |  [N] De tat / Dung Text thuong");
            
            // Render căn giữa bằng cách in dòng trống
            std::string empty_lines = "";
            for (int i = 0; i < start_row; i++) empty_lines += "\n";
            std::string box_str = r_font.render();
            std::istringstream bss(box_str);
            std::string line;
            std::string pad(start_col > 0 ? start_col : 0, ' ');
            while (std::getline(bss, line)) {
                empty_lines += pad + line + "\n";
            }
            std::cout << empty_lines << std::flush;
            
            int k = get_key();
            if (k == 'y' || k == 'Y') {
                EnableNerdFonts = true;
                save_font_config();
                setting_font = false;
            } else if (k == 'n' || k == 'N') {
                EnableNerdFonts = false;
                save_font_config();
                setting_font = false;
            }
        }
    }

    FocusPanel focus = FocusPanel::Sidebar;
    int sidebar_cursor = 0; // 0: Menu, 1: Tao Don, 2: Xu ly, 3: Thong ke, 4: Thoat
    int menu_cursor = 0;
    int order_menu_cursor = 0;
    
    // Tạo đơn hàng tạm thời
    std::string new_cust_name = "";
    bool new_is_vip = false;
    Vector<std::pair<MenuItem, int>> new_order_items; // {Item, Qty}
    FieldFocus order_focus = FieldFocus::CustomerName;

    // Trạng thái Input Inline
    InputState input_state = InputState::None;
    std::string input_buffer = "";
    std::string temp_name = "";
    double temp_price = 0.0;
    std::string flash_message = "";
    UIStyle flash_style = Style.success;
    int flash_timer = 0;

    // Sắp xếp
    int sort_state = 0; // 0: ID, 1: Gia Tang, 2: Gia Giam, 3: Ten A-Z, 4: Ten Z-A

    bool running = true;
    while (running) {
        if (g_resized) {
            g_resized = 0;
            std::tie(term_width, term_height) = get_terminal_size();
        }

        // ── Xử lý đếm ngược tin nhắn thông báo ────────────────────────────
        if (!flash_message.empty()) {
            if (++flash_timer > 30) { // Biến mất sau vài lượt render
                flash_message = "";
                flash_timer = 0;
            }
        }

        // ── Tính toán Layout ─────────────────────────────────────────────
        int sidebar_width = term_width * 25 / 100;
        if (sidebar_width < 18) sidebar_width = 18;
        if (sidebar_width > 28) sidebar_width = 28;
        int workspace_width = term_width - sidebar_width;
        int main_height = term_height - 5; // Để lại 5 dòng cho footer

        // ── Vẽ Sidebar (Menu Chính) ──────────────────────────────────────
        Renderer r_side(main_height, sidebar_width, true, Style.sidebar, 
                         focus == FocusPanel::Sidebar ? Style.border_active : Style.border_inactive, 
                         " COFFEE TUI ");
        r_side.add_line(Style.title.apply_bold("  DANH MỤC"));
        r_side.add_line("");
        
        std::vector<std::string> side_opts = {
            Icons::Coffee() + " Quan ly Menu",
            Icons::Plus() + " Tao don hang",
            Icons::Check() + " Xu ly don hang",
            Icons::Stats() + " Thong ke & bao cao",
            Icons::Cross() + " Thoat chuong trinh"
        };
        for (int i = 0; i < (int)side_opts.size(); i++) {
            std::string cursor_char = (i == sidebar_cursor) ? " > " : "   ";
            if (i == sidebar_cursor && focus == FocusPanel::Sidebar) {
                r_side.add_line(Style.selected.apply(cursor_char + side_opts[i]));
            } else if (i == sidebar_cursor) {
                r_side.add_line(Style.cursor.apply(cursor_char) + Style.sidebar.apply(side_opts[i]));
            } else {
                r_side.add_line(Style.sidebar.apply(cursor_char + side_opts[i]));
            }
        }
        r_side.set_info({"v1.0.0"});

        // ── Vẽ Workspace ─────────────────────────────────────────────────
        Renderer r_work(main_height, workspace_width, true, Style.panel,
                         focus == FocusPanel::Workspace ? Style.border_active : Style.border_inactive,
                         " WORKSPACE ");

        if (sidebar_cursor == 0) {
            // ── MÀN HÌNH 1: QUẢN LÝ MENU ──────────────────────────────────
            r_work.set_info({"Menu Items", std::to_string(menu.menu_list.size()) + " mon"});
            
            std::string sort_labels[] = {"Mac dinh ID", "Gia tang dan", "Gia giam dan", "Ten A-Z", "Ten Z-A"};
            r_work.add_line(Style.gray.apply(" " + Icons::Sorted() + " Sap xep: ") + Style.info.apply(sort_labels[sort_state]) + 
                            Style.gray.apply(" | [S] de doi kieu sap xep"));
            r_work.add_line("");

            if (input_state == InputState::AddName || input_state == InputState::AddPrice) {
                r_work.add_line(Style.warning.apply_bold("  " + Icons::Plus() + " THEM MON MOI"));
                r_work.add_line("");
                std::string display_name = (input_state == InputState::AddName) ? (input_buffer + "_") : temp_name;
                std::string display_price = (input_state == InputState::AddPrice) ? (input_buffer + "_") : "";
                r_work.add_line("  Ten mon: " + display_name);
                r_work.add_line("  Gia mon: " + display_price + "đ");
                r_work.add_line("");
                r_work.add_line(Style.gray.apply("  Enter: Xac nhan | ESC: Huy"));
            } 
            else if (input_state == InputState::EditName || input_state == InputState::EditPrice) {
                r_work.add_line(Style.warning.apply_bold("  " + Icons::Edit() + " SUA THONG TIN MON"));
                r_work.add_line("");
                std::string display_name = "";
                std::string display_price = "";
                if (input_state == InputState::EditName) {
                    display_name = input_buffer + "_";
                    if (menu_cursor < menu.menu_list.size()) {
                        display_price = format_currency(menu.menu_list.at(menu_cursor).price);
                    }
                } else { // EditPrice
                    display_name = temp_name;
                    display_price = input_buffer + "_đ";
                }
                r_work.add_line("  Ten mon: " + display_name);
                r_work.add_line("  Gia mon: " + display_price);
                r_work.add_line("");
                r_work.add_line(Style.gray.apply("  Enter: Xac nhan | ESC: Huy"));
            }
            else if (input_state == InputState::ConfirmDelete) {
                r_work.add_line(Style.error.apply_bold("  " + Icons::Trash() + " XAC NHAN XOA MON"));
                r_work.add_line("");
                if (menu_cursor < menu.menu_list.size()) {
                    auto item = menu.menu_list.at(menu_cursor);
                    r_work.add_line("  Ban co chac chan muon xoa mon: " + Style.error.apply(item.name) + "?");
                }
                r_work.add_line("");
                r_work.add_line(Style.error.apply("  [Enter] Xoa luon ") + Style.gray.apply(" | [ESC] Quay lai"));
            }
            else {
                // Hiển thị bảng món ăn
                int col_name_w = workspace_width - 27;
                if (col_name_w < 10) col_name_w = 10;
                
                std::string header = pad_right(" ID", 5) + pad_right(Icons::Coffee() + " Ten mon", col_name_w) + 
                                     pad_right(Icons::Money() + " Gia", 12) + "Da ban";
                r_work.add_line(Style.title.apply_bold(header));
                r_work.add_line(Style.gray.apply(std::string(workspace_width - 2, '-')));

                if (menu.menu_list.empty()) {
                    r_work.add_line("  (Khong co mon nao trong Menu. Nhan A de them)");
                } else {
                    for (int i = 0; i < menu.menu_list.size(); i++) {
                        const auto& item = menu.menu_list.at(i);
                        std::string id_str = " " + item.id;
                        std::string name_str = truncate_right(item.name, col_name_w - 2);
                        std::string price_str = format_currency(item.price);
                        std::string sold_str = std::to_string(item.sold_count);

                        std::string line = pad_right(id_str, 5) + pad_right(name_str, col_name_w) + 
                                           pad_right(price_str, 12) + sold_str;

                        if (i == menu_cursor && focus == FocusPanel::Workspace) {
                            r_work.add_line(Style.selected.apply(line));
                        } else {
                            r_work.add_line(Style.panel.apply(line));
                        }
                    }
                }
            }
        }
        else if (sidebar_cursor == 1) {
            // ── MÀN HÌNH 2: TẠO ĐƠN HÀNG ──────────────────────────────────
            r_work.set_info({"Order Creator"});

            // Tính tổng tiền đơn hàng tạm thời
            double current_total = 0;
            for (int i = 0; i < new_order_items.size(); i++) {
                current_total += new_order_items[i].first.price * new_order_items[i].second;
            }

            // Tiêu đề & Thông tin cơ bản
            r_work.add_line(Style.title.apply_bold("  " + Icons::Plus() + " HOA DON MOI"));
            r_work.add_line("");

            // Dòng nhập tên khách hàng
            std::string name_field = "  Ten khach hang: [" + new_cust_name + 
                                     ((order_focus == FieldFocus::CustomerName) ? "_" : "") + "]";
            if (order_focus == FieldFocus::CustomerName && focus == FocusPanel::Workspace) {
                r_work.add_line(Style.border_active.apply(name_field));
            } else {
                r_work.add_line(Style.panel.apply(name_field));
            }

            // Dòng chọn VIP
            std::string vip_char = new_is_vip ? Icons::VIP() : " ";
            std::string vip_field = "  Khach hang VIP: [" + vip_char + "] (Nhan Space de Toggle)";
            if (order_focus == FieldFocus::VipCheck && focus == FocusPanel::Workspace) {
                r_work.add_line(Style.border_active.apply(vip_field));
            } else {
                r_work.add_line(Style.panel.apply(vip_field));
            }

            r_work.add_line(Style.gray.apply("  " + std::string(workspace_width - 6, '-')));

            // Giỏ hàng hiện tại
            r_work.add_line("  Gio hang cua khach:");
            if (new_order_items.empty()) {
                r_work.add_line(Style.gray.apply("    (Chua chon mon nao. Di chuyen xuong de chon)"));
            } else {
                for (int i = 0; i < new_order_items.size(); i++) {
                    const auto& it = new_order_items[i].first;
                    int qty = new_order_items[i].second;
                    r_work.add_line("    - " + it.name + " x" + std::to_string(qty) + 
                                    " (" + format_currency(it.price * qty) + ")");
                }
            }
            r_work.add_line("  Tong cong: " + Style.success.apply_bold(format_currency(current_total)));
            r_work.add_line(Style.gray.apply("  " + std::string(workspace_width - 6, '-')));

            // Danh sách lựa chọn món ở dưới
            r_work.add_line(Style.info.apply("  Danh sach thuc don:"));
            int item_col_w = workspace_width - 25;
            for (int i = 0; i < menu.menu_list.size(); i++) {
                const auto& item = menu.menu_list.at(i);
                
                // Kiểm tra xem món này có trong giỏ hàng tạm chưa
                bool selected_in_cart = false;
                int cart_qty = 0;
                for (int j = 0; j < new_order_items.size(); j++) {
                    if (new_order_items[j].first.id == item.id) {
                        selected_in_cart = true;
                        cart_qty = new_order_items[j].second;
                        break;
                    }
                }

                std::string check_box = selected_in_cart ? "[" + Icons::Check() + "]" : "[ ]";
                std::string qty_str = selected_in_cart ? " x" + std::to_string(cart_qty) : "";
                
                std::string line = "    " + check_box + " " + item.id + " - " + 
                                   pad_right(item.name + qty_str, item_col_w) + 
                                   format_currency(item.price);

                if (i == order_menu_cursor && order_focus == FieldFocus::ItemsList && focus == FocusPanel::Workspace) {
                    r_work.add_line(Style.selected.apply(line));
                } else {
                    r_work.add_line(Style.panel.apply(line));
                }
            }
        }
        else if (sidebar_cursor == 2) {
            // ── MÀN HÌNH 3: XỬ LÝ ĐƠN HÀNG ────────────────────────────────
            r_work.set_info({"Orders Process"});

            // 1. Vẽ Đơn hàng đang thực hiện ở phía trên cùng
            r_work.add_line(Style.title.apply_bold("  " + Icons::Coffee() + " ĐƠN HÀNG ĐANG THỰC HIỆN"));
            if (!order_mgr.has_current_order) {
                r_work.add_line(Style.gray.apply("    (Khong co don hang nao dang thuc hien)"));
                r_work.add_line("");
            } else {
                const auto& o = order_mgr.current_order;
                std::string type_label = o.is_vip ? Style.warning.apply("[" + Icons::VIP() + " VIP]") : "[Thuong]";
                r_work.add_line("    Ma don: #" + std::to_string(o.id) + " | " + type_label + 
                                " | Khach: " + o.customer_name + 
                                " | Tong: " + format_currency(o.total_price));
                std::string items_summary = "    Mon: ";
                for (int i = 0; i < o.items.size(); i++) {
                    items_summary += o.items[i].first.name + " x" + std::to_string(o.items[i].second) + "; ";
                }
                r_work.add_line(Style.gray.apply(truncate_right(items_summary, workspace_width - 6, "...")));
                r_work.add_line("");
            }

            r_work.add_line(Style.gray.apply(std::string(workspace_width - 2, '=')));
            r_work.add_line("");
            
            // 2. Vẽ 2 Queue song song bên dưới
            int half_w = (workspace_width - 3) / 2;
            
            // Collect VIP Queue items
            std::vector<Order> vips;
            {
                PriorityQueue<Order, OrderCmp> temp_vip = order_mgr.vip_queue;
                while (!temp_vip.empty()) {
                    vips.push_back(temp_vip.extract());
                }
            }

            // Collect Normal Queue items
            std::vector<Order> norms;
            {
                Queue<Order> temp_norm = order_mgr.normal_queue;
                while (!temp_norm.empty()) {
                    norms.push_back(temp_norm.front());
                    temp_norm.pop();
                }
            }

            std::string vip_head = Style.warning.apply_bold(" " + Icons::VIP() + " HANG DOI VIP (Heap) [" + std::to_string(vips.size()) + "]");
            std::string norm_head = Style.info.apply_bold(" " + Icons::Coffee() + " HANG DOI THUONG [" + std::to_string(norms.size()) + "]");
            
            std::string left_col, right_col;
            
            left_col += vip_head + "\n";
            left_col += std::string(half_w, '-') + "\n";
            if (vips.empty()) {
                left_col += "  (Rong)\n";
            } else {
                for (const auto& o : vips) {
                    left_col += "  #" + std::to_string(o.id) + " - " + truncate_right(o.customer_name, half_w - 12) + 
                                " (" + format_currency(o.total_price) + ")\n";
                    std::string items_str = "   └─ ";
                    for (int j = 0; j < o.items.size(); j++) {
                        items_str += o.items[j].first.name + " x" + std::to_string(o.items[j].second);
                        if (j + 1 < o.items.size()) items_str += ", ";
                    }
                    left_col += Style.gray.apply(truncate_right(items_str, half_w - 2)) + "\n";
                }
            }

            right_col += norm_head + "\n";
            right_col += std::string(half_w, '-') + "\n";
            if (norms.empty()) {
                right_col += "  (Rong)\n";
            } else {
                for (const auto& o : norms) {
                    right_col += "  #" + std::to_string(o.id) + " - " + truncate_right(o.customer_name, half_w - 12) + 
                                 " (" + format_currency(o.total_price) + ")\n";
                    std::string items_str = "   └─ ";
                    for (int j = 0; j < o.items.size(); j++) {
                        items_str += o.items[j].first.name + " x" + std::to_string(o.items[j].second);
                        if (j + 1 < o.items.size()) items_str += ", ";
                    }
                    right_col += Style.gray.apply(truncate_right(items_str, half_w - 2)) + "\n";
                }
            }

            std::string queues_layout = merge_horizontal(left_col, right_col);
            
            std::istringstream qss(queues_layout);
            std::string line;
            while (std::getline(qss, line)) {
                r_work.add_line(line);
            }

            r_work.add_line("");
            r_work.add_line(Style.gray.apply(std::string(workspace_width - 2, '=')));
            r_work.add_line("");

            // 3. Đơn hàng vừa xử lý gần nhất ở dưới cùng
            r_work.add_line(Style.success.apply_bold("  " + Icons::History() + " DON DA XU LY GAN NHAT:"));
            if (order_mgr.history_list.empty()) {
                r_work.add_line(Style.gray.apply("    Chua co don hang nao duoc xu ly."));
            } else {
                int last_idx = order_mgr.history_list.size() - 1;
                const auto& last_order = order_mgr.history_list.at(last_idx);
                std::string type_label = last_order.is_vip ? Style.warning.apply("[" + Icons::VIP() + "]") : "[Thuong]";
                r_work.add_line("    Ma don: #" + std::to_string(last_order.id) + " | " + type_label + 
                                " | Khach: " + last_order.customer_name + 
                                " | Tong: " + format_currency(last_order.total_price));
                std::string items_summary = "    Mon: ";
                for (int i = 0; i < last_order.items.size(); i++) {
                    items_summary += last_order.items[i].first.name + " x" + std::to_string(last_order.items[i].second) + "; ";
                }
                r_work.add_line(Style.gray.apply(truncate_right(items_summary, workspace_width - 6, "...")));
            }
        }
        else if (sidebar_cursor == 3) {
            // ── MÀN HÌNH 4: THỐNG KÊ & BÁO CÁO ─────────────────────────────
            r_work.set_info({"Report & Analytics"});
            
            r_work.add_line(Style.title.apply_bold("  " + Icons::Stats() + " BAO CAO DOANH THU & TOP MON"));
            r_work.add_line("");

            // 1. Lấy Top 5 món bán chạy nhất bằng mergeSort
            std::vector<MenuItem> top_items;
            for (int i = 0; i < menu.menu_list.size(); i++) {
                top_items.push_back(menu.menu_list.at(i));
            }
            if (!top_items.empty()) {
                // Sắp xếp giảm dần theo sold_count
                merge_sort(top_items.data(), 0, top_items.size() - 1, [](const MenuItem& a, const MenuItem& b) {
                    return a.sold_count > b.sold_count;
                });
            }

            int half_w = (workspace_width - 3) / 2;
            std::string left_col, right_col;

            left_col += Style.success.apply_bold(" " + Icons::Coffee() + " Top 5 mon ban chay nhat:") + "\n";
            left_col += std::string(half_w, '-') + "\n";
            int show_cnt = std::min((int)top_items.size(), 5);
            if (show_cnt == 0) {
                left_col += "  Chua ban duoc mon nao.\n";
            } else {
                for (int i = 0; i < show_cnt; i++) {
                    left_col += "  " + std::to_string(i+1) + ". " + 
                                truncate_right(top_items[i].name, half_w - 15) + " -> " + 
                                Style.warning.apply(std::to_string(top_items[i].sold_count) + " luot") + "\n";
                }
            }

            // 2. Lấy doanh thu từ cây AVL (duyệt inorder)
            right_col += Style.info.apply_bold(" " + Icons::Money() + " Doanh thu theo ngay (Cây AVL):") + "\n";
            right_col += std::string(half_w, '-') + "\n";
            std::vector<DailyRevenue> revs;
            order_mgr.revenue_tree.inorder([&](const DailyRevenue& dr) {
                revs.push_back(dr);
            });
            
            if (revs.empty()) {
                right_col += "  Chua co doanh thu.\n";
            } else {
                // Hiển thị 5 ngày gần nhất
                int start_rev = std::max(0, (int)revs.size() - 5);
                for (int i = start_rev; i < (int)revs.size(); i++) {
                    right_col += "  " + revs[i].date + ": " + 
                                 Style.success.apply(format_currency(revs[i].revenue)) + "\n";
                }
            }

            std::string stats_layout = merge_horizontal(left_col, right_col);
            std::istringstream sss(stats_layout);
            std::string line;
            while (std::getline(sss, line)) {
                r_work.add_line(line);
            }

            r_work.add_line("");
            r_work.add_line(Style.gray.apply(std::string(workspace_width - 2, '=')));
            r_work.add_line("");

            // Tổng quan chung
            double total_revenue_all = 0;
            for (const auto& r : revs) total_revenue_all += r.revenue;
            
            r_work.add_line("  Tong doanh thu: " + Style.success.apply_bold(format_currency(total_revenue_all)));
            r_work.add_line("  Tong so don da xu ly: " + std::to_string(order_mgr.history_list.size()) + " don");
        }

        // ── Vẽ cả 2 Panel lên màn hình ───────────────────────────────────
        std::cout << "\033[H";
        std::string full_frame = merge_horizontal(r_side.render(), r_work.render());
        std::cout << full_frame;

        // ── Vẽ Footer (Status bar & Hướng dẫn phím tắt) ───────────────────
        std::string footer_line = std::string(term_width, '=') + "\n";
        
        // Dòng tin nhắn thông báo (Flash Message)
        if (!flash_message.empty()) {
            footer_line += "  MESSAGE: " + flash_style.apply_bold(flash_message) + "\n";
        } else {
            footer_line += "  Trang thai: " + Style.success.apply("Hoat dong on dinh") + " | Tab: Chuyen Panel | m: Ve menu chinh\n";
        }

        // Hướng dẫn phím tắt động theo ngữ cảnh
        std::string hotkeys = "  Phim tat: ";
        if (focus == FocusPanel::Sidebar) {
            if (sidebar_cursor == 2) {
                hotkeys += "[↑/↓] Di chuyen muc  |  [Enter] Chon  |  [P] Xu ly don  |  [U] Hoan tac";
            } else if (sidebar_cursor == 1) {
                hotkeys += "[↑/↓] Di chuyen muc  |  [Enter] Chon de Tao don";
            } else {
                hotkeys += "[↑/↓] Di chuyen muc  |  [Enter/Tab/→] Chon";
            }
        } else {
            if (sidebar_cursor == 0) { // Quản lý menu
                if (input_state != InputState::None) {
                    hotkeys += "[Phim chu/So] Nhap lieu  |  [Backspace] Xoa  |  [Enter] Tiep tuc/Luu  |  [ESC] Huy";
                } else {
                    hotkeys += "[↑/↓] Chon mon  |  [A] Them mon  |  [E] Sua  |  [D] Xoa  |  [S] Sap xep  |  [U] Hoan tac  |  [ESC/←] Menu";
                }
            } else if (sidebar_cursor == 1) { // Tạo đơn hàng
                if (order_focus == FieldFocus::CustomerName) {
                    hotkeys += "[Phim chu] Nhap ten  |  [Backspace] Xoa  |  [Enter/↓] Sang o VIP";
                } else if (order_focus == FieldFocus::VipCheck) {
                    hotkeys += "[Space] Toggle VIP  |  [Enter/↓] Chon mon  |  [T] Thanh toan  |  [C] Huy don  |  [↑] Nhap ten";
                } else {
                    hotkeys += "[↑/↓] Chon mon  |  [Space] Chon/Bo mon  |  [+] Tang qty  |  [-] Giam qty  |  [T] Thanh toan  |  [C] Huy don  |  [↑] VIP";
                }
            } else if (sidebar_cursor == 2) { // Xử lý đơn
                hotkeys += "[P] Xu ly don tiep theo  |  [U] Hoan tac xu ly don  |  [ESC/←] Menu";
            } else {
                hotkeys += "[ESC/←] Ve menu chinh";
            }
        }
        footer_line += Style.gray.apply(hotkeys) + "\n";
        
        std::cout << footer_line << "\033[J" << std::flush;

        // ── Đọc và Xử lý phím bấm ────────────────────────────────────────
        int key = get_key();
        if (key == 0) continue;

        // Bật phím 'm' làm hotkey quay lại menu chính từ bất cứ đâu
        if (key == 'm' && input_state == InputState::None) {
            focus = FocusPanel::Sidebar;
            continue;
        }

        if (focus == FocusPanel::Sidebar) {
            // Hỗ trợ xử lý nhanh phím bấm của màn hình Xử lý Đơn hàng ngay khi đang ở Sidebar
            if (sidebar_cursor == 2) {
                if (key == 'p' || key == 'P') {
                    Order processed;
                    if (order_mgr.process_next_order(processed, menu)) {
                        order_mgr.save_history("history.txt");
                        menu.save_to_file("menu.txt");
                        flash_message = "Da xu ly xong don #" + std::to_string(processed.id) + 
                                        " cho khach: " + processed.customer_name;
                        flash_style = Style.success;
                    } else {
                        flash_message = "Hang doi trong, khong co don nao can xu ly!";
                        flash_style = Style.error;
                    }
                    continue;
                }
                else if (key == 'u' || key == 'U') {
                    if (order_mgr.undo_process_order(menu)) {
                        order_mgr.save_history("history.txt");
                        menu.save_to_file("menu.txt");
                        flash_message = "Da hoan tac xu ly don hang cu nhat!";
                        flash_style = Style.success;
                    } else {
                        flash_message = "Lich su rong, khong the hoan tac!";
                        flash_style = Style.error;
                    }
                    continue;
                }
            }

            // ── ĐIỀU HƯỚNG SIDEBAR ────────────────────────────────────────
            switch (key) {
                case KEY_UP:
                case 'k':
                    if (sidebar_cursor > 0) sidebar_cursor--;
                    break;
                case KEY_DOWN:
                case 'j':
                    if (sidebar_cursor < 4) sidebar_cursor++;
                    break;
                case KEY_ENTER:
                case KEY_RIGHT:
                case KEY_TAB:
                    if (sidebar_cursor == 4) {
                        running = false; // Thoát
                    } else {
                        focus = FocusPanel::Workspace;
                        // Khởi tạo trạng thái khi tập trung vào Workspace
                        menu_cursor = 0;
                        order_menu_cursor = 0;
                        input_state = InputState::None;
                        input_buffer = "";
                    }
                    break;
            }
        } 
        else {
            // ── ĐIỀU HƯỚNG WORKSPACE ──────────────────────────────────────
            if (key == KEY_LEFT || (key == KEY_ESC && input_state == InputState::None)) {
                focus = FocusPanel::Sidebar;
                continue;
            }

            if (sidebar_cursor == 0) {
                // ── Nghiệp vụ Màn hình 1: Quản lý Menu ────────────────────
                if (input_state != InputState::None) {
                    // Đang ở chế độ nhập liệu inline
                    if (key == KEY_ESC) {
                        input_state = InputState::None;
                        input_buffer = "";
                        flash_message = "Da huy thao tac";
                        flash_style = Style.error;
                    } 
                    else if (key == KEY_ENTER) {
                        if (input_state == InputState::AddName) {
                            if (input_buffer.empty()) {
                                flash_message = "Ten mon khong duoc de trong!";
                                flash_style = Style.error;
                            } else {
                                temp_name = input_buffer;
                                input_buffer = "";
                                input_state = InputState::AddPrice;
                            }
                        } 
                        else if (input_state == InputState::AddPrice) {
                            try {
                                if (input_buffer.empty()) throw std::invalid_argument("");
                                temp_price = std::stod(input_buffer);
                                std::string new_id = generate_new_id(menu.menu_list);
                                menu.add_item({new_id, temp_name, temp_price, 0});
                                menu.save_to_file("menu.txt");
                                flash_message = "Them mon " + temp_name + " thanh cong!";
                                flash_style = Style.success;
                                input_state = InputState::None;
                                input_buffer = "";
                            } catch (...) {
                                flash_message = "Gia tien khong hop le!";
                                flash_style = Style.error;
                            }
                        }
                        else if (input_state == InputState::EditName) {
                            if (input_buffer.empty()) {
                                flash_message = "Ten mon khong duoc de trong!";
                                flash_style = Style.error;
                            } else {
                                temp_name = input_buffer;
                                input_buffer = "";
                                // Nạp giá trị cũ làm gợi ý
                                if (menu_cursor < menu.menu_list.size()) {
                                    std::stringstream ss;
                                    ss << std::fixed << std::setprecision(0) << menu.menu_list.at(menu_cursor).price;
                                    input_buffer = ss.str();
                                }
                                input_state = InputState::EditPrice;
                            }
                        }
                        else if (input_state == InputState::EditPrice) {
                            try {
                                if (input_buffer.empty()) throw std::invalid_argument("");
                                temp_price = std::stod(input_buffer);
                                if (menu_cursor < menu.menu_list.size()) {
                                    std::string id = menu.menu_list.at(menu_cursor).id;
                                    menu.edit_item(id, temp_name, temp_price);
                                    menu.save_to_file("menu.txt");
                                    flash_message = "Sua thong tin mon thanh cong!";
                                    flash_style = Style.success;
                                }
                                input_state = InputState::None;
                                input_buffer = "";
                            } catch (...) {
                                flash_message = "Gia tien khong hop le!";
                                flash_style = Style.error;
                            }
                        }
                        else if (input_state == InputState::ConfirmDelete) {
                            if (menu_cursor < menu.menu_list.size()) {
                                std::string id = menu.menu_list.at(menu_cursor).id;
                                std::string name = menu.menu_list.at(menu_cursor).name;
                                menu.delete_item(id);
                                menu.save_to_file("menu.txt");
                                flash_message = "Da xoa mon " + name;
                                flash_style = Style.success;
                                if (menu_cursor >= menu.menu_list.size() && menu_cursor > 0) {
                                    menu_cursor--;
                                }
                            }
                            input_state = InputState::None;
                        }
                    } 
                    else if (key == KEY_BACKSPACE) {
                        if (!input_buffer.empty()) {
                            // Xử lý xóa ký tự UTF-8 an toàn (pop_back byte cuối)
                            input_buffer.pop_back();
                        }
                    } 
                    else if (key >= 32 && key < 127) {
                        input_buffer += static_cast<char>(key);
                    }
                } 
                else {
                    // Chế độ xem & phím tắt thường
                    switch (key) {
                        case KEY_UP:
                        case 'k':
                            if (menu_cursor > 0) menu_cursor--;
                            break;
                        case KEY_DOWN:
                        case 'j':
                            if (menu_cursor < menu.menu_list.size() - 1) menu_cursor++;
                            break;
                        case 'a':
                        case 'A':
                            input_state = InputState::AddName;
                            input_buffer = "";
                            temp_name = "";
                            break;
                        case 'e':
                        case 'E':
                            if (!menu.menu_list.empty()) {
                                input_state = InputState::EditName;
                                temp_name = menu.menu_list.at(menu_cursor).name;
                                input_buffer = temp_name;
                            }
                            break;
                        case 'd':
                        case 'D':
                            if (!menu.menu_list.empty()) {
                                input_state = InputState::ConfirmDelete;
                            }
                            break;
                        case 'u':
                        case 'U':
                            if (menu.undo()) {
                                menu.save_to_file("menu.txt");
                                flash_message = "Da hoan tac thao tac truoc do!";
                                flash_style = Style.success;
                                if (menu_cursor >= menu.menu_list.size() && menu_cursor > 0) {
                                    menu_cursor = menu.menu_list.size() - 1;
                                }
                            } else {
                                flash_message = "Khong con thao tac nao de hoan tac!";
                                flash_style = Style.error;
                            }
                            break;
                        case 's':
                        case 'S':
                            sort_state = (sort_state + 1) % 5;
                            if (sort_state == 0) {
                                // Nạp lại mặc định từ file
                                menu.load_from_file("menu.txt");
                            } else if (sort_state == 1) {
                                menu.sort_by_price(true);
                            } else if (sort_state == 2) {
                                menu.sort_by_price(false);
                            } else if (sort_state == 3) {
                                menu.sort_by_name(true);
                            } else if (sort_state == 4) {
                                menu.sort_by_name(false);
                            }
                            menu_cursor = 0;
                            break;
                    }
                }
            } 
            else if (sidebar_cursor == 1) {
                // ── Nghiệp vụ Màn hình 2: Tạo Đơn hàng ─────────────────────
                // Phím tắt thanh toán T và hủy đơn C toàn cục khi không ở trong ô nhập tên
                if (order_focus != FieldFocus::CustomerName) {
                    if (key == 't' || key == 'T') {
                        if (new_cust_name.empty()) {
                            flash_message = "Vui long nhap ten khach hang!";
                            flash_style = Style.error;
                        } else if (new_order_items.empty()) {
                            flash_message = "Gio hang dang rong!";
                            flash_style = Style.error;
                        } else {
                            double total = 0;
                            for (int j = 0; j < new_order_items.size(); j++) {
                                total += new_order_items[j].first.price * new_order_items[j].second;
                            }
                            Order order;
                            order.customer_name = new_cust_name;
                            order.is_vip = new_is_vip;
                            order.items = new_order_items;
                            order.total_price = total;
                            
                            order_mgr.add_order(order);
                            flash_message = "Da them don hang #" + std::to_string(order.id) + " vao hang doi!";
                            flash_style = Style.success;
                            
                            // Reset form
                            new_cust_name = "";
                            new_is_vip = false;
                            new_order_items.clear();
                            order_focus = FieldFocus::CustomerName;
                        }
                        continue;
                    }
                    else if (key == 'c' || key == 'C') {
                        new_cust_name = "";
                        new_is_vip = false;
                        new_order_items.clear();
                        order_focus = FieldFocus::CustomerName;
                        flash_message = "Da xoa trang hoa don tam tinh";
                        flash_style = Style.warning;
                        continue;
                    }
                }

                if (order_focus == FieldFocus::CustomerName) {
                    if (key == KEY_ENTER || key == KEY_DOWN) {
                        order_focus = FieldFocus::VipCheck;
                    } else if (key == KEY_BACKSPACE) {
                        if (!new_cust_name.empty()) new_cust_name.pop_back();
                    } else if (key >= 32 && key < 127) {
                        new_cust_name += static_cast<char>(key);
                    }
                } 
                else if (order_focus == FieldFocus::VipCheck) {
                    if (key == ' ' || key == KEY_ENTER) {
                        new_is_vip = !new_is_vip;
                    } else if (key == KEY_UP) {
                        order_focus = FieldFocus::CustomerName;
                    } else if (key == KEY_DOWN) {
                        order_focus = FieldFocus::ItemsList;
                    }
                } 
                else if (order_focus == FieldFocus::ItemsList) {
                    switch (key) {
                        case KEY_UP:
                            if (order_menu_cursor > 0) {
                                order_menu_cursor--;
                            } else {
                                order_focus = FieldFocus::VipCheck;
                            }
                            break;
                        case KEY_DOWN:
                            if (order_menu_cursor < menu.menu_list.size() - 1) {
                                order_menu_cursor++;
                            }
                            break;
                        case ' ': {
                            // Tích chọn hoặc bỏ chọn món
                            if (order_menu_cursor < menu.menu_list.size()) {
                                auto item = menu.menu_list.at(order_menu_cursor);
                                int found_idx = -1;
                                for (int j = 0; j < new_order_items.size(); j++) {
                                    if (new_order_items[j].first.id == item.id) {
                                        found_idx = j;
                                        break;
                                    }
                                }
                                if (found_idx != -1) {
                                    // Bỏ chọn
                                    new_order_items.erase(found_idx);
                                    flash_message = "Da xoa " + item.name + " khoi gio hang";
                                    flash_style = Style.warning;
                                } else {
                                    // Thêm mới
                                    new_order_items.push_back({item, 1});
                                    flash_message = "Da them " + item.name + " vao gio hang";
                                    flash_style = Style.success;
                                }
                            }
                            break;
                        }
                        case '+':
                        case '=': { // Tăng số lượng
                            if (order_menu_cursor < menu.menu_list.size()) {
                                std::string id = menu.menu_list.at(order_menu_cursor).id;
                                for (int j = 0; j < new_order_items.size(); j++) {
                                    if (new_order_items[j].first.id == id) {
                                        new_order_items[j].second++;
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                        case '-': { // Giảm số lượng
                            if (order_menu_cursor < menu.menu_list.size()) {
                                std::string id = menu.menu_list.at(order_menu_cursor).id;
                                for (int j = 0; j < new_order_items.size(); j++) {
                                    if (new_order_items[j].first.id == id) {
                                        if (new_order_items[j].second > 1) {
                                            new_order_items[j].second--;
                                        } else {
                                            new_order_items.erase(j);
                                        }
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                    }
                }
            } 
            else if (sidebar_cursor == 2) {
                // ── Nghiệp vụ Màn hình 3: Xử lý Đơn hàng ──────────────────
                if (key == 'p' || key == 'P') {
                    Order processed;
                    if (order_mgr.process_next_order(processed, menu)) {
                        order_mgr.save_history("history.txt");
                        menu.save_to_file("menu.txt"); // Lưu lượt bán mới
                        flash_message = "Da xu ly xong don #" + std::to_string(processed.id) + 
                                        " cho khach: " + processed.customer_name;
                        flash_style = Style.success;
                    } else {
                        flash_message = "Hang doi trong, khong co don nao can xu ly!";
                        flash_style = Style.error;
                    }
                } 
                else if (key == 'u' || key == 'U') {
                    // Hoàn tác xử lý đơn
                    if (order_mgr.undo_process_order(menu)) {
                        order_mgr.save_history("history.txt");
                        menu.save_to_file("menu.txt");
                        flash_message = "Da hoan tac xu ly don hang cu nhat!";
                        flash_style = Style.success;
                    } else {
                        flash_message = "Lich su rong, khong the hoan tac!";
                        flash_style = Style.error;
                    }
                }
            }
        }
    }

    disable_raw_mode();
    clear_screen();
    std::cout << "Cam on ban da su dung ung dung Cafe Order TUI!\n";
    return 0;
}
