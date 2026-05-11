#include "ast/ast.hpp"
#include "ast/ast_utils.hpp"
#include "lexer/token.hpp"
#include "parser/parser.hpp"
#include <iostream>
#include <vector>

namespace Luna {
Attribute Parser::parse_attribute(){
    expect(TokenType::lbracket, "Expected '[' or identifier after '@' for attribute/decorator");
    expect(TokenType::identifier, "Expected identifier for attribute name after '['");
    std::string name = this->curr_tok.value;
    std::vector<AstNodePtr> args;
    std::vector<std::pair<std::string, AstNodePtr>> named_args;
    if(peek().type == TokenType::lparen){
        advance(); // consume '('
        while(peek().type != TokenType::rparen){
            advance();//On the first token of the argument
            if(peek().type == TokenType::assign){
                // named argument
                std::string arg_name = this->curr_tok.value;
                advance(); // on '='
                advance(); // the tok after '=' i.e first token of argument value
                AstNodePtr arg_value = parse_expression();
                named_args.push_back({arg_name, arg_value});
            }
            else{
                // positional argument
                AstNodePtr arg_value = parse_expression();
                args.push_back(arg_value);
                if(named_args.size() > 0){
                    error(peek(), "Positional argument cannot follow named argument in attribute");
                }
            }
            if(peek().type == TokenType::comma){
                advance(); // On ',
            }
            else if(peek().type != TokenType::rparen){
                error(peek(), "Expected ',' or ')' after argument in attribute");
            }
        }
        advance(); // consume ')'
    }
    expect(TokenType::rbracket, "Expected ']' at the end of attribute");
    return Attribute(name, args, named_args);
}
}
