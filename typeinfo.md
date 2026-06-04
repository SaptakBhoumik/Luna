# Luna Type System Reference

A reference for type system terminology and how Luna handles each case.
Useful when you forget what a term means or want to check Luna's behaviour quickly.

---

## Key Terms

### Transparent (Alias)
A name that is just another spelling for the same type. The compiler replaces it
before doing any type checking - like a text macro. After substitution, the two
names are completely identical and always interchangeable.

### Opaque (Nominal Alias)
A name that creates a genuinely new type, even though it is defined in terms of
another. The compiler does NOT substitute it away. `Meters` and `Seconds` are
different types even if both are `f64` underneath.

### Nominal Typing
Two types are the same only if they have the same **name**. Structure does not
matter. `Point` and `Circle` are different even if they happen to have the same
fields.

### Structural Typing
Two types are the same if they have the same **shape** (same fields, same method
signatures, same parameter types, etc.). Names do not matter.

### Anonymous Type
A type written inline without giving it a name, e.g. `{a:i32, b:str}` or an
inline enum. Anonymous types are **never** considered the same as a named type,
even if the structure matches exactly.

---

## How Luna Handles Each Case

### Type Aliases - `type A = B`

```luna
type A = B
type A = B1 | B2 | B3      // sum type alias
type A = (B1, B2, B3)      // tuple type alias
```

**Transparent by default.** The compiler replaces every occurrence of `A` with
its definition before any type checking happens - exactly like a macro expansion.
After expansion, `A` and `B` are indistinguishable.

```luna
type Meters = f64
type Seconds = f64
// After expansion both are just f64.
// A Meters value and a Seconds value are the same type. No error.
```

To make an alias opaque (nominal), use `#[opaque]`:

```luna
#[opaque] type Meters = f64
#[opaque] type Seconds = f64
// Now Meters and Seconds are distinct types. Assigning one to the other is a type error.
```

`#[opaque]` works on all three alias forms: scalar, sum type, and tuple.

---

### Structs

**Nominal.** Every named struct is its own type. Two structs with identical fields
are still different types if they have different names.

```luna
pub type Point  = { pub mut x:f64  pub mut y:f64 }
pub type Offset = { pub mut x:f64  pub mut y:f64 }
// Point != Offset even though the fields are identical.
```

**Anonymous structs** (written inline without a name) are also nominal in the
sense that they are never considered the same as any named struct, even if the
structure matches.

---

### Enums

**Nominal.** Same rule as structs. Two enums with the same variants are different
types if they have different names.

```luna
type Dir   = enum { N S E W }
type Compass = enum { N S E W }
// Dir != Compass.
```

Anonymous enums follow the same rule as anonymous structs - never equal to a
named enum.

---

### Interfaces

**Structural.** A type satisfies an interface if it has all the required methods
with matching signatures. There is no need to explicitly declare that a type
implements an interface (no `implements` keyword).

```luna
type Number = interface {
    fn (this:Number) __add__(other:Number) -> Number
    fn (this:Number) __sub__(other:Number) -> Number
}
// Any type that has __add__ and __sub__ with the right signatures satisfies Number.
// No annotation needed on the type itself.
```

---

### Generic Instantiations - `T<T1>` vs `T<T2>`

`T<T1>` and `T<T2>` are different types if `T1 != T2`, even if `T1` and `T2`
are structurally identical.

However, because aliases are expanded first (like a macro), if `T1` and `T2` are
both transparent aliases of the same underlying type, they become the same after
expansion:

```luna
type A = i32
type B = i32
// After expansion: Stack<A> and Stack<B> both become Stack<i32>. Same type.

#[opaque] type A = i32
#[opaque] type B = i32
// A and B are distinct. Stack<A> != Stack<B>.
```

---

### Function Types

**Transparent.** Two function types with the same parameter types and return type
are the same type, regardless of what names (if any) were used to define them.

```luna
type A = fn(i32) -> i32
type B = fn(i32) -> i32
// A and B are the same type.
```

Can be made opaque:

```luna
#[opaque] type A = fn(i32) -> i32
#[opaque] type B = fn(i32) -> i32
// Now A and B are distinct types.
```

---

### Pointer Types - `*T`

**Transparent.** Pointer types follow their pointee. `*i32` and `*i32` are always
the same type. Can also be made opaque with `#[opaque]`.

```luna
type A = *i32
type B = *i32
// A and B are the same type.

#[opaque] type A = *i32
#[opaque] type B = *i32
// Now distinct.
```

---

### Built-in Primitive Types

**Nominal.** `i32`, `i64`, `f32`, `f64`, `str`, `bool`, `u8`, etc. are all
distinct types. Same size does not mean same type.

```luna
// i32 != i64  even though both are integers.
// f32 != f64  even though both are floats.
// i32 != f32  even though both may be 32 bits.
```

---

### SIMD Types - `<elem, lanes>`

**Nominal by parameters.** `<f32, 4>` and `<f32, 8>` are different types.
`<f32, 4>` and `<i32, 4>` are also different types. Both the element type and
the lane count are part of the type identity.

```luna
a: <f32, 4> = (1.0, 2.0, 3.0, 4.0)
b: <f32, 8> = ...
// a and b are different types. Cannot assign one to the other.
```

---

### Optional Types - `?T`

**Transparent wrapper.** `?i32` from one place is the same as `?i32` from
another. Aliases expand first, so `?A` where `A = i32` becomes `?i32`.

```luna
type A = ?i32
type B = ?i32
// A and B are the same type.
```

---

### Error Types - `!T`

Same rule as optional types. `!i32` is always `!i32`. Transparent wrapper.

```luna
type A = !i32
type B = !i32
// A and B are the same type.
```

---

## Quick Reference Table

| Construct | Nominal or Structural | Notes |
|---|---|---|
| `type A = B` | Transparent (neither) | Macro-expanded before type checking |
| `type A = B1\|B2` | Transparent by default | `#[opaque]` makes it nominal |
| `type A = (B1,B2)` | Transparent by default | `#[opaque]` makes it nominal |
| `type A = fn(...)->T` | Transparent by default | `#[opaque]` makes it nominal |
| `type A = *T` | Transparent by default | `#[opaque]` makes it nominal |
| `type A = ?T` | Transparent by default | `#[opaque]` makes it nominal |
| `type A = !T` | Transparent by default | `#[opaque]` makes it nominal |
| Named struct | Nominal | Name is the identity |
| Named enum | Nominal | Name is the identity |
| Anonymous struct/enum | Nominal | Never equal to any named type |
| Interface | Structural | Satisfied by shape, no `implements` needed |
| Generic `T<T1>` | Nominal on params | After alias expansion |
| Primitives (`i32`, `f64`…) | Nominal | Distinct even if same size |
| SIMD `<f32, 4>` | Nominal on both params | elem type and lane count both matter |
