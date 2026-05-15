⚠️ AI-generated document; may contain errors.

# Luna Language & Compiler Reference

## Sources and status
- **small.luna** is the current syntax reference and is the primary source for syntax.
- **detailed.luna** contains older syntax but richer conceptual notes; when it disagrees with small.luna, both forms are listed below.

## Repository orientation (compiler codebase)
- `src/` and `include/` contain the compiler front-end modules:
  - `lexer/` for tokenization
  - `parser/` for syntax parsing (work in progress)
  - `ast/` for syntax tree definitions
  - `error/` for error handling infrastructure
- `small.luna` and `detailed.luna` are language demo/reference files.

## Lexical basics
- Line comments: `//`.
- Block comments: `/* ... */`.
- String interpolation uses `f"...{expr}..."`.

## Imports and namespaces
**Current syntax (small.luna):**
- `import math`, `import fs`.
- Selective import: `import std::io::{println, print}`.
- Module alias: `using io = std::io`.
- Namespace import: `using math` (similar to `using namespace` in C++).
- `import io{print}` restricts access to `print` only; you still call it as `io::print` unless you also `using io`.

## Types, values, and declarations
- Primitive scalars: `i8/i16/i32/i64`, `u8/u16/u32/u64`, `f32/f64`, `bool`, `str`.
- `None` is the null/empty value for optionals and pointers.
- Constants use `name:Type = value` (e.g., `PI:f64 = 3.14159`).
- Boolean literals are `True` and `False` (capitalized), matching the current small.luna syntax.

### Enums
**Current syntax (small.luna):**
- `pub type Dir = enum { N S E W }`
- Underlying type: `pub type Code = enum:f32 { Ok = 0 Err = 1 }`

**Legacy syntax (detailed.luna):**
- `pub enum Direction { North, South, East, West }`

### Type aliases and sum types
**Current syntax (small.luna):**
- Sum type: `type Shape = Circle | Rect`
- Tuple alias: `type Pair = (i32, i32)`
- Generic alias: `type GenericPair{T1,T2} = (T1, T2,)`
- Trailing commas appear in small.luna tuple examples (e.g., `(T1, T2,)`); they are intended to be supported even if parser work is ongoing.

**Legacy syntax (detailed.luna):**
- Same concept, but examples use `type Shape = Circle | Rect | Triangle` and `type Point3 = (f64, f64, f64)`.

### Structs and initialization
**Current syntax (small.luna):**
- Struct declaration uses `type` with braces:
  - `pub type Point = { pub mut x:= 192.0 pub mut y:f64 = 0.0 }`
- Initialization uses `=` in field assignments:
  - `Point(x = 1.0, y = 2.0)`
- Positional init is allowed (`Point(1, 0)`); structs behave like functions returning a struct.
- Destructor hook uses `__drop__` (example: `fn (mut b:Buf) __drop__()`)
- Field defaults can be inferred with `:=` or explicitly typed with `:Type = value`; both can be mixed in one struct.

**Legacy syntax (detailed.luna):**
- Struct declaration uses `struct`:
  - `pub struct Vec2 { pub mut x:f64 pub mut y:f64 = 0.0 }`
- Initialization uses `:` in field assignments:
  - `Vec2(x:3.0, y:4.0)`
- Destructor hook shown as `drop()`.
- Nested structs are supported (e.g., `Config.Network`, `Config.Logging`).

### Interfaces
**Current syntax (small.luna):**
- `type Number = interface { fn (this:Number)__add__(other:Number) -> Number }`
- `type Any = interface {}`; the runtime value provides `.id` for type checks.

## Functions, methods, and overloading
- Function definition: `fn name(params) -> ReturnType { ... }`.
- Type inference with `:=`:
  - `msg := "test"`
  - `mut is_enabled := True`
- Overloading: multiple functions/methods with the same name are allowed when parameter (or receiver) types differ.
- Methods use a receiver: `fn (p:Point)len() -> f64 { ... }`.
- A function and a method may share a name.
- Functions are first-class values; methods are not (you cannot pass a method as a value).

### Default arguments (disagreement)
- **Current (small.luna):** `fn f(a, b := 10)` uses `:=` for defaults.
- **Legacy (detailed.luna):** `fn greet(name:str, greeting:str = "Hello")` uses `=`.

### Operator overloading & indexing (disagreement)
- **Current (small.luna):** magic method names like `__add__`, `__eq__`, `__getitem__`, `__setitem__`.
- **Legacy (detailed.luna):** symbolic method names like `+(other)`, `==`, `[]` and `[]=`.

### Lambdas and closures
**Current syntax (small.luna):**
- `fn(x:i32)[=] -> i32 { ... }` capture by value
- `fn(x:i32)[&] -> i32 { ... }` capture by reference
- `fn(x:i32)[x, &y] -> i32 { ... }` mixed capture
- If capture list is omitted, capture is by value.
- Special call sugar: `func => { ... }` is equivalent to `func({ ... })` and `func(arg) => { ... }` is `func(arg, { ... })`.
  - Example: `doWork => { compute() }` becomes `doWork({ compute() })`.

### Decorators
- `@decorator` is sugar for `fn = decorator(fn)`.
- **Legacy note:** detailed.luna says a newline after `@decorator` is required.

## Expressions and operators
- Ternary: `cond ? a : b`.
- Pipe operator: `value |> fn1 |> fn2`.
- Range expression: `a..b` and `a..b..step` (e.g., `a..b..c` where `c` is the step); range values are first-class.

### Casting and type IDs (disagreement)
- **Current (small.luna):** `$cast{T}(value)` and `$typeid{T}()` / `$typeid(value)`; sumtypes expose `.id`.
- **Legacy (detailed.luna):** `cast{T}(value)` and `typeid(value)` / `typeid(Type)`.

### Boolean and bitwise (disagreement)
- **Current (small.luna):** only `&` and `|`. In boolean context they short-circuit; in non-boolean context they are bitwise.
- **Legacy (detailed.luna):** `&&` and `||` are shown for boolean logic.

## Control flow
### Conditionals (disagreement)
- **Current (small.luna):** `when` is used for if/else chains:
  - `when { cond1:{...} cond2:{...} ?:{...} }`
  - `when (cond):{ ... }` also exists for a single condition.
- **Legacy (detailed.luna):** `if/elif/else` syntax.

### Match (disagreement)
- **Current (small.luna):** `when (a, b) { 1,2:{...} ?:{...} }`; no fallthrough.
- **Legacy (detailed.luna):** `match a, b { case 1,2 { ... } case ? { ... } }`.

### Loops
- `loop i :0..5 { ... }` (range, end-exclusive).
- `loop i :0..10..2 { ... }` (range + step).
- `loop v :arr { ... }`, `loop k, v :map { ... }`.
- `loop condition { ... }` (while-style).
- `loop { ... }` (infinite loop) with `break`/`continue`.

### Parallel and SIMD loops
- `@[parallel]` to distribute loop iterations across threads.
- Reduction:
  - **Current (small.luna):** `@[parallel(reduce = add, into = total)]`
  - **Legacy (detailed.luna):** `@[parallel(reduce:+, into:total)]`
- `@[parallel(threads = 4)]` to hint worker count.
- `@[simd]` for auto-vectorization; can be combined with `@[parallel]`.

## Error handling and optionals
- Error type: `!T`. Optional type: `?T`.
- `Error` is a builtin error type with `.msg`. Custom error types are not allowed.
- `!T` has an implicit `.err` field; if there is no error it is `None`.
- `has_value()` exists on both `?T` and `!T`; `==`/`!=` work without coalescing, but ordering comparisons do not.

### Propagation and coalescing (disagreement)
- **Current (small.luna):**
  - `expr!` propagates an error.
  - `expr !! fallback` handles error coalescing.
  - `expr ?? fallback` handles optional coalescing.
  - Blocks may use `give value` to supply a fallback. If a block does not yield a value, it must terminate control flow (e.g., `ret`, `break`, or `continue`). The required termination depends on where the coalescing expression appears. Failure to do so is a compile-time error.
  - Coalescing blocks capture variables in scope by reference.
- **Legacy (detailed.luna):**
  - `expr or fallback` and `expr or { ... }` handle both errors and optionals.
  - `give value` provides the fallback value; `ret` exits the enclosing function.

### Error construction (disagreement)
- **Current (small.luna):** `Error("message")`.
- **Legacy (detailed.luna):** `error("message")`.

## Defer and drop
- `defer { ... }` or `defer statement` runs at scope exit.
- Drop order is reverse declaration order.
- **Current (small.luna):** destructor hook uses `__drop__`.
- **Legacy (detailed.luna):** destructor hook uses `drop()`.

## Pointers
- Raw pointers use `*T`, `&val` takes address, `*ptr` dereferences, `ptr->field` accesses a field.
- Pointers are nullable via `None`.
- **Legacy note:** `?*T` is disallowed because pointers are already nullable via `None`; `*?T` (pointer to optional) is allowed when the pointed value itself can be `None`.
- There is no `&Type`; use `*Type` when a pointer type is needed.

## Collections
### Arrays (disagreement)
- **Current (small.luna):** fixed arrays use `[T, N]` (e.g., `[i32,5]`).
- **Legacy (detailed.luna):** fixed arrays use `[N * T]` (e.g., `[5 * i32]`), and `[T]` denotes a dynamic list/slice.

### Maps (disagreement)
- **Current (small.luna):** `map{str,i32}`; map literals use `[]` (e.g., `["c":3]`).
- **Legacy (detailed.luna):** map literals use `{}` (e.g., `{}` and `{ "alice": 95 }`).

## Generics
- Generic types use braces: `Stack{T}`.
- Generic functions: `fn max{T}(a:T, b:T) -> T`.
- Interface constraints: `Stack{T:Number}`.
- **Method generics disagreement:**
  - **Current (small.luna):** `fn (s:Stack{T})push{T}(val:T)` repeats `{T}` on the method.
  - **Legacy (detailed.luna):** `fn (s:Stack{T})push(val:T)` omits the extra `{T}`.

## Concurrency
### Threads and tasks
- `Thread{T}` and `Task{T}` are typed handles.
- `thread { ... }` spawns an OS thread; `thread expr` is a single-expression shorthand.
- `task { ... }` spawns a concurrent task (M:N style).
- `join()` returns `!T` and can be coalesced with `!!`/`or`.
- `thread_local` and `task_local` declare per-thread/per-task storage.
- **Current note (small.luna):** thread/task bodies capture all variables in scope by reference.
- **Current (small.luna):** optional thread/task options include `restart_count`, `timeout`, `always_restart`, and `restart_delay`; tasks and threads also support `.cancel()` and `join(timeout = ms)`.

### Atomics and locks
- Atomic storage: `atomic{i32}` or `atomic mut x:i32`.
- Locks:
  - `lock x { ... }` for a single variable
  - `lock (x, y) { ... }` for multiple variables
  - `lock { ... }` for an implicit global lock on variables used inside the block

### Channels and select
- `Chan{T}` and `TaskChan{T}` are typed channels.
- `send` and `recv`; `recv()` yields `?T` (None when closed).
- `select { v <-- ch1:{...} 42 --> ch2:{...} ?:{...} }` handles multiple channel ops.
- Case ordering is deterministic. If multiple cases are ready simultaneously, the first ready case in source order executes. Messages in other ready channels remain buffered and can be handled in subsequent `select` operations. This is an intentional deviation from random selection in some other languages.

## SIMD
### Type syntax (disagreement)
- **Current (small.luna):** `<f32,4>` / `<bool,4>`.
- **Legacy (detailed.luna):** `<4 * f32>` / `<4 * bool>`.

### Semantics
- Lane count must be a compile-time constant or compile-time variable.
- SIMD operations use standard arithmetic operators; extra ops are in `simd::`.
- `@[align(N)]` on arrays ensures valid SIMD loads.
- `simd::splat`, `simd::load`, `simd::store`, `simd::blend`, `simd::shuffle`, `simd::hadd`, `simd::hmax`, `simd::dot` are available in examples.

## Compile-time features
- Compile-time variables and functions use a `$` prefix (e.g., `$compile_time_var`, `fn $compile_time_fn(...)`).
- `$when` for compile-time branching; `$loop` for compile-time unrolling.
- `$typesize{T}()` returns type size.
- `$has_method` / `$has_attribute` helpers exist.
- `$loop` can iterate tuples, structs, and variadic packs (compile-time shape).
- Variadic packs distinguish compile-time (`*$args`/`**$kwargs`) vs runtime (`*args`/`**kwargs`) values.

## Attributes and compiler directives
- `@[inline]`, `@[noinline]`, `@[deprecated(msg = ..., after = ...)]`, `@[noreturn]`.
- `@[extern("c_name")]` for C interop; extern functions cannot be `pub`.
- `@[export("name")]` to expose a symbol.
- `@[align(N)]` on structs, fields, variables, parameters, and return types.
- `@[simd]` and `@[parallel]` for loop hints.

## Compiler/runtime model (from detailed.luna)
- **Storage locations chosen by compiler:** stack, GC heap, `Shared{T}` (ref-counted), `thread_local`, or manual allocation.
- **Escape analysis:** values that escape a scope (returned or captured by opaque calls) are promoted to heap.
- **Copy vs move:** size ≤ 8 bytes always copied; size > 8 bytes is moved if the source is not used after, otherwise copied.
- **Drop order:** variables are dropped in reverse declaration order at scope exit.
- **Early drop:** `drop(v)` triggers immediate cleanup and invalidates the value.
- **Manual allocation:** `alloc{T}`, `free{T}`, and aligned variants bypass GC/drop safety.
- **Thread sharing:** values used across concurrent threads are promoted to `Shared{T}` automatically.

## FFI and ABI notes
- Extern/exported functions are restricted to scalar types, SIMD types, raw pointers, atomics/threadlocals, and structs composed of those types.
- `...` varargs are only valid on `@[extern]` functions.
- Extern functions accepting lambdas are wrapped by the compiler to match C calling convention.

## Additional notes
- `when`/`match` cases do not fall through.
- Loop variables are immutable by default; `loop mut var :iter` makes them mutable.
- `a..b..c` ranges can be used outside loops (e.g., assigned to a variable).
