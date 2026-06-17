# Hướng Dẫn Cấu Hình FTXUI với CMake (v1.0.0)

Dự án này là hướng dẫn thực hành cách tích hợp và sử dụng thư viện giao diện dòng lệnh đa nền tảng **FTXUI** bằng CMake thông qua `FetchContent`.

## Cấu Trúc Thư Mục
```text
my_tui_project/
├── CMakeLists.txt  # File cấu hình build system và tự động tải thư viện
└── main.cpp        # Mã nguồn chính chứa giao diện TUI
```

## Yêu Cầu Hệ Thống (Prerequisites)
* **Trình biên dịch (Compiler):** GCC 9+ hoặc MSVC 2019+
* **Build System:** CMake 3.15+
* **Chuẩn C++:** C++17 hoặc C++20

## Các File Cấu Hình

### 1. CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.15)
project(MyTuiApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG        v5.0.0
)
FetchContent_MakeAvailable(ftxui)

add_executable(MyTuiApp main.cpp)
target_link_libraries(MyTuiApp PRIVATE ftxui::screen ftxui::dom ftxui::component)
```

### 2. main.cpp (Interactive Version)
```cpp
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <iostream>
#include <string>

int main() {
    using namespace ftxui;

    int counter = 0;

    // Tạo một Renderer Component từ hàm lambda trả về Element DOM
    auto component = Renderer([&] {
        return vbox({
            text("App C++ TUI Da Nen Tang (FTXUI Interactive)") | bold | color(Color::Blue),
            separator(),
            text("So lan bam phim: " + std::to_string(counter)) | color(Color::Green),
            text("Tu dong giam tai va render vi sai thong qua Double Buffering!"),
            separator(),
            text("Phim tat: [Space/Up] Tang | [Down] Giam | [q/ESC] Thoat") | dim
        }) | border;
    });

    // Bắt sự kiện bàn phím để thay đổi trạng thái và vẽ lại (Redraw)
    auto component_with_events = CatchEvent(component, [&](Event event) {
        if (event == Event::Character('q') || event == Event::Escape) {
            ScreenInteractive::Active()->Exit();
            return true;
        }
        if (event == Event::ArrowUp || event == Event::Character(' ') || event == Event::Character('j')) {
            counter++;
            return true;
        }
        if (event == Event::ArrowDown || event == Event::Character('k')) {
            counter--;
            return true;
        }
        return false;
    });

    // Khởi chạy vòng lặp sự kiện (Event Loop) của FTXUI
    auto screen = ScreenInteractive::TerminalOutput();
    screen.Loop(component_with_events);

    return 0;
}
```

## Hướng Dẫn Khởi Chạy (Execution Commands)

Từ thư mục `my_tui_project/`, chạy các lệnh sau:

### Bước 1: Tạo thư mục build và cấu hình
```bash
cmake -B build
```

### Bước 2: Biên dịch dự án
```bash
cmake --build build
```

### Bước 3: Chạy ứng dụng
```bash
./build/MyTuiApp
```

## Ưu Điểm Kiến Trúc (Architecture Benefits)

1. **Khắc phục hiện tượng nhấp nháy màn hình (Flicker Fix):**
   * FTXUI sử dụng cơ chế **Virtual DOM (Double Buffering)** để so sánh vi sai (**Diffing**). 
   * Nó chỉ render các ký tự có sự thay đổi tại tọa độ `(x, y)` cụ thể thay vì gọi lệnh `clear()` toàn màn hình, giúp việc cập nhật giao diện cực kỳ mượt mà và không bị nhấp nháy.

2. **Khả năng Đa nền tảng vượt trội (Cross-platform):**
   * Ứng dụng chạy native mượt mà trên cả môi trường **Linux (Wayland/Niri/Ubuntu)** lẫn **Windows Console** mà không cần sửa đổi bất kỳ dòng mã nguồn nào.
