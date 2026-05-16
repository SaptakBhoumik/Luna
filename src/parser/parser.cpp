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
    std::vector<AstNodePtr> statements;
    while(this->curr_tok.type != TokenType::eof){
        statements.push_back(parse_stmt());
        advance();
        if(this->curr_tok.type == TokenType::newline){
            advance();
        }
        else if(this->curr_tok.type != TokenType::eof){
            error(this->curr_tok, "Expected newline after statement");
        }
    }
    return std::make_shared<Program>(statements);
}

AstNodePtr Parser::parse_block(){
    const Token block_tok = this->curr_tok;
    advance_on_newline();
    advance(); // after '{'
    std::vector<AstNodePtr> statements;
    while(this->curr_tok.type != TokenType::rbrace){
        if(this->curr_tok.type == TokenType::eof){
            error(this->curr_tok, "Unexpected end of file");
        }
        statements.push_back(parse_stmt());
        advance();
        if(this->curr_tok.type == TokenType::newline){
            advance();
        }
        else if(this->curr_tok.type != TokenType::rbrace){
            error(this->curr_tok, "Expected newline after statement in block");
        }
    }
    return std::make_shared<Block>(block_tok,statements);
}
}