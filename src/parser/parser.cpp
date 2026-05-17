#include "parser/parser.hpp"
#include "ast/ast_utils.hpp"
#include "lexer/token.hpp"
#include <iostream>

namespace Luna {
Parser::Parser(const std::vector<Token>& toks, const std::string& filename){
    this->toks = toks;
    this->filename = filename;
    if (!toks.empty()) {
        this->curr_tok = toks[0];
    }
    this->precedence_map = {
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
        advance_on_newline();
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
        if(peek().type == TokenType::newline){
            advance();
            advance();
        }
        else if(peek().type == TokenType::rbrace){
            advance(); // on '}'
            break;
        }
        else if(this->curr_tok.type != TokenType::rbrace){
            std::cout << this->curr_tok << std::endl;
            error(this->curr_tok, "Expected newline after statement in block");
        }
    }
    return std::make_shared<Block>(block_tok,statements);
}

AstNodePtr Parser::parse_stmt(){
    switch (this->curr_tok.type) {
        case TokenType::newline:{
            advance();
            return parse_stmt();
        }

        case TokenType::kw_defer:{
            return parse_defer_stmt();
        }
        case TokenType::lbrace:{
            return parse_scope_stmt();
        }
        case TokenType::kw_break:{
            return parse_break_stmt();
        }
        case TokenType::kw_continue:{
            return parse_continue_stmt();
        }
        case TokenType::kw_ret:{
            return parse_return_stmt();
        }
        case TokenType::kw_give:{
            return parse_give_stmt();
        }
        case TokenType::kw_lock:{
            return parse_lock_stmt();
        }

        case TokenType::kw_import:{
            return parse_import_stmt();
        }
        case TokenType::kw_using:{
            return parse_using_stmt();
        }

        case TokenType::kw_when:{
            return parse_when_stmt();
        }
        case TokenType::kw_select:{
            return parse_select_stmt();
        }

        case TokenType::dollar:{
            const Token stmt_tok = this->curr_tok;
            switch (peek().type){
                case TokenType::kw_defer:
                case TokenType::kw_break:
                case TokenType::kw_continue:
                case TokenType::kw_ret:
                case TokenType::kw_give:
                case TokenType::kw_when:
                case TokenType::kw_loop:{
                    advance();
                    return std::make_shared<CompTimeExpr>(stmt_tok, parse_stmt());
                }
                default:{
                    //Do nothing. Treate it like an expression statement and parse at end. IF it turns out to be var statement then also no issue since the $ will just be part of the name expression
                }
            }
            //Dont put break here since we want to handle the case where $ is used in an expression statement as well. 
        }
        default:{
            std::vector<Annotation> annotations;
            bool is_pub = false;
            bool is_mut = false;
            while(this->curr_tok.type == TokenType::at || this->curr_tok.type == TokenType::hash){
                annotations.push_back(parse_annotation());
                advance_on_newline();
                advance();
            }
            if(this->curr_tok.type == TokenType::kw_pub){
                is_pub = true;
                advance();
            }
            if(this->curr_tok.type == TokenType::kw_mut){
                is_mut = true;
                advance();
            }
            if(this->curr_tok.type == TokenType::kw_fn){
                if(is_mut){
                    error(this->curr_tok, "Functions/methods cannot be mutable");
                }
                return parse_func_or_method_def(annotations, is_pub);
            }
            else if(this->curr_tok.type == TokenType::kw_type){
                if(is_mut){
                    error(this->curr_tok, "Types cannot be mutable");
                }
                return parse_type_def_stmt(annotations, is_pub);
            }
            else if(this->curr_tok.type == TokenType::kw_loop){
                if(is_mut){
                    error(this->curr_tok, "Loop statements cannot be mutable");
                }
                else if(is_pub){
                    error(this->curr_tok, "Loop statements cannot be public");
                }
                return parse_loop_stmt(annotations);
            }
            std::vector<std::pair<AstNodePtr, std::pair<bool, bool>>> names;
            names.push_back(std::make_pair(parse_expression(), std::pair<bool, bool>{is_pub,is_mut}));
            while (peek().type == TokenType::comma) {
                advance(); // on ','
                advance(); // on next token after ','
                bool is_pub = false;
                bool is_mut = false;
                if(this->curr_tok.type == TokenType::kw_pub){
                    is_pub = true;
                    advance();
                }
                if(this->curr_tok.type == TokenType::kw_mut){
                    is_mut = true;
                    advance();
                }
                names.push_back(std::make_pair(parse_expression(), std::pair<bool, bool>{is_pub,is_mut}));
            }
            auto is_var_def = [](std::pair<bool, bool> info){
                //At this stage a valid code can only be var def/assign or aug assign
                //This just makes sure if the var is definately a var def or not. So not all var def return true but if true then must be var def
                return (info.second == true)|| (info.first == true);
            };
            switch (peek().type){
                case TokenType::assign:
                case TokenType::walrus:
                case TokenType::colon:{
                    advance(); // on =,:= or :
                    return parse_var_stmt(annotations, names);
                }
                case TokenType::plus_eq:          // +=
                case TokenType::minus_eq:         // -=
                case TokenType::star_eq:          // *=
                case TokenType::pow_eq:           // **=
                case TokenType::slash_eq:         // /=
                case TokenType::percent_eq:       // %=
                case TokenType::caret_eq:         // ^=
                case TokenType::amp_eq:           // &=
                case TokenType::pipe_eq:          // |=
                case TokenType::shl_eq:           // <<=
                case TokenType::shr_eq:{          // >>=
                    std::vector<AstNodePtr> targets;
                    if(annotations.size() > 0){
                        error(this->curr_tok, "Cannot have annotations on augmented assignment");
                    }
                    for(const auto& name : names){
                        if(is_var_def(name.second)){
                            error(this->curr_tok, "Unexpected token in variable defination");
                        }
                        targets.push_back(name.first);
                    }
                    advance(); // on the aug assign operator
                    return parse_aug_assign_stmt(targets);
                }
                default:{
                    if(annotations.size() > 0){
                        error(this->curr_tok, "Unexpected token after annotation");
                    }
                    else if(names.size() > 1){
                        error(this->curr_tok, "Unexpected token after multiple variable names");
                    }
                    else if(is_var_def(names[0].second)){
                        error(this->curr_tok, "Unexpected token after variable name");
                    }
                    return names[0].first;//This is just an expression statement, not a variable definition/assignment. So we just parse it as an expression and return
                }
            }
        }
    }
}
}