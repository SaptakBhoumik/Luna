#include "ast/ast.hpp"
#include <cstddef>

namespace Luna{
TypeDefStmt::TypeDefStmt(Token tok, std::vector<Attribute> attributes, bool pub, std::string name, std::vector<std::string> generics, AstNodePtr base_type){
    this->tok = tok;
    this->attributes = attributes;
    this->pub = pub;
    this->name = name;
    this->generics = generics;
    this->base_type = base_type;
}

std::vector<Attribute> TypeDefStmt::get_attributes() const {
    return this->attributes;
}

bool TypeDefStmt::is_pub() const {
    return this->pub;
}

std::string TypeDefStmt::get_name() const {
    return this->name;
}

std::vector<std::string> TypeDefStmt::get_generics() const {
    return this->generics;
}

AstNodePtr TypeDefStmt::get_base_type() const {
    return this->base_type;
}

Token TypeDefStmt::token() const {
    return this->tok;
}
AstKind TypeDefStmt::kind() const {
    return AstKind::TypeDefStmt;
}
std::string TypeDefStmt::stringify() const {
    std::string result;
    for (const auto& attr : attributes) {
        result += to_string(attr) + "\n";
    }
    if (pub) {
        result += "pub ";
    }
    result += "type " + name;
    if (!generics.empty()) {
        for(size_t i = 0; i < generics.size(); i++) {
            result += generics[i];
            if (i != generics.size() - 1) {
                result += ", ";
            }
        }
    }
    if(base_type->kind() != AstKind::NoLiteral) {
        result += " = " + base_type->stringify();
    }
    return result;
}


VarStmt::VarStmt(Token tok, AstNodePtr name, AstNodePtr type, AstNodePtr value, bool is_mut, bool is_def, bool pub, VarKind varkind, std::vector<Attribute> attributes){
    this->tok = tok;
    this->name = name;
    this->type = type;
    this->value = value;
    this->mut = is_mut;
    this->def = is_def;
    this->pub = pub;
    this->varkind = varkind;
    this->attributes = attributes;
}

AstNodePtr VarStmt::get_name() const {
    return this->name;
}
AstNodePtr VarStmt::get_var_type() const {
    return this->type;
}
AstNodePtr VarStmt::get_value() const {
    return this->value;
}
bool VarStmt::is_pub() const {
    return this->pub;
}
bool VarStmt::is_mut() const {
    return this->mut;
}
bool VarStmt::is_def() const {
    return this->def;
}
VarKind VarStmt::get_varkind() const {
    return this->varkind;
}
std::vector<Attribute> VarStmt::get_attributes() const {
    return this->attributes;
}

Token VarStmt::token() const {
    return this->tok;
}
AstKind VarStmt::kind() const {
    return AstKind::VarStmt;
}
std::string VarStmt::stringify() const {
    std::string result;
    for (const auto& attr : this->attributes) {
        result += to_string(attr) + "\n";
    }
    if(this->varkind == VarKind::ThreadLocal) {
        result += "thread_local ";
    } 
    else if(this->varkind == VarKind::TaskLocal) {
        result += "task_local ";
    }
    if (pub) {
        result += "pub ";
    }
    if(mut) {
        result += "mut ";
    }
    result += name->stringify();
    if (type->kind() != AstKind::NoLiteral) {
        result += " : " + type->stringify();
    }
    if(value->kind() != AstKind::NoLiteral) {
        if(def){
            result += " := ";
        }
        else{
            result += " = ";
        }
        result += value->stringify();
    }
    return result;
}


AugAssignStmt::AugAssignStmt(Token tok, Token op, AstNodePtr target, AstNodePtr value){
    this->tok = tok;
    this->op = op;
    this->target = target;
    this->value = value;
}

AstNodePtr AugAssignStmt::get_target() const {
    return this->target;
}
Token AugAssignStmt::get_op() const {
    return this->op;
}
AstNodePtr AugAssignStmt::get_value() const {
    return this->value;
}

Token AugAssignStmt::token() const {
    return this->tok;
}
AstKind AugAssignStmt::kind() const {
    return AstKind::AugAssignStmt;
}
std::string AugAssignStmt::stringify() const {
    return target->stringify() + " " + op.value + " " + value->stringify();
}

FuncDefStmt::FuncDefStmt(Token tok, bool pub, std::string name, std::vector<std::string> generics,std::vector<Parameter> parameters,AstNodePtr return_type, 
                         AstNodePtr body, std::vector<Annotation> annotation){
    this->tok = tok;
    this->pub = pub;
    this->name = name;
    this->generics = generics;
    this->parameters = parameters;
    this->return_type = return_type;
    this->body = body;
    this->annotation = annotation;
}

bool FuncDefStmt::is_pub() const {
    return this->pub;
}
std::string FuncDefStmt::get_name() const {
    return this->name;
}
std::vector<std::string> FuncDefStmt::get_generics() const {
    return this->generics;
}
std::vector<Parameter> FuncDefStmt::get_parameters() const {
    return this->parameters;
}
AstNodePtr FuncDefStmt::get_return_type() const {
    return this->return_type;
}
AstNodePtr FuncDefStmt::get_body() const {
    return this->body;
}
std::vector<Annotation> FuncDefStmt::get_annotation() const {
    return this->annotation;
}

Token FuncDefStmt::token() const {
    return this->tok;
}
AstKind FuncDefStmt::kind() const {
    return AstKind::FuncDefStmt;
}
std::string FuncDefStmt::stringify() const{
    std::string result;
    for (const auto& annot : annotation) {
        result += to_string(annot) + "\n";
    }
    if (is_pub()) {
        result += "pub ";
    }
    result += "fn " + name;
    if (!generics.empty()) {
        result += "{";
        for(size_t i = 0; i < generics.size(); i++) {
            result += generics[i];
            if (i != generics.size() - 1) {
                result += ", ";
            }
        }
        result += "}";
    }
    result += "(";
    for(size_t i = 0; i < parameters.size(); i++) {
        result += to_string(parameters[i]);
        if (i != parameters.size() - 1) {
            result += ", ";
        }
    }
    result += ")";
    if (return_type->kind() != AstKind::NoLiteral) {
        result += " -> " + return_type->stringify();
    }
    if (body->kind() != AstKind::NoLiteral) {
        result += " {\n" + body->stringify() + "\n}";
    }
    return result;
}


MethodDefStmt::MethodDefStmt(Token tok, bool pub, Parameter receiver, std::string name, std::vector<std::string> generics, std::vector<Parameter> parameters, 
                             AstNodePtr return_type, AstNodePtr body, std::vector<Attribute> attributes){

    this->tok = tok;
    this->pub = pub;
    this->receiver = receiver;
    this->name = name;
    this->generics = generics;
    this->parameters = parameters;
    this->return_type = return_type;
    this->body = body;
    this->attributes = attributes;
}

bool MethodDefStmt::is_pub() const {
    return this->pub;
}
Parameter MethodDefStmt::get_receiver() const {
    return this->receiver;
}
std::string MethodDefStmt::get_name() const {
    return this->name;
}
std::vector<std::string> MethodDefStmt::get_generics() const {
    return this->generics;
}
std::vector<Parameter> MethodDefStmt::get_parameters() const {
    return this->parameters;
}
AstNodePtr MethodDefStmt::get_return_type() const {
    return this->return_type;
}
AstNodePtr MethodDefStmt::get_body() const {
    return this->body;
}
std::vector<Attribute> MethodDefStmt::get_attributes() const {
    return this->attributes;
}

Token MethodDefStmt::token() const {
    return this->tok;
}
AstKind MethodDefStmt::kind() const {
    return AstKind::MethodDefStmt;
}
std::string MethodDefStmt::stringify() const {
    std::string result;
    for (const auto& attr : this->attributes) {
        result += to_string(attr) + "\n";
    }
    if (is_pub()) {
        result += "pub ";
    }
    result += "fn (" + to_string(receiver) + ") " + name;
    if (!generics.empty()) {
        result += "{";
        for(size_t i = 0; i < generics.size(); i++) {
            result += generics[i];
            if (i != generics.size() - 1) {
                result += ", ";
            }
        }
        result += "}";
    }
    result += "(";
    for(size_t i = 0; i < parameters.size(); i++) {
        result += to_string(parameters[i]);
        if (i != parameters.size() - 1) {
            result += ", ";
        }
    }
    result += ")";
    if (return_type->kind() != AstKind::NoLiteral) {
        result += " -> " + return_type->stringify();
    }
    if (body->kind() != AstKind::NoLiteral) {
        result += " {\n" + body->stringify() + "\n}";
    }
    return result;
}
}