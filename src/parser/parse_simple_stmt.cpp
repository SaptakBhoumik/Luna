#include "ast/ast.hpp"
#include "ast/ast_utils.hpp"
#include "lexer/token.hpp"
#include "parser/parser.hpp"
#include <iostream>
#include <memory>
#include <vector>

namespace Luna {
AstNodePtr Parser::parse_defer_stmt(){
    Token tok = this->curr_tok;
    this->advance();
    if(this->curr_tok.type == TokenType::lbrace){
        AstNodePtr body = parse_block();
        return std::make_shared<DeferStmt>(tok, body);
    }
    else if(this->curr_tok.type == TokenType::newline){
        error(this->curr_tok, "Unexpected newline after 'defer'", 
            "In Luna, you cant enter new line before the '{' token in any kind of statement. This is the make the code more readable and uniform");
    }
    else{
        AstNodePtr body = parse_stmt();
        return std::make_shared<DeferStmt>(tok, body);
    }
}
AstNodePtr Parser::parse_scope_stmt(){
    Token tok = this->curr_tok;
    return std::make_shared<ScopeStmt>(tok, parse_block());
}
AstNodePtr Parser::parse_break_stmt(){
    return std::make_shared<BreakStmt>(this->curr_tok);
}
AstNodePtr Parser::parse_continue_stmt(){
    return std::make_shared<ContinueStmt>(this->curr_tok);
}
AstNodePtr Parser::parse_return_stmt(){
    Token tok = this->curr_tok;
    this->advance();
    std::vector<AstNodePtr> values = {parse_expression()};
    while(peek().type == TokenType::comma){
        advance();//on ','
        advance();//After ','
        values.push_back(parse_expression());
    }
    return std::make_shared<ReturnStmt>(tok, values);
}
AstNodePtr Parser::parse_give_stmt(){
    Token tok = this->curr_tok;
    this->advance();
    std::vector<AstNodePtr> values = {parse_expression()};
    while(peek().type == TokenType::comma){
        advance();//on ','
        advance();//After ','
        values.push_back(parse_expression());
    }
    return std::make_shared<GiveStmt>(tok, values);
}
AstNodePtr Parser::parse_lock_stmt(){
    Token tok = this->curr_tok;
    this->advance();
    if(this->curr_tok.type == TokenType::lbrace){
        AstNodePtr body = parse_block();
        return std::make_shared<LockStmt>(tok, std::make_shared<NoLiteral>(), body);
    }
    else if(this->curr_tok.type == TokenType::newline){
        error(this->curr_tok, "Unexpected newline after 'lock'", 
            "In Luna, you cant enter new line before the '{' token in any kind of statement. This is the make the code more readable and uniform");
    }
    else{
        AstNodePtr target = parse_tuple_or_paren_expr();
        if(this->curr_tok.type == TokenType::newline){
            error(this->curr_tok, "Unexpected newline after lock target", 
            "In Luna, you cant enter new line before the '{' token in any kind of statement. This is the make the code more readable and uniform");
        }
        expect(TokenType::lbrace, "Expected '{' after lock target");
        AstNodePtr body = parse_block();
        return std::make_shared<LockStmt>(tok, target, body);
    }
}
}