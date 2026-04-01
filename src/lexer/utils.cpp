#include "lexer/lexer.hpp"
#include <algorithm>
#include <cctype>
namespace Luna {
bool Lexer::advance() {
    if (this->curr_index < this->input.size() - 1) {
        this->curr_index++;
        this->col++;
        this->curr_char = this->input[this->curr_index];
        return true;
    }
    return false;
}

char Lexer::peek() const {
    if ((this->curr_index + 1) < this->input.size()) {
        return this->input[this->curr_index + 1];
    }
    return '\0';
}

std::vector<std::string> Lexer::split_lines(const std::string& code) {
    std::vector<std::string> lines;
    std::string line;
    char prev = '\0';

    for (char c : code) {
        if (c == '\n' || c == '\r') {
            if (prev == '\r' && c == '\n') {
                // \r\n counts as a single line ending; already pushed on '\r'
            } 
            else {
                lines.push_back(line);
                line.clear();
            }
        } 
        else if (c == '"') {
            line += "\\\"";   // escape embedded quotes for diagnostic display
        }
        else {
            line += c;
        }
        prev = c;
    }
    lines.push_back(line);
    return lines;
}


bool Lexer::is_digits(const std::string& s) {
    return !s.empty() &&
           std::all_of(s.begin(), s.end(),
                       [](unsigned char c) { return std::isdigit(c) != 0; });
}

bool Lexer::is_hex_literal(const std::string& s) {
    return s.size() > 2 &&
           (s.compare(0, 2, "0x") == 0 || s.compare(0, 2, "0X") == 0) &&
           s.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos;
}

// Accepts strings of the form: digits* '.' digits*  (with optional e/E exponent)
bool Lexer::looks_like_decimal(const std::string& s) {
    if (s.empty()) {
        return false;
    }

    size_t i = 0;
    bool has_digit = false;
    bool has_dot = false;
    bool has_exp = false;

    // integer or fractional part
    while (i < s.size()) {
        char c = s[i];

        if (std::isdigit((unsigned char)c)) {
            has_digit = true;
            i++;
        }
        else if (c == '.') {
            if (has_dot || has_exp) {
                return false;
            }
            has_dot = true;
            i++;
        }
        else {
            break;
        }
    }

    // must have at least one digit before exponent
    if (!has_digit) {
        return false;
    }
    // exponent part
    if (i < s.size() && (s[i] == 'e' || s[i] == 'E')) {
        has_exp = true;
        i++;

        // optional sign
        if (i < s.size() && (s[i] == '+' || s[i] == '-')) {
            i++;
        }

        bool exp_digits = false;

        while (i < s.size() && std::isdigit((unsigned char)s[i])) {
            exp_digits = true;
            i++;
        }

        if (!exp_digits) {
            return false;
        }
    }

    // no garbage at end
    if (i != s.size()){
        return false;
    }

    // must have either dot OR exponent to be "decimal"
    return has_dot || has_exp;
}
bool Lexer::starts_like_decimal(const std::string& s) {
    if (s.empty()) {
        return false;
    }

    size_t i = 0;
    bool has_dot = false;

    while (i < s.size()) {
        char c = s[i];

        if (std::isdigit((unsigned char)c)) {
            i++;
        }
        else if (c == '.') {
            if (has_dot) {
                return false;
            }
            has_dot = true;
            i++;
        }
        else {
            if(i == s.size() - 1 && (c == 'e' || c == 'E') && i != 0) {
                return true;  // allow "3e" as a valid start of a decimal literal
            }
            else {
                return false;
            }
        }
    }

    return false;
}

bool Lexer::is_valid_identifier(const std::string& s) {
    if (s.empty()) {
        return false;
    }
    if (!std::isalpha((unsigned char)s[0]) && s[0] != '_') {
        return false;
    }
    for (size_t i = 1; i < s.size(); ++i) {
        if (!std::isalnum((unsigned char)s[i]) && s[i] != '_') {
            return false;
        }
    }
    return true;
}

bool Lexer::inside_paren_or_bracket() const {
    return this->paren_depth > 0 || this->bracket_depth > 0;
}
} // namespace Luna