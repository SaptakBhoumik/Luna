#include "ast/ast.hpp"
#include "ast/ast_utils.hpp"
#include "lexer/token.hpp"
#include "parser/parser.hpp"
#include <iostream>
#include <vector>

namespace Luna {
AstNodePtr Parser::parse_type_expr(bool can_be_sumtype){
    const Token tok = this->curr_tok;
    AstNodePtr type_expr;
    switch(this->curr_tok.type){
        case TokenType::identifier:{
            type_expr = parse_identifier_type_expr();
            break;
        }
        case TokenType::lbracket:{
            type_expr = parse_list_type_expr();
            break;
        }
        case TokenType::question:
        case TokenType::bang:
        case TokenType::star:{
            type_expr = parse_ptr_optional_error_type_expr();
            break;
        }
        case TokenType::kw_fn:{
            type_expr = parse_func_type_expr();
            break;
        }
        case TokenType::lparen:{
            type_expr = parse_tuple_or_paren_type_expr();
            break;
        }
        case TokenType::lt:{
            type_expr = parse_simd_type_expr();
            break;
        }
        case TokenType::kw_enum:{
            type_expr = parse_enum_type_expr();
            break;
        }
        case TokenType::lbrace:{
            type_expr = parse_struct_type_expr();
            break;
        }
        default:{
            error(this->curr_tok, "Unexpected token in type expression");
        }
    }
    if(peek().type==TokenType::pipe && can_be_sumtype){
        advance();
        std::vector<AstNodePtr> sum_types = {type_expr};
        while(this->curr_tok.type==TokenType::pipe){
            advance();
            sum_types.push_back(parse_type_expr(false));
            if(peek().type==TokenType::pipe){
                advance();
            }
        } 
        type_expr = std::make_shared<SumTypeExpr>(tok,sum_types);   
    }
    return type_expr;
}
AstNodePtr Parser::parse_list_type_expr(){
    const Token list_tok = this->curr_tok;
    advance();//After [
    AstNodePtr elem_type = parse_type_expr();
    AstNodePtr size = std::make_shared<NoLiteral>();
    if(peek().type != TokenType::rbracket){
        expect(TokenType::comma, "Expected ',' after element type in list type expression");
        advance();//After ,
        size = parse_expression();
    }
    expect(TokenType::rbracket, "Expected ']' at the end of list type expression");
    return std::make_shared<ListTypeExpr>(list_tok, elem_type, size);
}
AstNodePtr Parser::parse_ptr_optional_error_type_expr(){
    const Token tok = this->curr_tok;
    advance();//On the token after ?, !, or *
    AstNodePtr base_type = parse_type_expr(false);
    if(tok.type == TokenType::question){
        return std::make_shared<OptionalTypeExpr>(tok, base_type);
    }
    else if(tok.type == TokenType::bang){
        return std::make_shared<ErrorTypeExpr>(tok, base_type);
    }
    else if(tok.type == TokenType::star){//TokenType::star
        return std::make_shared<PtrTypeExpr>(tok, base_type);
    }
    else{
        std::cout << "Internal parser error: unexpected token type in parse_ptr_optional_error_type_expr: " << to_string(tok.type) 
                  << "\nPlease create an issue on the github repository." << std::endl;
        exit(1);
    }
}
AstNodePtr Parser::parse_func_type_expr(){
    const Token fn_tok = this->curr_tok;
    expect(TokenType::lparen, "Expected '(' after 'fn' in function type expression");
    std::vector<AstNodePtr> param_types;
    bool c_variadic = false;
    while(peek().type != TokenType::rparen){
        advance();//On the first token of the parameter type or the token after ,
        if(this->curr_tok.type == TokenType::ellipsis){
            c_variadic = true;
            if(peek().type != TokenType::rparen){
                error(peek(), "Expected ')' after '...' in function type expression", 
                "C-variadic '...' must be at the end of the parameter list");
            }
            break;
        }
        param_types.push_back(parse_type_expr());
        if(peek().type == TokenType::comma){
            advance();//On ,
        }
        else if(peek().type != TokenType::rparen){
            error(peek(), "Expected ',' or ')' after parameter type in function type expression",
            "Parameters in function type expressions must be separated by ','");
        }
    }
    expect(TokenType::rparen, "Expected ')' after parameter types in function type expression");
    AstNodePtr return_type = std::make_shared<NoLiteral>();
    if(peek().type == TokenType::arrow){
        advance();//On ->
        advance();//On the first token of the return type
        return_type = parse_type_expr();
    }
    return std::make_shared<FuncTypeExpr>(fn_tok, param_types, c_variadic, return_type);
}

AstNodePtr Parser::parse_tuple_or_paren_type_expr(){
    //Note:- () is not allowed cuz not makes sense
    const Token tok = this->curr_tok;
    advance(); // consume '('
    std::vector<AstNodePtr> elements;
    elements.push_back(parse_type_expr());
    if(peek().type == TokenType::comma){
        // tuple literal
        while(peek().type == TokenType::comma){
            advance(); // on ,
            if(peek().type == TokenType::rparen){
                break;
            }
            advance(); // On the element after ,
            elements.push_back(parse_type_expr());
        }
        advance(); // consume ')'
        return std::make_shared<TupleTypeExpr>(tok,elements);
    }
    else{
        // parenthesized expression
        expect(TokenType::rparen,"expected ')' after parenthesized expression");
        return elements[0];
    }
}

AstNodePtr Parser::parse_simd_type_expr(){
    const Token simd_tok = this->curr_tok;
    advance();
    AstNodePtr elem_type = parse_type_expr();
    expect(TokenType::comma, "Expected ',' after element type in SIMD type expression");
    advance();//After ,
    AstNodePtr size;
    if(this->curr_tok.type == TokenType::dollar){
        const auto size_tok = this->curr_tok;
        expect(TokenType::identifier, "Expected identifier after '$' in SIMD type expression");
        auto size_identifier = parse_identifier();
        size = std::make_shared<CompTimeExpr>(size_tok, size_identifier);
    }
    else{
        if(this->curr_tok.type != TokenType::integer){
            error(this->curr_tok, "Expected integer literal or compile time variable after ',' in SIMD type expression");
        }
        size = parse_int();
    }
    expect(TokenType::gt, "Expected '>' at the end of SIMD type expression");
    return std::make_shared<SimdTypeExpr>(simd_tok, elem_type, size);
}

AstNodePtr Parser::parse_enum_type_expr(){
    const Token enum_tok = this->curr_tok;
    AstNodePtr base_type = std::make_shared<NoLiteral>();
    if(peek().type == TokenType::colon){
        advance();//On :
        advance();//On the first token of the base type
        base_type = parse_type_expr();
    }
    if(peek().type == TokenType::newline){
        error(peek(), "Unexpected newline before '{' in enum type expression", 
        "In Luna, you cant enter new line before the '{' token in any kind of statement. This is the make the code more readable and uniform");
    }
    expect(TokenType::lbrace, "Expected '{' at the beginning of enum type expression");
    std::vector<std::pair<Token, AstNodePtr>> variants; // (name, value)
    while(peek().type != TokenType::rbrace){
        advance_on_newline();//On '{' or on newline after '{' or on , between variants
        if(peek().type == TokenType::rbrace){
            break;
        }
        expect(TokenType::identifier, "Expected identifier for enum variant name in enum type expression");
        Token name = this->curr_tok;
        advance_on_newline();//On the identifier or newline after the identifier
        AstNodePtr value = std::make_shared<NoLiteral>();
        if(peek().type == TokenType::assign){
            advance();//On =
            advance_on_newline();
            advance();
            value = parse_expression();
        }
        advance_on_newline();
        if(peek().type == TokenType::comma){
            advance();//On ,
        }
        else if(peek().type != TokenType::rbrace){
            error(peek(), "Expected ',' or '}' after enum variant in enum type expression");
        }
        variants.push_back(std::make_pair(name, value));
    }
    expect(TokenType::rbrace, "Expected '}' at the end of enum type expression");
    return std::make_shared<EnumTypeExpr>(enum_tok, base_type, variants);
}

AstNodePtr Parser::parse_struct_type_expr(){
    std::vector<StructField> fields;
    const Token struct_tok = this->curr_tok;
    while(peek().type != TokenType::rbrace){
        advance_on_newline();//On '{' or on newline after '{' or on , between fields
        if(peek().type == TokenType::rbrace){
            break;
        }
        advance(); 
        fields.push_back(parse_struct_field());
        advance_on_newline();
        if(peek().type == TokenType::comma){
            advance();//On ,
        }
        else if(peek().type != TokenType::rbrace){
            error(peek(), "Expected ',' or '}' after struct field in struct type expression");
        }
    }
    expect(TokenType::rbrace, "Expected '}' at the end of struct type expression");
    return std::make_shared<StructTypeExpr>(struct_tok, fields);
}
}