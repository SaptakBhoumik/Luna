#include "parser/parser.hpp"

namespace Luna {
Parser::Parser(const std::vector<Token>& toks, const std::string& filename){
    this->toks = toks;
    this->filename = filename;
    if (!toks.empty()) {
        this->curr_tok = toks[0];
    }
    std::map<TokenType, PrecedenceType> precedence_map = {
        //'f' i.e format is handled as nud in parse_expr using the parse_formatted_string function
        //'$' is also handled as a nud using parse_compile_time_expr
        //'#' is also handled by parse_stmt. Dont worry about it in parse_expr
        //'~' is handled as a nud
        // -- Trailing-block call -------------------------------------------
        { TokenType::thick_arrow,PrecedenceType::pr_arrow_block },// =>

        // -- Ternary ------------------------------------------------------
        // The ':' is consumed inside parse_ternary_expr, not as its own infix entry.
        { TokenType::question,PrecedenceType::pr_ternary },// ?

        // -- Coalescing ---------------------------------------------------
        { TokenType::null_coalesce,PrecedenceType::pr_coalescing },// ??
        { TokenType::error_coalesce,PrecedenceType::pr_coalescing },// !!

        // -- Channel select operators --------------------------------------
        // { TokenType::select_recv,PrecedenceType::pr_select },// <--
        // { TokenType::select_send,PrecedenceType::pr_select },// -->

        // -- Pipeline -----------------------------------------------------
        { TokenType::pipeline,PrecedenceType::pr_pipeline },// |>

        // -- Range --------------------------------------------------------
        { TokenType::double_dot,PrecedenceType::pr_range },// ..

        // -- Bitwise / logical or -----------------------------------------
        { TokenType::pipe,PrecedenceType::pr_or },// |

        // -- Bitwise xor --------------------------------------------------
        { TokenType::caret,PrecedenceType::pr_xor },// ^

        // -- Bitwise / logical and ----------------------------------------
        { TokenType::ampersand,PrecedenceType::pr_and },          // &
        // Note: prefix & (address-of) is handled as a nud, not here.

        // -- Equality -----------------------------------------------------
        { TokenType::eq,PrecedenceType::pr_equality },// ==
        { TokenType::neq,PrecedenceType::pr_equality },// !=

        // -- Relational comparison -----------------------------------------
        { TokenType::lt,PrecedenceType::pr_compare },// <
        { TokenType::gt,PrecedenceType::pr_compare },// >
        { TokenType::leq,PrecedenceType::pr_compare },// <=
        { TokenType::geq,PrecedenceType::pr_compare },// >=

        // -- Bit shifts ---------------------------------------------------
        { TokenType::shl,PrecedenceType::pr_shift },// <<
        { TokenType::shr,PrecedenceType::pr_shift },// >>

        // -- Additive -----------------------------------------------------
        { TokenType::plus,PrecedenceType::pr_sum },// +
        // Note: prefix + (unary plus) is handled as a nud, not here.
        { TokenType::minus,PrecedenceType::pr_sum },// -
        // Note: prefix - (negation) is handled as a nud, not here.

        // -- Multiplicative -----------------------------------------------
        { TokenType::star,PrecedenceType::pr_product },// *
        // Note: prefix * (pointer deref) is handled as a nud, not here.
        { TokenType::at,PrecedenceType::pr_product },// @  (matrix multiplication)
        // Note: prefix @ i.e decorator is not handled as nud or infix, but directly in parse_stmt. Dont worry about it in parse_expr
        { TokenType::slash,PrecedenceType::pr_product },// /
        { TokenType::percent,PrecedenceType::pr_product },// %

        // -- Exponentiation (right-associative) ---------------------------
        // When parsing the RHS of **, pass pr_product (one level below pr_power)
        // so that  a ** b ** c  folds as  a ** (b ** c).
        { TokenType::pow,PrecedenceType::pr_power },// **
        // Note: prefix ** is handled as a nud, not here.

        // -- Postfix operators --------------------------------------------
        { TokenType::incr,PrecedenceType::pr_postfix },// x++
        // Note: prefix ++ is handled as a nud, not here.
        { TokenType::decr,PrecedenceType::pr_postfix },// x--
        // Note: prefix -- is handled as a nud, not here.
        { TokenType::bang,PrecedenceType::pr_postfix },// x!  (error propagation)
        // Note: prefix ! (logical not) is handled as a nud, not here.

        // -- Member / path access -----------------------------------------
        { TokenType::dot,PrecedenceType::pr_member },// x.field
        { TokenType::arrow,PrecedenceType::pr_member },// ptr->field
        // { TokenType::double_colon,PrecedenceType::pr_member },// mod::item

        // -- Subscript ----------------------------------------------------
        { TokenType::lbracket,PrecedenceType::pr_index },// x[i] , x[i,j]

        // -- Function / method call ---------------------------------------
        { TokenType::lparen,PrecedenceType::pr_call },// f(args)

        // -- Generic instantiation ----------------------------------------
        // Must be above pr_call so that in  max{i32}(a, b)  the {i32} is
        // consumed first, producing a GenericExpr, and then (a,b) is parsed
        // as the call on top of that.
        // { TokenType::lbrace,PrecedenceType::pr_generic },// Foo{T}
    };
}

AstNodePtr Parser::parse(){
    if(this->toks.empty()){
        return std::make_shared<Program>(std::vector<AstNodePtr>{});
    }
    std::vector<AstNodePtr> statements;
    while(this->curr_tok.type != TokenType::eof){
        statements.push_back(parse_stmt());
        advance();
        if(this->curr_tok.type == TokenType::newline){
            advance();
        }
        else if(this->curr_tok.type != TokenType::eof){
            error(this->curr_tok, "Expected newline after statement");
        }
    }
    return std::make_shared<Program>(statements);
}

AstNodePtr Parser::parse_block(){
    const Token block_tok = this->curr_tok;
    advance_on_newline();
    advance(); // after '{'
    std::vector<AstNodePtr> statements;
    while(this->curr_tok.type != TokenType::rbrace){
        if(this->curr_tok.type == TokenType::eof){
            error(this->curr_tok, "Unexpected end of file");
        }
        statements.push_back(parse_stmt());
        advance();
        if(this->curr_tok.type == TokenType::newline){
            advance();
        }
        else if(this->curr_tok.type != TokenType::rbrace){
            error(this->curr_tok, "Expected newline after statement in block");
        }
    }
    return std::make_shared<Block>(block_tok,statements);
}
}