#include "ast/ast.hpp"
#include "ast/ast_utils.hpp"
#include "lexer/token.hpp"
#include "parser/parser.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace Luna {
AstNodePtr Parser::parse_expression(PrecedenceType precedence) {
    AstNodePtr left;

    // -- NUD (prefix / atom) --------------------------------------------------
    switch (this->curr_tok.type) {
        case TokenType::integer:{
            left = parse_int();
            break;
        }

        case TokenType::decimal:{
            left = parse_decimal();
            break;
        }

        // raw string: curr_tok is `raw`, parse_string() handles the advance internally
        case TokenType::raw:
        case TokenType::string:{
            left = parse_string();
            break;
        }

        // f"..." — parse_formatted_string advances past the 'f' token itself
        case TokenType::format:{
            left = parse_formatted_string();
            break;
        }

        case TokenType::kw_true:
        case TokenType::kw_false:{
            left = parse_bool();
            break;
        }

        case TokenType::kw_none:{
            left = parse_none();
            break;
        }

        // identifier also handles path (a::b::c) and generic args (Foo{T}) internally
        case TokenType::identifier:{
            left = parse_identifier();
            break;
        }

        // grouped expr  (x)  or  tuple  (a, b)  or  assign-tuple  (pub mut a, b)
        case TokenType::lparen:{
            left = parse_tuple_or_paren_expr();
            break;
        }

        // list literal [1,2,3]  or  dict literal ["a":1]  or  empty []
        case TokenType::lbracket:{
            left = parse_map_or_list();
            break;
        }
        // prefix operators: - + ! ~ & * ** ++ --
        // parse_prefix_op reads curr_tok as the prefix then advances and recurses at pr_prefix
        // Special case for **: parse_prefix_op already rewrites it as two dereferences (*(*x))
        case TokenType::minus:
        case TokenType::plus:
        case TokenType::bang:       // logical not
        case TokenType::tilde:      // bitwise not
        case TokenType::ampersand:  // address-of   &val
        case TokenType::star:       // pointer deref *ptr
        case TokenType::pow:        // double deref  **ptr  (rewritten to *(*ptr) in parse_prefix_op)
        case TokenType::incr:       // prefix ++
        case TokenType::decr:{      // prefix --
            left = parse_prefix_op();
            break;
        }
        // $expr — compile-time expression; parse_compile_time_expr advances past '$' then
        // recurses at pr_prefix so the whole CT sub-expression binds tightly
        case TokenType::dollar:{
            if(peek().type == TokenType::identifier){
                left = parse_identifier();
            }
            else{
                left = parse_compile_time_expr();
            }
            break;
        }
        // fn(params)[capture] -> T { body }
        case TokenType::kw_fn:{
            left = parse_lambda_expr();
            break;
        }

        // thread { } / thread expr   or   task { } / task expr
        case TokenType::kw_thread:
        case TokenType::kw_task:{
            left = parse_thread_or_task_expr();
            break;
        }

        case TokenType::eof:{
            error(this->curr_tok, "Unexpected end of file");
        }
        default:{
            error(this->curr_tok,"'" + this->curr_tok.value + "' is not an expression");
        }
    }
    // -- LED (infix) loop -----------------------------------------------------
    while (peek_precedence() > precedence) {
        advance(); // curr_tok is now the infix / postfix operator
        switch (this->curr_tok.type) {

            // -- postfix — consume no RHS ----------------------------------
            // ++ and -- are in the map at pr_postfix.
            // ! (bang) is also at pr_postfix here; in the NUD switch above it
            // acted as logical-not. The disambiguation is positional: if we
            // reach the LED loop, a left operand already exists, so it must
            // be error-propagation postfix.
            case TokenType::incr:
            case TokenType::decr:
            case TokenType::bang:{
                left = parse_postfix_op(left);
                break;
            }

            // -- member access — . and -> ----------------------------------
            case TokenType::dot:
            case TokenType::arrow:{
                left = parse_dot_or_arrow_expr(left);
                break;
            }

            // -- subscript — x[i]  or  x[i,j] ----------------------------
            case TokenType::lbracket:{
                left = parse_index_expr(left);
                break;
            }

            // -- function / method call — f(args) -------------------------
            case TokenType::lparen:{
                left = parse_func_call(left);
                break;
            }

            // -- trailing-block call — f => { }  or  f(args) => fn { } ----
            case TokenType::thick_arrow:{
                left = parse_arrow_block_call(left);
                break;
            }

            // -- ternary — cond ? then : else -----------------------------
            case TokenType::question:{
                left = parse_ternary_expr(left);
                break;
            }

            // -- null / error coalescing — ?? and !! ----------------------
            case TokenType::null_coalesce:
            case TokenType::error_coalesce:{
                left = parse_coalescing_op(left);
                break;
            }

            // -- range — start..end  or  start..end..step -----------------
            case TokenType::double_dot:{
                left = parse_range_expr(left);
                break;
            }

            // -- exponentiation — right-associative -----------------------
            // parse_bin_op would pass pr_power as the minimum for the RHS,
            // making a**b**c left-associative.  We instead pass pr_product
            // (one level below pr_power) so that  a**b**c -> a**(b**c).
            case TokenType::pow: {
                Token op = this->curr_tok;
                advance(); // move to first token of RHS
                AstNodePtr right = parse_expression(PrecedenceType::pr_product);
                left = std::make_shared<BinOp>(op, left, op, right);
                break;
            }

            // -- all remaining binary operators ----------------------------
            // Covers: + - * / % @ & | ^ << >> == != < > <= >= |>
            // parse_bin_op reads curr_tok as the op, advances to the RHS,
            // then recurses at the op's own precedence (left-associative).
            default:{
                left = parse_bin_op(left);
                break;
            }
        }
    }

    // Consume an optional trailing newline so callers don't have to.
    advance_on_newline();
    return left;
}

AstNodePtr Parser::parse_bin_op(AstNodePtr left){
    Token op = this->curr_tok;
    PrecedenceType op_precedence = this->precedence_map[op.type];

    advance();
    AstNodePtr right = parse_expression(op_precedence);
    return std::make_shared<BinOp>(op, left, op, right);
}
AstNodePtr Parser::parse_prefix_op(){
    Token prefix = this->curr_tok;
    advance();
    AstNodePtr right = parse_expression(PrecedenceType::pr_prefix);
    if(prefix.type == TokenType::pow){
        Token fake_op0 = Token{prefix.col, prefix.source_line, "*", 
                             prefix.start, prefix.start+1, prefix.line, TokenType::star};
        Token fake_op1 = Token{prefix.col+1, prefix.source_line, "*", 
                             prefix.start+1, prefix.start+2, prefix.line, TokenType::star};
        return std::make_shared<PrefixOp>(fake_op1, fake_op1, std::make_shared<PrefixOp>(fake_op0, fake_op0, right));
    }
    return std::make_shared<PrefixOp>(prefix, prefix, right);
}
AstNodePtr Parser::parse_postfix_op(AstNodePtr left){
    return std::make_shared<PostfixOp>(this->curr_tok, this->curr_tok, left);
}
AstNodePtr Parser::parse_coalescing_op(AstNodePtr left){
    Token op = this->curr_tok;
    bool is_null_coalescing = (op.type == TokenType::null_coalesce);
    if(is_null_coalescing){
        if(peek().type == TokenType::lbrace){
            advance(); // on '{'
            AstNodePtr body = parse_block();
            return std::make_shared<CoalescingOP>(op, left, body, true, std::nullopt);
        }
    }
    if (peek().type == TokenType::lbrace) {
        advance();
        AstNodePtr body = parse_block();
        return std::make_shared<CoalescingOP>(op, left, body, false, std::nullopt);
    }
    if(peek().type != TokenType::lparen){
        advance();
        AstNodePtr right = parse_expression(PrecedenceType::pr_coalescing);
        return std::make_shared<CoalescingOP>(op, left, right, false, std::nullopt);
    }
    if(peek(2).type == TokenType::kw_mut){
        advance();//On '('
        advance(); // on 'mut'
        expect(TokenType::identifier, "Expected identifier for coalescing assignment");
        Token var_name = this->curr_tok;
        expect(TokenType::rparen, "Expected ')' after coalescing assignment variable");
        expect(TokenType::lbrace, "Expected '{' after coalescing assignment");
        AstNodePtr body = parse_block();
        return std::make_shared<CoalescingOP>(op, left, body, false, std::make_pair(var_name, true));//True means mutable
    }
    if(peek(2).type == TokenType::identifier && peek(3).type == TokenType::rparen && peek(4).type == TokenType::lbrace){
        advance();//On '('
        advance(); // on identifier
        Token var_name = this->curr_tok;
        advance(); // on ')'
        advance(); // on '{'
        AstNodePtr body = parse_block();
        return std::make_shared<CoalescingOP>(op, left, body, false, std::make_pair(var_name, false));//False means immutable
    }

    advance();
    AstNodePtr right = parse_expression(PrecedenceType::pr_coalescing);
    return std::make_shared<CoalescingOP>(op, left, right, false, std::nullopt);
}

AstNodePtr Parser::parse_range_expr(AstNodePtr left){
    Token range_op = this->curr_tok;
    advance();
    AstNodePtr end = parse_expression(PrecedenceType::pr_range);
    AstNodePtr step = std::make_shared<NoLiteral>();// default step of 1
    if(peek().type == TokenType::double_dot){
        advance(); // on second '..'
        advance(); // on step expression
        step = parse_expression(PrecedenceType::pr_range);
    }
    return std::make_shared<RangeExpr>(range_op, left, end, step);
}

AstNodePtr Parser::parse_index_expr(AstNodePtr container){
    Token index_op = this->curr_tok;
    advance();
    std::vector<AstNodePtr> indices = {parse_expression()};
    while(peek().type == TokenType::comma){
        advance(); // on comma
        if(peek().type == TokenType::rbracket){
            break;
        }
        advance(); // on next index expression
        indices.push_back(parse_expression());
    }
    expect(TokenType::rbracket, "Expected ']' after index expression");
    return std::make_shared<IndexExpr>(index_op, container, indices);
}

AstNodePtr Parser::parse_dot_or_arrow_expr(AstNodePtr left){
    Token op = this->curr_tok;
    bool compile_time = false;
    advance();
    if(peek().type == TokenType::dollar){
        compile_time = true;
        advance();
    }
    expect(TokenType::identifier, "Expected identifier after '.' or '->' operator");
    Token member_name = this->curr_tok;
    if(op.type == TokenType::dot){
        return std::make_shared<DotExpr>(op, left, std::make_pair(member_name, compile_time));
    }
    else{
        return std::make_shared<ArrowExpr>(op, left, std::make_pair(member_name, compile_time));
    }
}
AstNodePtr Parser::parse_func_call(AstNodePtr left){
    Token tok = this->curr_tok;
    std::vector<AstNodePtr> args;
    std::vector<std::pair<std::pair<Token,bool>, AstNodePtr>> named_args;
    while(peek().type != TokenType::rparen){
        if(peek().type == TokenType::identifier && peek(2).type == TokenType::assign){
            // named argument
            advance(); // on identifier
            Token name = this->curr_tok;
            advance(); // on '='
            advance(); // on start of expression
            AstNodePtr arg = parse_expression();
            named_args.push_back(std::make_pair(std::make_pair(name, false), arg));
        }
        if(peek().type == TokenType::dollar && peek(2).type == TokenType::identifier && peek(3).type == TokenType::assign){
            advance(); // on '$'
            advance(); // on identifier
            Token name = this->curr_tok;
            advance(); // on '='
            advance(); // on start of expression
            AstNodePtr arg = parse_expression();
            named_args.push_back(std::make_pair(std::make_pair(name, true), arg));
        }
        else{
            // positional argument
            advance(); // on start of expression
            AstNodePtr arg = parse_expression();
            args.push_back(arg);
        }
        if(peek().type == TokenType::comma){
            advance(); // on comma
        }
        else{
            break;
        }
    }
    expect(TokenType::rparen, "Expected ')' after function call arguments");
    return std::make_shared<FuncCall>(tok, left, args, named_args);
}

AstNodePtr Parser::parse_ternary_expr(AstNodePtr condition){
    Token tok = this->curr_tok;
    advance();
    AstNodePtr then_value = parse_expression(PrecedenceType::pr_ternary);
    expect(TokenType::colon, "Expected ':' in ternary expression");
    advance();
    AstNodePtr else_value = parse_expression(PrecedenceType::pr_ternary);
    return std::make_shared<TernaryIf>(tok, condition, then_value, else_value);
}
AstNodePtr Parser::parse_compile_time_expr(){
    Token tok = this->curr_tok;
    advance(); 
    AstNodePtr expr_or_stmt = parse_expression(PrecedenceType::pr_prefix);
    return std::make_shared<CompTimeExpr>(tok, expr_or_stmt);
}
AstNodePtr Parser::parse_lambda_expr(){
    const Token tok = this->curr_tok;
    LambdaFuncSignature signature = parse_lambda_signature();
    expect(TokenType::lbrace, "Expected '{' at the start of lambda body");
    AstNodePtr body = parse_block();
    return std::make_shared<LambdaExpr>(tok, signature.capture, signature.parameters, signature.return_type, body);
}

AstNodePtr Parser::parse_formatted_string(){
    const Token tok = this->curr_tok;
    advance();//After the 'f' token
    std::vector<Token> parts;
    std::vector<AstNodePtr> expressions;

    while(this->curr_tok.type != TokenType::format_str_end){
        if(this->curr_tok.type == TokenType::format_str){
            parts.push_back(this->curr_tok);
            advance();
        }
        else{
            // expression part
            AstNodePtr expr = parse_expression();
            expressions.push_back(expr);
            advance();
        }
    }
    return std::make_shared<FormattedStr>(tok, parts, expressions);
}

AstNodePtr Parser::parse_thread_or_task_expr(){
    const Token tok = this->curr_tok;
    bool is_thread = (tok.type == TokenType::kw_thread);
    AstNodePtr body;
    AstNodePtr restart_count = std::make_shared<NoLiteral>();
    AstNodePtr timeout = std::make_shared<NoLiteral>();
    AstNodePtr always_restart = std::make_shared<NoLiteral>();
    AstNodePtr restart_delay = std::make_shared<NoLiteral>();
    
    if(peek().type == TokenType::lparen && peek(2).type == TokenType::identifier && peek(3).type == TokenType::assign){
        advance(); // on '('
        advance(); // after '('
        while(this->curr_tok.type != TokenType::rparen){
            if(this->curr_tok.type != TokenType::identifier){
                error(this->curr_tok, "Expected identifier in thread/task options");
            }
            std::string option_name = this->curr_tok.value;
            expect(TokenType::assign, "Expected '=' after thread/task option name");
            advance();
            AstNodePtr option_value = parse_expression();
            if(option_name == "restart_count"){
                if(restart_delay->kind() != AstKind::NoLiteral){
                    error(this->curr_tok, "Duplicate restart_count option");
                }
                restart_count = option_value;
            }
            else if(option_name == "timeout"){
                if(timeout->kind() != AstKind::NoLiteral){
                    error(this->curr_tok, "Duplicate timeout option");
                }
                timeout = option_value;
            }
            else if(option_name == "always_restart"){
                if(always_restart->kind() != AstKind::NoLiteral){
                    error(this->curr_tok, "Duplicate always_restart option");
                }
                always_restart = option_value;
            }
            else if(option_name == "restart_delay"){
                if(restart_delay->kind() != AstKind::NoLiteral){
                    error(this->curr_tok, "Duplicate restart_delay option");
                }
                restart_delay = option_value;
            }
            else{
                error(this->curr_tok, "Unknown thread/task option: " + option_name);
            }
            if(peek().type == TokenType::comma){
                advance();
                if(peek().type == TokenType::rparen){
                    advance();
                    break;
                }
            }
        }
    }
    advance();
    if(this->curr_tok.type == TokenType::lbrace){
        body = parse_block();
    }
    else{
        body = parse_expression(PrecedenceType::pr_lowest);
    }
    return std::make_shared<ThreadOrTaskExpr>(tok, is_thread, body, restart_count, timeout, always_restart, restart_delay);   
}

AstNodePtr Parser::parse_arrow_block_call(AstNodePtr left){
    const Token tok = this->curr_tok;
    AstNodePtr callee;
    std::vector<AstNodePtr> args;
    std::vector<std::pair<std::pair<Token,bool>, AstNodePtr>> named_args;
    if(left->kind() == AstKind::FuncCall){
        auto func_call = std::dynamic_pointer_cast<FuncCall>(left);
        callee = func_call->get_callee();
        args = func_call->get_arguments();
        named_args = func_call->get_named_arguments();
    }
    else{
        callee = left;
    }
    advance(); // after '=>'
    AstNodePtr body;
    if(this->curr_tok.type == TokenType::kw_fn){
        body = parse_lambda_expr();
    }
    else if(this->curr_tok.type == TokenType::lbracket){
        auto capture = parse_capture_clause();
        expect(TokenType::lbrace, "Expected '{' after capture clause in arrow block call");
        AstNodePtr lambda_body = parse_block();
        body = std::make_shared<LambdaExpr>(tok, capture, std::vector<Parameter>{}, std::make_shared<NoLiteral>(), lambda_body);
    }
    else if(this->curr_tok.type == TokenType::lbrace){
        AstNodePtr lambda_body = parse_block();
        CaptureClause empty_capture = CaptureClause{CaptureKind::None, {}};
        body = std::make_shared<LambdaExpr>(tok, empty_capture, std::vector<Parameter>{}, std::make_shared<NoLiteral>(), lambda_body);
    }
    else{
        error(this->curr_tok, "Expected 'fn' , '[' or '{' after '=>'");
    }
    return std::make_shared<ArrowBlockCallExpr>(tok, callee, args, named_args, body);
}
}