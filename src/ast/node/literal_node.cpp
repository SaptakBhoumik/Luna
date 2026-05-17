#include "ast/ast.hpp"
#include "lexer/token.hpp"

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
    return AstKind::IntegerLiteral;
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
    return AstKind::DecimalLiteral;
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
    return AstKind::StringLiteral;
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
    return AstKind::BoolLiteral;
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
    return AstKind::NoneLiteral;
}
std::string NoneLiteral::stringify() const{
    return "None";
}

IdentifierLiteral::IdentifierLiteral(Token tok, std::vector<Token> path, bool compile_time, std::vector<AstNodePtr> generic_args){
    this->tok = tok;
    this->path = path;
    this->compile_time = compile_time;
    this->generic_args = generic_args;
}

std::vector<Token> IdentifierLiteral::get_path() const{
    return this->path;
}
bool IdentifierLiteral::is_compile_time() const{
    return this->compile_time;
}
std::vector<AstNodePtr> IdentifierLiteral::get_generic_args() const{
    return this->generic_args;
}

Token IdentifierLiteral::token() const{
    return this->tok;
}
AstKind IdentifierLiteral::kind() const{
    return AstKind::IdentifierLiteral;
}
std::string IdentifierLiteral::stringify() const{
    std::string res;
    for(size_t i = 0; i < this->path.size(); i++){
        if(i == this->path.size() - 1 && this->compile_time){
            res += "$";
        }
        res += this->path[i].value;
        if(i != this->path.size() - 1){
            res += "::";
        }
    }
    if(!this->generic_args.empty()){
        res += "::<";
        for(size_t i = 0; i < this->generic_args.size(); i++){
            res += this->generic_args[i]->stringify();
            if(i != this->generic_args.size() - 1){
                res += ", ";
            }
        }
        res += ">";
    }
    return res;
}


ListLiteral::ListLiteral(Token tok, std::vector<AstNodePtr> elements){
    this->tok = tok;
    this->elements = elements;
}

std::vector<AstNodePtr> ListLiteral::get_elements() const{
    return this->elements;
}

Token ListLiteral::token() const{
    return this->tok;
}
AstKind ListLiteral::kind() const{
    return AstKind::ListLiteral;
}
std::string ListLiteral::stringify() const{
    std::string res = "[";
    for(size_t i = 0; i < this->elements.size(); i++){
        res += this->elements[i]->stringify();
        if(i != this->elements.size() - 1){
            res += ", ";
        }
    }
    res += "]";
    return res;
}


DictLiteral::DictLiteral(Token tok, std::vector<std::pair<AstNodePtr, AstNodePtr>> elements){
    this->tok = tok;
    this->elements = elements;
}

std::vector<std::pair<AstNodePtr, AstNodePtr>> DictLiteral::get_elements() const{
    return this->elements;
}

Token DictLiteral::token() const{
    return this->tok;
}
AstKind DictLiteral::kind() const{
    return AstKind::DictLiteral;
}
std::string DictLiteral::stringify() const{
    std::string res = "{";
    for(size_t i = 0; i < this->elements.size(); i++){
        res += this->elements[i].first->stringify() + ": " + this->elements[i].second->stringify();
        if(i != this->elements.size() - 1){
            res += ", ";
        }
    }
    res += "}";
    return res;
}

EmptyDictOrListLiteral::EmptyDictOrListLiteral(Token tok){
    this->tok = tok;
}

Token EmptyDictOrListLiteral::token() const{
    return this->tok;
}
AstKind EmptyDictOrListLiteral::kind() const{
    return AstKind::EmptyDictOrListLiteral;
}
std::string EmptyDictOrListLiteral::stringify() const{
    return "[]";
}

TupleLiteral::TupleLiteral(Token tok, std::vector<AstNodePtr> elements){
    this->tok = tok;
    this->elements = elements;
}
std::vector<AstNodePtr> TupleLiteral::get_elements() const{
    return this->elements;
}

Token TupleLiteral::token() const{
    return this->tok;
}
AstKind TupleLiteral::kind() const{
    return AstKind::TupleLiteral;
}
std::string TupleLiteral::stringify() const{
    std::string res = "(";
    for(size_t i = 0; i < this->elements.size(); i++){
        res += this->elements[i]->stringify();
        if(i != this->elements.size() - 1){
            res += ", ";
        }
    }
    res += ")";
    return res;
}


AssignTupleLiteral::AssignTupleLiteral(Token tok, std::vector<AstNodePtr> elements, std::vector<std::pair<bool, bool>> is_pub_mut){
    this->tok = tok;
    this->elements = elements;
    this->is_pub_mut = is_pub_mut;
}

std::vector<AstNodePtr> AssignTupleLiteral::get_elements() const{
    return this->elements;
}
std::vector<std::pair<bool, bool>> AssignTupleLiteral::get_is_pub_mut() const{
    return this->is_pub_mut;
}

Token AssignTupleLiteral::token() const{
    return this->tok;
}
AstKind AssignTupleLiteral::kind() const{
    return AstKind::AssignTupleLiteral;
}
std::string AssignTupleLiteral::stringify() const{
    std::string res = "(";
    for(size_t i = 0; i < this->elements.size(); i++){
        if(this->is_pub_mut[i].first){
            res += "pub ";
        }
        if(this->is_pub_mut[i].second){
            res += "mut ";
        }
        res += this->elements[i]->stringify();
        if(i != this->elements.size() - 1){
            res += ", ";
        }
    }
    res += ")";
    return res;
}
}