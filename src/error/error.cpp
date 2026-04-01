#include "error/error.hpp"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

namespace Luna {
static const char* level_colour(DiagLevel level) {
    switch (level) {
        case DiagLevel::error:{
            return colour::bold_red;
        }   
        case DiagLevel::warning:{
            return colour::yellow;
        }
        case DiagLevel::note:{
            return colour::cyan;
        }
    }
    return colour::reset;
}

static const char* level_label(DiagLevel level) {
    switch (level) {
        case DiagLevel::error:{
            return "error";
        }
        case DiagLevel::warning:{
            return "warning";
        }
        case DiagLevel::note:{
            return "note";
        }
    }
    return "unknown";
}

// Produce a caret string "   ^~~~~" that points at column `col`
// with `len` tildes following the caret (len >= 1).
static std::string make_caret(size_t col, size_t len = 1) {
    std::string out(col > 0 ? col - 1 : 0, ' ');
    out += '^';
    if (len > 1) {
        out += std::string(len - 1, '~');
    }
    return out;
}


void display(const Diagnostic& diag) {
    const auto& lc = level_colour(diag.level);
    const auto& lbl = level_label(diag.level);

    // ---- header: file:line:col: severity: message ----
    // Example:  src/main.luna:12:5: error[E0042]: unexpected token
    std::cerr << colour::bold_white << diag.loc.file
              << ':' << diag.loc.line
              << ':' << diag.loc.col
              << ": "
              << colour::reset;

    std::cerr << lc << colour::bold << lbl;

    if (!diag.code.empty()) {
        std::cerr << '[' << diag.code << ']';
    }

    std::cerr << colour::reset
              << colour::bold_white << ": " << diag.message
              << colour::reset << '\n';

    // ---- source line ----
    if (!diag.loc.source_line.empty()) {
        // Line number gutter
        std::string line_no = std::to_string(diag.loc.line);
        std::cerr << colour::dim
                  << ' ' << line_no << " | "
                  << colour::reset
                  << diag.loc.source_line << '\n';

        // Caret line
        std::string gutter_pad(line_no.size() + 1, ' ');
        std::cerr << colour::dim
                  << gutter_pad << " | "
                  << colour::reset
                  << lc << colour::bold
                  << make_caret(diag.loc.col)
                  << colour::reset;

        // Optional sub-message on same caret line
        if (!diag.sub_message.empty()) {
            std::cerr << ' '
                      << colour::dim << diag.sub_message
                      << colour::reset;
        }
        std::cerr << '\n';
    } 
    else if (!diag.sub_message.empty()) {
        // No source line available, but we still have a note
        std::cerr << colour::dim
                  << "    note: " << diag.sub_message
                  << colour::reset << '\n';
    }

    std::cerr << '\n';   // blank line between diagnostics
}

void display_all(const std::vector<Diagnostic>& diags) {
    bool has_error = false;
    for (const auto& d : diags) {
        display(d);
        if (d.level == DiagLevel::error){
            has_error = true;
        }
    }
    if (has_error){
        std::exit(1);
    }
}



Diagnostic make_error(const Location& loc,const std::string& message,const std::string& sub_message,const std::string& code) {
    return Diagnostic{loc, message, sub_message, code, DiagLevel::error};
}

Diagnostic make_warning(const Location& loc,const std::string& message,const std::string& sub_message) {
    return Diagnostic{loc, message, sub_message, "", DiagLevel::warning};
}

Diagnostic make_note(const Location& loc, const std::string& message) {
    return Diagnostic{loc, message, "", "", DiagLevel::note};
}

} // namespace Luna