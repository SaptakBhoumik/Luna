#include "ast/ast.hpp"
#include "ast/ast_utils.hpp"
#include "lexer/token.hpp"
#include "parser/parser.hpp"
#include <iostream>
#include <memory>
#include <vector>

namespace Luna {
AstNodePtr Parser::parse_import_stmt(){
    expect(TokenType::identifier, "Expected identifier after 'import'");
    std::vector<Token> path = parse_path();
    std::vector<std::vector<Token>> symbols;
    if(peek().type == TokenType::double_colon){
        advance(); // on '::'
        expect(TokenType::lbrace, "Expected '{' after '::' in import statement");
        while(peek().type != TokenType::rbrace){
            advance_on_newline();
            if(peek().type == TokenType::rbrace){
                break;
            }
            advance();
            symbols.push_back(parse_path());
            advance_on_newline();
            if(peek().type == TokenType::comma){
                advance(); // on ','
            }
            else if(peek().type != TokenType::rbrace){
                error(peek(), "Expected ',' or '}' after imported symbol in import statement");
            }
        }
        advance(); // on '}'
    }
    return std::make_shared<ImportStmt>(this->curr_tok, path, symbols);
}
AstNodePtr Parser::parse_using_stmt(){
    const Token using_tok = this->curr_tok;
    std::optional<Token> alias = std::nullopt;
    if(peek(2).type == TokenType::assign){
        expect(TokenType::identifier, "Expected identifier as alias in using statement");
        alias = this->curr_tok;
        advance(); // On =
    }
    expect(TokenType::identifier, "Expected identifier in using statement");
    std::vector<Token> path = parse_path();
    return std::make_shared<UsingStmt>(using_tok, path, alias);
}
}
