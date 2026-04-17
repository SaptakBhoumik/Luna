#include "lexer/lexer.hpp"
#include "lexer/token.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <unordered_map>

namespace Luna {
Lexer::Lexer(const std::string& source, const std::string& filename): input(source), filename(filename){
    if (this->input.empty()) {
        this->result.push_back(Token{0, "", "<eof>", 0, 0, 1, TokenType::eof});
        return;
    }
    this->source_lines = Lexer::split_lines(this->input);
    this->curr_char = this->input[0];
    this->curr_line = this->source_lines[0];
    this->lex();
    this->finalize();
}

TokenList Lexer::get_tokens() const {
    return this->result;
}

void Lexer::push(const std::string& value, TokenType type, size_t start, size_t end) {
    this->result.emplace_back(Token{
        this->col,
        this->curr_line,
        value,
        start,
        end,
        this->line_num,
        type
    });
}

void Lexer::push_current(TokenType type) {
    this->push(
        std::string(1, this->curr_char),
        type,
        this->curr_index,
        this->curr_index + 1
    );
}

void Lexer::flush_keyword() {
    if (this->curr_keyword.empty()){
        return;
    }

    const std::string& kw = this->curr_keyword;
    const size_t start = this->curr_index - kw.size();
    const size_t end = this->curr_index;

    // f-string prefix: the 'f' immediately before a quote
    if (kw == "f" && (this->curr_char == '"' || this->curr_char == '\'')) {
        this->push(kw, TokenType::format, start, end);
        this->curr_keyword.clear();
        return;
    }
    // raw string prefix: the 'r' immediately before a quote
    if (kw == "r" && (this->curr_char == '"' || this->curr_char == '\'')) {
        this->push(kw, TokenType::raw, start, end);
        this->curr_keyword.clear();
        return;
    }
    //No. Spaceing is not allowed after f or r if u want raw or formated string

    static const std::unordered_map<std::string, TokenType> keywords = {
        { "import", TokenType::kw_import },
        { "using", TokenType::kw_using },
        { "pub", TokenType::kw_pub },
        { "enum", TokenType::kw_enum },
        { "type", TokenType::kw_type },
        // { "struct", TokenType::kw_struct },
        { "fn", TokenType::kw_fn },

        { "mut", TokenType::kw_mut },
        { "ret", TokenType::kw_ret },
        { "if", TokenType::kw_if },
        // { "elif", TokenType::kw_elif },
        // { "else", TokenType::kw_else },
        { "loop", TokenType::kw_loop },
        { "break", TokenType::kw_break },
        { "continue", TokenType::kw_continue },
        
        { "match", TokenType::kw_match },
        // { "case", TokenType::kw_case },

        { "True", TokenType::kw_true },
        { "False", TokenType::kw_false },
        { "None", TokenType::kw_none },

        { "give", TokenType::kw_give },
        { "defer", TokenType::kw_defer },

        { "atomic", TokenType::kw_atomic },
        { "thread_local", TokenType::kw_thread_local },
        { "thread", TokenType::kw_thread },
        { "lock", TokenType::kw_lock },

        { "typesize", TokenType::kw_typesize },
        { "decltype", TokenType::kw_decltype },
        { "typeid", TokenType::kw_typeid }
    };

    TokenType type;
    auto it = keywords.find(kw);
    if (it != keywords.end()) {
        type = it->second;
    } 
    else if (Lexer::is_digits(kw) || Lexer::is_hex_literal(kw)) {
        type = TokenType::integer;
    } 
    else if (Lexer::looks_like_decimal(kw)) {
        type = TokenType::decimal;
    } 
    else {
        type = TokenType::identifier;
        if (!Lexer::is_valid_identifier(kw)) {
            this->report_error("Invalid identifier: '" + kw + "'");
        }
    }

    this->push(kw, type, start, end);
    this->curr_keyword.clear();
}


void Lexer::handle_newline_tracking() {
    this->line_num++;
    this->col = 0;
    if (this->line_num <= this->source_lines.size()) {
        this->curr_line = this->source_lines[this->line_num - 1];
    }
}


void Lexer::lex() {
    while (true) {
        switch (this->curr_char) {

            // ---- string literals ----
            case '"':
            case '\'': {
                this->flush_keyword();
                if (!this->result.empty()) {
                    if(this->result.back().type == TokenType::format){
                        this->lex_fstring();
                        break;
                    }
                } 
                this->lex_string();
                break;
            }

            // ---- parentheses ----
            case '(': {
                this->flush_keyword();
                this->paren_depth++;
                this->push_current(TokenType::lparen);
                break;
            }
            case ')': {
                this->flush_keyword();
                if (this->paren_depth == 0) {
                    this->report_error("')' without matching '('");
                } 
                else {
                    this->paren_depth--;
                }
                this->push_current(TokenType::rparen);
                break;
            }

            // ---- square brackets ----
            case '[': {
                this->flush_keyword();
                this->bracket_depth++;
                this->push_current(TokenType::lbracket);
                break;
            }
            case ']': {
                this->flush_keyword();
                if (this->bracket_depth == 0) {
                    this->report_error("']' without matching '['");
                } 
                else {
                    this->bracket_depth--;
                }
                this->push_current(TokenType::rbracket);
                break;
            }

            // ---- curly braces ----
            // Note: newlines are NOT suppressed inside {} in Luna.
            // brace_depth is tracked only for error checking.
            // When inside an f-string expression, '}' exits lex() back to lex_fstring.
            case '{': {
                this->flush_keyword();
                this->brace_depth++;
                this->push_current(TokenType::lbrace);
                break;
            }
            case '}': {
                this->flush_keyword();
                // If we are lexing the interior of an f-string expression and
                // no unmatched '{' is open inside it, this '}' closes the expression.
                if (this->in_fstring && this->brace_depth == 0) {
                    return;  // return to lex_fstring(); curr_char is still '}'
                }
                if (this->brace_depth == 0) {
                    this->report_error("'}' without matching '{'");
                } else {
                    this->brace_depth--;
                }
                this->push_current(TokenType::rbrace);
                break;
            }

            // ---- operators ----
            case '+': {
                // this->flush_keyword(); 
                this->lex_plus();      
                break;
            }
            case '-': {
                // this->flush_keyword();
                this->lex_minus();
                break;
            }
            case '*': {
                this->flush_keyword();
                this->lex_star();
                break;
            }
            case '/': {
                this->flush_keyword();
                this->lex_slash();
                break;
            }
            case '%': {
                this->flush_keyword();
                this->lex_percent();
                break;
            }
            case '^': {
                this->flush_keyword();
                this->lex_caret();
                break;
            }
            case '&': {
                this->flush_keyword();
                this->lex_ampersand();
                break;
            }
            case '|': {
                this->flush_keyword();
                this->lex_pipe();
                break;
            }
            case '~': {
                this->flush_keyword();
                this->push_current(TokenType::tilde);
                break;
            }
            case '!':{
                this->flush_keyword();
                this->lex_bang();
                break;
            }
            case '=': {
                this->flush_keyword();
                this->lex_equal();
                break;
            }
            case '<': {
                this->flush_keyword();
                this->lex_less();
                break;
            }
            case '>': {
                this->flush_keyword();
                this->lex_greater();
                break;
            }
            case '.': {
                this->lex_dot();
                break;
            }
            case ':': {
                this->flush_keyword();
                this->lex_colon();
                break;
            }
            case '?': {
                this->flush_keyword();
                this->lex_question();
                break;
            }
            case '@': {
                this->flush_keyword();
                this->push_current(TokenType::at);
                break;
            }
            case ',': {
                this->flush_keyword();
                this->push_current(TokenType::comma);
                break;
            }

            // ---- whitespace (Luna is not indentation-sensitive) ----
            case ' ':
            case '\t': {
                this->flush_keyword();
                break;
            }

            // ---- newlines ----
            case '\n': {
                this->flush_keyword();
                // Emit a newline token only when outside () and []
                // Newlines inside {} are significant and are always emitted.
                if (!this->result.empty()) {
                    if(this->result.back().type != TokenType::newline && !this->inside_paren_or_bracket()) {
                        this->push("<newline>", TokenType::newline,this->curr_index, this->curr_index + 1);
                    }
                }
                this->handle_newline_tracking();
                break;
            }
            case '\r': {
                this->flush_keyword();
                if (this->peek() == '\n') {
                    this->advance();  // consume the \n of \r\n
                }
                if (!this->result.empty()) {
                    if(this->result.back().type != TokenType::newline && !this->inside_paren_or_bracket()) {
                        this->push("<newline>", TokenType::newline,this->curr_index, this->curr_index + 1);
                    }
                }
                this->handle_newline_tracking();
                break;
            }

            // ---- accumulate keyword / identifier / number ----
            default: {
                this->curr_keyword += this->curr_char;
                break;
            }
        }

        if (!this->advance()){
            // this->flush_keyword();//Yeah no need to flush keyword because finalize will do it and we are at end of file so no more tokens anyway.
            break;
        }
    }
}

void Lexer::finalize() {
    this->flush_keyword();

    if (this->paren_depth != 0){
        this->report_error("Unclosed '('", "expected a matching ')'");
    }
    if (this->bracket_depth != 0){
        this->report_error("Unclosed '['", "expected a matching ']'");
    }
    if (this->brace_depth != 0){
        this->report_error("Unclosed '{'", "expected a matching '}'");
    }
    // Let display_all handle printing + exit(1) if there are errors.
    display_all(this->diagnostics);

    if (!this->result.empty()) {
        if(this->result.back().type != TokenType::newline) {
            this->push("<newline>", TokenType::newline,this->curr_index, this->curr_index + 1);
        }
    }

    this->push("<eof>", TokenType::eof,this->curr_index, this->curr_index + 1);
}


void Lexer::lex_string() {
    const char quote = this->curr_char;
    const size_t start = this->curr_index + 1;
    std::string str;

    if (!this->advance()) {
        this->report_error("Unexpected end of file: unterminated string literal");
        return;
    }

    while (this->curr_char != quote) {
        if (this->curr_char == '\\') {
            // Escape sequence: unconditionally consume the next character.
            // This correctly handles \\, \", \n, \t, etc. without any
            // back-tracking or goto tricks.
            str += this->curr_char;                          // '\'
            if (!this->advance()) {
                this->report_error("Unexpected end of file inside string escape sequence");
                return;
            }
            str += this->curr_char;                          // escaped char
        } 
        else if (this->curr_char == '\n') {
            str += this->curr_char;
            this->handle_newline_tracking();
        } 
        else if (this->curr_char == '\r') {
            str += this->curr_char;
            if (this->peek() == '\n') {
                this->advance();
                str += this->curr_char;
            }
            this->handle_newline_tracking();
        } 
        else {
            str += this->curr_char;
        }

        if (!this->advance()) {
            this->report_error("Unexpected end of file: unterminated string literal");
            return;
        }
    }
    this->push(str, TokenType::string, start, this->curr_index);
}

void Lexer::lex_fstring() {
    //TODO:Stress test it. I used LLM for this part but not so confident in it since f-string lexing is quite tricky. So check in future 
    const bool saved_in_fstring = this->in_fstring;
    this->in_fstring = true;

    const char quote = this->curr_char;
    size_t start = this->curr_index + 1;
    std::string str;
    bool has_expr = false;

    if (!this->advance()) {
        this->report_error("Unexpected end of file: unterminated f-string");
        this->in_fstring = saved_in_fstring;
        return;
    }

    while (this->curr_char != quote) {

        // ---- escape sequences ----
        if (this->curr_char == '\\') {
            str += this->curr_char;
            if (!this->advance()) {
                this->report_error("Unexpected end of file inside f-string escape sequence");
                this->in_fstring = saved_in_fstring;
                return;
            }
            str += this->curr_char;
        }

        // ---- opening brace ----
        else if (this->curr_char == '{') {
            if (this->peek() == '{') {
                // Escaped '{{' → literal '{'
                str += '{';
                if (!this->advance()) {
                    this->report_error("Unexpected end of file after '{{'");
                    this->in_fstring = saved_in_fstring;
                    return;
                }
                // second '{' consumed by loop advance
            } else {
                // Start of expression
                has_expr = true;

                if (!str.empty()) {
                    this->push(str, TokenType::format_str, start, this->curr_index);
                    str.clear();
                }

                if (!this->advance()) {
                    this->report_error("Unexpected end of file in f-string expression");
                    this->in_fstring = saved_in_fstring;
                    return;
                }

                this->lex(); // returns at matching '}'

                if (!this->advance()) {
                    this->report_error("Unexpected end of file after f-string expression");
                    this->in_fstring = saved_in_fstring;
                    return;
                }

                start = this->curr_index;
                continue;
            }
        }

        // ---- closing brace ----
        else if (this->curr_char == '}') {
            if (this->peek() == '}') {
                // Escaped '}}' → literal '}'
                str += '}';
                if (!this->advance()) {
                    this->report_error("Unexpected end of file after '}}'");
                    this->in_fstring = saved_in_fstring;
                    return;
                }
                // second '}' consumed by loop advance
            } else {
                // Unmatched '}' inside f-string
                this->report_error("Unmatched '}' in f-string");
                this->in_fstring = saved_in_fstring;
                return;
            }
        }

        // ---- newline handling ----
        else if (this->curr_char == '\n') {
            str += this->curr_char;
            this->handle_newline_tracking();
        }
        else if (this->curr_char == '\r') {
            str += this->curr_char;
            if (this->peek() == '\n') {
                this->advance();
                str += this->curr_char;
            }
            this->handle_newline_tracking();
        }

        // ---- normal character ----
        else {
            str += this->curr_char;
        }

        // ---- advance ----
        if (!this->advance()) {
            this->report_error("Unexpected end of file: unterminated f-string");
            this->in_fstring = saved_in_fstring;
            return;
        }
    }

    // Emit remaining text (or whole string if no expressions)
    if (!str.empty() || !has_expr) {
        this->push(str, TokenType::format_str, start, this->curr_index);
    }

    // End-of-f-string marker
    this->push("", TokenType::format_str_end,
               this->curr_index, this->curr_index + 1);

    this->in_fstring = saved_in_fstring;
}


void Lexer::lex_plus() {
    if(this->starts_like_decimal(this->curr_keyword)){
        //For stuff like 3e+10 or 3.14e+2. We want to treat the '+' as part of the number, not as an operator.
        this->curr_keyword += this->curr_char;
        return;
    }
    this->flush_keyword();
    const size_t start = this->curr_index;
    if (this->peek() == '+') {
        this->advance();
        this->push("++", TokenType::incr, start, this->curr_index + 1);
    } 
    else if (this->peek() == '=') {
        this->advance();
        this->push("+=", TokenType::plus_eq, start, this->curr_index + 1);
    } 
    else {
        this->push("+", TokenType::plus, start, this->curr_index + 1);
    }
}

void Lexer::lex_minus() {
    if(this->starts_like_decimal(this->curr_keyword)){
        //For stuff like 3e-10 or 3.14e-2. We want to treat the '-' as part of the number, not as an operator.
        this->curr_keyword += this->curr_char;
        return;
    }
    this->flush_keyword();
    const size_t start = this->curr_index;
    if (this->peek() == '-') {
        this->advance();
        this->push("--", TokenType::decr, start, this->curr_index + 1);
    } 
    else if (this->peek() == '=') {
        this->advance();
        this->push("-=", TokenType::minus_eq, start, this->curr_index + 1);
    } 
    else if (this->peek() == '>') {
        this->advance();
        this->push("->", TokenType::arrow, start, this->curr_index + 1);
    } 
    else {
        this->push("-", TokenType::minus, start, this->curr_index + 1);
    }
}

void Lexer::lex_star() {
    const size_t start = this->curr_index;
    if (this->peek() == '=') {
        this->advance();
        this->push("*=", TokenType::star_eq, start, this->curr_index + 1);
    } 
    else if(this->peek() == '*') {
        this->advance();
        if (this->peek() == '=') {
            this->advance();
            this->push("**=", TokenType::pow_eq, start, this->curr_index + 1);
        } 
        else {
            this->push("**", TokenType::pow, start, this->curr_index + 1);
        }
    }
    else {
        this->push("*", TokenType::star, start, this->curr_index + 1);
    }
}

void Lexer::lex_slash() {
    const size_t start = this->curr_index;

    if (this->peek() == '/') {
        // Line comment: consume everything up to (but not including) the newline.
        // The newline itself will be processed normally on the next iteration.
        this->advance();  // consume second '/'
        while (this->peek() != '\n' && this->peek() != '\r' && this->peek() != '\0') {
            if (!this->advance()) return;
        }
        return;  // no token emitted
    }

    if (this->peek() == '*') {
        // Block comment
        this->advance();  // consume '*'
        while (true) {
            if (!this->advance()) {
                this->report_error("Unexpected end of file: unterminated block comment");
                return;
            }
            if (this->curr_char == '\n') {
                this->handle_newline_tracking();
            } 
            else if (this->curr_char == '\r') {
                if (this->peek() == '\n') this->advance();
                this->handle_newline_tracking();
            } 
            else if (this->curr_char == '*' && this->peek() == '/') {
                this->advance();  // consume '/'
                return;
            }
        }
    }

    if (this->peek() == '=') {
        this->advance();
        this->push("/=", TokenType::slash_eq, start, this->curr_index + 1);
    } 
    else {
        this->push("/", TokenType::slash, start, this->curr_index + 1);
    }
}

void Lexer::lex_percent() {
    const size_t start = this->curr_index;
    if (this->peek() == '=') {
        this->advance();
        this->push("%=", TokenType::percent_eq, start, this->curr_index + 1);
    } 
    else {
        this->push("%", TokenType::percent, start, this->curr_index + 1);
    }
}

void Lexer::lex_caret() {
    const size_t start = this->curr_index;
    if (this->peek() == '=') {
        this->advance();
        this->push("^=", TokenType::caret_eq, start, this->curr_index + 1);
    } 
    else {
        this->push("^", TokenType::caret, start, this->curr_index + 1);
    }
}

void Lexer::lex_ampersand() {
    const size_t start = this->curr_index;
    if (this->peek() == '=') {
        this->advance();
        this->push("&=", TokenType::amp_eq, start, this->curr_index + 1);
    } 
    // else if(this->peek() == '&') {
    //     this->advance();
    //     this->push("&&", TokenType::logical_and, start, this->curr_index + 1);
    // }
    else {
        this->push("&", TokenType::ampersand, start, this->curr_index + 1);
    }
}

void Lexer::lex_pipe() {
    const size_t start = this->curr_index;
    if (this->peek() == '=') {
        this->advance();
        this->push("|=", TokenType::pipe_eq, start, this->curr_index + 1);
    } 
    else if (this->peek() == '>') {
        this->advance();
        this->push("|>", TokenType::pipeline, start, this->curr_index + 1);
    } 
    // else if(this->peek() == '|') {
    //     this->advance();
    //     this->push("||", TokenType::logical_or, start, this->curr_index + 1);
    // }
    else {
        this->push("|", TokenType::pipe, start, this->curr_index + 1);
    }
}

void Lexer::lex_bang() {
    const size_t start = this->curr_index;
    if (this->peek() == '=') {
        this->advance();
        this->push("!=", TokenType::neq, start, this->curr_index + 1);
    }
    else if(this->peek() == '!') {
        this->advance();
        this->push("!!", TokenType::error_coalesce, start, this->curr_index + 1);
    }
    else {
        // '!' alone is valid in Luna — it's the error-propagation operator
        this->push("!", TokenType::bang, start, this->curr_index + 1);
    }
}
void Lexer::lex_question() {
    const size_t start = this->curr_index;
    if (this->peek() == '?') {
        this->advance();
        this->push("??", TokenType::null_coalesce, start, this->curr_index + 1);
    }
    else {
        this->push("?", TokenType::question, start, this->curr_index + 1);
    }
}
    
void Lexer::lex_equal() {
    const size_t start = this->curr_index;
    if (this->peek() == '=') {
        this->advance();
        this->push("==", TokenType::eq, start, this->curr_index + 1);
    } 
    else {
        this->push("=", TokenType::assign, start, this->curr_index + 1);
    }
}

void Lexer::lex_less() {
    const size_t start = this->curr_index;
    if (this->peek() == '=') {
        this->advance();
        this->push("<=", TokenType::leq, start, this->curr_index + 1);
    } 
    else if (this->peek() == '<') {
        this->advance();
        if (this->peek() == '=') {
            this->advance();
            this->push("<<=", TokenType::shl_eq, start, this->curr_index + 1);
        } else {
            this->push("<<", TokenType::shl, start, this->curr_index + 1);
        }
    }
    else {
        this->push("<", TokenType::lt, start, this->curr_index + 1);
    }
}

void Lexer::lex_greater() {
    const size_t start = this->curr_index;
    if (this->peek() == '=') {
        this->advance();
        this->push(">=", TokenType::geq, start, this->curr_index + 1);
    } 
    else if (this->peek() == '>') {
        this->advance();
        if (this->peek() == '=') {
            this->advance();
            this->push(">>=", TokenType::shr_eq, start, this->curr_index + 1);
        } else {
            this->push(">>", TokenType::shr, start, this->curr_index + 1);
        }
    } 
    else {
        this->push(">", TokenType::gt, start, this->curr_index + 1);
    }
}

void Lexer::lex_dot() {
    // If we have accumulated digits and the next char is also a digit,
    // this dot is the decimal point of a float literal (e.g. 3.14).
    // Add it to the buffer and keep accumulating.
    if (Lexer::is_digits(this->curr_keyword) && std::isdigit((unsigned char)this->peek())) {
        this->curr_keyword += this->curr_char;
        return;
    }

    this->flush_keyword();

    // if(std::isdigit((unsigned char)this->peek())){
    //     // Handle the case where we have something like ".5" which should be a valid float literal.
    //     // We want to treat the '.' as part of the number, not as an operator.
    //     this->curr_keyword += this->curr_char;
    //     return;
    // }

    const size_t start = this->curr_index;
    if (this->peek() == '.') {
        this->advance();
        if (this->peek() == '.') {
            this->advance();
            this->push("...", TokenType::ellipsis, start, this->curr_index + 1);
        } 
        else {
            this->push("..", TokenType::double_dot, start, this->curr_index + 1);
        }
    } 
    else {
        this->push(".", TokenType::dot, start, this->curr_index + 1);
    }
}

void Lexer::lex_colon() {
    const size_t start = this->curr_index;
    if (this->peek() == ':') {
        this->advance();
        this->push("::", TokenType::double_colon, start, this->curr_index + 1);
    } 
    else if(this->peek() == '=') {
        this->advance();
        this->push(":=", TokenType::walrus, start, this->curr_index + 1);
    }
    else {
        this->push(":", TokenType::colon, start, this->curr_index + 1);
    }
}

} // namespace luna