#pragma once
#include "lexer/token.hpp"
#include "error/error.hpp"
#include <string>
#include <vector>

namespace Luna {
using TokenList = std::vector<Token>;
class Lexer {
    // ---- stored data ----
    TokenList result;
    std::string input;
    std::string filename;
    std::vector<std::string> source_lines;
    std::vector<Diagnostic> diagnostics;   // collected errors/warnings

    // ---- cursor state ----
    size_t curr_index = 0;
    char curr_char = '\0';
    std::string curr_keyword;
    std::string curr_line;
    size_t line_num = 1;
    size_t col = 0;

    // ---- bracket nesting ----
    size_t paren_depth = 0;
    size_t bracket_depth = 0;
    size_t brace_depth = 0;

    // ---- f-string recursion flag ----
    bool in_fstring = false;

    // ---- core loop ----
    void lex();
    void finalize();

    // ---- navigation ----
    bool advance();
    char peek() const;

    // ---- helpers ----
    static std::vector<std::string> split_lines(const std::string& code);
    static bool is_digits(const std::string& s);
    static bool is_hex_literal(const std::string& s);
    static bool looks_like_decimal(const std::string& s);
    static bool starts_like_decimal(const std::string& s);
    static bool is_valid_identifier(const std::string& s);

    // Build a Location from the current cursor position
    Location here() const;

    void flush_keyword();
    void push(const std::string& value, TokenType type, size_t start, size_t end);
    void push_current(TokenType type);
    void handle_newline_tracking();

    // Record a diagnostic (does not stop lexing immediately)
    void report(DiagLevel level,const std::string& message,const std::string& sub_message = "",const std::string& code = "");

    void report_error(const std::string& msg, const std::string& sub = "");
    void report_warning(const std::string& msg, const std::string& sub = "");

    bool inside_paren_or_bracket() const;

    // ---- sub-lexers ----
    void lex_string();
    void lex_fstring();
    void lex_plus();
    void lex_minus();
    void lex_star();
    void lex_slash();
    void lex_percent();
    void lex_caret();
    void lex_ampersand();
    void lex_pipe();
    void lex_bang();
    void lex_equal();
    void lex_less();
    void lex_greater();
    void lex_dot();
    void lex_colon();

public:
    Lexer(const std::string& source, const std::string& filename);
    TokenList get_tokens()  const;
    bool has_errors()  const;

    // Expose collected diagnostics so other compiler stages can merge them
    const std::vector<Diagnostic>& get_diagnostics() const;
};

} // namespace luna