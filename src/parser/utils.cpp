#include "lexer/token.hpp"
#include "parser/parser.hpp"
#include <cstddef>
#include <iostream>

namespace Luna {
void Parser::advance() {
    this->curr_index++;
    if (this->curr_index < this->toks.size()) {
        this->curr_tok = this->toks[this->curr_index];
    }
}
void Parser::advance_on_newline(){
    if(peek().type == TokenType::newline){
        this->advance();
    }
}
Token Parser::peek(std::size_t i) const {
    std::size_t index = this->curr_index + i;
    if (index < this->toks.size()) {
        return this->toks[index];
    }
    Token res;
    res.type = TokenType::eof;
    return res;
}
PrecedenceType Parser::peek_precedence(size_t i) const {
    auto it = this->precedence_map.find(peek(i).type);
    if (it == this->precedence_map.end()) {
        return PrecedenceType::pr_lowest;
    }
    return it->second;
}
void Parser::handle_angle_bracket(){
    //The curr_index is on token '<' or '<<'.
    std::vector<Token> new_tokens;
    size_t angle_bracket_count = 0;
    size_t i = 0;
    while(i < this->toks.size()){
        const Token tok = this->toks[i];
        if(i<this->curr_index){
            new_tokens.push_back(tok);
        }
        else{
            if(tok.type == TokenType::lt){
                new_tokens.push_back(tok);
                angle_bracket_count++;
            }
            else if(this->toks[i].type == TokenType::shl){
                new_tokens.push_back(Token{tok.col, tok.source_line,"<",
                                            tok.start,tok.start+1,tok.line, TokenType::lt});
                new_tokens.push_back(Token{tok.col+1, tok.source_line,"<",
                                            tok.start+1,tok.start+2,tok.line, TokenType::lt});
                angle_bracket_count+=2;
            }
            else if(tok.type == TokenType::gt){
                new_tokens.push_back(tok);
                angle_bracket_count--;//No need to check for negative count because angle_bracket_count is atleast 1 when we encounter '>'(Cuz or else we would have broken out of the loop already)
            }
            else if(tok.type == TokenType::shr){
                new_tokens.push_back(Token{tok.col, tok.source_line,">",
                                            tok.start,tok.start+1,tok.line, TokenType::gt});
                new_tokens.push_back(Token{tok.col+1, tok.source_line,">",
                                            tok.start+1,tok.start+2,tok.line, TokenType::gt});
                if(angle_bracket_count>=2){
                    angle_bracket_count-=2;
                }
                else{//i.e angle bracket count is 1 and we encounter '>>' which means the first '>' is closing the angle bracket and the second '>' is just a '>' token
                    angle_bracket_count--;
                }
            }
            else if(tok.type == TokenType::geq){
                new_tokens.push_back(Token{tok.col, tok.source_line,">",
                                            tok.start,tok.start+1,tok.line, TokenType::gt});
                new_tokens.push_back(Token{tok.col+1, tok.source_line,"=",
                                            tok.start+1,tok.start+2,tok.line, TokenType::assign});
                angle_bracket_count--;//No need to check for negative count because angle_bracket_count is atleast 1 when we encounter '>='(Cuz or else we would have broken out of the loop already)
            }
            else if(tok.type == TokenType::shr_eq){
                if(angle_bracket_count>=2){
                    new_tokens.push_back(Token{tok.col, tok.source_line,">",
                                                tok.start,tok.start+1,tok.line, TokenType::gt});
                    new_tokens.push_back(Token{tok.col+1, tok.source_line,">",
                                                tok.start+1,tok.start+2,tok.line, TokenType::gt});
                    new_tokens.push_back(Token{tok.col+2, tok.source_line,"=",
                                                tok.start+2,tok.start+3,tok.line, TokenType::assign});
                    angle_bracket_count-=2;
                }
                else{//i.e angle bracket count is 1 and we encounter '>>=' which means the first '>' is closing the angle bracket and the second '>' and '=' are just '>' and '=' tokens
                    new_tokens.push_back(Token{tok.col, tok.source_line,">",
                                                tok.start,tok.start+1,tok.line, TokenType::gt});
                    new_tokens.push_back(Token{tok.col+1, tok.source_line,">=",
                                                tok.start+1,tok.start+3,tok.line, TokenType::geq});
                    angle_bracket_count--;
                }
            }
            else{
                new_tokens.push_back(tok);
            }
            if(angle_bracket_count == 0){
                //We have processed all the angle brackets in the current token stream, so we can break out of the loop and update the tokens with the new tokens we have created to handle angle brackets
                i++;
                break;
            }
            
        }
        i++;
    }
    if(angle_bracket_count != 0){
        error(this->curr_tok, "Unmatched angle brackets in expression");
    }
    //Add the remaining tokens after the current token to the new_tokens vector
    while(i < this->toks.size()){
        new_tokens.push_back(this->toks[i]);
        i++;
    }
    this->toks = std::move(new_tokens);
    this->curr_tok = this->toks[this->curr_index];
}
void Parser::expect(TokenType expected_type, std::string msg,std::string submsg,std::string ecode) {
    //check if the next toke is what we expect or else show error
    if (peek().type != expected_type) {
        if(msg==""){
            msg="expected token of type " + to_string(expected_type) +", got " + to_string(peek().type) + " instead";
        }
        
        if(peek().type==TokenType::newline){
            error(this->curr_tok,msg,submsg,ecode);
        }
        else{
            error(peek(),msg,submsg,ecode);
        }
    }
    advance();
}

void Parser::error(Token tok, std::string msg,std::string submsg,std::string ecode) {
    //display error
    Location loc{tok.line, tok.col, this->filename, tok.source_line};
    Diagnostic err = {loc,
                   std::string(msg),
                   submsg,
                   ecode,
        };

    display(err);
    exit(1);
}
}