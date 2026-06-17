#pragma once
#include <string>

inline bool EnableNerdFonts = false;

namespace Icons {
    inline std::string Coffee()   { return EnableNerdFonts ? "" : "D"; }
    inline std::string Food()     { return EnableNerdFonts ? "" : "F"; }
    inline std::string VIP()      { return EnableNerdFonts ? "👑" : "VIP"; }
    inline std::string History()  { return EnableNerdFonts ? "" : "History"; }
    inline std::string Stats()    { return EnableNerdFonts ? "📊" : "Stats"; }
    inline std::string Trash()    { return EnableNerdFonts ? "" : "Xoa"; }
    inline std::string Edit()     { return EnableNerdFonts ? "" : "Sua"; }
    inline std::string Plus()     { return EnableNerdFonts ? "" : "+"; }
    inline std::string Check()    { return EnableNerdFonts ? "" : "v"; }
    inline std::string Cross()    { return EnableNerdFonts ? "" : "x"; }
    inline std::string Money()    { return EnableNerdFonts ? "" : "Gia"; }
    inline std::string Sorted()   { return EnableNerdFonts ? "" : "Sort"; }
}
