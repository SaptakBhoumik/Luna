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
    std::pair<std::vector<Token>, bool> path = parse_path();
    std::vector<std::pair<std::vector<Token>, bool>> symbols;
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
    if(path.second){
        error(this->curr_tok, "Compile-time paths cannot be used in import statements");
    }
    return std::make_shared<ImportStmt>(this->curr_tok, path.first, symbols);
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
    std::pair<std::vector<Token>, bool> path = parse_path();
    if(path.second){
        error(this->curr_tok, "Compile-time paths cannot be used in using statements");
    }
    return std::make_shared<UsingStmt>(using_tok, path.first, alias);
}
}
