#pragma once
#include <string>
#include <vector>

namespace Luna {

// ANSI colour codes - Should work on most modern terminals. If not then not my headache lol
namespace colour {
    inline constexpr const char* reset = "\033[0m";
    inline constexpr const char* bold = "\033[1m";
    inline constexpr const char* red = "\033[31m";
    inline constexpr const char* bold_red = "\033[1;31m";
    inline constexpr const char* yellow = "\033[33m";
    inline constexpr const char* bold_white = "\033[1;37m";
    inline constexpr const char* cyan = "\033[36m";
    inline constexpr const char* magenta = "\033[35m";
    inline constexpr const char* green = "\033[32m";
    inline constexpr const char* dim = "\033[2m";
}

struct Location {
    size_t line   = 1;
    size_t col    = 0;
    std::string file;
    std::string source_line;   // the full text of the offending line
};

// Severity of the diagnostic
enum class DiagLevel {
    error,
    warning,
    note,
};

struct Diagnostic {
    Location loc;
    std::string message;
    std::string sub_message; // optional secondary note shown below the caret
    std::string code; // optional error code string, e.g. "E0042"
    DiagLevel level = DiagLevel::error;
};

// Pretty-print a single diagnostic to stderr.
// Displays file:line:col, the source line, a caret, and the message.
void display(const Diagnostic& diag);

// Print a list of diagnostics and then call std::exit(1) if any are errors.
void display_all(const std::vector<Diagnostic>& diags);

// Convenience builders
Diagnostic make_error(const Location& loc,const std::string& message,const std::string& sub_message = "",const std::string& code = "");
Diagnostic make_warning(const Location& loc,const std::string& message,const std::string& sub_message = "");
Diagnostic make_note(const Location& loc,const std::string& message);

} // namespace Luna