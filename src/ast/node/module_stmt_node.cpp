#include "ast/ast.hpp"

namespace Luna{
ImportStmt::ImportStmt(Token tok, std::vector<Token> module_path, std::vector<std::vector<Token>> imported_symbols){
    this->tok = tok;
    this->module_path = module_path;
    this->imported_symbols = imported_symbols;
}

std::vector<Token> ImportStmt::get_module_path() const{
    return this->module_path;
}
std::vector<std::vector<Token>> ImportStmt::get_imported_symbols() const{
    return this->imported_symbols;
}

Token ImportStmt::token() const{
    return this->tok;
}
AstKind ImportStmt::kind() const{
    return AstKind::ImportStmt;
}
std::string ImportStmt::stringify() const{
    std::string res = "import ";
    for (size_t i = 0; i < this->module_path.size(); i++){
        res += this->module_path[i].value;
        if (i != this->module_path.size() - 1){
            res += "::";
        }
    }
    if (!this->imported_symbols.empty()){
        res += "::{";
        for (size_t i = 0; i < this->imported_symbols.size(); i++){
            for (size_t j = 0; j < this->imported_symbols[i].size(); j++){
                res += this->imported_symbols[i][j].value;
                if (j != this->imported_symbols[i].size() - 1){
                    res += "::";
                }
            }
            if (i != this->imported_symbols.size() - 1){
                res += ", ";
            }
        }
        res += "}";
    }
    return res;
}


UsingStmt::UsingStmt(Token tok, std::vector<Token> path, std::optional<Token> alias){
    this->tok = tok;
    this->path = path;
    this->alias = alias;
}

std::vector<Token> UsingStmt::get_path() const{
    return this->path;
}
std::optional<Token> UsingStmt::get_alias() const{
    return this->alias;
}

Token UsingStmt::token() const{
    return this->tok;
}
AstKind UsingStmt::kind() const{
    return AstKind::UsingStmt;
}
std::string UsingStmt::stringify() const{
    std::string res = "using ";
    if (this->alias.has_value()){
        res += this->alias.value().value + " = ";
    }
    for (size_t i = 0; i < this->path.size(); i++){
        res += this->path[i].value;
        if (i != this->path.size() - 1){
            res += "::";
        }
    }
    return res;
}
}