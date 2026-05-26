#pragma once

#include "ast/ast.hpp"
#include "ast/ast_utils.hpp"
#include "ast/visitor.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token.hpp"
#include <cstddef>

namespace Luna {
enum class PrecedenceType {
    pr_lowest,       // base — not an operator / default for unmapped tokens

    pr_arrow_block,  // =>   trailing-block call (Kotlin-style):  f => { } , f(x) => fn{ }
    pr_ternary,      // ?    ternary conditional:                  cond ? a : b
    pr_coalescing,   // ?? !!  null / error coalescing:            expr ?? default , expr !! handler
    // pr_select,       // <-- -->  channel recv / send inside select: v <-- ch , val --> ch
    pr_pipeline,     // |>   pipe operator (Elixir-style):         x |> f |> g
    pr_range,        // ..   range expression:                     0..10 , 0..10..2
    pr_or,           // |    bitwise-or / short-circuit logical-or (context-dependent)
    pr_xor,          // ^    bitwise xor
    pr_and,          // &    bitwise-and / short-circuit logical-and (context-dependent)
    pr_equality,     // == !=   equality comparison
    pr_compare,      // < > <= >=  relational comparison (tighter than equality, following C)
    pr_shift,        // << >>  bit shifts
    pr_sum,          // + -   additive
    pr_product,      // * / % @ multiplicative
    pr_power,        // **   exponentiation — RIGHT-associative: pass pr_product as rhs min
    pr_prefix,       // sentinel used as argument when recursing for prefix operands (-x, !x, ~x, $x, *x, &x)
    pr_postfix,      // ++ -- !   postfix ops (++ / -- / error-propagation !)
    pr_member,       // . -> member access, pointer member access
    pr_index,        // [   subscript / multi-index (__getitem__)
    pr_call,         // (   function / method call
    // pr_generic,      // {   generic instantiation: Foo{T} , func{T}(...)  — above call so {T} is eaten before (args)
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
    void handle_angle_bracket();
    void expect(TokenType expected_type, std::string msg="",std::string submsg="",std::string ecode="");
    void error(Token tok, std::string msg,std::string submsg="",std::string ecode="");

    //Parse utils
    Attribute parse_attribute();
    Decorator parse_decorator();
    Annotation parse_annotation();
    StructField parse_struct_field();
    std::pair<std::vector<Token>, bool> parse_path(bool allow_turbo_fish = false);//if allow_turbo_fish and turbo fish is detected then it will return at :: token before the <
    Parameter parse_parameter();
    CaptureClause parse_capture_clause();
    LambdaFuncSignature parse_lambda_signature();
    SelectArm parse_select_arm();
    std::vector<std::pair<Token, AstNodePtr>> parse_generic_params();

    // Parse literal nodes 
    AstNodePtr parse_int();
    AstNodePtr parse_decimal();
    AstNodePtr parse_string();
    AstNodePtr parse_bool();
    AstNodePtr parse_none();
    AstNodePtr parse_identifier(bool turbo_fish_required = true);//turbo_fish_required is true if u want generic to be like name::<T> and false if you want it to be like name<T> or name::<T>
    AstNodePtr parse_map_or_list();
    AstNodePtr parse_tuple_or_paren_expr();

    // Parse type expression nodes
    AstNodePtr parse_type_expr(bool can_be_sumtype = true);
    AstNodePtr parse_decltype_expr();
    AstNodePtr parse_identifier_type_expr();
    AstNodePtr parse_list_type_expr();
    AstNodePtr parse_ptr_optional_error_type_expr();
    AstNodePtr parse_func_type_expr();
    AstNodePtr parse_tuple_or_paren_type_expr();
    AstNodePtr parse_simd_type_expr();
    AstNodePtr parse_enum_type_expr();
    AstNodePtr parse_struct_type_expr();
    AstNodePtr parse_interface_type_expr();

    // Parse expression nodes
    AstNodePtr parse_expression(PrecedenceType precedence = PrecedenceType::pr_lowest);
    //We are on the op node for the following
    AstNodePtr parse_bin_op(AstNodePtr left);
    AstNodePtr parse_prefix_op();
    AstNodePtr parse_postfix_op(AstNodePtr left);
    AstNodePtr parse_coalescing_op(AstNodePtr left);
    AstNodePtr parse_range_expr(AstNodePtr left);
    AstNodePtr parse_index_expr(AstNodePtr container);
    AstNodePtr parse_dot_or_arrow_expr(AstNodePtr left);
    AstNodePtr parse_func_call(AstNodePtr left);
    AstNodePtr parse_ternary_expr(AstNodePtr condition);
    AstNodePtr parse_compile_time_expr();
    AstNodePtr parse_lambda_expr();
    AstNodePtr parse_formatted_string();
    AstNodePtr parse_thread_or_task_expr();
    AstNodePtr parse_arrow_block_call(AstNodePtr left);

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
    // AstNodePtr parse_import_stmt();
    // AstNodePtr parse_using_stmt();

    //Parse branch statement nodes
    AstNodePtr parse_when_stmt(std::vector<Annotation> annotations);
    AstNodePtr parse_loop_stmt(std::vector<Annotation> annotations);
    AstNodePtr parse_select_stmt(std::vector<Annotation> annotations);

    //Parse definition statement nodes
    AstNodePtr parse_type_def_stmt(std::vector<Annotation> annotations, bool is_pub);
    //For parse_var_or_import_stmt, we are either on =,:= or : token. Basically the token after the variable name(s)
    AstNodePtr parse_var_or_import_stmt(std::vector<Annotation> annotations, std::vector<std::pair<AstNodePtr, std::pair<bool, bool>>> names);
    AstNodePtr parse_aug_assign_stmt(std::vector<AstNodePtr> targets);//On the op token
    //For the following we sent when the curr tok is fn
    AstNodePtr parse_func_def(std::vector<Annotation> annotations, bool is_pub);
    AstNodePtr parse_method_def(std::vector<Annotation> annotations, bool is_pub);
    AstNodePtr parse_func_or_method_def(std::vector<Annotation> annotations, bool is_pub);

    public:
    Parser(const std::vector<Token>& toks, const std::string& filename);

    AstNodePtr parse();
};
}