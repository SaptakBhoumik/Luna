#pragma once

#include "ast/ast.hpp"
#include "ast/ast_utils.hpp"
#include "ast/visitor.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token.hpp"
#include <cstddef>

namespace Luna {
enum class PrecedenceType {
    //TODO: Finalize operator precedence levels
    pr_lowest,      // lowest possible precedence
    pr_range,       // ..
    pr_or,      // or
    pr_coalescing, // ?? and !!
    pr_and,      // and
    pr_not,         // not
    pr_compare,     // ==, !=, <, >, <=, >=
    pr_bit_or,      // |
    pr_bit_xor,     // ^
    pr_bit_and,     // &
    pr_bit_shift_pipeline,   // >> , <<
    pr_sum_minus,   // +, -
    pr_mul_div,     // *, /, %, //
    pr_expo,        // **
    pr_prefix,      // -x
    pr_dot_arrow_ref,     // x.test(), x.prop ,x->y
    pr_list_access, // x[0], x["test"]
    pr_call,         // x()
    pr_postfix      // x++
};

class Parser{
    std::size_t curr_index = 0;
    Token curr_tok;
    std::vector<Token> toks;
    std::string filename;
    std::map<TokenType, PrecedenceType> precedence_map;// We set the value in the constructor

    void advance();
    void advance_on_newline();
    Token peek(std::size_t i=1) const;// peek the token at curr_index + i without advancing
    PrecedenceType peek_precedence(size_t i=1) const;
    void expect(TokenType expected_type, std::string msg="",std::string submsg="",std::string ecode="");
    void error(Token tok, std::string msg,std::string submsg="",std::string ecode="");

    //Parse utils
    Attribute parse_attribute();
    StructField parse_struct_field();
    std::vector<Token> parse_path();

    // Parse literal nodes
    AstNodePtr parse_int();
    AstNodePtr parse_decimal();
    AstNodePtr parse_string();
    AstNodePtr parse_bool();
    AstNodePtr parse_none();
    AstNodePtr parse_identifier();
    AstNodePtr parse_map_or_list();
    AstNodePtr parse_tuple_or_paren_expr();

    // Parse type expression nodes
    AstNodePtr parse_type_expr(bool can_be_sumtype = false);
    AstNodePtr parse_identifier_type_expr();
    AstNodePtr parse_list_type_expr();
    AstNodePtr parse_ptr_optional_error_type_expr();
    AstNodePtr parse_func_type_expr();
    AstNodePtr parse_tuple_or_paren_type_expr();
    AstNodePtr parse_simd_type_expr();
    AstNodePtr parse_enum_type_expr();
    AstNodePtr parse_struct_type_expr();

    // Parse expression nodes
    AstNodePtr parse_expression(PrecedenceType precedence = PrecedenceType::pr_lowest);
    AstNodePtr parse_bin_op(AstNodePtr left);
    AstNodePtr parse_prefix_op();
    AstNodePtr parse_postfix_op(AstNodePtr left);
    AstNodePtr parse_coalescing_op(AstNodePtr left);
    AstNodePtr parse_range_expr(AstNodePtr left);
    AstNodePtr parse_index_expr(AstNodePtr container);
    AstNodePtr parse_dot_or_arrow_expr(AstNodePtr left);
    //TODO:

    // Parse statement nodes and blocks
    AstNodePtr parse_stmt();
    AstNodePtr parse_block();//Expects the current token to be lbrace('{'), and parses until the matching rbrace('}')

    // Parse simple statement nodes
    AstNodePtr parse_defer_stmt();
    AstNodePtr parse_scope_stmt();
    AstNodePtr parse_break_stmt();
    AstNodePtr parse_continue_stmt();
    AstNodePtr parse_return_stmt();
    AstNodePtr parse_give_stmt();
    AstNodePtr parse_lock_stmt();

    //Parse module statement nodes
    AstNodePtr parse_import_stmt();
    AstNodePtr parse_using_stmt();
    public:
    Parser(const std::vector<Token>& toks, const std::string& filename);

    AstNodePtr parse();
};
}