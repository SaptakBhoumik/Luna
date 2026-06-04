# Luna Memory Model

> Target: Linux x86-64 only. All size thresholds and calling convention rules assume the System V AMD64 ABI.

---

## Storage Locations

The compiler picks the storage location automatically. You never annotate this except for `#[thread_local]`, manual `Shared<T>`, and manual allocation.

| Location | When | Freed |
|---|---|---|
| **Stack** | Default. Value does not escape its declaring scope. | `__drop__()` at scope exit, reverse declaration order. |
| **Thread-local** | Declared with `#[thread_local]`. Each thread gets an independent copy. | `__drop__()` when the owning thread exits. |
| **Auto Shared** | Compiler-promoted. Value is accessed by two or more concurrently live threads. Ref-counted internally. User never writes this. | `__drop__()` when ref count reaches zero. |
| **Manual Shared** | User explicitly writes `Shared<T>`. Same ref-counting as auto shared. | `__drop__()` when ref count reaches zero. |
| **Manual** | `alloc` / `free`. No safety rails, no automatic cleanup. C interop only. | `free(v)` calls `v.__drop__()` first if defined, then releases memory. |

---

## Pass and Return Threshold

Luna targets Linux x86-64. The System V ABI can pass up to 16 bytes (two registers) without a pointer. Luna uses this as its copy threshold.

```
sizeof(T) <= 16 bytes   always copied, passed in registers
sizeof(T) >  16 bytes   passed by pointer (see Calling Convention)
```

**What counts toward size:**
- Every field in a struct, tuple, or sum type as laid out in memory.
- The header fields of `Str`, `List<T>`, `Tensor<T>` (pointer + length). The heap content they point to does NOT count.

**Examples:**

```
i32          4 bytes   <= 16, always copy
f64          8 bytes   <= 16, always copy
Vec2         16 bytes  <= 16, always copy  (two f64 fields)
Vec3         24 bytes  >  16, pointer path
Str          16 bytes  <= 16, header copy only (heap content is not copied unless explicit)
Mat4         64 bytes  >  16, pointer path
```

---

## Copy vs Move

For values above 16 bytes, Luna avoids unnecessary copies using move semantics.

| Situation | What happens |
|---|---|
| Source is used after the assignment | Full copy. Both source and destination are valid. |
| Source is NOT used after the assignment | Move. Destination takes the source's data. Source is invalidated. No copy. |
| `copy(v)` explicit call | Always deep copies heap content regardless of usage. |

**Examples:**

```luna
// Small value - always copied
x: i32 = 42
y := x          // copy, x still valid

// Str header is 16 bytes - copied by header, heap content shared until modified
a: str = "hello"
b := a          // a IS used after -> copy of header + new heap allocation
io::println(a)  // still valid
c := b          // b is NOT used after -> move, b invalidated
io::println(c)  // "hello"

// Force a deep copy regardless
d := copy(a)

// Large struct - same rules apply
pt_a := Vec3(x = 1.0, y = 2.0, z = 3.0)  // 24 bytes
pt_b := pt_a    // pt_a IS used after -> full copy (24 bytes)
io::println(pt_a.x)
pt_c := pt_b    // pt_b NOT used after -> move, pt_b invalidated
io::println(pt_c.x)
```

---

## Calling Convention

For functions with arguments or return values above 16 bytes, the compiler generates multiple internal versions. This is an implementation detail - call sites look normal.

### Argument above 16 bytes

Given a user-written function:

```luna
fn name(arg: T) -> R   // sizeof(T) > 16
```

The compiler generates these internal versions:

```luna
// Canonical implementation. Receives a pointer. Does NOT call __drop__() on T.
// Used internally when the compiler manages the lifetime itself.
fn name_ptr(argptr: *T) -> R

// Owns the pointed-to value. Calls name_ptr then calls __drop__() on T.
// Called when arg is not used after this call.
fn name_owned_ptr(argptr: *T) -> R {
    val := name_ptr(argptr)
    argptr.__drop__()   // destructor of T runs here, pointer itself is not freed
    ret val
}

// Public surface. Takes value, forwards to name_owned_ptr.
fn name(arg: T) -> R {
    ret name_owned_ptr(&arg)
}

// Shared version. Used when arg is accessed by concurrent threads.
fn name(arg: Shared<T>) -> R {
    val := name_ptr(arg.get())
    arg.release()   // decrement ref count, __drop__() fires if count hits zero
    ret val
}
```

**Dispatch rules at each call site:**

```
arg not used after call, no concurrent thread access
    -> name_owned_ptr(&arg)

arg IS used after call and name_ptr doesnt modify it, no concurrent thread access
    -> name_ptr(&arg), caller is responsible for lifetime

arg IS used after call and name_ptr does modify it, no concurrent thread access
    -> name(arg), callee is responsible for lifetime of the copy and caller is responsible for lifetime of the original

arg shared with another thread, join is guaranteed before arg goes out of scope
    -> name_ptr(&arg), join acts as the lifetime barrier, no Shared needed

arg shared with another thread, join is uncertain or never happens
    -> Shared<T> version, ref count manages lifetime
```

### Return value above 16 bytes

The System V ABI already handles this via a hidden return pointer passed by the caller. No manual implementation needed. Documented here for reference only:

```luna
// What the ABI does internally:
fn name_ptr(arg: T, retptr: *R) {
    // result written to *retptr instead of returned
}
fn name(arg: T) -> R {
    R ret
    name_ptr(arg, &ret)   // caller allocates ret on its own stack
    ret ret
}
```

---

## Thread Memory Rules

### Automatic Shared promotion

The compiler analyses the thread lifetime graph at compile time. No user annotation required.

```luna
fn demo() {
    // shared_buf: used by t1 AND t2 which are both live at the same time
    // -> compiler promotes to Shared<GpuBuffer>
    shared_buf := GpuBuffer(handle = gpu::alloc())

    // solo: used ONLY by t1, main never touches it after launch
    // -> moved into t1, zero overhead, no ref counting
    solo := GpuBuffer(handle = gpu::alloc())

    // concurrent_b: used by t2 AND by main after t2 launches but before join
    // -> compiler promotes to Shared<GpuBuffer>
    concurrent_b := GpuBuffer(handle = gpu::alloc())

    // thread_local: each thread gets its own independent copy
    // -> no promotion, no sharing
    #[thread_local] scratch := GpuBuffer(handle = gpu::alloc())

    t1 := thread { work(shared_buf, solo, scratch) }
    t2 := thread { work(shared_buf, concurrent_b, scratch) }
    process(concurrent_b)   // main uses concurrent_b while t2 is live
    t1.join()!
    t2.join()!

    // seq: declared after both joins, used by one thread at a time
    // -> no promotion needed, compiler sees sequential use
    seq := GpuBuffer(handle = gpu::alloc())
    t3 := thread { work(shared_buf, seq, scratch) }
    t3.join()!
    t4 := thread { work(shared_buf, seq, scratch) }
    t4.join()!

}   // shared_buf.__drop__() here - last ref released after both joins
    // seq.__drop__() here - scope exits
    // scratch.__drop__() called per thread when each thread exits
```

### Promotion decision table

| Situation | What compiler does |
|---|---|
| Value used by exactly one thread, main does not use it after launch | Move into thread. No ref counting. |
| Value used by multiple concurrent threads, OR by thread AND main simultaneously | Promote to `Shared<T>`. Ref counted. |
| Join is guaranteed before value goes out of scope | No promotion needed. Join is the lifetime barrier. |
| Thread is detached (never joined) and value is shared with main | Must promote to `Shared<T>`. No other safe option. |
| `#[thread_local]` annotation | Copy per thread. No sharing, no ref counting. |

### Why cycles cannot occur in auto-promoted Shared

`Shared<T>` is only inserted at thread argument boundaries. The struct fields themselves remain plain values - they are never `Shared<T>`. A cycle requires `Shared` pointers inside struct fields pointing back to other `Shared` objects. Since auto-promoted `Shared` only wraps thread arguments and never appears inside struct definitions, the reference graph is always a flat set of roots pointing into plain data. Ref counts always reach zero. No cycle collector is needed.

---

## Drop Ordering

When a scope exits, variables are dropped in reverse declaration order. This is always safe because a later-declared variable may reference an earlier-declared one.

```luna
fn demo() -> ! {
    file_a := FileHandle(fd = fs::open("a.txt")!)   // declared first
    conn_b := DbConnection(host = "localhost")       // declared second, may reference file_a
    // ...
}   // conn_b.__drop__() runs first, then file_a.__drop__()
```

---

## Explicit Early Drop

Call `v.__drop__()` to release a value before scope exit. The compiler marks `v` invalid at that point. Any use after is a compile error. The automatic scope-exit drop is suppressed so there is no double drop.

```luna
fn demo() -> ! {
    gpu_res := GpuBuffer(handle = gpu::alloc())
    render(gpu_res)
    gpu_res.__drop__()     // destructor runs now, GPU resource released immediately
    // use(gpu_res)        // compile error - gpu_res was dropped
    heavy_cpu_work()       // runs without holding the GPU resource
}                          // no second drop - compiler knows gpu_res is already gone
```

---

## Manual Shared and Weak

For cases where the user explicitly needs shared ownership with potential cycles. This is opt-in and the user is responsible for correctness.

```luna
// Manual shared ownership - user written
parent := Shared::<Node>(...)
child  := Shared::<Node>(...)

// Weak does not increment the ref count - breaks cycles
child.parent = Weak::<Node>(parent)

// To use a Weak reference, upgrade it first
// Upgrade returns None if the value was already dropped
p := child.parent.upgrade() ?? {
    // parent was dropped, handle this case
}
```

Use `Weak<T>` wherever a back-edge exists in the ownership graph - parent pointers in trees, observer/listener lists, caches, worker-to-pool references. The strong `Shared<T>` references must form a DAG. `Weak<T>` references can point anywhere without affecting lifetimes.

---

## Raw Pointers

`*T` is an unmanaged raw pointer. No destructor, no ref counting, no safety checks. Always 8 bytes and always copied. Exists for C interop only. For safe shared ownership use `Shared<T>`.

---

## Builtin Complex Types

These are compiler-managed. Their heap content does not count toward the 16-byte threshold - only their in-struct header fields do.

| Type | Header size | Notes |
|---|---|---|
| `Str` | 16 bytes (ptr + len) | Heap content managed automatically |
| `List<T>` | 16 bytes (ptr + len) | Heap content managed automatically |
| `Tensor<T>` | 16 bytes (ptr + len) | Heap content managed automatically |
| `Map<K,V>` | 16 bytes (ptr + len) | Heap content managed automatically |
| `Shared<T>` | 8 bytes (ptr) | Ref count lives alongside the heap object, not in the header |