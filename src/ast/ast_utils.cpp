#include "ast/ast.hpp"
#include "lexer/token.hpp"

namespace Luna{
std::string to_string(const Attribute& attr){
    std::string res = "@["+attr.name;
    if(attr.args.empty() && attr.named_args.empty()){
        return res+"]";
    }
    res += "(";
    for(size_t i = 0; i < attr.args.size(); i++){
        res += attr.args[i]->stringify();
        if(i != attr.args.size() - 1 || !attr.named_args.empty()){
            res += ", ";
        }
    }
    if(!attr.named_args.empty()){
        for(const auto& [name, arg] : attr.named_args){
            res += name + "=" + arg->stringify() + ", ";
        }
        res.pop_back(); // remove last space
        res.pop_back(); // remove last comma
    }
    return res + ")]";
}

std::string to_string(const Parameter& param){
    if(param.kind == ParamKind::CVariadic){
        return "...";
    } 
    std::string res;
    res += param.name + ":" + param.type->stringify();
    if(param.kind == ParamKind::VarArg){
        res = "*" + res;
    } 
    else if(param.kind == ParamKind::CompileTimeVarArg){
        res = "*$" + res;
    } 
    else if(param.kind == ParamKind::CompileTimeKwarg){
        res = "**$" + res;
    }
    if(param.is_mut){
        res = "mut " + res;
    }
    if(param.default_value && param.default_value->kind() != AstKind::NoLiteral){
        res += " = " + param.default_value->stringify();
    }
    return res;
}

std::string to_string(const CaptureClause& capture){
    switch(capture.kind){
        case CaptureKind::None:
            return "[]";
        case CaptureKind::AllCopy:
            return "[=]";
        case CaptureKind::AllRef:
            return "[&]";
        case CaptureKind::List: {
            std::string res = "[";
            for(size_t i = 0; i < capture.entries.size(); i++){
                const auto& entry = capture.entries[i];
                res += (entry.by_ref ? "&" : "") + entry.name->stringify();
                if(i != capture.entries.size() - 1){
                    res += ", ";
                }
            }
            return res + "]";
        }
    }
    return "[]"; // should never reach here
}

std::string to_string(const StructField& field){
    std::string res;
    for(const auto& attr : field.attributes){
        res += to_string(attr) + " ";
    }
    if(field.is_pub){
        res += "pub ";
    }
    if(field.is_mut){
        res += "mut ";
    }
    res += field.name->stringify() + ":" + field.type->stringify();
    if(field.default_value->kind() != AstKind::NoLiteral){
        res += " = " + field.default_value->stringify();
    }
    return res;
}
}