#pragma once
#include <memory>
#include <map>
#include <vector>
namespace Luna{
class AstNode;

using AstNodePtr = std::shared_ptr<AstNode>;

// ---- Attribute: @[name] or @[name(args...)] stored directly on owning nodes ----

struct Attribute {
    std::string name;
    std::vector<AstNodePtr> args = {}; // empty if no argument list
    std::map<std::string, AstNodePtr> named_args = {}; // empty if no named arguments
};

// ---- Parameter ----

enum class ParamKind {
    Normal,
    CVariadic, // ...
    VarArg, // *args
    CompileTimeVarArg, // *$args 
    CompileTimeKwarg, // **$kwargs
};

struct Parameter {
    AstNodePtr type;
    std::string name;//No literal for CVariadic
    AstNodePtr default_value; // NoLiteral if absent
    bool is_mut = false;
    ParamKind kind = ParamKind::Normal;
};

// ---- Lambda capture ----

enum class CaptureKind {
    None,    // []
    AllCopy, // [=]
    AllRef,  // [&]
    List,    // [a, &b, ...]
};

struct CaptureEntry {
    bool by_ref;
    AstNodePtr name;
};

struct CaptureClause {
    CaptureKind kind = CaptureKind::None;
    std::vector<CaptureEntry> entries = {}; // populated when kind == List
};

// ---- Struct field (for struct definitions) ----

// A single field inside a struct definition
struct StructField {
    AstNodePtr name;
    AstNodePtr type;
    AstNodePtr default_value; // NoLiteral when absent
    bool is_pub = false;
    bool is_mut = false;
    std::vector<Attribute> attributes;
};
}