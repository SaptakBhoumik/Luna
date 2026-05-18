#include "ast/ast.hpp"
#include <string>

namespace Luna{
TypeExpr::TypeExpr(Token tok, AstNodePtr value, AstNodePtr idx){
    this->tok = tok;
    this->value = value;
    this->idx = idx;
}

AstNodePtr TypeExpr::get_value() const{
    return this->value;
}

AstNodePtr TypeExpr::get_idx() const{
    return this->idx;
}

Token TypeExpr::token() const{
    return this->tok;
}

AstKind TypeExpr::kind() const{
    return AstKind::TypeExpr;
}

std::string TypeExpr::stringify() const{
    std::string res = this->value->stringify();
    if(this->idx->kind() != AstKind::NoLiteral){
        res += "[" + this->idx->stringify() + "]";
    }
    return res;
}


ListTypeExpr::ListTypeExpr(Token tok, AstNodePtr elem_type, AstNodePtr size){
    this->tok = tok;
    this->elem_type = elem_type;
    this->size = size;
}

AstNodePtr ListTypeExpr::get_elem_type() const{
    return this->elem_type;
}

AstNodePtr ListTypeExpr::get_size() const{
    return this->size;
}

Token ListTypeExpr::token() const{
    return this->tok;
}

AstKind ListTypeExpr::kind() const{
    return AstKind::ListTypeExpr;
}

std::string ListTypeExpr::stringify() const{
    std::string res = "[";
    res += this->elem_type->stringify();
    if (this->size->kind() != AstKind::NoLiteral){
        res += ", " + this->size->stringify();
    }
    res += "]";
    return res;
}


PtrTypeExpr::PtrTypeExpr(Token tok, AstNodePtr base_type){
    this->tok = tok;
    this->base_type = base_type;
}

AstNodePtr PtrTypeExpr::get_base_type() const{
    return this->base_type;
}

Token PtrTypeExpr::token() const{
    return this->tok;
}
AstKind PtrTypeExpr::kind() const{
    return AstKind::PtrTypeExpr;
}
std::string PtrTypeExpr::stringify() const{
    return "*" + this->base_type->stringify();
}


OptionalTypeExpr::OptionalTypeExpr(Token tok, AstNodePtr base_type){
    this->tok = tok;
    this->base_type = base_type;
}

AstNodePtr OptionalTypeExpr::get_base_type() const{
    return this->base_type;
}

Token OptionalTypeExpr::token() const{
    return this->tok;
}
AstKind OptionalTypeExpr::kind() const{
    return AstKind::OptionalTypeExpr;
}
std::string OptionalTypeExpr::stringify() const{
    return "?" + this->base_type->stringify();
}

ErrorTypeExpr::ErrorTypeExpr(Token tok, AstNodePtr base_type){
    this->tok = tok;
    this->base_type = base_type;
}

AstNodePtr ErrorTypeExpr::get_base_type() const{
    return this->base_type;
}

Token ErrorTypeExpr::token() const{
    return this->tok;
}
AstKind ErrorTypeExpr::kind() const{
    return AstKind::ErrorTypeExpr;
}
std::string ErrorTypeExpr::stringify() const{
    return "!" + this->base_type->stringify();
}


FuncTypeExpr::FuncTypeExpr(Token tok, std::vector<AstNodePtr> param_types,bool c_variadic, AstNodePtr return_type){
    this->tok = tok;
    this->param_types = param_types;
    this->c_variadic = c_variadic;
    this->return_type = return_type;
}

std::vector<AstNodePtr> FuncTypeExpr::get_param_types() const{
    return this->param_types;
}
AstNodePtr FuncTypeExpr::get_return_type() const{
    return this->return_type;
}
bool FuncTypeExpr::has_c_variadic() const{
    return this->c_variadic;
}

Token FuncTypeExpr::token() const{
    return this->tok;
}
AstKind FuncTypeExpr::kind() const{
    return AstKind::FuncTypeExpr;
}
std::string FuncTypeExpr::stringify() const{
    std::string res = "fn(";
    for (size_t i = 0; i < this->param_types.size(); i++){
        res += this->param_types[i]->stringify();
        if (i != this->param_types.size() - 1){
            res += ", ";
        }
    }
    if (this->c_variadic){
        if (!this->param_types.empty()){
            res += ", ";
        }
        res += "...";
    }
    res += ")";
    if (this->return_type->kind() != AstKind::NoLiteral){
        res += " -> " + this->return_type->stringify();
    }
    return res;
}


TupleTypeExpr::TupleTypeExpr(Token tok, std::vector<AstNodePtr> elem_types){
    this->tok = tok;
    this->elem_types = elem_types;
}

std::vector<AstNodePtr> TupleTypeExpr::get_elem_types() const{
    return this->elem_types;
}

Token TupleTypeExpr::token() const{
    return this->tok;
}
AstKind TupleTypeExpr::kind() const{
    return AstKind::TupleTypeExpr;
}
std::string TupleTypeExpr::stringify() const{
    std::string res = "(";
    for (size_t i = 0; i < this->elem_types.size(); i++){
        res += this->elem_types[i]->stringify();
        if (i != this->elem_types.size() - 1){
            res += ", ";
        }
    }
    res += ")";
    return res;
}


SimdTypeExpr::SimdTypeExpr(Token tok, AstNodePtr elem_type, AstNodePtr lanes){
    this->tok = tok;
    this->elem_type = elem_type;
    this->lanes = lanes;
}

AstNodePtr SimdTypeExpr::get_elem_type() const{
    return this->elem_type;
}
AstNodePtr SimdTypeExpr::get_lanes() const{
    return this->lanes;
}

Token SimdTypeExpr::token() const{
    return this->tok;
}
AstKind SimdTypeExpr::kind() const{
    return AstKind::SimdTypeExpr;
}
std::string SimdTypeExpr::stringify() const{
    return "<" + this->elem_type->stringify() + ", " + this->lanes->stringify() + ">";
}


SumTypeExpr::SumTypeExpr(Token tok, std::vector<AstNodePtr> variants){
    this->tok = tok;
    this->variants = variants;
}

std::vector<AstNodePtr> SumTypeExpr::get_variants() const{
    return this->variants;
}

Token SumTypeExpr::token() const{
    return this->tok;
}
AstKind SumTypeExpr::kind() const{
    return AstKind::SumTypeExpr;
}
std::string SumTypeExpr::stringify() const{
    std::string res = "";
    for (size_t i = 0; i < this->variants.size(); i++){
        res += this->variants[i]->stringify();
        if (i != this->variants.size() - 1){
            res += " | ";
        }
    }
    return res;
}


EnumTypeExpr::EnumTypeExpr(Token tok, AstNodePtr base_type, std::vector<std::pair<Token, AstNodePtr>> variants){
    this->tok = tok;
    this->base_type = base_type;
    this->variants = variants;
}

AstNodePtr EnumTypeExpr::get_base_type() const{
    return this->base_type;
}
std::vector<std::pair<Token, AstNodePtr>> EnumTypeExpr::get_variants() const{
    return this->variants;
}

Token EnumTypeExpr::token() const{
    return this->tok;
}
AstKind EnumTypeExpr::kind() const{
    return AstKind::EnumTypeExpr;
}
std::string EnumTypeExpr::stringify() const{
    std::string res = "enum";
    if (this->base_type->kind() != AstKind::NoLiteral){
        res += ":" + this->base_type->stringify();
    }
    res += " {\n";
    for (size_t i = 0; i < this->variants.size(); i++){
        res += "\t" + this->variants[i].first.value;
        if (this->variants[i].second->kind() != AstKind::NoLiteral){
            res += " = " + this->variants[i].second->stringify();
        }
        res += "\n";
    }
    res += "}";
    return res;
}


StructTypeExpr::StructTypeExpr(Token tok, std::vector<StructField> fields){
    this->tok = tok;
    this->fields = fields;
}

std::vector<StructField> StructTypeExpr::get_fields() const{
    return this->fields;
}

Token StructTypeExpr::token() const{
    return this->tok;
}
AstKind StructTypeExpr::kind() const{
    return AstKind::StructTypeExpr;
}
std::string StructTypeExpr::stringify() const{
    std::string res = "{\n";
    for (size_t i = 0; i < this->fields.size(); i++){
        res += "\t" + to_string(this->fields[i]) + "\n";
    }
    res += "}";
    return res;
}


InterfaceTypeExpr::InterfaceTypeExpr(Token tok, std::vector<AstNodePtr> methods){
    this->tok = tok;
    this->methods = methods;
}

std::vector<AstNodePtr> InterfaceTypeExpr::get_methods() const{
    return this->methods;
}

Token InterfaceTypeExpr::token() const{
    return this->tok;
}
AstKind InterfaceTypeExpr::kind() const{
    return AstKind::InterfaceTypeExpr;
}
std::string InterfaceTypeExpr::stringify() const{
    std::string res = "interface {\n";
    for (size_t i = 0; i < this->methods.size(); i++){
        res += "\t" + this->methods[i]->stringify() + "\n";
    }
    res += "}";
    return res;
}
}