#pragma once

#include "lexer/token.hpp"
#include "ast_utils.hpp"
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string>
#include <vector>
namespace Luna {
enum class AstKind {
    //Basic nodes
    Program,
    Block,
    NoLiteral,

    // Literal nodes
    IntegerLiteral,
    DecimalLiteral,
    StringLiteral,
    BoolLiteral,
    NoneLiteral,
    IdentifierLiteral,
    ListLiteral,
    DictLiteral,
    EmptyDictOrListLiteral,
    TupleLiteral,
    AssignTupleLiteral,//Stuff like (mut A,pub B.....)

    // Type nodes
    TypeExpr,
    ListTypeExpr,
    PtrTypeExpr,
    OptionalTypeExpr,
    ErrorTypeExpr,
    FuncTypeExpr,
    TupleTypeExpr,
    SimdTypeExpr,
    SumTypeExpr,
    EnumTypeExpr,
    StructTypeExpr,
    InterfaceTypeExpr,

    // Expression nodes
    BinOp,
    PrefixOp,
    PostfixOp,
    CoalescingOP,
    RangeExpr,
    IndexExpr,
    DotExpr,
    ArrowExpr,
    FuncCall,
    TernaryIf,
    CompTimeExpr,
    LambdaExpr,
    FormattedStr,
    ThreadOrTaskExpr,
    ArrowBlockCallExpr,

    // Simple Statement nodes
    DeferStmt,
    ScopeStmt,
    BreakStmt,
    ContinueStmt,
    ReturnStmt,
    GiveStmt,
    LockStmt,

    //Module statement nodes
    ImportStmt,
    UsingStmt,

    //Branch statement nodes
    WhenStmt,
    LoopStmt,
    SelectStmt,

    //Defination/Assignment statement nodes
    TypeDefStmt,
    VarStmt,
    AugAssignStmt,
    FuncDefStmt,
    MethodDefStmt,
};

// ---- Forward declarations ----

class AstVisitor;

class AstNode {
public:
    virtual ~AstNode() = default;

    virtual Token token() const = 0;
    virtual AstKind kind() const = 0;
    virtual std::string stringify() const = 0;

    virtual void accept(AstVisitor& visitor) const = 0;
};

using AstNodePtr = std::shared_ptr<AstNode>;

// ============================
//  Basic nodes
// ============================
class Program : public AstNode {
    std::vector<AstNodePtr> statements;
public:
    Program(std::vector<AstNodePtr> statements);

    std::vector<AstNodePtr> get_statements() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class Block : public AstNode {
    Token tok;
    std::vector<AstNodePtr> statements;
public:
    Block(Token tok, std::vector<AstNodePtr> statements);

    std::vector<AstNodePtr> get_statements() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// For absent optional AST fields
class NoLiteral : public AstNode {
public:
    NoLiteral() = default;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// ============================
//  Literal nodes
// ============================

class IntegerLiteral : public AstNode {
    Token tok;
public:
    IntegerLiteral(Token tok);

    std::string get_value() const;// Why std::string here? Because the literal might be too big to fit in built-in integer types, so we keep it as a string and 
                              // parse it into a big integer type later when needed.
    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class DecimalLiteral : public AstNode {
    Token tok;
public:
    DecimalLiteral(Token tok);

    std::string get_value() const;//Same reason for string as IntegerLiteral

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class StringLiteral : public AstNode {
    Token tok;
    bool raw;
public:
    StringLiteral(Token tok,bool raw);
    std::string get_value() const;
    bool is_raw() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class BoolLiteral : public AstNode {
    Token tok;
public:
    BoolLiteral(Token tok);
    bool get_value() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class NoneLiteral : public AstNode {
    Token tok;
public:
    NoneLiteral(Token tok);

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class IdentifierLiteral : public AstNode {
    Token tok;
    std::vector<Token> path;//The path. Like A::B::C will be ["A", "B", "C"]
    std::vector<AstNodePtr> generic_args; // populated if this identifier is something like func{generic_arg1, generic_arg2} but we dont call the funciton yet. 
    // This is just the identifier with generic args, the actual function call will be a separate FunctionCall node with this as the callee.
public:
    IdentifierLiteral(Token tok, std::vector<Token> path, std::vector<AstNodePtr> generic_args);

    std::vector<Token> get_path() const;
    std::vector<AstNodePtr> get_generic_args() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    
    void accept(AstVisitor& visitor) const;
};

class ListLiteral : public AstNode {
    Token tok;
    std::vector<AstNodePtr> elements;
public:
    ListLiteral(Token tok, std::vector<AstNodePtr> elements);
    std::vector<AstNodePtr> get_elements() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class DictLiteral : public AstNode {
    Token tok;
    std::vector<std::pair<AstNodePtr, AstNodePtr>> elements; // (key, value)
public:
    DictLiteral(Token tok,std::vector<std::pair<AstNodePtr, AstNodePtr>> elements);
    std::vector<std::pair<AstNodePtr, AstNodePtr>> get_elements() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class EmptyDictOrListLiteral : public AstNode {
    //Both empty dict and empty list is represented by [] and we have no way of knowing it is what until we do semantic analysis and see how it is used. 
    //So we use this node for both empty dict and empty list literals and later replace it with either DictLiteral or ListLiteral during semantic analysis.
    Token tok;
public:    
    EmptyDictOrListLiteral(Token tok);

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};
// (a, b, c) - expression tuple / multiple-return value
class TupleLiteral : public AstNode {
    Token tok;
    std::vector<AstNodePtr> elements;
public:
    TupleLiteral(Token tok, std::vector<AstNodePtr> elements);

    std::vector<AstNodePtr> get_elements() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};


// (pub a, mut b, c) - expression tuple / multiple-return value
class AssignTupleLiteral : public AstNode {
    Token tok;
    std::vector<AstNodePtr> elements;
    std::vector<std::pair<bool,bool>> is_pub_mut; // parallel to elements vector, first bool is true if pub, second bool is true if mut
public:
    AssignTupleLiteral(Token tok, std::vector<AstNodePtr> elements, std::vector<std::pair<bool,bool>> is_pub_mut);

    std::vector<AstNodePtr> get_elements() const;
    std::vector<std::pair<bool,bool>> get_is_pub_mut() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};
// ============================
//  Type expression nodes
// ============================

// Named type, optionally generic: i32, Stack{T}, Map{K,V}
class TypeExpr : public AstNode {
    Token tok;
    AstNodePtr value;//Will be a identifier expression node representing the type name and its generic args if any. 
    AstNodePtr idx; //When we do stuff like T[0] where T is a tuple type. T[0] represents the first type of T. And so on, NoLiteral if not applicable.
                    //It can also be a string as the idx. Like T["field"] for accessing the field type of a struct
                    //Note:-Only one itm as idx. Unlike regular indexing, a type expr can have only one idx
public:
    TypeExpr(Token tok, AstNodePtr value, AstNodePtr idx);

    AstNodePtr get_value() const;
    AstNodePtr get_idx() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    
    void accept(AstVisitor& visitor) const;
};

// [T, N] - fixed-size array; size is NoLiteral for unsized [T]
class ListTypeExpr : public AstNode {
    Token tok;
    AstNodePtr elem_type;
    AstNodePtr size;
public:
    ListTypeExpr(Token tok, AstNodePtr elem_type, AstNodePtr size);

    AstNodePtr get_elem_type() const;
    AstNodePtr get_size() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    
    void accept(AstVisitor& visitor) const;
};

// *T
class PtrTypeExpr : public AstNode {
    Token tok;
    AstNodePtr base_type;
public:
    PtrTypeExpr(Token tok, AstNodePtr base_type);

    AstNodePtr get_base_type() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// ?T
class OptionalTypeExpr : public AstNode {
    Token tok;
    AstNodePtr base_type;
public:
    OptionalTypeExpr(Token tok, AstNodePtr base_type);

    AstNodePtr get_base_type() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// !T - error-or-value type
class ErrorTypeExpr : public AstNode {
    Token tok;
    AstNodePtr base_type;
public:
    ErrorTypeExpr(Token tok, AstNodePtr base_type);

    AstNodePtr get_base_type() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// fn(T1, T2) -> T; return_type is NoLiteral for void
class FuncTypeExpr : public AstNode {
    Token tok;
    std::vector<AstNodePtr> param_types = {};
    bool c_variadic = false; // true if ... is present at the end of the parameter list
    AstNodePtr return_type;
public:
    FuncTypeExpr(Token tok, std::vector<AstNodePtr> param_types, bool c_variadic, AstNodePtr return_type);

    std::vector<AstNodePtr> get_param_types() const;
    AstNodePtr get_return_type() const;
    bool has_c_variadic() const;


    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// (T1, T2, T3) - tuple type
class TupleTypeExpr : public AstNode {
    Token tok;
    std::vector<AstNodePtr> elem_types;
public:
    TupleTypeExpr(Token tok, std::vector<AstNodePtr> elem_types);

    std::vector<AstNodePtr> get_elem_types() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// <T, N> - SIMD vector type
class SimdTypeExpr : public AstNode {
    Token tok;

    AstNodePtr elem_type;
    AstNodePtr lanes; // integer literal/compile time variable for lane count
    /*
    What I mean by only compile time variable is that we can have the following
    <f32,4>,<f32,$lanes>
    But not <f32, 2+2> or <f32, $some_func()>, <f32, $some_var + 1> and so on. 
    This is just done for making the parser simpler without reducing features 
    */
public:
    SimdTypeExpr(Token tok, AstNodePtr elem_type, AstNodePtr lanes);
    AstNodePtr get_elem_type() const;
    AstNodePtr get_lanes() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// T1 | T2 | T3 - sum / tagged-union type alias
class SumTypeExpr : public AstNode {
    Token tok;
    std::vector<AstNodePtr> variants;
public:
    SumTypeExpr(Token tok, std::vector<AstNodePtr> variants);
    std::vector<AstNodePtr> get_variants() const;
    
    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};


// enum { A = expr, B, ... }  or  enum:base_type { ... }
// variant value is NoLiteral when not explicitly set.
class EnumTypeExpr : public AstNode {
    Token tok;
    AstNodePtr base_type; // NoLiteral -> default underlying type
    std::vector<std::pair<Token, AstNodePtr>> variants; // (name, value)
public:
    EnumTypeExpr(Token tok, AstNodePtr base_type, std::vector<std::pair<Token, AstNodePtr>> variants);

    AstNodePtr get_base_type() const;
    std::vector<std::pair<Token, AstNodePtr>> get_variants() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// { fields... }
class StructTypeExpr : public AstNode {
    Token tok;
    std::vector<StructField> fields;
public:
    StructTypeExpr(Token tok, std::vector<StructField> fields);

    std::vector<StructField> get_fields() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class InterfaceTypeExpr : public AstNode {
    Token tok;
    std::vector<AstNodePtr> methods;//Forward declaration of method def. No pub or attributes. To keep things simple, reusing MethodDefStmt
public:
    InterfaceTypeExpr(Token tok, std::vector<AstNodePtr> methods);

    std::vector<AstNodePtr> get_methods() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};
// ============================
//  Expression/Operator nodes
// ============================
class BinOp : public AstNode {
    Token tok;
    AstNodePtr left;
    Token op;
    AstNodePtr right;
public:
    BinOp(Token tok, AstNodePtr left, Token op, AstNodePtr right);

    AstNodePtr get_left() const;
    Token get_op() const;
    AstNodePtr get_right() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class PrefixOp : public AstNode {
    Token tok;
    Token prefix;
    AstNodePtr right;
public:
    PrefixOp(Token tok, Token prefix, AstNodePtr right);
    Token get_prefix() const;
    AstNodePtr get_right() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// Covers ++, --, ! (error propagation)
class PostfixOp : public AstNode {
    Token tok;
    Token postfix;//Must be the operator token
    AstNodePtr left;
public:
    PostfixOp(Token tok, Token postfix, AstNodePtr left);
    Token get_postfix() const;
    AstNodePtr get_left() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

//expr ?? block , expr !! block
class CoalescingOP : public AstNode {
    Token tok;
    AstNodePtr left;
    AstNodePtr right;
    bool null_coalescing; // true for ??, false for !!
    std::optional<std::pair<Token,bool>> error_var_name; // populated for !! (err){...} form, empty otherwise. The pair is (variable name, is_mutable)
public:
    CoalescingOP(Token tok, AstNodePtr left, AstNodePtr right, bool null_coalescing, std::optional<std::pair<Token,bool>> error_var_name);

    AstNodePtr get_left() const;
    AstNodePtr get_right() const;
    bool is_null_coalescing() const;
    std::optional<std::pair<Token,bool>> get_error_var_name() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class RangeExpr : public AstNode {
    Token tok;
    AstNodePtr start;
    AstNodePtr end;   
    AstNodePtr step;  // NoLiteral for default step of 1
public:
    RangeExpr(Token tok, AstNodePtr start, AstNodePtr end, AstNodePtr step);
    AstNodePtr get_start() const;
    AstNodePtr get_end() const;
    AstNodePtr get_step() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// a[i]  or  a[i, j]  (multi-index for magic __getitem__)
class IndexExpr : public AstNode {
    Token tok;
    AstNodePtr container;
    std::vector<AstNodePtr> indices;
public:
    IndexExpr(Token tok, AstNodePtr container,std::vector<AstNodePtr> indices);

    AstNodePtr get_container() const;
    std::vector<AstNodePtr> get_indices() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    void accept(AstVisitor& visitor) const;
};

// a.b
class DotExpr : public AstNode {
    Token tok;
    AstNodePtr owner;
    Token member;
public:
    DotExpr(Token tok, AstNodePtr owner, Token member);
    AstNodePtr get_owner() const;
    Token get_member() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    void accept(AstVisitor& visitor) const;
};

// a->b  (pointer member access)
class ArrowExpr : public AstNode {
    Token tok;
    AstNodePtr owner;
    Token member;
public:
    ArrowExpr(Token tok, AstNodePtr owner, Token member);
    AstNodePtr get_owner() const;
    Token get_member() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class FuncCall : public AstNode {
    Token tok;
    AstNodePtr callee;
    std::vector<AstNodePtr> args;
    std::vector<std::pair<Token, AstNodePtr>> named_args; // for calls with named arguments
public:
    FuncCall(Token tok, AstNodePtr callee,std::vector<AstNodePtr> args, std::vector<std::pair<Token, AstNodePtr>> named_args);

    AstNodePtr get_callee() const;
    std::vector<AstNodePtr> get_arguments() const;
    std::vector<std::pair<Token, AstNodePtr>> get_named_arguments() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// condition ? then_value : else_value
class TernaryIf : public AstNode {
    Token tok;
    AstNodePtr condition;
    AstNodePtr then_value;
    AstNodePtr else_value;
public:
    TernaryIf(Token tok, AstNodePtr condition, AstNodePtr then_value,AstNodePtr else_value);
    
    AstNodePtr get_condition() const;
    AstNodePtr get_then_value() const;
    AstNodePtr get_else_value() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// $expr - any compile-time expression ($typesize, $typeid, $has_method, etc. Even for compile time functions,loop,when and stuff)
class CompTimeExpr : public AstNode {
    Token tok;
    AstNodePtr expr_or_stmt;
public:
    CompTimeExpr(Token tok, AstNodePtr expr_or_stmt);
    AstNodePtr get_expr_or_stmt() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    
    void accept(AstVisitor& visitor) const;
};

// fn(params)[capture] -> ret_type { body }
class LambdaExpr : public AstNode {
    Token tok;
    CaptureClause capture;
    std::vector<Parameter> parameters;//Note for lambda parameter:-Only regular parameters without named argument is allowed
    AstNodePtr return_type; // NoLiteral if inferred to be void
    AstNodePtr body;
public:
    LambdaExpr(Token tok, CaptureClause capture, std::vector<Parameter> parameters,AstNodePtr return_type, AstNodePtr body);

    CaptureClause get_capture() const;
    std::vector<Parameter> get_parameters() const;
    AstNodePtr get_return_type() const;
    AstNodePtr get_body() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    
    void accept(AstVisitor& visitor) const;
};

// f"text {expr} more text"
// parts alternates: StringLiteral segments and interpolated expression nodes
class FormattedStr : public AstNode {
    Token tok;
    std::vector<Token> string_parts; // The literal string segments, in order. Will have one more element than embedded_expr
    std::vector<AstNodePtr> embedded_expr;
    //The above 2 just alternates. Like for f"Hello {name}, you are {age} years old!" string_parts will be ["Hello ", ", you are ", " years old!"] and 
    //embedded_expr will be [IdentifierLiteral(name), IdentifierLiteral(age)]
public:
    FormattedStr(Token tok, std::vector<Token> string_parts, std::vector<AstNodePtr> embedded_expr);

    std::vector<Token> get_string_parts() const;
    std::vector<AstNodePtr> get_embedded_expr() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};


// thread { body }  or  thread single_expr or task { body } or task single_expr
// restart_count / timeout / restart_delay are NoLiteral when absent
class ThreadOrTaskExpr : public AstNode {
    Token tok;
    bool thread; // true for thread, false for task
    AstNodePtr body;
    AstNodePtr restart_count;
    AstNodePtr timeout;
    AstNodePtr always_restart;
    AstNodePtr restart_delay;
public:
    ThreadOrTaskExpr(Token tok, bool thread, AstNodePtr body,
                     AstNodePtr restart_count, AstNodePtr timeout,
                     AstNodePtr always_restart, AstNodePtr restart_delay);
    
    bool is_thread() const;
    AstNodePtr get_body() const;
    AstNodePtr get_restart_count() const;
    AstNodePtr get_timeout() const;
    AstNodePtr get_always_restart() const;
    AstNodePtr get_restart_delay_mss() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

//Something like func => {...} or func(args) => {...} 
class ArrowBlockCallExpr:public AstNode{
    Token tok;
    AstNodePtr callee; // For func(args) => {...} case we need it callee to store ``func`` and not the whole func(args) as callee because dont make sense
    std::vector<AstNodePtr> args; // empty for func => {...}
    std::vector<std::pair<Token, AstNodePtr>> named_args; // empty for func => {...}
    AstNodePtr body;//THis is a lamda defination. 
    /*
    For something like the following:-
    _test(123) => fn(a:i8)[=]->i8{

    }

    This is a lambda. Parse it like any other lambda

    But dor something like the following:-
    _test => {

    }

    We still store it in LambdaExpr where capture is [] and no parameter and void return type
    */
public:
    ArrowBlockCallExpr(Token tok, AstNodePtr callee, std::vector<AstNodePtr> args, std::vector<std::pair<Token, AstNodePtr>> named_args, AstNodePtr body);

    AstNodePtr get_callee() const;
    std::vector<AstNodePtr> get_arguments() const;
    std::vector<std::pair<Token, AstNodePtr>> get_named_arguments() const;
    AstNodePtr get_body() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};


// ============================
//  Simple Statement nodes
// ============================
class DeferStmt : public AstNode {
    Token tok;
    AstNodePtr body;//A block or a single expression statement. Like the following are all valid defer statements:-
    /*
    defer {
        //some statements
    }
    defer some_function()
*/
public:
    DeferStmt(Token tok, AstNodePtr body);

    AstNodePtr get_body() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class ScopeStmt : public AstNode {
    Token tok;
    AstNodePtr body;
public:
    ScopeStmt(Token tok, AstNodePtr body);
    
    AstNodePtr get_body() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class BreakStmt : public AstNode {
    Token tok;
public:
    BreakStmt(Token tok);
    
    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class ContinueStmt : public AstNode {
    Token tok;
public:
    ContinueStmt(Token tok);

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// ret expr  (expr is NoLiteral for bare `ret`)
class ReturnStmt : public AstNode {
    Token tok;
    std::vector<AstNodePtr> values;
public:
    ReturnStmt(Token tok, std::vector<AstNodePtr> values);

    std::vector<AstNodePtr> get_values() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// give expr - value-yielding exit from a ?? / !! handler block
class GiveStmt : public AstNode {
    Token tok;
    std::vector<AstNodePtr> values;
public:
    GiveStmt(Token tok, std::vector<AstNodePtr> values);

    std::vector<AstNodePtr> get_values() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    void accept(AstVisitor& visitor) const;
};

// lock var { body }  or  lock (var1, var2) { body } or lock {body}
class LockStmt : public AstNode {
    Token tok;
    AstNodePtr target;//Either a tuple of identifiers or an identifier. It can also be no literal
    AstNodePtr body;
public:
    LockStmt(Token tok, AstNodePtr target, AstNodePtr body);

    AstNodePtr get_target() const;
    AstNodePtr get_body() const;
    
    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    
    void accept(AstVisitor& visitor) const;
};

// ============================
//  Module statement nodes
// ============================

// import path::sym  or  import path::{s1, s2}  
class ImportStmt : public AstNode {
    Token tok;
    std::vector<Token> module_path;
    std::vector<std::vector<Token>> imported_symbols;
    /*
    import std::io::{println, print}
    For the above imported_symbols = {{println}, {print}} and module_path = {std, io}

    import os::{platform::Linux}
    For the above imported_symbols = {{platform, Linux}} and module_path = {os}

    Basically each element of imported symbols is the path of the imported symbol
    */
public:
    ImportStmt(Token tok, std::vector<Token> module_path, std::vector<std::vector<Token>> imported_symbols);

    std::vector<Token> get_module_path() const;
    std::vector<std::vector<Token>> get_imported_symbols() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// using alias = path  or  using path  (alias is NoLiteral for the second form)
class UsingStmt : public AstNode {
    /*
    For something like ``fmt::println``, u can only do ``using fmt`` or ``using f = fmt`` 
    U cant do ``using fmt::println``. Because using is meant for bringing modules into scope and fmt::println is not a module but a function.
    */
    Token tok;
    std::vector<Token> path;
    std::optional<Token> alias;
public:
    UsingStmt(Token tok, std::vector<Token> path, std::optional<Token> alias);

    std::vector<Token> get_path() const;
    std::optional<Token> get_alias() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};


// ============================
//  Branch statement nodes
// ============================

// when { cond1 { body1 }  cond2 { body2 }  ? { else_body } }
// or  when expr { case1: body1  case2: body2  ? : else_body }
class WhenStmt : public AstNode {
    Token tok;
    std::vector<AstNodePtr> subjects;//For switch style when statement. IT will be empty for if-else style when statement
    /*
    when expr {
        case1
        case2:
        {
            //body for both case1 and case2
        }
        case3:
        {
            //body for case3
        }
    }
    Here the subjects will be [expr] and branches will be [([case1, case2], body_for_case1_and_case2), ([case3], body_for_case3)]
    when {
        cond1 {
            //body1 
        }
    }
    Here the subjects will be empty and branches will be [([cond1], body1)]
    */
    std::vector<std::pair<std::vector<std::vector<AstNodePtr>>, AstNodePtr>> branches;//<conditions,body> pairs. or <cases, body> 
    /*
        when p, q {
            3, 5 
            1, 2:{
                //body for both 3,5 and 1,2 cases
            }
            1, ?:{
                //body for 1,? case
            }
            ?:{ 
                //else body
            }
        }
        branches = {
            {{{3, 5},{1, 2}}, body_for_3_5_and_1_2},
            {{{1, ?}}, body_for_1_and_wildcard},
            {{{?}}, else_body}
        }

        The ? is represented by a NoLiteral.

        when {
            cond1 {
                //body1
            }
            cond2
            cond3 {
                //body for both cond2 and cond3
            }
            ? {
                //else body
            }
        }
        branches = {
            {{{cond1}}, body1},
            {{{cond2}, {cond3}}, body_for_cond2_and_cond3},
            {{{?}}, else_body}
        }
    */
public:
    WhenStmt(Token tok, std::vector<AstNodePtr> subjects, std::vector<std::pair<std::vector<std::vector<AstNodePtr>>, AstNodePtr>> branches);

    std::vector<AstNodePtr> get_subjects() const;
    std::vector<std::pair<std::vector<std::vector<AstNodePtr>>, AstNodePtr>> get_branches() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};



// Fields that are irrelevant for a given LoopKind hold NoLiteral.
class LoopStmt : public AstNode {
    Token tok;
    LoopKind loop_kind;
    std::vector<std::pair<AstNodePtr,bool>> variables; // loop variable; empty for Infinite/WhileStyle. vector of (variable/expression,is mutable)
    std::vector<AstNodePtr> values;// Array of iterable for `for` loop and condition for `while`, no literal for infinite loop
    AstNodePtr body;
    std::vector<Attribute> attributes; // #[parallel], #[simd], etc.
public:
    LoopStmt(Token tok, LoopKind loop_kind,std::vector<std::pair<AstNodePtr,bool>> variables,std::vector<AstNodePtr> values,AstNodePtr body,std::vector<Attribute> attributes);

    LoopKind get_loop_kind() const;
    std::vector<std::pair<AstNodePtr,bool>> get_variables() const;
    std::vector<AstNodePtr> get_values() const;
    AstNodePtr get_body() const;
    std::vector<Attribute> get_attributes() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// select { arm1  arm2  ... }
class SelectStmt : public AstNode {
    Token tok;
    std::vector<std::pair<std::vector<SelectArm>, AstNodePtr>> cases;
    //The above is a vector of (arms, body) pairs. Each arm is a separate case that can trigger the same body. Like for the following select statement:-
    /*
    select {
        case1:
        case2:
        {
            //body for both case1 and case2
        }
        case3:
        {
            //body for case3
        }
    }
    */
public:
    SelectStmt(Token tok, std::vector<std::pair<std::vector<SelectArm>, AstNodePtr>> cases);

    std::vector<std::pair<std::vector<SelectArm>, AstNodePtr>> get_cases() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// ============================
//  Defination/Assignment statement nodes
// ============================
// type Name{generics} = base_type
class TypeDefStmt : public AstNode {
    Token tok;
    std::vector<Attribute> attributes; // e.g. #[align(16)]
    bool pub = false;
    Token name;
    std::vector<Token> generics;
    AstNodePtr base_type; //No literal for opaque type defination like `type Name{generics}`
public:
    TypeDefStmt(Token tok, std::vector<Attribute> attributes, bool pub, Token name, std::vector<Token> generics, AstNodePtr base_type);
    
    std::vector<Attribute> get_attributes() const;
    bool is_pub() const;
    Token get_name() const;
    std::vector<Token> get_generics() const;
    AstNodePtr get_base_type() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// Covers all variable introduction forms:
//   x:T
//   x := expr
//   pub x:T = expr
//   mut x:T = expr
//   thread_local mut x:T = expr
//   task_local mut x:T = expr
//   A,mut B = expr1, expr2
// type is NoLiteral when inferred; value is NoLiteral when uninitialized.
class VarStmt : public AstNode {
    Token tok;
    std::vector<std::pair<AstNodePtr, std::pair<bool, bool>>> names;//Can be tuple, list access or dot access etc etc. (expr,(is_pub,is_mut))
    AstNodePtr type;
    std::vector<AstNodePtr> values;
    /*pub, mut if the mut,pub exists at the variable level. That is for something like the following:-
    ``pub (x,y) = 5,6``
    we set pub = true
    for something like
    ``pub x,y = 5,6`` or ``x,pub y = 5,6`` or ``(pub x,pub y) = 5,6`` or ``(x,pub y) = 5,6`` or ``(pub x,y) = 5,6``
    we set pub = false
    Same logic applies to mut as well
    */
    bool pub = false;//If pub
    bool mut = false;//If mut
    bool def = false;//Means if it is an assignment or defination. IF false then the type and stuff is no literal but the type checker will fill that up later. 
                     //Like for the following statement:- x = 5. It is an assignment and not a defination. So is_def will be false and type and value will be 
                     //no literal. But the type checker will fill the type as i32 and value as 5 later when it processes this statement.
    VarKind varkind = VarKind::Normal;
    std::vector<Attribute> attributes;
public:
    VarStmt(Token tok, std::vector<std::pair<AstNodePtr, std::pair<bool, bool>>> names, AstNodePtr type, std::vector<AstNodePtr> values,
            bool is_mut, bool is_def, bool pub, VarKind varkind, std::vector<Attribute> attributes);
    
    std::vector<std::pair<AstNodePtr, std::pair<bool, bool>>> get_names() const;
    AstNodePtr get_var_type() const;
    std::vector<AstNodePtr> get_values() const;
    bool is_pub() const;
    bool is_mut() const;
    bool is_def() const;
    VarKind get_varkind() const;
    std::vector<Attribute> get_attributes() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};


class AugAssignStmt : public AstNode {
    Token tok;
    Token op;
    std::vector<AstNodePtr> targets;
    std::vector<AstNodePtr> values;
public:
    AugAssignStmt(Token tok, Token op, std::vector<AstNodePtr> targets, std::vector<AstNodePtr> values);

    std::vector<AstNodePtr> get_targets() const;
    Token get_op() const;
    std::vector<AstNodePtr> get_values() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// fn name{generics}(params) -> return_type { body }
class FuncDefStmt : public AstNode {
    Token tok;
    bool pub;
    Token name;
    std::vector<Token> generics;
    std::vector<Parameter> parameters;
    AstNodePtr return_type; // NoLiteral for void / inferred
    AstNodePtr body;// NoLiteral for forward declaration
    std::vector<Annotation> annotation;
public:
    FuncDefStmt(Token tok, bool pub, Token name, std::vector<Token> generics,std::vector<Parameter> parameters,AstNodePtr return_type, 
                AstNodePtr body, std::vector<Annotation> annotation);
    
    bool is_pub() const;
    Token get_name() const;
    std::vector<Token> get_generics() const;
    std::vector<Parameter> get_parameters() const;
    AstNodePtr get_return_type() const;
    AstNodePtr get_body() const;
    std::vector<Annotation> get_annotation() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// fn (receiver) name{generics}(params) -> return_type { body }
class MethodDefStmt : public AstNode {
    Token tok;
    bool pub;
    Parameter receiver;//Only normal type with no default value
    Token name;
    std::vector<Token> generics;
    std::vector<Parameter> parameters;
    AstNodePtr return_type;
    AstNodePtr body;// NoLiteral for forward declaration
    std::vector<Attribute> attributes;//A method cant have decorator
public:
    MethodDefStmt(Token tok, bool pub, Parameter receiver, Token name, std::vector<Token> generics, std::vector<Parameter> parameters, 
                  AstNodePtr return_type, AstNodePtr body, std::vector<Attribute> attributes);

    bool is_pub() const;
    Parameter get_receiver() const;
    Token get_name() const;
    std::vector<Token> get_generics() const;
    std::vector<Parameter> get_parameters() const;
    AstNodePtr get_return_type() const;
    AstNodePtr get_body() const;
    std::vector<Attribute> get_attributes() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};
} // namespace ast