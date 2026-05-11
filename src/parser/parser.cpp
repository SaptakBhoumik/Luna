#include "parser/parser.hpp"

namespace Luna {
Parser::Parser(const std::vector<Token>& toks, const std::string& filename){
    this->toks = toks;
    this->filename = filename;
    if (!toks.empty()) {
        this->curr_tok = toks[0];
    }
    //TODO:Create the actual precedence map based on Luna's operator precedence rules
    this->precedence_map = {
        {TokenType::dot, PrecedenceType::pr_dot_arrow_ref},
        {TokenType::arrow, PrecedenceType::pr_dot_arrow_ref},
        //TODO: Finish it
    };
}

AstNodePtr Parser::parse(){
    if(this->toks.empty()){
        return std::make_shared<Program>(std::vector<AstNodePtr>{});
    }
    //TODO: Implement the actual parsing logic here. For now, just return an empty program.
    return std::make_shared<Program>(std::vector<AstNodePtr>{});
}
}