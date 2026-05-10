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
    TupleLiteral,

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
    GenericCall,
    TernaryIf,
    TernaryFor,
    CastExpr,
    CompTimeExpr,
    LambdaExpr,
    FormattedStr,
    ThreadOrTaskExpr,
    ArrowBlockCallExpr,

    // Statement nodes
    ImportStmt,
    UsingStmt,
    VariableStmt,
    MultipleAssign,
    AugAssign,
    FunctionDef,
    MethodDef,
    ExternFuncDef,
    ReturnStmt,
    GiveStmt,
    DeferStmt,
    IfStmt,
    LoopStmt,
    BreakStmt,
    ContinueStmt,
    MatchStmt,
    TypeDefinition,
    DecoratorStmt,
    PubDef,
    InlineAsm,
    LockStmt,
    SelectStmt,
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
    StringLiteral(Token tok,bool raw = false);
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
    std::vector<std::string> path;//The path. Like A::B::C will be ["A", "B", "C"]
    std::vector<AstNodePtr> generic_args; // populated if this identifier is something like func{generic_arg1, generic_arg2} but we dont call the funciton yet. 
    // This is just the identifier with generic args, the actual function call will be a separate FunctionCall node with this as the callee.
public:
    IdentifierLiteral(Token tok, std::vector<std::string> path, std::vector<AstNodePtr> generic_args = {});

    std::vector<std::string> get_path() const;
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
    ListLiteral(Token tok, std::vector<AstNodePtr> elements = {});
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
    AstNodePtr lanes; // integer literal/compile time for lane count
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


// enum { A = expr, B, ... }  or  type Name = enum:base_type { ... }
// variant value is NoLiteral when not explicitly set.
class EnumTypeExpr : public AstNode {
    Token tok;
    AstNodePtr base_type; // NoLiteral -> default underlying type
    std::vector<std::pair<AstNodePtr, AstNodePtr>> variants; // (name, value)
public:
    EnumTypeExpr(Token tok, AstNodePtr base_type, std::vector<std::pair<AstNodePtr, AstNodePtr>> variants);

    AstNodePtr get_base_type() const;
    std::vector<std::pair<AstNodePtr, AstNodePtr>> get_variants() const;

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
public:
    CoalescingOP(Token tok, AstNodePtr left, AstNodePtr right, bool null_coalescing);

    AstNodePtr get_left() const;
    AstNodePtr get_right() const;
    bool is_null_coalescing() const;

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
    AstNodePtr member;
public:
    DotExpr(Token tok, AstNodePtr owner, AstNodePtr member);
    AstNodePtr get_owner() const;
    AstNodePtr get_member() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    void accept(AstVisitor& visitor) const;
};

// a->b  (pointer member access)
class ArrowExpr : public AstNode {
    Token tok;
    AstNodePtr owner;
    AstNodePtr member;
public:
    ArrowExpr(Token tok, AstNodePtr owner, AstNodePtr member);
    AstNodePtr get_owner() const;
    AstNodePtr get_member() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class FuncCall : public AstNode {
    Token tok;
    AstNodePtr callee;
    std::vector<AstNodePtr> args;
    std::map<std::string, AstNodePtr> named_args; // for calls with named arguments
public:
    FuncCall(Token tok, AstNodePtr callee,std::vector<AstNodePtr> args, std::map<std::string, AstNodePtr> named_args);

    AstNodePtr get_callee() const;
    std::vector<AstNodePtr> get_arguments() const;
    std::map<std::string, AstNodePtr> get_named_arguments() const;

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
    Token m_token;
    AstNodePtr m_expression;
public:
    CompTimeExpr(Token tok, AstNodePtr expression);
    AstNodePtr expression() const;

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
    std::vector<std::string> string_parts; // The literal string segments, in order. Will have one more element than embedded_expr
    std::vector<AstNodePtr> embedded_expr;
    //The above 2 just alternates. Like for f"Hello {name}, you are {age} years old!" string_parts will be ["Hello ", ", you are ", " years old!"] and 
    //embedded_expr will be [IdentifierLiteral(name), IdentifierLiteral(age)]
public:
    FormattedStr(Token tok, std::vector<std::string> string_parts, std::vector<AstNodePtr> embedded_expr);

    std::vector<std::string> get_string_parts() const;
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
    std::vector<Parameter> args; // empty for func => {...}
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
    ArrowBlockCallExpr(Token tok, AstNodePtr callee, std::vector<Parameter> args, AstNodePtr body);

    AstNodePtr get_callee() const;
    std::vector<Parameter> get_arguments() const;
    AstNodePtr get_body() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// ============================
//  Statement nodes
// ============================

// import path::sym  or  import path::{s1, s2}  
class ImportStatement : public AstNode {
    Token m_token;
    std::vector<std::string> m_module_path;
    std::vector<std::string> m_imported_symbols;
public:
    ImportStatement(Token tok, std::vector<std::string> module_path, std::vector<std::string> imported_symbols);
    std::vector<std::string> modulePath() const;
    std::vector<std::string> importedSymbols() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// using alias = path  or  using path  (alias is NoLiteral for the second form)
class UsingStatement : public AstNode {
    Token m_token;
    std::vector<std::string> m_path;
    std::optional<std::string> m_alias;
public:
    UsingStatement(Token tok, std::vector<std::string> path, std::optional<std::string> alias);
    std::vector<std::string> path() const;
    std::optional<std::string> alias() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};


enum class VarKind {
    Normal,
    Atomic,
    ThreadLocal,
    TaskLocal,
};

// Covers all variable introduction forms:
//   x := expr
//   x:T = expr
//   mut x:T = expr
//   atomic mut x:T = expr
//   thread_local mut x:T = expr
//   task_local mut x:T = expr
// type is NoLiteral when inferred; value is NoLiteral when uninitialized.
class VariableStatement : public AstNode {
    Token m_token;
    AstNodePtr m_name;
    AstNodePtr m_type;
    AstNodePtr m_value;
    bool m_is_mut   = false;
    bool m_is_def = false;//Means if it is an assignment or defination
    VarKind m_var_kind = VarKind::Normal;
    std::vector<Attribute> m_attributes;
public:
    VariableStatement(Token tok, AstNodePtr name, AstNodePtr type,
                      AstNodePtr value, bool is_mut, bool is_def,
                      VarKind var_kind = VarKind::Normal,
                      std::vector<Attribute> attributes = {});
    AstNodePtr name() const;
    AstNodePtr varType() const;
    AstNodePtr value() const;
    bool isMut() const;
    bool isDef() const;
    VarKind varKind() const;
    std::vector<Attribute> attributes() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};


class AugAssign : public AstNode {
    Token m_token;
    Token m_op;
    AstNodePtr m_target;
    AstNodePtr m_value;
public:
    AugAssign(Token tok, Token op, AstNodePtr target, AstNodePtr value);
    AstNodePtr target() const;
    Token op() const;
    AstNodePtr value() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// fn name{generics}(params) -> return_type { body }
// body is NoLiteral for a forward declaration.
class FunctionDefinition : public AstNode {
    Token m_token;
    std::string m_name;
    std::vector<std::string> m_generics;
    std::vector<Parameter> m_parameters;
    AstNodePtr m_return_type; // NoLiteral for void / inferred
    AstNodePtr m_body;
    std::vector<Attribute> m_attributes;
public:
    FunctionDefinition(Token tok, std::string name,
                       std::vector<std::string> generics,
                       std::vector<Parameter> parameters,
                       AstNodePtr return_type, AstNodePtr body,
                       std::vector<Attribute> attributes = {});
    std::string name() const;
    std::vector<std::string> generics() const;
    std::vector<Parameter> parameters() const;
    AstNodePtr returnType() const;
    AstNodePtr body() const;
    std::vector<Attribute> attributes() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// fn (receiver) name{generics}(params) -> return_type { body }
class MethodDefinition : public AstNode {
    Token m_token;
    Parameter m_receiver;//Only normal type with no default value
    std::string m_name;
    std::vector<std::string> m_generics;
    std::vector<Parameter> m_parameters;
    AstNodePtr m_return_type;
    AstNodePtr m_body;
    std::vector<Attribute> m_attributes;
public:
    MethodDefinition(Token tok, Parameter receiver, std::string name,
                     std::vector<std::string> generics,
                     std::vector<Parameter> parameters,
                     AstNodePtr return_type, AstNodePtr body,
                     std::vector<Attribute> attributes = {});
    Parameter receiver() const;
    std::string name() const;
    std::vector<std::string> generics() const;
    std::vector<Parameter> parameters() const;
    AstNodePtr returnType() const;
    AstNodePtr body() const;
    std::vector<Attribute> attributes() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// ret expr  (expr is NoLiteral for bare `ret`)
class ReturnStatement : public AstNode {
    Token m_token;
    AstNodePtr m_value;
public:
    ReturnStatement(Token tok, AstNodePtr value);
    AstNodePtr value() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// give expr - value-yielding exit from a ?? / !! handler block
class GiveStatement : public AstNode {
    Token m_token;
    AstNodePtr m_value;
public:
    GiveStatement(Token tok, AstNodePtr value);
    AstNodePtr value() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    void accept(AstVisitor& visitor) const;
};

// defer { body }
class DeferStatement : public AstNode {
    Token m_token;
    AstNodePtr m_body;
public:
    DeferStatement(Token tok, AstNodePtr body);
    AstNodePtr body() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// if { cond1 { body1 }  cond2 { body2 }  ? { else_body } }
// Each branch is (condition, body); condition is NoLiteral for the ? (else) arm.
class IfStatement : public AstNode {
    Token m_token;
    std::vector<std::pair<AstNodePtr, AstNodePtr>> m_branches;
public:
    IfStatement(Token tok, std::vector<std::pair<AstNodePtr, AstNodePtr>> branches);
    std::vector<std::pair<AstNodePtr, AstNodePtr>> branches() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// Unified loop node - all forms use the single `loop` keyword.
enum class LoopKind {
    Infinite,    // loop { }
    WhileStyle,  // loop cond { }
    IteratorFor, // loop v :iter  or  loop k, v :iter
};

// Fields that are irrelevant for a given LoopKind hold NoLiteral.
class LoopStatement : public AstNode {
    Token m_token;
    LoopKind m_loop_kind;
    std::vector<AstNodePtr> m_variables; // loop variable(s); empty for Infinite/WhileStyle. Not a string cuz u can have complex stuff like tuples and all
    AstNodePtr m_value;// Iterable for `for` loop and condition for `while`, no literal for infinite loop
    AstNodePtr m_body;
    std::vector<Attribute> m_attributes; // @[parallel], @[simd], etc.
public:
    LoopStatement(Token tok, LoopKind loop_kind,
                  std::vector<AstNodePtr> variables,
                  AstNodePtr value,
                  AstNodePtr body,
                  std::vector<Attribute> attributes = {});
    LoopKind loopKind() const;
    std::vector<AstNodePtr> variables() const;
    AstNodePtr value() const;
    AstNodePtr body() const;
    std::vector<Attribute> attributes() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};


class BreakStatement : public AstNode {
    Token m_token;
public:
    BreakStatement(Token tok);
    
    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class ContinueStatement : public AstNode {
    Token m_token;
public:
    ContinueStatement(Token tok);

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// match subj1, subj2 { pat1, pat2 { body }  ? { default_body } }
// default body is the one where the m_cases[i] has only one pattern which is the ?
class MatchStatement : public AstNode {
    Token m_token;
    std::vector<AstNodePtr> m_subjects;
    std::vector<std::pair<std::vector<AstNodePtr>, AstNodePtr>> m_cases;
public:
    MatchStatement(Token tok, std::vector<AstNodePtr> subjects,
                   std::vector<std::pair<std::vector<AstNodePtr>, AstNodePtr>> cases);
    std::vector<AstNodePtr> subjects() const;
    std::vector<std::pair<std::vector<AstNodePtr>, AstNodePtr>> cases() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// type Name{generics} = base_type
class TypeDefinition : public AstNode {
    Token m_token;
    AstNodePtr m_name;
    std::vector<std::string> m_generics;
    AstNodePtr m_base_type;
    bool m_is_pub = false;
    std::vector<Attribute> m_attributes; // e.g. @[align(16)]
public:
    TypeDefinition(Token tok, AstNodePtr name,
                   std::vector<std::string> generics, AstNodePtr base_type,std::vector<Attribute> attributes, bool is_pub = false);
    AstNodePtr name() const;
    std::vector<std::string> generics() const;
    AstNodePtr baseType() const;
    std::vector<Attribute> attributes() const;
    bool isPub() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// @decorator  or  @decorator(args)  applied to the wrapped definition
class DecoratorStatement : public AstNode {
    Token m_token;
    // Each decorator is an IdentifierLiteral or a FuncCall
    std::vector<AstNodePtr> m_decorators;
    AstNodePtr m_body;
public:
    DecoratorStatement(Token tok, std::vector<AstNodePtr> decorators,AstNodePtr body);
    std::vector<AstNodePtr> decorators() const;
    AstNodePtr body() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    
    void accept(AstVisitor& visitor) const;
};

class ScopeStmt : public AstNode {
    Token m_token;
    AstNodePtr m_body;
public:
    ScopeStmt(Token tok, AstNodePtr body);
    AstNodePtr body() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};
// lock var { body }  or  lock (var1, var2) { body }
class LockStatement : public AstNode {
    Token m_token;
    std::vector<AstNodePtr> m_targets;
    AstNodePtr m_body;
public:
    LockStatement(Token tok, std::vector<AstNodePtr> targets,AstNodePtr body);
    std::vector<AstNodePtr> targets() const;
    AstNodePtr body() const;
    
    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    
    void accept(AstVisitor& visitor) const;
};

enum class SelectArmKind {
    Recv,    // pattern <- channel { body }
    Send,    // value -> channel { body }
    Default, // ? { body }
};

// A single arm inside a select block.
// For Default arms, pattern and channel are NoLiteral.
struct SelectArm {
    SelectArmKind arm_kind;
    AstNodePtr pattern; // receive variable or literal pattern for value-matching recv
    AstNodePtr channel;
    AstNodePtr body;
};

// select { arm1  arm2  ... }
class SelectStatement : public AstNode {
    Token m_token;
    std::vector<SelectArm> m_arms;
public:
    SelectStatement(Token tok, std::vector<SelectArm> arms);
    std::vector<SelectArm> arms() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};
} // namespace ast