#include "ast/ast.hpp"

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


ReturnStmt::ReturnStmt(Token tok, AstNodePtr value){
    this->tok = tok;
    this->value = value;
}

AstNodePtr ReturnStmt::get_value() const {
    return this->value;
}

Token ReturnStmt::token() const {
    return this->tok;
}
AstKind ReturnStmt::kind() const {
    return AstKind::ReturnStmt;
}
std::string ReturnStmt::stringify() const {
    if (this->value->kind() == AstKind::NoLiteral){
        return "ret";
    }
    else {
        return "ret " + this->value->stringify();
    }
}


GiveStmt::GiveStmt(Token tok, AstNodePtr value){
    this->tok = tok;
    this->value = value;
}

AstNodePtr GiveStmt::get_value() const {
    return this->value;
}

Token GiveStmt::token() const {
    return this->tok;
}
AstKind GiveStmt::kind() const {
    return AstKind::GiveStmt;
}
std::string GiveStmt::stringify() const {
    return "give " + this->value->stringify();
}


LockStmt::LockStmt(Token tok, AstNodePtr target, AstNodePtr body){
    this->tok = tok;
    this->target = target;
    this->body = body;
}

AstNodePtr LockStmt::get_target() const {
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
    return "lock " + this->target->stringify() + " {\n" + this->body->stringify() + "\n}";
}
}