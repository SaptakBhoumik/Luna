#include "lexer/token.hpp"

namespace Luna {
std::string to_string(TokenType type) {
    //Dont use map. Use switch case because in case u forget to add a case the compiler will warn u even before u run the code, 
    //whereas with a map it will just return an empty string/throw error and u might not notice the bug until runtime
    switch (type) {
        case TokenType::eof: return "<EOF>";
        case TokenType::integer: return "<integer>";
        case TokenType::decimal: return "<decimal>";
        case TokenType::string: return "<string>";
        case TokenType::identifier: return "<identifier>";
        case TokenType::raw: return "<raw string prefix>";
        case TokenType::format: return "<format string prefix>";
        case TokenType::format_str: return "<format string segment>";
        case TokenType::format_str_end: return "<end of format string>";
        case TokenType::lparen: return "(";
        case TokenType::rparen: return ")";
        case TokenType::lbracket: return "[";
        case TokenType::rbracket: return "]";
        case TokenType::lbrace: return "{";
        case TokenType::rbrace: return "}";
        case TokenType::comma: return ",";
        case TokenType::dollar: return "$";
        case TokenType::colon: return ":";
        case TokenType::walrus: return ":=";
        case TokenType::double_colon: return "::";
        case TokenType::dot: return ".";
        case TokenType::double_dot: return "..";
        case TokenType::ellipsis: return "...";
        case TokenType::at: return "@";
        case TokenType::hash: return "#";
        case TokenType::question: return "?";
        case TokenType::null_coalesce: return "??";
        case TokenType::error_coalesce: return "!!";
        case TokenType::plus: return "+";
        case TokenType::minus: return "-";
        case TokenType::star: return "*";
        case TokenType::pow: return "**";
        case TokenType::slash: return "/";
        case TokenType::percent: return "%";
        case TokenType::caret: return "^";
        case TokenType::ampersand: return "&";
        case TokenType::pipe: return "|";
        case TokenType::tilde: return "~";
        case TokenType::bang: return "!";
        case TokenType::assign: return "=";
        case TokenType::arrow: return "->";
        case TokenType::thick_arrow: return "=>";
        case TokenType::pipeline: return "|>";
        case TokenType::select_recv: return "<--";
        case TokenType::select_send: return "-->";
        case TokenType::plus_eq: return "+=";
        case TokenType::minus_eq: return "-=";
        case TokenType::star_eq: return "*=";
        case TokenType::pow_eq: return "**=";
        case TokenType::slash_eq: return "/=";
        case TokenType::percent_eq: return "%=";
        case TokenType::caret_eq: return "^=";
        case TokenType::amp_eq: return "&=";
        case TokenType::pipe_eq: return "|=";
        case TokenType::shl_eq: return "<<=";
        case TokenType::shr_eq: return ">>=";
        case TokenType::eq: return "==";
        case TokenType::neq: return "!=";
        case TokenType::lt: return "<";
        case TokenType::gt: return ">";
        case TokenType::leq: return "<=";
        case TokenType::geq: return ">=";
        case TokenType::shl: return "<<";
        case TokenType::shr: return ">>";
        case TokenType::incr: return "++";
        case TokenType::decr: return "--";
        // Keywords
        case TokenType::kw_import: return "import";
        // case TokenType::kw_using: return "using";
        case TokenType::kw_pub: return "pub";
        case TokenType::kw_enum: return "enum";
        case TokenType::kw_interface: return "interface";
        case TokenType::kw_type: return "type";
        case TokenType::kw_fn: return "fn";
        case TokenType::kw_mut: return "mut";
        case TokenType::kw_ret: return "ret";
        case TokenType::kw_raise: return "raise";
        case TokenType::kw_loop: return "loop";
        case TokenType::kw_break: return "break";
        case TokenType::kw_continue: return "continue";
        case TokenType::kw_when: return "when";
        case TokenType::kw_true: return "True";
        case TokenType::kw_false: return "False";
        case TokenType::kw_none: return "None";
        case TokenType::kw_give: return "give";
        case TokenType::kw_defer: return "defer";
        // case TokenType::kw_thread_local: return "thread_local";
        // case TokenType::kw_task_local: return "task_local";
        case TokenType::kw_thread: return "thread";
        case TokenType::kw_task: return "task";
        case TokenType::kw_lock: return "lock";
        case TokenType::kw_select: return "select";
        case TokenType::kw_decltype: return "decltype";
        case TokenType::newline: return "<newline>";
    }
    return "<unknown token>";
}
std::string to_string(const Token& tok){
    return "Token(col : " + std::to_string(tok.col) + ", line: " + std::to_string(tok.line) + ", value: " + tok.value + ", type: " + to_string(tok.type) + ")";
}

std::ostream& operator<<(std::ostream& os, const Token& tok){
    os << to_string(tok);
    return os;
}
}