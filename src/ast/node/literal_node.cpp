#include "ast/ast.hpp"

namespace Luna{
IntegerLiteral::IntegerLiteral(Token tok){
    this->tok = tok;
}

std::string IntegerLiteral::get_value() const{
    return this->tok.value;
}

Token IntegerLiteral::token() const{
    return this->tok;
}
AstKind IntegerLiteral::kind() const{
    return AstKind::Integer;
}
std::string IntegerLiteral::stringify() const{
    return this->tok.value;
}


DecimalLiteral::DecimalLiteral(Token tok){
    this->tok = tok;
}

std::string DecimalLiteral::get_value() const{
    return this->tok.value;
}

Token DecimalLiteral::token() const{
    return this->tok;
}
AstKind DecimalLiteral::kind() const{
    return AstKind::Decimal;
}
std::string DecimalLiteral::stringify() const{
    return this->tok.value;
}


StringLiteral::StringLiteral(Token tok,bool raw){
    this->tok = tok;
    this->raw = raw;
}

std::string StringLiteral::get_value() const{
    return this->tok.value;
}
bool StringLiteral::is_raw() const{
    return this->raw;
}

Token StringLiteral::token() const{
    return this->tok;
}
AstKind StringLiteral::kind() const{
    return AstKind::String;
}
std::string StringLiteral::stringify() const{
    std::string res = this->tok.value;
    if(this->raw){
        return "r\"" + res + "\"";
    } 
    else {
        return "\"" + res + "\"";
    }
}


BoolLiteral::BoolLiteral(Token tok){
    this->tok = tok;
}

bool BoolLiteral::get_value() const{
    return this->tok.value == "True";
}

Token BoolLiteral::token() const{
    return this->tok;
}
AstKind BoolLiteral::kind() const{
    return AstKind::Bool;
}
std::string BoolLiteral::stringify() const{
    return this->tok.value;
}


NoneLiteral::NoneLiteral(Token tok){
    this->tok = tok;
}

Token NoneLiteral::token() const{
    return this->tok;
}
AstKind NoneLiteral::kind() const{
    return AstKind::None;
}
std::string NoneLiteral::stringify() const{
    return "None";
}

IdentifierExpression::IdentifierExpression(Token tok, std::vector<std::string> path, std::vector<AstNodePtr> generic_args){
    this->tok = tok;
    this->path = path;
    this->generic_args = generic_args;
}

std::vector<std::string> IdentifierExpression::get_path() const{
    return this->path;
}
std::vector<AstNodePtr> IdentifierExpression::get_generic_args() const{
    return this->generic_args;
}

Token IdentifierExpression::token() const{
    return this->tok;
}
AstKind IdentifierExpression::kind() const{
    return AstKind::Identifier;
}
std::string IdentifierExpression::stringify() const{
    std::string res;
    for(size_t i = 0; i < this->path.size(); i++){
        res += this->path[i];
        if(i != this->path.size() - 1){
            res += "::";
        }
    }
    if(!this->generic_args.empty()){
        res += "{";
        for(size_t i = 0; i < this->generic_args.size(); i++){
            res += this->generic_args[i]->stringify();
            if(i != this->generic_args.size() - 1){
                res += ", ";
            }
        }
        res += "}";
    }
    return res;
}
}