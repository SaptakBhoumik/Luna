#include "ast/ast.hpp"
#include "parser/parser.hpp"

namespace Luna {
AstNodePtr Parser::parse_int(){
    return std::make_shared<IntegerLiteral>(this->curr_tok);
}
AstNodePtr Parser::parse_decimal(){
    return std::make_shared<DecimalLiteral>(this->curr_tok);
}
AstNodePtr Parser::parse_string(){
    bool raw = false;
    if(this->curr_tok.type == TokenType::raw){
        raw = true;
        advance();
    }
    return std::make_shared<StringLiteral>(this->curr_tok,raw);
}
AstNodePtr Parser::parse_bool(){
    return std::make_shared<BoolLiteral>(this->curr_tok);
}
AstNodePtr Parser::parse_none(){
    return std::make_shared<NoneLiteral>(this->curr_tok);
}
AstNodePtr Parser::parse_identifier(){
    std::vector<std::string> path = {this->curr_tok.value};
    std::vector<AstNodePtr> generic_args = {};
    if(peek().type == TokenType::double_colon){
        // parse path like A::B::C
        while(peek().type == TokenType::double_colon){
            advance(); // consume current identifier
            advance(); // consume double colon
            expect(TokenType::identifier,"expected identifier after '::' in path");
            path.push_back(this->curr_tok.value);
        }
    }
    if(peek().type == TokenType::lbrace){
        // parse generic args like Type{T, U}
        advance(); // consume identifier
        advance(); // consume '{'
        while(peek().type != TokenType::rbrace){
            advance_on_newline();
            if(peek().type == TokenType::rbrace){
                break;
            }
            if(this->curr_tok.type == TokenType::newline){
                advance();
                continue;
            }
            generic_args.push_back(parse_type_expr());
            advance_on_newline();
            if(peek().type == TokenType::comma){
                advance(); // consume comma and continue parsing generic args
            }
            else if(peek().type != TokenType::rbrace){
                error(peek(),"expected ',' or '}' in generic argument list");
            }
        }
        advance(); // consume '}'
    }
    return std::make_shared<IdentifierLiteral>(this->curr_tok,path,generic_args);
}
}