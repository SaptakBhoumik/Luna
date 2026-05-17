//TODO:THis entire file is really error prone. Test it properly
#include "ast/ast.hpp"
#include "ast/ast_utils.hpp"
#include "lexer/token.hpp"
#include "parser/parser.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace Luna {
AstNodePtr Parser::parse_when_stmt(){
    Token tok = this->curr_tok;
    std::vector<AstNodePtr> subjects;
    std::vector<std::pair<std::vector<std::vector<AstNodePtr>>, AstNodePtr>> branches;
    advance();
    if(this->curr_tok.type != TokenType::lbrace){
        if(this->curr_tok.type == TokenType::colon){
            error(this->curr_tok,"Expected condition before ':' in when statement");
        }
        // parse subjects
        while(this->curr_tok.type != TokenType::lbrace && this->curr_tok.type != TokenType::colon){
            subjects.push_back(parse_expression());
            if(peek().type == TokenType::comma){
                advance(); // on comma and continue parsing subjects
                advance(); // after comma
            }
            else if(peek().type == TokenType::lbrace || peek().type == TokenType::colon){
                advance(); // on '{' or ':' 
            }
            else{
                error(peek(), "Expected ',' or '{' after when statement subjects");
            }
        }
        if(this->curr_tok.type == TokenType::colon){
            expect(TokenType::lbrace,"Expected '{' after ':' in when statement");
            if(subjects.size() != 1){
                error(tok,"Error. Expected exactly one subject before ':' in when statement with ':'");
            }
            branches = {{{subjects}, parse_block()}};
            subjects = {};
            return std::make_shared<WhenStmt>(tok,subjects,branches);
        }
    }
    //On '{'
    advance_on_newline();
    advance();//No longer on '{' or '\n'. We on the Tok after it
    while(this->curr_tok.type != TokenType::rbrace){
        std::vector<std::vector<AstNodePtr>> __conditions_or_cases;
        while(this->curr_tok.type != TokenType::colon){
            std::vector<AstNodePtr> conditions_or_case;
            while(this->curr_tok.type != TokenType::colon && this->curr_tok.type != TokenType::newline){
                if(this->curr_tok.type == TokenType::question){
                    conditions_or_case.push_back(std::make_shared<NoLiteral>());
                }
                else{
                    conditions_or_case.push_back(parse_expression());
                }
                if(peek().type == TokenType::comma){
                    advance(); // on comma and continue parsing conditions/cases in the same branch
                    advance(); // after comma
                }
                else if(peek().type == TokenType::colon || peek().type == TokenType::lbrace || peek().type == TokenType::newline){
                    advance(); // on ':' or '{' or '\n'
                    break;
                }
                else{
                    error(peek(), "Expected ',' or ':' or '{' after condition/case in when statement");
                }
            }
            __conditions_or_cases.push_back(conditions_or_case);
            if(this->curr_tok.type == TokenType::newline){
                advance();
            }
            else if(this->curr_tok.type != TokenType::colon){
                error(peek(), "Expected newline or ':' after when condition/case");
            }
        }
        expect(TokenType::lbrace,"expected '{' before when statement body");
        branches.push_back({__conditions_or_cases, parse_block()});
        advance_on_newline();
        advance();
    }
    return std::make_shared<WhenStmt>(tok,subjects,branches);
}
AstNodePtr Parser::parse_loop_stmt(std::vector<Annotation> annotations){
    std::vector<Attribute> attributes;
    for(const auto& annotation : annotations){
        if(annotation.is_decorator){
            error(annotation.decorator.decorator->token(),"Decorators are not allowed on loop statements");
        }
        else{
            attributes.push_back(annotation.attribute);
        }
    }
    Token tok = this->curr_tok;
    std::vector<std::pair<AstNodePtr,bool>> variables = {};
    std::vector<AstNodePtr> values = {};
    advance();
    if(this->curr_tok.type == TokenType::lbrace){
        return std::make_shared<LoopStmt>(tok,LoopKind::Infinite,variables,values,parse_block(),attributes);
    }
    LoopKind loop_kind = LoopKind::WhileStyle;
    while(true){
        bool is_mut = false;
        if(this->curr_tok.type == TokenType::kw_mut){
            is_mut = true;
            loop_kind = LoopKind::IteratorFor;
            advance();
        }
        variables.push_back({parse_expression(),is_mut});
        if(peek().type == TokenType::comma){
            advance(); // on comma and continue parsing variables
            if(peek().type == TokenType::colon || peek().type == TokenType::lbrace){
                break;
            }
            advance(); //After comma
        }
        else{
            break;
        }
    }
    if((loop_kind == LoopKind::IteratorFor || variables.size() > 1) && peek().type != TokenType::colon){
        error(peek(),"expected ':' after loop variables in for loop");
    }
    if(peek().type == TokenType::colon){
        advance();
        advance();
        while(true) {
            values.push_back(parse_expression());
            if(peek().type == TokenType::comma){
                advance(); // on comma and continue parsing values
                if(peek().type == TokenType::lbrace){
                    break;
                }
                advance(); //After comma
            }
            else{
                break;
            }
        }
    }
    else{
        values = {variables[0].first};
        variables = {};
    }
    expect(TokenType::lbrace,"expected '{' before loop body");
    return std::make_shared<LoopStmt>(tok,loop_kind,variables,values,parse_block(),attributes);
}
AstNodePtr Parser::parse_select_stmt(){
    Token tok = this->curr_tok;
    expect(TokenType::lbrace,"expected '{' after 'select'");
    std::vector<std::pair<std::vector<SelectArm>, AstNodePtr>> cases;
    advance_on_newline();
    advance();
    while(this->curr_tok.type != TokenType::rbrace){
        std::vector<SelectArm> arms;
        while(this->curr_tok.type != TokenType::colon){
            arms.push_back(parse_select_arm());
            if(peek().type == TokenType::newline){
                advance();
                advance();
            }
            else if(peek().type == TokenType::colon){
                advance(); // on colon and break to parse the body of the case
            }
            else{
                error(peek(), "Expected newline or ':' after select arm");
            }
        }
        expect(TokenType::lbrace,"expected '{' before select arm body");
        cases.push_back({arms, parse_block()});
        advance_on_newline();
        advance();
    }
    return std::make_shared<SelectStmt>(tok,cases);
}
}