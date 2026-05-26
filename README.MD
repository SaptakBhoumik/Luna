# Luna

A programming language that is simple and pleasant to use. It is a statically typed, compiled language with inspiration from langages like Go and Lua. 
It also takes inspiration from a few other langages. 
The language is still WIP. I am making it public so that I can keep myself accountable and not abandon it like my countless other projects

Check out the small.luna file for a small demo of the language features. The code is not meant to be perfect or even good, it is just meant to show the features of the language. I will be adding more features and improving the existing ones in the future.
Check out detailed.luna for a detailed demo. But remember the detailed demo is an old syntax and is not updated to the latest syntax. So some of the code in detailed.luna might not work with the latest version of the language. I will be updating the detailed demo in the future.

# Code snippet

The syntax is more or less fixed at this point. It looks something like this

```rust
io := import("std/io")

fn divide(a, b) -> !f64 {
    when b == 0: { ret Error("div by zero") }
    ret a / b
}

// $loop unrolls at compile time - mixed types, zero runtime overhead
fn dump(*args) {
    $loop $v: args { io::println(f"{$v}") }
}
fn logged(f: fn(i32) -> i32) -> fn(i32) -> i32 {
    ret fn(x: i32)[f] -> i32 {
        io::println(f"in:  {x}")
        r := f(x)
        io::println(f"out: {r}")
        ret r
    }
}
@logged  // decorators are just higher-order functions
fn process(x: i32) -> i32 { ret x |> double |> inc |> square }

pub fn main() {
    // errors and optionals - no try/catch, no unwrap()
    safe := divide(10.0, 0.0) !! (err) { 
        io::println("Error:", err)
        give -1.0 
    }
    opt: ?i32 = None
    val := opt ?? 42

    // resolved entirely at compile time
    dump("pi =", 3.14159, "answer =", 42, True)

    // pipe + @logged = automatic input/output tracing
    io::println(process(3))

    // tasks, channels, graceful drain
    ch := Chan::<i32>(cap = 4)
    task { 
        loop i:0..3 { ch.send(i) } 
        ch.close() 
    }

    thread(restart_count = 100, timeout = 1000) {
        //Time out is in ms. If task is running for more than 1000ms then it will be killed and restarted. Note if we have no restart then we just kill the thread on time out
        //And timeout duration is calculated fresh from each restart
        #[no_fallthrough]
        select {
            v <-- ch: { io::println(f"received {v}") }
            ?: { io::println("channel didnt send") }
        }
    }

    // pattern match on values or ranges
    // `when` uses pattern.__match_eq__(x) - so Range(1,100).__match_eq__(x) just works
    // customize matching behaviour per type by implementing __match_eq__
    // uses __eq__ when __match_eq__ not available
    when val {
        0:      { io::println("zero") }
        1..100: { io::println("in range") }
        ?:      { io::println("other") }
    }

    // one attribute. auto-vectorised.
    #[simd]
    loop i: 0..1024 { out[i] = ia[i] * ib[i] }
}

```

# Features

- Powerful type system with type inference, generics, interfaces, and more
- First class simd support
- First class os threads and green threads(tasks)
- First class channels for inter-thread communication
- Powerful select mechanism for waiting on multiple channels and timers
- Powerful pattern matching with value and range patterns
- Powerful compile time features but not too powerful that it becomes a mess
- More

# Goals

Make a language based on what I think is ideal design. So you may find some syntax that you will hate but you will also find some syntax that you will love. I am not trying to make a language that is perfect for everyone, but rather a language that is perfect for me. This language is meant to be a personal test bed to try out some of the ideas and ideals I have for language design. One of then is achieve more with less. That is why this language packs a lot of features with only 22 reserved keywords. Another goal is to make the compiler do most of the work while you sit back and relax


# Inspiration 

It innitially stated with Lua as the main inspiration and the backbone. But as time went on, I added more features and changed syntax to the point it no longer looks like Lua. It also draws heavily from Go,python,vlang and rust

# Reason behind the name

My main inspiration was Lua(Although it is significantly different from Lua right now) so there's that + Luna is a cute name

# Current progress
 - [x] Error lib
 - [x] Test error lib
 - [x] Lexer
 - [x] Test Lexer
 - [x] Ast
 - [x] Parser
 - [x] Test Parser
 - [ ] Create better errors
 - [ ] Syntax analyzer and name mangler lvl 1
 - [ ] Type checker, compile time resolution, name mangler lvl 2 and ast lowering lvl 1
 - [ ] Ast lowering lvl 2 
 - [ ] Ast lowering lvl 3 and code generation
 - [ ] Standard library
 - [ ] Expand this list as I add more features to the language

 Actually I am using code from my previous project named peregrine so there will be some inconsistancies in the code and will take time to fix them

# Compiling and running

```sh
git clone https://github.com/SaptakBhoumik/Luna.git
cd Luna
meson builddir
cd builddir
ninja
./luna.elf <path_to_luna_file>
```
 
After running ``./luna.elf <path_to_luna_file>`` it will print out the luna file reconstruted from the ast and also the token stream. Yes the compiler is not mature enough to do anything useful yet

# Contributing note

Note:-Not accepting contributions at the moment. I will be accepting contributions in the future when the language is more stable and has more features. For now, I only made it public to keep myself accountable 

# Contact 

- 💌 Reach me at: `saptakbhoumik.acad@gmail.com`
- 📢 Follow me on [X](https://x.com/saptakbhoumik)