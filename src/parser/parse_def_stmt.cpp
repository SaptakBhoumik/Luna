#include "ast/ast.hpp"
#include "ast/ast_utils.hpp"
#include "lexer/token.hpp"
#include "parser/parser.hpp"
#include <vector>

namespace Luna {
AstNodePtr Parser::parse_type_def_stmt(std::vector<Annotation> annotations, bool is_pub){
    std::vector<Attribute> attributes;
    for(const auto& ann : annotations){
        if(ann.is_decorator){
            error(ann.decorator.decorator->token(), "Decorators are not allowed on type definitions", "", "LUNA_ERR_DECORATOR_ON_TYPE_DEF");
        }
        else{
            attributes.push_back(ann.attribute);
        }
    }
    Token tok = this->curr_tok; // 'type' token
    expect(TokenType::identifier, "Expected type name after 'type' keyword");
    Token name = this->curr_tok;
    std::vector<std::pair<Token, AstNodePtr>> generics;
    advance();
    if(curr_tok.type == TokenType::lbrace){
        generics = parse_generic_params();
        if(peek().type == TokenType::assign){
            advance();
        }
    }
    if(curr_tok.type == TokenType::assign){
        advance(); // after '='
        AstNodePtr base_type = parse_type_expr();
        return std::make_shared<TypeDefStmt>(tok, attributes, is_pub, name, generics, base_type);
    }
    else{
        // Opaque type definition without a base type
        return std::make_shared<TypeDefStmt>(tok, attributes, is_pub, name, generics, std::make_shared<NoLiteral>());
    }

}

AstNodePtr Parser::parse_var_stmt(std::vector<Annotation> annotations, std::vector<std::pair<AstNodePtr, triplet<bool, VarKind, bool>>> names){
    std::vector<Attribute> attributes;
    for(const auto& ann : annotations){
        if(ann.is_decorator){
            error(ann.decorator.decorator->token(), "Decorators are not allowed on variable definitions", "", "LUNA_ERR_DECORATOR_ON_VAR_DEF");
        }
        else{
            attributes.push_back(ann.attribute);
        }
    }
    Token tok = this->curr_tok; // The token on which the variable declaration starts (could be 'var' or the first variable name)
    AstNodePtr type = std::make_shared<NoLiteral>();
    std::vector<AstNodePtr> values;
    bool def = false;
    if(curr_tok.type == TokenType::colon){
        advance(); // after ':'
        type = parse_type_expr();
        if(peek().type == TokenType::assign){
            advance(); // on '='
            advance(); // after '='
            values.push_back(parse_expression());
        }
        def = true;
    }
    else if(curr_tok.type == TokenType::assign){
        advance(); // after '='
        values.push_back(parse_expression());
    } 
    else if(curr_tok.type == TokenType::walrus){
        advance(); // after ':='
        values.push_back(parse_expression());
        def = true;
    }
    if(values.size()!=0){
        while(peek().type == TokenType::comma){
            advance(); // on comma
            advance(); // after comma
            values.push_back(parse_expression());
        }
    }
    return std::make_shared<VarStmt>(tok, names, type, values, def, attributes);
}

AstNodePtr Parser::parse_aug_assign_stmt(std::vector<AstNodePtr> targets){
    Token op_tok = this->curr_tok; // The operator token (e.g., +=, -=)
    advance(); // Move past the operator
    std::vector<AstNodePtr> values = {parse_expression()};
    while(peek().type == TokenType::comma){
        advance(); // on comma
        advance(); // after the comma
        values.push_back(parse_expression());
    }
    return std::make_shared<AugAssignStmt>(op_tok, op_tok, targets, values);
}

AstNodePtr Parser::parse_func_def(std::vector<Annotation> annotations, bool is_pub){
    const Token tok = this->curr_tok; // 'fn' token
    bool is_compile_time = false;
    if(peek().type == TokenType::dollar){
        is_compile_time = true;
        advance(); // on '$'
    }
    expect(TokenType::identifier, "Expected function name after 'fn' keyword");
    Token name = this->curr_tok;
    std::vector<std::pair<Token, AstNodePtr>> generics;
    if(peek().type == TokenType::lbrace){
        advance(); // on '{'
        generics = parse_generic_params();
    }
    expect(TokenType::lparen, "Expected '(' at the beginning of parameter list in function definition");
    std::vector<Parameter> parameters;
    if(peek().type != TokenType::rparen){
        while(true){
            advance(); // on the first token of the parameter
            parameters.push_back(parse_parameter());
            if(peek().type == TokenType::comma){
                advance(); // on comma
                if(peek().type == TokenType::rparen){
                    advance();
                    break;
                }
            }
            else if(peek().type == TokenType::rparen){
                advance();
                break;
            }
            else{
                error(peek(), "Expected ',' or ')' in parameter list of function definition");
            }
        }
    }
    else{
        advance(); // on ')'
    }
    AstNodePtr return_type = std::make_shared<NoLiteral>();
    if(peek().type == TokenType::arrow){
        advance(); // on '->'
        advance(); // on the first token of the return type expression
        return_type = parse_type_expr();
    }
    AstNodePtr body = std::make_shared<NoLiteral>();
    if(this->peek().type == TokenType::lbrace){//Else it is forward declaration
        advance(); // on '{'
        body = parse_block();
    }
    AstNodePtr func = std::make_shared<FuncDefStmt>(tok, is_pub, name, generics, parameters, return_type, body, annotations);
    if(is_compile_time){
        return std::make_shared<CompTimeExpr>(tok, func);
    }
    else{
        return func;
    }
}

AstNodePtr Parser::parse_method_def(std::vector<Annotation> annotations, bool is_pub){
    std::vector<Attribute> attributes;
    for(const auto& ann : annotations){
        if(ann.is_decorator){
            error(ann.decorator.decorator->token(), "Decorators are not allowed on method definitions");
        }
        else{
            attributes.push_back(ann.attribute);
        }
    }
    Token tok = this->curr_tok; // 'fn' token
    expect(TokenType::lparen, "Expected '(' after 'fn' keyword for method definition");
    advance(); // after '('
    Parameter receiver = parse_parameter();
    expect(TokenType::rparen, "Expected ')' after method receiver in method definition");
    bool is_compile_time = false;
    if(peek().type == TokenType::dollar){
        is_compile_time = true;
        advance(); // on '$'
    }
    expect(TokenType::identifier, "Expected method name after method receiver");
    Token name = this->curr_tok;
    std::vector<std::pair<Token, AstNodePtr>> generics;
    if(peek().type == TokenType::lbrace){
        advance(); // on '{'
        generics = parse_generic_params();
    }
    expect(TokenType::lparen, "Expected '(' at the beginning of parameter list in method definition");
    std::vector<Parameter> parameters;
    if(peek().type != TokenType::rparen){
        while(true){
            advance(); // on the first token of the parameter
            parameters.push_back(parse_parameter());
            if(peek().type == TokenType::comma){
                advance(); // on comma
                if(peek().type == TokenType::rparen){
                    advance();
                    break;
                }
            }
            else if(peek().type == TokenType::rparen){
                advance();
                break;
            }
            else{
                error(peek(), "Expected ',' or ')' in parameter list of function definition");
            }
        }
    }
    else{
        advance(); // on ')'
    }
    AstNodePtr return_type = std::make_shared<NoLiteral>();
    if(peek().type == TokenType::arrow){
        advance(); // on '->'
        advance(); // on the first token of the return type expression
        return_type = parse_type_expr();
    }
    AstNodePtr body = std::make_shared<NoLiteral>();
    if(this->peek().type == TokenType::lbrace){//Else it is forward declaration
        advance(); // on '{'
        body = parse_block();
    }
    AstNodePtr method = std::make_shared<MethodDefStmt>(tok, is_pub, receiver, name, generics, parameters, return_type, body, attributes);
    if(is_compile_time){
        return std::make_shared<CompTimeExpr>(tok, method);
    }
    else{
        return method;
    }
}

AstNodePtr Parser::parse_func_or_method_def(std::vector<Annotation> annotations, bool is_pub){
    if(peek().type == TokenType::lparen){
        return parse_method_def(annotations, is_pub);
    }
    else{
        return parse_func_def(annotations, is_pub);
    }
}
}