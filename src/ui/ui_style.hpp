#pragma once
#include <string>
#include <iostream>

inline std::string ansi_fg(int r, int g, int b) {
    return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}
inline std::string ansi_bg(int r, int g, int b) {
    return "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}
inline std::string ansi_reset() { return "\033[0m"; }
inline std::string ansi_bold()  { return "\033[1m"; }

struct RGB { int r, g, b; };

inline RGB parse_hex(const std::string& hex) {
    std::string h = hex;
    if (!h.empty() && h[0] == '#') h = h.substr(1);
    if (h.size() < 6) return {255, 255, 255};
    try {
        return {
            std::stoi(h.substr(0, 2), nullptr, 16),
            std::stoi(h.substr(2, 2), nullptr, 16),
            std::stoi(h.substr(4, 2), nullptr, 16)
        };
    } catch (...) {
        return {255, 255, 255};
    }
}

struct UIStyle {
    std::string fg;
    std::string bg;

    std::string apply(const std::string& text) const {
        return fg + bg + text + ansi_reset();
    }
    std::string apply_bold(const std::string& text) const {
        return fg + bg + ansi_bold() + text + ansi_reset();
    }
};

struct ThemeType {
    // Backgrounds
    std::string bg_dark       = "#1e1e2e";
    std::string bg_panel      = "#181825";
    std::string bg_sidebar    = "#11111b";
    
    // Foregrounds
    std::string fg_text       = "#cdd6f4";
    std::string fg_subtext    = "#a6adc8";
    
    // Borders
    std::string border_active   = "#cba6f7"; // Lavender
    std::string border_inactive = "#45475a"; // Surface 1
    
    // States
    std::string cursor          = "#cba6f7";
    std::string success         = "#a6e3a1";
    std::string error           = "#f38ba8";
    std::string warning         = "#f9e2af";
    std::string info            = "#89b4fa";
    
    // Selection
    std::string selected_fg     = "#11111b";
    std::string selected_bg     = "#cba6f7";
};

struct StyleContext {
    UIStyle main;
    UIStyle sidebar;
    UIStyle panel;
    UIStyle border_active;
    UIStyle border_inactive;
    
    UIStyle selected;
    UIStyle cursor;
    UIStyle success;
    UIStyle error;
    UIStyle warning;
    UIStyle info;
    UIStyle title;
    UIStyle gray;

    void init(const ThemeType& t) {
        auto fg = [](const std::string& h) { auto c = parse_hex(h); return ansi_fg(c.r, c.g, c.b); };
        auto bg = [](const std::string& h) { auto c = parse_hex(h); return ansi_bg(c.r, c.g, c.b); };

        main            = { fg(t.fg_text), bg(t.bg_dark) };
        sidebar         = { fg(t.fg_text), bg(t.bg_sidebar) };
        panel           = { fg(t.fg_text), bg(t.bg_panel) };
        border_active   = { fg(t.border_active), bg(t.bg_panel) };
        border_inactive = { fg(t.border_inactive), bg(t.bg_panel) };
        
        selected        = { fg(t.selected_fg), bg(t.selected_bg) };
        cursor          = { fg(t.border_active), bg(t.bg_panel) };
        success         = { fg(t.success), bg(t.bg_panel) };
        error           = { fg(t.error), bg(t.bg_panel) };
        warning         = { fg(t.warning), bg(t.bg_panel) };
        info            = { fg(t.info), bg(t.bg_panel) };
        title           = { fg(t.info), bg(t.bg_panel) };
        gray            = { fg(t.fg_subtext), bg(t.bg_panel) };
    }
};

inline ThemeType Theme;
inline StyleContext Style;

inline void init_ui_styles() {
    Theme = ThemeType{};
    Style.init(Theme);
}
