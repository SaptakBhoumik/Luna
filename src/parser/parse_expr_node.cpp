#include "ast/ast.hpp"
#include "ast/ast_utils.hpp"
#include "lexer/token.hpp"
#include "parser/parser.hpp"
#include <iostream>
#include <vector>

namespace Luna {
AstNodePtr Parser::parse_expression(PrecedenceType precedence){
    //TODO:
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
        advance(); // on next index expression
        indices.push_back(parse_expression());
    }
    expect(TokenType::rbracket, "Expected ']' after index expression");
    return std::make_shared<IndexExpr>(index_op, container, indices);
}

AstNodePtr Parser::parse_dot_or_arrow_expr(AstNodePtr left){
    Token op = this->curr_tok;
    advance();
    expect(TokenType::identifier, "Expected identifier after '.' or '->' operator");
    Token member_name = this->curr_tok;
    if(op.type == TokenType::dot){
        return std::make_shared<DotExpr>(op, left, member_name);
    }
    else{
        return std::make_shared<ArrowExpr>(op, left, member_name);
    }
}
}