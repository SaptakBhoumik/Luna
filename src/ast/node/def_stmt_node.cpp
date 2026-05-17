#include "ast/ast.hpp"
#include <cstddef>

namespace Luna{
TypeDefStmt::TypeDefStmt(Token tok, std::vector<Attribute> attributes, bool pub, Token name, std::vector<std::pair<Token, AstNodePtr>> generics, AstNodePtr base_type){
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

Token TypeDefStmt::get_name() const {
    return this->name;
}

std::vector<std::pair<Token, AstNodePtr>> TypeDefStmt::get_generics() const {
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
    result += "type " + name.value;
    if (!generics.empty()) {
        result += "::<";
        for(size_t i = 0; i < generics.size(); i++) {
            result += generics[i].first.value;
            if(generics[i].second->kind() != AstKind::NoLiteral) {
                result += " : " + generics[i].second->stringify();
            }
            if (i != generics.size() - 1) {
                result += ", ";
            }
        }
        result += ">";
    }
    if(base_type->kind() != AstKind::NoLiteral) {
        result += " = " + base_type->stringify();
    }
    return result;
}


VarStmt::VarStmt(Token tok, std::vector<std::pair<AstNodePtr, std::pair<bool, bool>>> names, AstNodePtr type, std::vector<AstNodePtr> values,
                 bool is_def, std::vector<Attribute> attributes){
    this->tok = tok;
    this->names = names;
    this->type = type;
    this->values = values;
    this->def = is_def;
    this->attributes = attributes;
}

std::vector<std::pair<AstNodePtr, std::pair<bool, bool>>> VarStmt::get_names() const {
    return this->names;
}
AstNodePtr VarStmt::get_var_type() const {
    return this->type;
}
std::vector<AstNodePtr> VarStmt::get_values() const {
    return this->values;
}
bool VarStmt::is_def() const {
    return this->def;
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
    for (size_t i = 0; i < names.size(); ++i) {
        const auto& name_pair = names[i];
        if(name_pair.second.first) { // is_pub
            result += "pub ";
        }  
        if(name_pair.second.second) { // is_mut
            result += "mut ";
        }

        result += name_pair.first->stringify();
        if (i != names.size() - 1) {
            result += ", ";
        }
    }
    if (type->kind() != AstKind::NoLiteral) {
        result += " : " + type->stringify();
    }
    if(values.empty()) {
        // Do nothing
    } 
    else {
        if(def){
            result += " := ";
        }
        else{
            result += " = ";
        }
        for(size_t i = 0; i < values.size(); ++i) {
            result += values[i]->stringify();
            if (i != values.size() - 1) {
                result += ", ";
            }
        }
    }
    return result;
}


AugAssignStmt::AugAssignStmt(Token tok, Token op, std::vector<AstNodePtr> targets, std::vector<AstNodePtr> values){
    this->tok = tok;
    this->op = op;
    this->targets = targets;
    this->values = values;
}

std::vector<AstNodePtr> AugAssignStmt::get_targets() const {
    return this->targets;
}
Token AugAssignStmt::get_op() const {
    return this->op;
}
std::vector<AstNodePtr> AugAssignStmt::get_values() const {
    return this->values;
}

Token AugAssignStmt::token() const {
    return this->tok;
}
AstKind AugAssignStmt::kind() const {
    return AstKind::AugAssignStmt;
}
std::string AugAssignStmt::stringify() const {
    std::string result;
    for(size_t i = 0; i < targets.size(); ++i) {
        result += targets[i]->stringify();
        if (i != targets.size() - 1) {
            result += ", ";
        }
    }
    result += " " + op.value + " ";
    for(size_t i = 0; i < values.size(); ++i) {
        result += values[i]->stringify();
        if (i != values.size() - 1) {
            result += ", ";
        }
    }
    return result;
}

FuncDefStmt::FuncDefStmt(Token tok, bool pub, Token name, std::vector<std::pair<Token, AstNodePtr>> generics,std::vector<Parameter> parameters,AstNodePtr return_type, 
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
Token FuncDefStmt::get_name() const {
    return this->name;
}
std::vector<std::pair<Token, AstNodePtr>> FuncDefStmt::get_generics() const {
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
    result += "fn " + name.value;
    if (!generics.empty()) {
        result += "::<";
        for(size_t i = 0; i < generics.size(); i++) {
            result += generics[i].first.value;
            if(generics[i].second->kind() != AstKind::NoLiteral) {
                result += " : " + generics[i].second->stringify();
            }
            if (i != generics.size() - 1) {
                result += ", ";
            }
        }
        result += ">";
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


MethodDefStmt::MethodDefStmt(Token tok, bool pub, Parameter receiver, Token name, std::vector<std::pair<Token, AstNodePtr>> generics, std::vector<Parameter> parameters, 
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
Token MethodDefStmt::get_name() const {
    return this->name;
}
std::vector<std::pair<Token, AstNodePtr>> MethodDefStmt::get_generics() const {
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
    result += "fn (" + to_string(receiver) + ") " + name.value;
    if (!generics.empty()) {
        result += "::<";
        for(size_t i = 0; i < generics.size(); i++) {
            result += generics[i].first.value;
            if(generics[i].second->kind() != AstKind::NoLiteral) {
                result += " : " + generics[i].second->stringify();
            }
            if (i != generics.size() - 1) {
                result += ", ";
            }
        }
        result += ">";
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