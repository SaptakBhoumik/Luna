#include "ast/ast.hpp"
#include <cstddef>

namespace Luna{
DeferStmt::DeferStmt(Token tok, AstNodePtr body){
    this->tok = tok;
    this->body = body;
}

AstNodePtr DeferStmt::get_body() const {
    return this->body;
}

Token DeferStmt::token() const {
    return this->tok;
}
AstKind DeferStmt::kind() const {
    return AstKind::DeferStmt;
}
std::string DeferStmt::stringify() const {
    if(this->body->kind() == AstKind::Block){
        return "defer {\n" + this->body->stringify() + "\n}";
    } 
    else {
        return "defer " + this->body->stringify();
    }
}


ScopeStmt::ScopeStmt(Token tok, AstNodePtr body){
    this->tok = tok;
    this->body = body;
}

AstNodePtr ScopeStmt::get_body() const {
    return this->body;
}

Token ScopeStmt::token() const {
    return this->tok;
}
AstKind ScopeStmt::kind() const {
    return AstKind::ScopeStmt;
}
std::string ScopeStmt::stringify() const {
    return "{\n" + this->body->stringify() + "\n}";
}


BreakStmt::BreakStmt(Token tok){
    this->tok = tok;
}

Token BreakStmt::token() const {
    return this->tok;
}
AstKind BreakStmt::kind() const {
    return AstKind::BreakStmt;
}
std::string BreakStmt::stringify() const {
    return "break";
}


ContinueStmt::ContinueStmt(Token tok){
    this->tok = tok;
}

Token ContinueStmt::token() const {
    return this->tok;
}
AstKind ContinueStmt::kind() const {
    return AstKind::ContinueStmt;
}
std::string ContinueStmt::stringify() const {
    return "continue";
}


ReturnStmt::ReturnStmt(Token tok, std::vector<AstNodePtr> values){
    this->tok = tok;
    this->values = values;
}

std::vector<AstNodePtr> ReturnStmt::get_values() const {
    return this->values;
}

Token ReturnStmt::token() const {
    return this->tok;
}
AstKind ReturnStmt::kind() const {
    return AstKind::ReturnStmt;
}
std::string ReturnStmt::stringify() const {
    if (this->values.empty()){
        return "ret";
    }
    else {
        std::string result = "ret ";
        for(size_t i=0; i<this->values.size(); i++){
            result += this->values[i]->stringify();
            if(i != this->values.size() - 1){
                result += ", ";
            }
        }
        return result;
            
    }
}


RaiseStmt::RaiseStmt(Token tok, AstNodePtr value){
    this->tok = tok;
    this->value = value;
}

AstNodePtr RaiseStmt::get_value() const {
    return this->value;
}

Token RaiseStmt::token() const {
    return this->tok;
}
AstKind RaiseStmt::kind() const {
    return AstKind::RaiseStmt;
}
std::string RaiseStmt::stringify() const {
    return "raise " + this->value->stringify();
}

GiveStmt::GiveStmt(Token tok, std::vector<AstNodePtr> values){
    this->tok = tok;
    this->values = values;
}

std::vector<AstNodePtr> GiveStmt::get_values() const {
    return this->values;
}

Token GiveStmt::token() const {
    return this->tok;
}
AstKind GiveStmt::kind() const {
    return AstKind::GiveStmt;
}
std::string GiveStmt::stringify() const {
    if(this->values.empty()){
        return "give";
    }
    else{
        std::string result = "give ";
        for(size_t i=0; i<this->values.size(); i++){
            result += this->values[i]->stringify();
            if(i != this->values.size() - 1){
                result += ", ";
            }
        }
        return result;
    }
}


LockStmt::LockStmt(Token tok, std::vector<AstNodePtr> target, AstNodePtr body){
    this->tok = tok;
    this->target = target;
    this->body = body;
}

std::vector<AstNodePtr> LockStmt::get_target() const {
    return this->target;
}
AstNodePtr LockStmt::get_body() const {
    return this->body;
}

Token LockStmt::token() const {
    return this->tok;
}
AstKind LockStmt::kind() const {
    return AstKind::LockStmt;
}
std::string LockStmt::stringify() const {
    std::string target_str = "";
    for(size_t i=0; i<this->target.size(); i++){
        target_str += this->target[i]->stringify();
        if(i != this->target.size() - 1){
            target_str += ", ";
        }
    }
    return "lock " + target_str + " {\n" + this->body->stringify() + "\n}";
}
}