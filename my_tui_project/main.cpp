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
