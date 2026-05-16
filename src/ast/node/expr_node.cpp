#include "ast/ast.hpp"

namespace Luna{
BinOp::BinOp(Token tok, AstNodePtr left, Token op, AstNodePtr right){
    this->tok = tok;
    this->left = left;
    this->op = op;
    this->right = right;
}

AstNodePtr BinOp::get_left() const {
    return this->left;
}
Token BinOp::get_op() const {
    return this->op;
}
AstNodePtr BinOp::get_right() const {
    return this->right;
}

Token BinOp::token() const {
    return this->tok;
}

AstKind BinOp::kind() const {
    return AstKind::BinOp;
}

std::string BinOp::stringify() const {
    return "(" + this->left->stringify() + " " + this->op.value + " " + this->right->stringify() + ")";
}


PrefixOp::PrefixOp(Token tok, Token prefix, AstNodePtr right){
    this->tok = tok;
    this->prefix = prefix;
    this->right = right;
}

Token PrefixOp::get_prefix() const {
    return this->prefix;
}
AstNodePtr PrefixOp::get_right() const {
    return this->right;
}

Token PrefixOp::token() const {
    return this->tok;
}
AstKind PrefixOp::kind() const {
    return AstKind::PrefixOp;
}
std::string PrefixOp::stringify() const {
    return "(" + this->prefix.value + this->right->stringify() + ")";
}


PostfixOp::PostfixOp(Token tok, Token postfix, AstNodePtr left){
    this->tok = tok;
    this->postfix = postfix;
    this->left = left;
}

Token PostfixOp::get_postfix() const {
    return this->postfix;
}
AstNodePtr PostfixOp::get_left() const {
    return this->left;
}

Token PostfixOp::token() const {
    return this->tok;
}
AstKind PostfixOp::kind() const {
    return AstKind::PostfixOp;
}
std::string PostfixOp::stringify() const {
    return "(" + this->left->stringify() + this->postfix.value + ")";
}


CoalescingOP::CoalescingOP(Token tok, AstNodePtr left, AstNodePtr right, bool null_coalescing, std::optional<std::pair<Token,bool>> error_var_name){
    this->tok = tok;
    this->left = left;
    this->right = right;
    this->null_coalescing = null_coalescing;
    this->error_var_name = error_var_name;
}

AstNodePtr CoalescingOP::get_left() const {
    return this->left;
}
AstNodePtr CoalescingOP::get_right() const {
    return this->right;
}
bool CoalescingOP::is_null_coalescing() const {
    return this->null_coalescing;
}
std::optional<std::pair<Token,bool>> CoalescingOP::get_error_var_name() const {
    return this->error_var_name;
}

Token CoalescingOP::token() const {
    return this->tok;
}
AstKind CoalescingOP::kind() const {
    return AstKind::CoalescingOP;
}
std::string CoalescingOP::stringify() const {
    std::string res = this->left->stringify() + (this->null_coalescing ? " ?? " : " !! ");
    if(this->error_var_name.has_value()){
        res += "(" + this->error_var_name->first.value + (this->error_var_name->second ? ": mut" : "") + ")";
    }
    if(this->right->kind() == AstKind::Block){
        res += "{\n" + this->right->stringify() + "\n}";
    }
    else{
        res += this->right->stringify();
    }
    return res;
}


RangeExpr::RangeExpr(Token tok, AstNodePtr start, AstNodePtr end, AstNodePtr step){
    this->tok = tok;
    this->start = start;
    this->end = end;
    this->step = step;
}

AstNodePtr RangeExpr::get_start() const {
    return this->start;
}
AstNodePtr RangeExpr::get_end() const {
    return this->end;
}
AstNodePtr RangeExpr::get_step() const {
    return this->step;
}

Token RangeExpr::token() const {
    return this->tok;
}
AstKind RangeExpr::kind() const {
    return AstKind::RangeExpr;
}
std::string RangeExpr::stringify() const {
    std::string res = this->start->stringify() + ".." + this->end->stringify();
    if(this->step->kind() != AstKind::NoLiteral){
        res += ".." + this->step->stringify();
    }
    return res;
}


IndexExpr::IndexExpr(Token tok, AstNodePtr container,std::vector<AstNodePtr> indices){
    this->tok = tok;
    this->container = container;
    this->indices = indices;
}

AstNodePtr IndexExpr::get_container() const {
    return this->container;
}
std::vector<AstNodePtr> IndexExpr::get_indices() const {
    return this->indices;
}

Token IndexExpr::token() const {
    return this->tok;
}
AstKind IndexExpr::kind() const {
    return AstKind::IndexExpr;
}
std::string IndexExpr::stringify() const {
    std::string res = this->container->stringify() + "[";
    for(size_t i = 0; i < this->indices.size(); i++){
        res += this->indices[i]->stringify();
        if(i != this->indices.size() - 1){
            res += ", ";
        }
    }
    res += "]";
    return res;
}


DotExpr::DotExpr(Token tok, AstNodePtr owner, std::pair<Token, bool> member){
    this->tok = tok;
    this->owner = owner;
    this->member = member;
}

AstNodePtr DotExpr::get_owner() const {
    return this->owner;
}
std::pair<Token, bool> DotExpr::get_member() const {
    return this->member;
}

Token DotExpr::token() const {
    return this->tok;
}
AstKind DotExpr::kind() const {
    return AstKind::DotExpr;
}
std::string DotExpr::stringify() const {
    return "(" + this->owner->stringify() + "." + (this->member.second ? "$" : "") + this->member.first.value + ")";
}

ArrowExpr::ArrowExpr(Token tok, AstNodePtr owner, std::pair<Token, bool> member){
    this->tok = tok;
    this->owner = owner;
    this->member = member;
}

AstNodePtr ArrowExpr::get_owner() const {
    return this->owner;
}
std::pair<Token, bool> ArrowExpr::get_member() const {
    return this->member;
}

Token ArrowExpr::token() const {
    return this->tok;
}
AstKind ArrowExpr::kind() const {
    return AstKind::ArrowExpr;
}
std::string ArrowExpr::stringify() const {
    return "(" + this->owner->stringify() + "->" + (this->member.second ? "$" : "") + this->member.first.value + ")";
}


FuncCall::FuncCall(Token tok, AstNodePtr callee, std::vector<AstNodePtr> args, std::vector<std::pair<Token, AstNodePtr>> named_args){
    this->tok = tok;
    this->callee = callee;
    this->args = args;
    this->named_args = named_args;
}

AstNodePtr FuncCall::get_callee() const {
    return this->callee;
}
std::vector<AstNodePtr> FuncCall::get_arguments() const {
    return this->args;
}
std::vector<std::pair<Token, AstNodePtr>> FuncCall::get_named_arguments() const {
    return this->named_args;
}

Token FuncCall::token() const {
    return this->tok;
}
AstKind FuncCall::kind() const {
    return AstKind::FuncCall;
}
std::string FuncCall::stringify() const {
    std::string res = this->callee->stringify() + "(";
    for(size_t i = 0; i < this->args.size(); i++){
        res += this->args[i]->stringify();
        if(i != this->args.size() - 1 || !this->named_args.empty()){
            res += ", ";
        }
    }
    size_t count = 0;
    for(const auto& [name, arg] : this->named_args){
        res += name.value + "=" + arg->stringify();
        if(count != this->named_args.size() - 1){
            res += ", ";
        }
        count++;
    }
    res += ")";
    return res;
}


TernaryIf::TernaryIf(Token tok, AstNodePtr condition, AstNodePtr then_value, AstNodePtr else_value){
    this->tok = tok;
    this->condition = condition;
    this->then_value = then_value;
    this->else_value = else_value;
}

AstNodePtr TernaryIf::get_condition() const {
    return this->condition;
}
AstNodePtr TernaryIf::get_then_value() const {
    return this->then_value;
}
AstNodePtr TernaryIf::get_else_value() const {
    return this->else_value;
}

Token TernaryIf::token() const {
    return this->tok;
}
AstKind TernaryIf::kind() const {
    return AstKind::TernaryIf;
}
std::string TernaryIf::stringify() const {
    return this->condition->stringify() + " ? " + this->then_value->stringify() + " : " + this->else_value->stringify();
}

CompTimeExpr::CompTimeExpr(Token tok, AstNodePtr expr_or_stmt){
    this->tok = tok;
    this->expr_or_stmt = expr_or_stmt;
}

AstNodePtr CompTimeExpr::get_expr_or_stmt() const {
    return this->expr_or_stmt;
}

Token CompTimeExpr::token() const {
    return this->tok;
}
AstKind CompTimeExpr::kind() const {
    return AstKind::CompTimeExpr;
}
std::string CompTimeExpr::stringify() const {
    return "$" + this->expr_or_stmt->stringify() + "";
}


LambdaExpr::LambdaExpr(Token tok, CaptureClause capture, std::vector<Parameter> parameters, AstNodePtr return_type, AstNodePtr body){
    this->tok = tok;
    this->capture = capture;
    this->parameters = parameters;
    this->return_type = return_type;
    this->body = body;
}

CaptureClause LambdaExpr::get_capture() const {
    return this->capture;
}
std::vector<Parameter> LambdaExpr::get_parameters() const {
    return this->parameters;
}
AstNodePtr LambdaExpr::get_return_type() const {
    return this->return_type;
}
AstNodePtr LambdaExpr::get_body() const {
    return this->body;
}

Token LambdaExpr::token() const {
    return this->tok;
}
AstKind LambdaExpr::kind() const {
    return AstKind::LambdaExpr;
}
std::string LambdaExpr::stringify() const {
    std::string res = "fn(";
    for(size_t i = 0; i < this->parameters.size(); i++){
        res += to_string(this->parameters[i]);
        if(i != this->parameters.size() - 1){
            res += ", ";
        }
    }
    res+=")"+to_string(this->capture);

    if(this->return_type->kind() != AstKind::NoLiteral){
        res += " -> " + this->return_type->stringify();
    }
    res += "{\n" + this->body->stringify() + "\n}";
    return res;
}


FormattedStr::FormattedStr(Token tok, std::vector<Token> string_parts, std::vector<AstNodePtr> embedded_expr){
    this->tok = tok;
    this->string_parts = string_parts;
    this->embedded_expr = embedded_expr;
}

std::vector<Token> FormattedStr::get_string_parts() const{
    return this->string_parts;
}
std::vector<AstNodePtr> FormattedStr::get_embedded_expr() const{
    return this->embedded_expr;
}

Token FormattedStr::token() const{
    return this->tok;
}
AstKind FormattedStr::kind() const{
    return AstKind::FormattedStr;
}
std::string FormattedStr::stringify() const{
    std::string res = "f\"";
    std::size_t min_size = std::min(this->string_parts.size(), this->embedded_expr.size());
    for(size_t i = 0; i < min_size; i++){
        res += this->string_parts[i].value + "{" + this->embedded_expr[i]->stringify() + "}";
    }
    if(this->string_parts.size() > min_size){
        res += this->string_parts.back().value;
    }
    else if(this->embedded_expr.size() > min_size){
        res += "{" + this->embedded_expr.back()->stringify() + "}";
    }
    res += "\"";
    return res;
}

ThreadOrTaskExpr::ThreadOrTaskExpr(Token tok, bool thread, AstNodePtr body, 
                                   AstNodePtr restart_count, AstNodePtr timeout, 
                                   AstNodePtr always_restart, AstNodePtr restart_delay){
    this->tok = tok;
    this->thread = thread;
    this->body = body;
    this->restart_count = restart_count;
    this->timeout = timeout;
    this->always_restart = always_restart;
    this->restart_delay = restart_delay;
}

bool ThreadOrTaskExpr::is_thread() const{
    return this->thread;
}
AstNodePtr ThreadOrTaskExpr::get_body() const{
    return this->body;
}
AstNodePtr ThreadOrTaskExpr::get_restart_count() const{
    return this->restart_count;
}
AstNodePtr ThreadOrTaskExpr::get_timeout() const{
    return this->timeout;
}
AstNodePtr ThreadOrTaskExpr::get_always_restart() const{
    return this->always_restart;
}
AstNodePtr ThreadOrTaskExpr::get_restart_delay_mss() const{
    return this->restart_delay;
}

Token ThreadOrTaskExpr::token() const{
    return this->tok;
}
AstKind ThreadOrTaskExpr::kind() const{
    return AstKind::ThreadOrTaskExpr;
}
std::string ThreadOrTaskExpr::stringify() const{
    std::string res =  this->thread ? "thread(" : "task(";
    bool to_pop = false;
    if(this->restart_count->kind() != AstKind::NoLiteral){
        res += " restart=" + this->restart_count->stringify() + ",";
        to_pop = true;
    }
    if(this->timeout->kind() != AstKind::NoLiteral){
        res += " timeout=" + this->timeout->stringify() + ",";
        to_pop = true;
    }
    if(this->always_restart->kind() != AstKind::NoLiteral){
        res += " always_restart=" + this->always_restart->stringify() + ",";
        to_pop = true;
    }
    if(this->restart_delay->kind() != AstKind::NoLiteral){
        res += " restart_delay=" + this->restart_delay->stringify() + ",";
        to_pop = true;
    }
    if(to_pop){
        res.pop_back();//Remove last comma
    }
    res += ")";


    if (this->body->kind() == AstKind::Block){
        res += " {\n" + this->body->stringify() + "\n}";
    }
    else{
        res += " " + this->body->stringify();
    }
    return res;
}


ArrowBlockCallExpr::ArrowBlockCallExpr(Token tok, AstNodePtr callee, std::vector<AstNodePtr> args, std::vector<std::pair<Token, AstNodePtr>> named_args, AstNodePtr body){
    this->tok = tok;
    this->callee = callee;
    this->args = args;
    this->named_args = named_args;
    this->body = body;
}

AstNodePtr ArrowBlockCallExpr::get_callee() const{
    return this->callee;
}
std::vector<AstNodePtr> ArrowBlockCallExpr::get_arguments() const{
    return this->args;
}
std::vector<std::pair<Token, AstNodePtr>> ArrowBlockCallExpr::get_named_arguments() const{
    return this->named_args;
}
AstNodePtr ArrowBlockCallExpr::get_body() const{
    return this->body;
}

Token ArrowBlockCallExpr::token() const{
    return this->tok;
}
AstKind ArrowBlockCallExpr::kind() const{
    return AstKind::ArrowBlockCallExpr;
}
std::string ArrowBlockCallExpr::stringify() const{
    std::string res = this->callee->stringify();
    res += "(";
    if(!this->args.empty()){
        for(size_t i = 0; i < this->args.size(); i++){
            res += this->args[i]->stringify();
            if(i != this->args.size() - 1){
                res += ", ";
            }
        }
    }
    if(!this->named_args.empty()){
        if(!this->args.empty()){
            res += ", ";
        }
        size_t count = 0;
        for(const auto& [name, arg] : this->named_args){
            res += name.value + "=" + arg->stringify();
            if(count != this->named_args.size() - 1){
                res += ", ";
            }
            count++;
        }
    }
    res += ")";
    res += " => " + this->body->stringify();
    return res;
}
}