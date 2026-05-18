#include "ast/ast.hpp"

namespace Luna{
WhenStmt::WhenStmt(Token tok, std::vector<AstNodePtr> subjects, std::vector<std::pair<std::vector<std::vector<AstNodePtr>>, AstNodePtr>> branches, std::vector<Attribute> attributes){
    this->tok = tok;
    this->subjects = subjects;
    this->branches = branches;
    this->attributes = attributes;
}

std::vector<AstNodePtr> WhenStmt::get_subjects() const {
    return this->subjects;
}
std::vector<std::pair<std::vector<std::vector<AstNodePtr>>, AstNodePtr>> WhenStmt::get_branches() const {
    return this->branches;
}
std::vector<Attribute> WhenStmt::get_attributes() const {
    return this->attributes;
}

Token WhenStmt::token() const {
    return this->tok;
}

AstKind WhenStmt::kind() const {
    return AstKind::WhenStmt;
}

std::string WhenStmt::stringify() const {
    std::string res;
    for(const auto& attr : this->attributes){
        res += to_string(attr) + "\n";
    }
    res += "when ";
    if(!this->subjects.empty()){
        for(size_t i = 0; i < this->subjects.size(); i++){
            res += this->subjects[i]->stringify();
            if(i != this->subjects.size() - 1){
                res += ", ";
            }
        }
    }
    res += "{\n";
    for(const auto& branch : this->branches){
        const auto& conditions = branch.first;
        const auto& body = branch.second;
        for(size_t i = 0; i < conditions.size(); i++){
            const auto& case_conditions = conditions[i];
            res += "\t";
            for(size_t j = 0; j < case_conditions.size(); j++){
                if(case_conditions[j]->kind() == AstKind::NoLiteral){
                    res += "?";
                } 
                else {
                    res += case_conditions[j]->stringify();
                }
                if(j != case_conditions.size() - 1){
                    res += ", ";
                }
            }
            if(i != conditions.size() - 1){
                res += "\n\t";
            }
            else{
                res += ":";
            }
        }
        res += "{\n" + body->stringify() + "\n\t}\n";
    }
    res += "}";
    return res;
}


LoopStmt::LoopStmt(Token tok, LoopKind loop_kind,std::vector<std::pair<AstNodePtr,bool>> variables,std::vector<AstNodePtr> values,
                   AstNodePtr body,std::vector<Attribute> attributes){
    this->tok = tok;
    this->loop_kind = loop_kind;
    this->variables = variables;
    this->values = values;
    this->body = body;
    this->attributes = attributes;
}

LoopKind LoopStmt::get_loop_kind() const {
    return this->loop_kind;
}

std::vector<std::pair<AstNodePtr,bool>> LoopStmt::get_variables() const {
    return this->variables;
}

std::vector<AstNodePtr> LoopStmt::get_values() const {
    return this->values;
}

AstNodePtr LoopStmt::get_body() const {
    return this->body;
}

std::vector<Attribute> LoopStmt::get_attributes() const {
    return this->attributes;
}

Token LoopStmt::token() const {
    return this->tok;
}
AstKind LoopStmt::kind() const {
    return AstKind::LoopStmt;
}
std::string LoopStmt::stringify() const {
    std::string res;
    for(const auto& attr : this->attributes){
        res += to_string(attr) + "\n";
    }
    res += "loop ";
    if(!this->variables.empty()){
        for(size_t i = 0; i < this->variables.size(); i++){
            if(this->variables[i].second){
                res += "mut ";
            }
            res += this->variables[i].first->stringify();
            if(i != this->variables.size() - 1){
                res += ", ";
            }
        }
        res += " : ";
    }
    if(!this->values.empty()){
        for(size_t i = 0; i < this->values.size(); i++){
            res += this->values[i]->stringify();
            if(i != this->values.size() - 1){
                res += ", ";
            }
        }
    }
    res += " {\n" + this->body->stringify() + "\n}";
    return res;
}


SelectStmt::SelectStmt(Token tok, std::vector<std::pair<std::vector<SelectArm>, AstNodePtr>> cases, std::vector<Attribute> attributes){
    this->tok = tok;
    this->cases = cases;
    this->attributes = attributes;
}

std::vector<std::pair<std::vector<SelectArm>, AstNodePtr>> SelectStmt::get_cases() const {
    return this->cases;
}
std::vector<Attribute> SelectStmt::get_attributes() const {
    return this->attributes;
}

Token SelectStmt::token() const {
    return this->tok;
}
AstKind SelectStmt::kind() const {
    return AstKind::SelectStmt;
}
std::string SelectStmt::stringify() const {
    std::string res;
    for(const auto& attr : this->attributes){
        res += to_string(attr) + "\n";
    }
    res += "select {\n";
    for(auto& c : this->cases){
        auto& arms = c.first;
        auto& body = c.second;
        for(size_t i = 0; i < arms.size(); i++){
            res += "\t" + to_string(arms[i]);
            if(i != arms.size() - 1){
                res += "\n";
            }
            else{
                res += ":\n";
            }
        }
        res += "\t{\n" + body->stringify() + "\n\t}\n";
    }
    res += "}";
    return res;
}
}