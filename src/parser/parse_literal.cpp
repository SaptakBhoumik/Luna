#include "ast/ast.hpp"
#include "lexer/token.hpp"
#include "parser/parser.hpp"
#include <vector>

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
    const Token tok = this->curr_tok;
    std::vector<Token> path = parse_path();
    std::vector<AstNodePtr> generic_args = {};
    
    if(peek().type == TokenType::lbrace){
        // parse generic args like Type{T, U}
        advance(); // On '{'
        // advance(); // consume '{'
        while(peek().type != TokenType::rbrace){
            advance_on_newline();
            if(peek().type == TokenType::rbrace){
                break;
            }
            advance();
            generic_args.push_back(parse_type_expr());
            advance_on_newline();
            if(peek().type == TokenType::comma){
                advance(); // On comma and continue parsing generic args
            }
            else if(peek().type != TokenType::rbrace){
                error(peek(),"expected ',' or '}' in generic argument list");
            }
        }
        advance(); // consume '}'
    }
    return std::make_shared<IdentifierLiteral>(tok,path,generic_args);
}

AstNodePtr Parser::parse_map_or_list(){
    const Token tok = this->curr_tok;
    advance(); // consume '['
    if(this->curr_tok.type == TokenType::rbracket){
        // empty list or dict literal. We will decide which one it is in the semantic analysis phase when we have type information. For now we just create a special node for it.
        return std::make_shared<EmptyDictOrListLiteral>(tok);
    }
    // std::vector<AstNodePtr> list_elements;
    // std::vector<std::pair<AstNodePtr, AstNodePtr>> dict_elements;
    AstNodePtr key_or_itm = parse_expression();
    if(peek().type == TokenType::colon){
        // dict literal
        std::vector<std::pair<AstNodePtr, AstNodePtr>> dict_elements;
        while(peek().type == TokenType::colon){
            advance(); // on :
            advance(); // on value after :
            dict_elements.push_back({key_or_itm, parse_expression()});
            if(peek().type == TokenType::comma){
                advance(); // consume comma and continue parsing dict elements
                if(peek().type == TokenType::rbracket){
                    break; // allow trailing comma
                }
                advance(); // on the element after comma
                key_or_itm = parse_expression(); // parse key of next element
            }
            else if(peek().type != TokenType::rbracket){
                error(peek(),"expected ',' or ']' in dict literal");
            }
        }
        advance(); // consume ']'
        return std::make_shared<DictLiteral>(tok,dict_elements);
    }
    else{
        std::vector<AstNodePtr> list_elements = {key_or_itm};
        // list literal
        while(peek().type == TokenType::comma){
            advance(); // on ,
            if(peek().type == TokenType::rbracket){
                break;
            }
            advance(); // on the element after ,
            list_elements.push_back(parse_expression());
        }
        expect(TokenType::rbracket,"expected ']' after list literal");
        return std::make_shared<ListLiteral>(tok,list_elements);
    }
}

AstNodePtr Parser::parse_tuple_or_paren_expr(){
    //Note:- () is not allowed cuz not makes sense
    const Token tok = this->curr_tok;
    advance(); // consume '('
    std::vector<AstNodePtr> elements;
    std::vector<std::pair<bool,bool>> is_pub_mut; // only used for assign tuple literal, ignored for regular tuple literal and parenthesized expression.
    bool is_assign_tuple_literal = false;
    if(this->curr_tok.type == TokenType::kw_mut || this->curr_tok.type == TokenType::kw_pub){
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
        elements.push_back(parse_expression());
        is_pub_mut.push_back({is_pub,is_mut});
        is_assign_tuple_literal = true;
    }
    else{
        elements.push_back(parse_expression());
        is_pub_mut.push_back({false,false});// dummy value, not used for regular tuple literal and parenthesized expression
    }
    if(peek().type == TokenType::comma || is_assign_tuple_literal){
        // tuple literal
        bool is_pub = false;
        bool is_mut = false;
        while(peek().type == TokenType::comma){
            advance(); // on ,
            if(peek().type == TokenType::rparen){
                break;
            }
            advance(); // On the element after ,
            if(this->curr_tok.type == TokenType::kw_pub){
                is_pub = true;
                advance();
                is_assign_tuple_literal = true;
            }
            else{
                is_pub = false;
            }
            if(this->curr_tok.type == TokenType::kw_mut){
                is_mut = true;
                advance();
                is_assign_tuple_literal = true;
            }
            else{
                is_mut = false;
            }
            elements.push_back(parse_expression());
            is_pub_mut.push_back({is_pub,is_mut});
        }
        advance(); // consume ')'
        if(is_assign_tuple_literal){
            return std::make_shared<AssignTupleLiteral>(tok,elements,is_pub_mut);
        }
        return std::make_shared<TupleLiteral>(tok,elements);
    }
    else{
        // parenthesized expression
        expect(TokenType::rparen,"expected ')' after parenthesized expression");
        return elements[0];
    }
}
}