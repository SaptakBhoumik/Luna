#include "ast/ast.hpp"
#include "lexer/token.hpp"

namespace Luna{
// ---- Attribute: #[name] or #[name(args...)] stored directly on owning nodes ----
std::string to_string(const Attribute& attr){
    std::string res = "#["+attr.name.value;
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
            res += name.value + "=" + arg->stringify() + ", ";
        }
        res.pop_back(); // remove last space
        res.pop_back(); // remove last comma
    }
    return res + ")]";
}

// ---- Decorator: @name or @name(args...) stored directly on owning nodes ----
std::string to_string(const Decorator& decorator){
    std::string res = "@" + decorator.decorator->stringify();
    if(decorator.args.empty() && decorator.named_args.empty()){
        return res;
    }
    res += "(";
    for(size_t i = 0; i < decorator.args.size(); i++){
        res += decorator.args[i]->stringify();
        if(i != decorator.args.size() - 1 || !decorator.named_args.empty()){
            res += ", ";
        }
    }
    if(!decorator.named_args.empty()){
        for(const auto& [name, arg] : decorator.named_args){
            res += name.value + "=" + arg->stringify() + ", ";
        }
        res.pop_back(); // remove last space
        res.pop_back(); // remove last comma
    }
    return res + ")";
}

// ---- Annotation: A decorator or a attribute used on a function
std::string to_string(const Annotation& annotation){
    if(annotation.is_decorator){
        return to_string(annotation.decorator);
    } 
    else {
        return to_string(annotation.attributes);
    }
}

// ---- Parameter ----
std::string to_string(const Parameter& param){
    if(param.kind == ParamKind::CVariadic){
        return "...";
    } 
    std::string res = param.name.value;
    if(param.type->kind() != AstKind::NoLiteral){
        res += ":" + param.type->stringify();
    }
    if(param.kind == ParamKind::VarArg){
        res = "*" + res;
    } 
    else if(param.kind == ParamKind::CompileTimeVarArg){
        res = "*$" + res;
    } 
    else if(param.kind == ParamKind::KwVararg){
        res = "**" + res;
    }
    else if(param.kind == ParamKind::CompileTimeKwVararg){
        res = "**$" + res;
    }
    else if(param.kind == ParamKind::CompileTime){
        res = "$" + res;
    }
    if(param.is_mut){
        res = "mut " + res;
    }
    if(param.default_value && param.default_value->kind() != AstKind::NoLiteral){
        res += " = " + param.default_value->stringify();
    }
    return res;
}

// ---- Lambda capture ----
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

// ---- Struct field (for struct definitions) ----
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
    res += field.name.value + ":" + field.type->stringify();
    if(field.default_value->kind() != AstKind::NoLiteral){
        res += " = " + field.default_value->stringify();
    }
    return res;
}

// ---- Loop and select arms ----
std::string to_string(const SelectArm& arm){
    std::string res;
    for(size_t i = 0; i < arm.value.size(); i++){
        const auto& [val, is_mut] = arm.value[i];
        res += (is_mut ? "mut " : "") + val->stringify();
        if(i != arm.value.size() - 1){
            res += ", ";
        }
    }
    if(arm.arm_kind == SelectArmKind::Default){
        res += "?";
    }
    else if(arm.arm_kind == SelectArmKind::Recv){
        res += " <-- ";
    }
    else if(arm.arm_kind == SelectArmKind::Send){
        res += " --> ";
    }
    for(size_t i = 0; i < arm.channel.size(); i++){
        res += arm.channel[i]->stringify();
        if(i != arm.channel.size() - 1){
            res += ", ";
        }
    }
    return res;
}
}