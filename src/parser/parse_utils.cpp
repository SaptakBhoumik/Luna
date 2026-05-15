#include "ast/ast.hpp"
#include "ast/ast_utils.hpp"
#include "lexer/token.hpp"
#include "parser/parser.hpp"
#include <iostream>
#include <vector>

namespace Luna {
Attribute Parser::parse_attribute(){
    expect(TokenType::lbracket, "Expected '[' after '@' for attributel");
    expect(TokenType::identifier, "Expected identifier for attribute name after '['");
    Token name = this->curr_tok;
    std::vector<AstNodePtr> args;
    std::vector<std::pair<Token, AstNodePtr>> named_args;
    if(peek().type == TokenType::lparen){
        advance(); // consume '('
        while(peek().type != TokenType::rparen){
            advance();//On the first token of the argument
            if(peek().type == TokenType::assign){
                // named argument
                Token arg_name = this->curr_tok;
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

StructField Parser::parse_struct_field(){
    std::vector<Attribute> attributes;
    if(this->curr_tok.type == TokenType::at){
        while(this->curr_tok.type == TokenType::at){
            attributes.push_back(parse_attribute()); // we can ignore the parsed attribute here because we will parse the attributes of the struct field again when we are parsing the struct type expression. We just need to advance the tokens here.
            advance_on_newline();
            advance();
        }
    }
    bool is_pub = false;
    bool is_mut = false;
    if(this->curr_tok.type == TokenType::kw_pub){
        is_pub = true;
        // advance_on_newline();
        advance();
    }
    if(this->curr_tok.type == TokenType::kw_mut){
        is_mut = true;
        // advance_on_newline();
        advance();
    }
    if(this->curr_tok.type != TokenType::identifier){
        error(this->curr_tok, "Expected identifier for struct field name");
    }
    Token name = this->curr_tok;
    // advance_on_newline();
    advance();
    AstNodePtr type = std::make_shared<NoLiteral>();
    AstNodePtr default_value = std::make_shared<NoLiteral>();
    if(this->curr_tok.type == TokenType::colon){
        advance(); // after ':'
        type = parse_type_expr();
        if (peek().type == TokenType::assign){
            advance(); // on '='
            advance(); // on the first token of the default value expression
            default_value = parse_expression();
        }
    }
    else if(this->curr_tok.type == TokenType::walrus){
        advance(); // after ':='
        type = std::make_shared<NoLiteral>();
        default_value = parse_expression();
    }
    else{
        error(this->curr_tok, "Expected ':' or ':=' after struct field name");
    }
    return StructField(name, type, default_value, is_pub, is_mut, attributes);
}

std::vector<Token> Parser::parse_path(){
    std::vector<Token> path = {this->curr_tok};
    // if(peek().type == TokenType::double_colon){
        // parse path like A::B::C
    while(peek().type == TokenType::double_colon && peek(2).type == TokenType::identifier){
        advance(); // On double colon
        expect(TokenType::identifier,"expected identifier after '::' in path");
        path.push_back(this->curr_tok);
    }
    // }
    return path;
}

Parameter Parser::parse_parameter(){
    if(this->curr_tok.type == TokenType::ellipsis){//ellipsis cant be mut
        advance();
        return Parameter{std::make_shared<NoLiteral>(), Token{}, std::make_shared<NoLiteral>(), false, ParamKind::CVariadic};
    }
    bool is_mut = false;
    if(this->curr_tok.type == TokenType::kw_mut){
        is_mut = true;
        advance();
    }
    ParamKind kind = ParamKind::Normal;
    if(this->curr_tok.type == TokenType::star){
        kind = ParamKind::VarArg;
        if(peek().type == TokenType::dollar){
            advance();
            kind = ParamKind::CompileTimeVarArg;
        }
        expect(TokenType::identifier, "Expected identifier after '*' for variadic parameter");
    }
    else if(this->curr_tok.type == TokenType::pow){
        kind = ParamKind::KwVararg;
        if(peek().type == TokenType::dollar){
            advance();
            kind = ParamKind::CompileTimeKwVararg;
        }
        expect(TokenType::identifier, "Expected identifier after '**' for compile-time keyword variadic parameter");
    }
    else if(this->curr_tok.type == TokenType::dollar){
        kind = ParamKind::CompileTime;
        expect(TokenType::identifier, "Expected identifier after '$' for compile-time parameter");
    }
    else if(this->curr_tok.type != TokenType::identifier){
        error(this->curr_tok, "Expected identifier for parameter name");
    }
    Token name = this->curr_tok;
    AstNodePtr type = std::make_shared<NoLiteral>();
    AstNodePtr default_value = std::make_shared<NoLiteral>();
    if(peek().type == TokenType::colon){
        advance(); // on ':'
        advance(); // on the first token of the type expression
        type = parse_type_expr();
        if(peek().type == TokenType::assign){
            advance(); // on '='
            advance(); // on the first token of the default value expression
            default_value = parse_expression();
        }
    }
    else if(peek().type == TokenType::walrus){
        advance(); // on ':='
        advance(); // on the first token of the default value expression
        default_value = parse_expression();
    }
    return Parameter{type, name, default_value, is_mut, kind};
}

CaptureClause Parser::parse_capture_clause(){
    if(peek().type == TokenType::rbracket){
        advance();
        return CaptureClause{CaptureKind::None, {}};
    }
    else if(peek().type == TokenType::ampersand && peek(2).type == TokenType::rbracket){
        advance(); // on '&'
        advance(); // on ']'
        return CaptureClause{CaptureKind::AllRef, {}};
    }
    else if(peek().type == TokenType::assign){
        advance(); // on '='
        expect(TokenType::rbracket, "Expected ']' after '=' in lambda capture clause");
        return CaptureClause{CaptureKind::AllCopy, {}};
    }
    else{
        std::vector<CaptureEntry> entries;
        while(true){
            bool by_ref = false;
            if(peek().type == TokenType::ampersand){
                by_ref = true;
                advance();
            }
            expect(TokenType::identifier, "Expected identifier in lambda capture clause");
            entries.push_back(CaptureEntry{by_ref, parse_identifier()});
            if(peek().type == TokenType::comma){
                advance();
                if(peek().type == TokenType::rbracket){
                    advance();
                    break;
                }
            }
            else if(peek().type == TokenType::rbracket){
                advance();
                break;
            }
            else{
                error(peek(), "Expected ',' or ']' in lambda capture clause");
            }
        }
        return CaptureClause{CaptureKind::List, entries};
    }
}

LambdaFuncSignature Parser::parse_lambda_signature(){
    expect(TokenType::lparen, "Expected '(' at the beginning of lambda function signature");
    std::vector<Parameter> parameters;
    if(peek().type != TokenType::rparen){
        while(true){
            advance(); // on the first token of the parameter
            parameters.push_back(parse_parameter());
            if(peek().type == TokenType::comma){
                advance();
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
                error(peek(), "Expected ',' or ')' in lambda function signature");
            }
        }
    }
    CaptureClause capture = CaptureClause{CaptureKind::None, {}};
    if(peek().type == TokenType::lbracket){
        advance(); // on '['
        capture = parse_capture_clause();
    }
    AstNodePtr return_type = std::make_shared<NoLiteral>();
    if(peek().type == TokenType::arrow){
        advance(); // on '->'
        advance(); // on the first token of the return type expression
        return_type = parse_type_expr();
    }
    return LambdaFuncSignature{parameters, capture, return_type};
}
}