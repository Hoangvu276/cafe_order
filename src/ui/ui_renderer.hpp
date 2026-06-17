#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <set>
#include <algorithm>
#include "ui_style.hpp"

// ── UTF-8 Codepoint Parser & Width Calculation ───────────────────────────

inline int utf8_to_codepoint(const std::string& s, size_t& i) {
    if (i >= s.size()) return 0;
    unsigned char c1 = s[i];
    if (c1 < 0x80) {
        i += 1;
        return c1;
    }
    if ((c1 & 0xE0) == 0xC0) {
        if (i + 1 >= s.size()) { i += 1; return 0; }
        unsigned char c2 = s[i+1];
        i += 2;
        return ((c1 & 0x1F) << 6) | (c2 & 0x3F);
    }
    if ((c1 & 0xF0) == 0xE0) {
        if (i + 2 >= s.size()) { i += 1; return 0; }
        unsigned char c2 = s[i+1];
        unsigned char c3 = s[i+2];
        i += 3;
        return ((c1 & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
    }
    if ((c1 & 0xF8) == 0xF0) {
        if (i + 3 >= s.size()) { i += 1; return 0; }
        unsigned char c2 = s[i+1];
        unsigned char c3 = s[i+2];
        unsigned char c4 = s[i+3];
        i += 4;
        return ((c1 & 0x07) << 18) | ((c2 & 0x3F) << 12) | ((c3 & 0x3F) << 6) | (c4 & 0x3F);
    }
    i += 1;
    return 0;
}

inline int codepoint_width(int cp) {
    if (cp == 0) return 0;
    if (cp < 32 || (cp >= 0x7F && cp < 0xA0)) return 0;
    
    // Emojis & symbols (Wide display in terminals)
    if ((cp >= 0x2600 && cp <= 0x27BF) ||
        (cp >= 0x1F300 && cp <= 0x1F6FF) ||
        (cp >= 0x1F900 && cp <= 0x1FAFF)) {
        return 2;
    }
    
    // Nerd Fonts PUA are single-width (width 1) in standard monospace terminals
    if (cp >= 0xE000 && cp <= 0xF8FF) {
        return 1;
    }
    
    // CJK Ranges (Wide)
    if ((cp >= 0x1100 && cp <= 0x115F) || 
        (cp >= 0x2E80 && cp <= 0x303E) ||
        (cp >= 0x3040 && cp <= 0xA4CF) || 
        (cp >= 0xAC00 && cp <= 0xD7A3) || 
        (cp >= 0xF900 && cp <= 0xFAFF) || 
        (cp >= 0xFE30 && cp <= 0xFE6F) || 
        (cp >= 0xFF00 && cp <= 0xFF60) || 
        (cp >= 0xFFE0 && cp <= 0xFFE6) || 
        (cp >= 0x20000 && cp <= 0x2FA1F)) {
        return 2;
    }
    return 1;
}

inline int str_display_width(const std::string& s) {
    int w = 0;
    bool in_esc = false;
    for (size_t i = 0; i < s.size(); ) {
        if (s[i] == '\033') {
            in_esc = true;
            i++;
            continue;
        }
        if (in_esc) {
            if (s[i] == 'm') in_esc = false;
            i++;
            continue;
        }
        int cp = utf8_to_codepoint(s, i);
        w += codepoint_width(cp);
    }
    return w;
}

// Truncate s to max_width display columns, preserving ANSI escape codes
inline std::string truncate_right(const std::string& s, int max_width, const std::string& suffix = "") {
    int w = str_display_width(s);
    if (w <= max_width) return s;
    int target = max_width - (int)str_display_width(suffix);
    if (target <= 0) return suffix;

    std::string out;
    int cur = 0;
    bool in_esc = false;
    for (size_t i = 0; i < s.size(); ) {
        if (s[i] == '\033') {
            in_esc = true;
            out += s[i];
            i++;
            continue;
        }
        if (in_esc) {
            out += s[i];
            if (s[i] == 'm') in_esc = false;
            i++;
            continue;
        }
        size_t next_i = i;
        int cp = utf8_to_codepoint(s, next_i);
        int cw = codepoint_width(cp);
        if (cur + cw > target) break;
        for (size_t j = i; j < next_i; j++) out += s[j];
        cur += cw;
        i = next_i;
    }
    return out + suffix;
}

inline std::string pad_right(const std::string& s, int width, const std::string& bg_color = "") {
    int w = str_display_width(s);
    if (w >= width) return s;
    if (!bg_color.empty()) {
        return s + bg_color + std::string(width - w, ' ') + ansi_reset();
    }
    return s + std::string(width - w, ' ');
}

// Merge two multi-line strings side-by-side, automatically padding the left lines
inline std::string merge_horizontal(const std::string& left, const std::string& right) {
    std::vector<std::string> l_lines, r_lines;
    {
        std::istringstream ss(left);
        std::string line;
        while (std::getline(ss, line)) l_lines.push_back(line);
    }
    {
        std::istringstream ss(right);
        std::string line;
        while (std::getline(ss, line)) r_lines.push_back(line);
    }
    
    // Find the maximum display width of the left lines
    int max_l_width = 0;
    for (const auto& l : l_lines) {
        int w = str_display_width(l);
        if (w > max_l_width) max_l_width = w;
    }
    
    size_t max_lines = std::max(l_lines.size(), r_lines.size());
    std::string out;
    for (size_t i = 0; i < max_lines; i++) {
        std::string l = (i < l_lines.size()) ? l_lines[i] : "";
        std::string r = (i < r_lines.size()) ? r_lines[i] : "";
        
        // Pad the left line to max_l_width
        int lw = str_display_width(l);
        if (lw < max_l_width) {
            l += std::string(max_l_width - lw, ' ');
        }
        
        out += l + r + "\n";
    }
    return out;
}

// ── Border & Layout Renderer ─────────────────────────────────────────────

struct BorderChars {
    std::string top           = "─";
    std::string bottom        = "─";
    std::string left          = "│";
    std::string right         = "│";
    std::string top_left      = "╭";
    std::string top_right     = "╮";
    std::string bottom_left   = "╰";
    std::string bottom_right  = "╯";
    std::string middle_left   = "├";
    std::string middle_right  = "┤";
};

struct BorderConfig {
    std::string title;
    std::vector<std::string> info_items;
    std::vector<int> divider_idx;
    int width = 0;
    int height = 0;

    void set_title(const std::string& t) { title = t; }
    void set_info_items(std::vector<std::string> items) { info_items = std::move(items); }
    void add_divider(int idx) { divider_idx.push_back(idx); }

    std::string top_line(const BorderChars& bc, const std::string& border_color, const std::string& bg) const {
        int inner = width - 2;
        std::string mid;
        if (!title.empty()) {
            std::string trunc = truncate_right(title, inner - 4);
            int pad = inner - 3 - (int)str_display_width(trunc);
            std::string left_pad = bc.top;
            std::string right_pad;
            for (int i = 0; i < pad; i++) {
                right_pad += bc.top;
            }
            mid = left_pad + " " + trunc + " " + right_pad;
        } else {
            for (int i = 0; i < inner; i++) mid += bc.top;
        }
        return border_color + bg + bc.top_left + mid + bc.top_right + ansi_reset() + "\n";
    }

    std::string bottom_line(const BorderChars& bc, const std::string& border_color, const std::string& bg) const {
        int inner = width - 2;
        std::string mid;
        if (!info_items.empty()) {
            mid = bc.bottom; // Start with one border char
            for (size_t i = 0; i < info_items.size(); i++) {
                std::string item = " " + info_items[i] + " ";
                mid += item;
                if (i + 1 < info_items.size()) {
                    mid += bc.bottom;
                }
            }
            int cur_w = str_display_width(mid);
            while (cur_w < inner) {
                mid += bc.bottom;
                cur_w++;
            }
            mid = truncate_right(mid, inner);
        } else {
            for (int i = 0; i < inner; i++) mid += bc.bottom;
        }
        return border_color + bg + bc.bottom_left + mid + bc.bottom_right + ansi_reset() + "\n";
    }

    std::string divider_line(const BorderChars& bc, const std::string& border_color, const std::string& bg) const {
        int inner = width - 2;
        std::string mid;
        for (int i = 0; i < inner; i++) mid += bc.top;
        return border_color + bg + bc.middle_left + mid + bc.middle_right + ansi_reset() + "\n";
    }
};

struct SectionContent {
    int max_height;
    int content_width;
    std::vector<std::string> lines;

    SectionContent(int h, int w) : max_height(h), content_width(w) {}

    void add_line(const std::string& line) {
        if ((int)lines.size() >= max_height) return;
        lines.push_back(truncate_right(line, content_width));
    }

    std::string render(const std::string& fg, const std::string& bg) const {
        std::string out;
        for (const auto& l : lines) {
            // Restore background after any ansi_reset()
            std::string processed_l;
            std::string reset_seq = ansi_reset();
            size_t pos = 0;
            std::string src = l;
            while (true) {
                size_t next_pos = src.find(reset_seq, pos);
                if (next_pos == std::string::npos) {
                    processed_l += src.substr(pos);
                    break;
                }
                processed_l += src.substr(pos, next_pos - pos) + reset_seq + bg;
                pos = next_pos + reset_seq.size();
            }
            out += fg + bg + pad_right(processed_l, content_width, bg) + "\n";
        }
        for (int i = (int)lines.size(); i < max_height; i++) {
            out += fg + bg + std::string(content_width, ' ') + ansi_reset() + "\n";
        }
        return out;
    }
};

class Renderer {
private:
    int total_height;
    int total_width;
    bool has_border;
    UIStyle style;
    UIStyle border_style;
    std::string title;
    
    BorderChars bc;
    BorderConfig border;
    std::vector<SectionContent> sections;
    int content_height;
    int content_width;
    int committed_height = 0;

public:
    Renderer(int h, int w, bool border_required, const UIStyle& s, const UIStyle& bs, const std::string& t = "")
        : total_height(h), total_width(w), has_border(border_required), style(s), border_style(bs), title(t) {
        
        int ch = total_height - (has_border ? 2 : 0);
        int cw = total_width - (has_border ? 2 : 0);
        content_height = std::max(ch, 0);
        content_width = std::max(cw, 0);
        
        border.width = total_width;
        border.height = total_height;
        border.set_title(title);
        
        sections.emplace_back(content_height, content_width);
    }

    Renderer& add_line(const std::string& line) {
        sections.back().add_line(line);
        return *this;
    }

    Renderer& add_section() {
        int committed = committed_height + (int)sections.back().lines.size();
        border.add_divider(committed);
        committed_height = committed + 1;
        int remaining = content_height - committed_height;
        sections.emplace_back(std::max(remaining, 0), content_width);
        return *this;
    }

    void set_info(const std::vector<std::string>& info) {
        border.set_info_items(info);
    }

    std::string render() const {
        std::string out;
        if (has_border) {
            out += border.top_line(bc, border_style.fg, border_style.bg);
        }

        std::set<int> dividers(border.divider_idx.begin(), border.divider_idx.end());
        int current_row = 0;
        for (size_t s = 0; s < sections.size(); s++) {
            std::string sec_render = sections[s].render(style.fg, style.bg);
            std::istringstream ss(sec_render);
            std::string line;
            while (std::getline(ss, line)) {
                if (has_border) {
                    out += border_style.fg + border_style.bg + bc.left + ansi_reset();
                }
                out += line;
                if (has_border) {
                    out += border_style.fg + border_style.bg + bc.right + ansi_reset();
                }
                out += "\n";
                current_row++;
            }

            if (s + 1 < sections.size() && has_border) {
                out += border.divider_line(bc, border_style.fg, border_style.bg);
                current_row++;
            }
        }

        if (has_border) {
            out += border.bottom_line(bc, border_style.fg, border_style.bg);
        }
        return out;
    }

    int get_content_width() const { return content_width; }
    int get_content_height() const { return content_height; }
};
