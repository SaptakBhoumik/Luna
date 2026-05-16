
enum class TokenType {
    eof,

    
    // Brackets
    lbracket,         // [
    rbracket,         // ]
    lbrace,           // {
    rbrace,           // }

    // Punctuation
    comma,            // ,
    dollar,           // $
    colon,            // :
    walrus,           // :=
    double_colon,     // ::
    dot,              // .
    double_dot,       // ..
    ellipsis,         // ...
    at,               // @. Used for decorators and matrix multiplication
    hash,             // #
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
    // logical_and,      // &&
    // logical_or,       // ||

    // Bitwise
    caret,            // ^
    ampersand,        // &
    pipe,             // |
    tilde,            // ~

    // Other operators
    bang,             // !   (error propagation)
    assign,           // =
    arrow,            // ->
    thick_arrow,      // =>
    pipeline,         // |>

    select_recv,      // <--
    select_send,      // -->

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
    kw_pub,//pub

    kw_mut,//mut
    

    kw_true,//True
    kw_false,//False
    kw_none,//None


    // kw_atomic,//atomic
    kw_thread,//thread
    kw_task,//task

    newline,
};