#pragma once
#include <cstddef>
#include <string>

namespace Luna {

enum class TokenType {
    eof,

    // Literals
    integer,
    decimal,
    string,
    identifier,
    
    // Raw string prefix (e.g. r"foo") - the 'r' token before the string literal
    raw,
    // Format strings
    format,           // the 'f' prefix token
    format_str,       // a text segment inside an f-string
    format_str_end,   // end-of-f-string marker

    // Brackets
    lparen,           // (
    rparen,           // )
    lbracket,         // [
    rbracket,         // ]
    lbrace,           // {
    rbrace,           // }

    // Punctuation
    comma,            // ,
    colon,            // :
    walrus,           // :=
    double_colon,     // ::
    dot,              // .
    double_dot,       // ..
    ellipsis,         // ...
    at,               // @
    question,         // ?
    null_coalesce,    // ??
    error_coalesce,   // !!

    // Arithmetic
    plus,             // +
    minus,            // -
    star,             // *
    pow,            // **
    slash,            // /
    percent,          // %

    //Logical
    logical_and,      // &&
    logical_or,       // ||

    // Bitwise
    caret,            // ^
    ampersand,        // &
    pipe,             // |
    tilde,            // ~

    // Other operators
    bang,             // !   (error propagation)
    assign,           // =
    arrow,            // ->
    pipeline,         // |>

    // Compound assignment
    plus_eq,          // +=
    minus_eq,         // -=
    star_eq,          // *=
    pow_eq,          // **=
    slash_eq,         // /=
    percent_eq,       // %=
    caret_eq,         // ^=
    amp_eq,           // &=
    pipe_eq,          // |=
    shl_eq,           // <<=
    shr_eq,           // >>=

    // Comparison
    eq,               // ==
    neq,              // !=
    lt,               // <
    gt,               // >
    leq,              // <=
    geq,              // >=

    // Bit shifts
    shl,              // <<
    shr,              // >>

    // Increment / decrement
    incr,             // ++
    decr,             // --

    // Keywords
    kw_import,//import
    kw_using,//using
    kw_pub,//pub
    kw_enum,//enum
    kw_type,//type
    kw_struct,//struct
    kw_fn,//fn

    kw_mut,//mut
    kw_ret,//ret
    kw_if,//if
    kw_elif,//elif
    kw_else,//else
    kw_loop,//loop
    kw_break,//break
    kw_continue,//continue
    
    kw_match,//match
    kw_case,//case

    kw_true,//True
    kw_false,//False
    kw_none,//None

    kw_give,//give
    kw_defer,//defer

    kw_atomic,//atomic
    kw_thread_local,//thread_local
    kw_thread,//thread
    kw_lock,//lock

    // kw_cast,//Technically not a function but I will treat it as one for simplicity
    kw_typesize,//typesize
    kw_decltype,//decltype
    kw_typeid,//typeid

    newline,
};

struct Token {
    size_t col;          // column of the token start
    std::string source_line;  // the full source line for diagnostics
    std::string value;        // raw text of the token
    //start and end are probably useless but keeping it just in case. They are byte offsets from the start of the file.
    size_t start;        // byte offset start
    size_t end;          // byte offset end  
    size_t line;         // 1-based line number
    TokenType type;
};

} // namespace luna