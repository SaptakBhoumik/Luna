#include "ast/ast.hpp"

namespace Luna{
ImportStmt::ImportStmt(Token tok, bool pub, std::optional<Token> name, AstNodePtr module_path){
    this->tok = tok;
    this->pub = pub;
    this->name = name;
    this->module_path = module_path;
}

bool ImportStmt::is_pub() const{
    return this->pub;
}
std::optional<Token> ImportStmt::get_name() const{
    return this->name;
}
AstNodePtr ImportStmt::get_module_path() const{
    return this->module_path;
}

Token ImportStmt::token() const{
    return this->tok;
}
AstKind ImportStmt::kind() const{
    return AstKind::ImportStmt;
}
std::string ImportStmt::stringify() const{
    std::string res = (this->pub ? "pub " : "");
    if(this->name.has_value()){
        res += name.value().value + " := ";
    }
    res += "import(" + this->module_path->stringify() + ")";
    return res;
}
}