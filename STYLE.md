# Coding Style

This is the general coding style used throughout this project, please consider
using them to improve consistency and readability of the code.


# Contents

- [Indentation](#indent)
- [Comments](#comments)
- [Naming](#naming)
- [Line Breaks](#linebreaks)
- [Braces](#braces)
- [Spaces and Parantheses](#spaces)
- [Macros and Enums](#macros)
- [Typedefs](#typedefs)
- [Functions](#functions)
- [Function return values and names](#retvals)
- [Conditional Compilation](#conditionals)
- [Printing debug messages](#printing)
- [Filenames](#filenames)
- [tl;dr](#tldr)
- [Appendix A: Referenced](#appendixa)


## <a name="indent"></a>Indentation

Indentations are done with 4 spaces, do not leave trailing whitespace,
or whitespace only lines. Do not, *ever*, use tabs for indenting.

To reduce indentation in `switch` statements, align the `switch` and `case` on
the same line:

    switch (prefix) {
    case 'R':
        foo = 42;
        break;
    case 'B':
        foo = 43;
        break;
    default:
        break;
    }

Don't put multiple statements on a single line:

    if (cond) do_this();
        do_something_else();


## <a name="comments"></a>Comments
Comments are good, but there is also a danger of over-commenting.  NEVER
try to explain HOW your code works in a comment: it's much better to
write the code so that the *working* is obvious, and it's a waste of
time to explain badly written code.

Generally, you want your comments to tell WHAT your code does, not HOW.

Insert a space after the comment begin, and write in english. End them with
a dot. Make sure to keep everything below the 80 column limit.
Avoid use of trailing comments, if possible. But use them if it's easier.

**DO NOT USE** C++ style comments `//`. Use ISO C90 comment `/* ... */` style.

**Style:**

Multiple comment styles are acceptable:

    /* This is a comment. */

    /*
     * This is also a comment, but it can
     * now span multiple lines.
     */

**Special Comments:**

- `TODO:`: For tracking todos left in the code.
- `FIXME:`: For bugs left open in the code.

These comments are in UPPERCASE, and also might include an assignee like this:
`TODO(Arvid)`


## <a name="naming"></a>Naming

**LOCAL** variables should be named short and concise, `tmp` is probably a better
name for a temporary variable than `ThisVariableIsATemporaryCounter`.

**GLOBAL** variables (to be used only if you *really* need them) need to
have descriptive names, as do global functions.  If you have a function
that counts the number of active users, you should call that
`count_active_users()` or similar, you should *not* call it `cntusr()`.

All exported functions must follow the naming convention:

a) All lowercase seperated by underscores:

    void
    create_foo_bar(void);

b) Name it accordingly where it's defined:

    void
    mem_    vec_     create(usize size);
    ^        ^         ^
    |        |         |
    module  "class"  method
    name    name     name

Encoding the type of a function into the name (so-called Hungarian
notation) is brain damaged - the compiler knows the types anyway and can
check those, and it only confuses the programmer. No wonder Microsoft
makes buggy programs.

**DO NOT USE RESERVED NAMES:**
http://www.gnu.org/software/libc/manual/html_node/Reserved-Names.html


## <a name="linebreaks"></a>Line Breaks

The limit on the length of lines is 80 columns and this is a strongly preferred
limit.

Break statements longer than 80 columns into sensible chunks, unless exceeding
80 columns is significantly improving readability.
Never break user visible strings, or error messages, because this breaks the
ability to grep for them.

Keep the return type of a function definition on a line above the rest of the
definition:

    struct foobar *
    create_foobar(int foo, int bar);


## <a name="braces"></a>Braces

The preferred way, as shown to us by the prophets Kernighan and Ritchie,
is to put the opening brace last on the line, and put the closing brace
first. This applies to all non-function statement blocks
(`if`, `switch`, `for`, `while`, `do`):

    if (x > 42) {
        do_foo();
    }

However, there is one special case, namely functions they have the
opening brace at the beginning of the next line:

    static int
    foo(int x)
    {
        /* body of function */
    }

Do not unncessarily use braces where a single statement will do:

    if (cond)
        do_foobar();

and

    if (cond)
        do_foo();
    else
        do_bar();

This, however, does not apply if either of the branches is a single statement:

    if (cond) {
        do_foo();
        do_bar();
    } else {
        do_baz();
    }


## <a name="spaces"></a>Spaces and Parantheses

Use a space after these keywords:<br>
    `if`, `switch`, `case`, `for`, `do`, `while`.

Use parantheses (and no space) after these:<br>
    `sizeof`, `typeof`, `alignof`, or `__attribute__`.

Do not add spaces around (inside) parenthesized expressions.
This example is **bad**:<br>
    `s = sizeof( struct file );`

Use one space around (on each side of) most binary and ternary operators,
such as any of these:<br>
    `=`, `+`, `-`, `<`, `>`, `/`, `%`, `|`, `&`, `^`, `<=`, `>=`, `==`, `!=`, `?`, `:`.

but no space after unary operators:<br>
    `&`, `+`, `-`, `~`, `!`, `sizeof`, `typeof`, `alignof`, `__attribute__`, `defined`.

no space before the postfix increment & decrement unary operators:<br>
    `++`, `--`.

no space after the prefix increment & decrement unary operators:<br>
    `++`, `--`.

no space after casts:<br>
    `(int)sizeof(tmp)`.

and no space around the `.` and `->` structure member operators.

When declaring pointer data or a function that returns a pointer type, the
preferred use of `*` is adjacent to the data name or function name and not
adjacent to the type name:

    char *banner;
    unsigned long long memparse(char *ptr, char **retptr);
    char *match_strdup(substring_t *s);


## <a name="macros"></a>Macros and Enums

Constants have uppercase names, delimited with underscores:
If the macro is not just a constant, enclose it in parantheses.

    #define CONSTANT_FOO 0x12345
    #define CONSTANT_EXP (CONSTANT_FOO | 0x3)

Enums are preferred when defining several related constants,
and are named similar to macro constants:

    enum foo { BAR, BAZ };

Macros resembling functions may be named lowercase:

    #define foomacro(a, b, c)                                                     \
        do {                                                                      \
            if ((a) > 5)                                                          \
                foo((b), (c));                                                    \
        } while (0)

Enclose macros in `do { } while (0)` if they're longer than a single statement.

Enclose single statements in parantheses. Wrap arguments in parantheses.


**Things to avoid:**

1) Macros that affect control flow:

    #define FOO(x)                                                                \
        do {                                                                      \
            if ((x) > 42)                                                         \
                return 42;                                                        \
        } while (0)

2) Macros that rely on a magic variable:

    #define FOO(x) bar(idx, val)

3) Macros that are used as lvalues:

    FOO(x) = y;

4) Namespace collisions when defining local variables:

    #define FOO(x)                                                                \
        do {                                                                      \
            int tmp = (x) * 2;                                                    \
            do_bar(tmp);                                                          \
        while (0)

`tmp` is a common name, `__foo_tmp` is less likely to collide with an existing
variables.


## <a name="typedefs"></a>Typedefs

Please don't use things like `vps_t`.
It's a *mistake* to use typedef for structures and pointers. When you see a

    vps_t a;

in the source, what does it mean?
In contrast, if it says

    struct virtual_container *a;

you can actually tell what `a` is.


Lots of people think that typedefs "help readability". Not so. They are
useful only for:

**(a)** totally opaque objects (where the typedef is actively used to *hide*
        what the object is).

        *NOTE!* Opaqueness and "accessor functions" are generally not good in
        themselves. The reason to have them is, that there are some types which
        contain absolutely *zero* safely accesible information.

**(b)** Clear integer types, where the abstraction *helps* avoid confusion
        whether it is `int` or `long`. `u8`/`u16`/`u32` are perfectly fine
        typedefs.

        *NOTE!* Again - there needs to be a *reason* for this. If something is
        `unsigned long`, then there's no reason to do

    typedef unsigned long myflags_t;

        but if there is a clear reason for why it under certain circumstances
        might be an "unsigned int" and under other configurations might be
        "unsigned long", then by all means go ahead and use a typedef.

**(c)** when you use sparse to literally create a *new* type for type-checking.


Maybe there are other cases too, but the rule should basically be
to **NEVER EVER** use a typedef unless you can clearly match one
of those rules.

In general, a pointer, or a struct that has elements that can reasonably
be directly accessed should *never* be a typedef.


## <a name="functions"></a>Functions

Functions should be short and sweet, and do just one thing. They should
fit on one or two screenfuls of text and do one thing and do that well.

The maximum length of a function is inversely proportional to the
complexity and indentation level of that function.  So, if you have a
conceptually simple function that is just one long (but simple)
case-statement, where you have to do lots of small things for a lot of
different cases, it's OK to have a longer function.

However, if you have a complex function, you should adhere to the maximum
limit more closely. Use helper functions with descriptive names
(you can ask the compiler to in-line them if you think
it's performance-critical, and it will probably do a better job of it
than you would have done).

Another measure of the function is the number of local variables.  They
shouldn't exceed 5-10, or you're doing something wrong.  Re-think the
function, and split it into smaller pieces.  A human brain can
generally easily keep track of about 7 different things, anything more
and it gets confused.  You know you're brilliant, but maybe you'd like
to understand what you did 2 weeks from now.

In function prototypes, include parameter names with their data types.
Although this is not required by the C language, it is preferred because
it is a simple way to add valuable information for the reader.

There are exceptions to this rules, if the function cannot be logically well
splitted, creating a single big function is preferred than to split them
arbitrarily. Consider this two styles:

    void
    smallA(void)
    {
    }

    void
    smallB(void)
    {
    }

    void
    big(void)
    {
        smallA();
        smallB();
    }

    /* and ... */

    void
    big(void)
    {
        /* part a */
        ...

        /* part b */
        ...
    }

it is preferred to use the second style. Refer to Appendix A for an explanation
by John Carmack.

In source files, seperate each function with one blacnk line. Return type on a
line before rest of definition:

    struct foo *
    create_foo(int foo, int bar);


## <a name="retvals"></a>Function return values and names

Functions can return values of many different kinds, and one of the
most common is a value indicating whether the function succeeded or
failed.  Such a value can be represented as an error-code integer
(1 = failure, 0 = success) or a "succeeded" boolean (0 = failure,
non-zero = success).

Mixing up these two sorts of representations is a fertile source of
difficult-to-find bugs.  If the C language included a strong distinction
between integers and booleans then the compiler would find these mistakes
for us... but it doesn't.  To help prevent such bugs, always follow this
convention:

If the name of a function is an action or an imperative command,
the function should return an error-code integer.  If the name
is a predicate, the function should return a "succeeded" boolean.

For example, "add work" is a command, and the `add_work()` function returns `0`
for success or `-EBUSY` for failure.  In the same way, "PCI device present" is
a predicate, and the `pci_dev_present()` function returns `1` if it succeeds in
finding a matching device or `0` if it doesn't.


## <a name="conditionals"></a>Conditional Compilation

Wherever possible, don't use preprocessor conditionals (`#if`, `#ifdef`) in `.c`
files; doing so makes code harder to read and logic harder to follow. Instead,
use such conditionals in a header file defining functions for use in those `.c`
files, providing no-op stub versions in the #else case, and then call those
functions unconditionally from .c files.  The compiler will avoid generating
any code for the stub calls, producing identical results, but the logic will
remain easy to follow.

Prefer to compile out entire functions, rather than portions of functions or
portions of expressions.  Rather than putting an ifdef in an expression, factor
out part or all of the expression into a separate helper function and apply the
conditional to that function.

If you have a function or variable which may potentially go unused in a
particular configuration, and the compiler would warn about its definition
going unused, mark the definition as `__maybe_unused` rather than wrapping it in
a preprocessor conditional. (However, if a function or variable *always* goes
unused, delete it.)

Within code, where possible, use the `IS_ENABLED` macro to convert conditions
into a C boolean expression, and use it in a normal C conditional:

    if (IS_ENABLED(CONFIG_SOMETHING)) {
        /* ... */
    }

The compiler will constant-fold the conditional away, and include or exclude
the block of code just as with an #ifdef, so this will not add any runtime
overhead.  However, this approach still allows the C compiler to see the code
inside the block, and check it for correctness (syntax, types, symbol
references, etc).  Thus, you still have to use an #ifdef if the code inside the
block references symbols that will not exist if the condition is not met.

At the end of any non-trivial #if or #ifdef block (more than a few lines),
place a comment after the #endif on the same line, noting the conditional
expression used.  For instance:

    #ifdef CONFIG_SOMETHING
    /* ... */
    #endif /* CONFIG_SOMETHING */


## <a name="printing"></a>Printing debug messages

Mind the spelling of system messages, to make a good impression. Do not use
crippled words like "dont"; use "do not" or "don't" instead. Make the messages
concise, clear, and unambiguous.

Messages do not have to be terminated with a period.

Printing of messages is controlled by a project wide verbosity setting, make
sure to choose an appropriate level for your messages. We do not want to liter
the users log with unnecessary debug messages on release builds


## <a name="filenames"></a>Filenames
- C files: `.c`
- C headers: `.h`
- Perl files: `.pl`
- Shell scripts: `.sh` (or if using bash `.bash`)
- Batch scripts: `.bat`

Filenames are all lowercase, seperated by underscores


## <a name="tldr"></a>tl;dr
- Column limit: 80
    - Strongly preferred limit, break longer statements into sensible chunks
- Don't break userfacing strings, it breaks the ability to grep for them
- Avoid non-ASCII characters
- Indent with 4 spaces, not tabs.
- Don't put multiple statements on a single line, unless you have very good reason.
- Don't leave trailing whitespace, get a decent editor.
- Braces on the newline line for functions, on the same for everything else.
- Keep it simple!


## <a name="appendixa"></a>Appendix A: References


This document is very similar to Linux Kernel Style, found at:
https://www.kernel.org/doc/Documentation/CodingStyle

Comment style also adapted from Linus:
https://lkml.org/lkml/2016/7/8/625

John Carmack on inlined code:
http://number-none.com/blow/john_carmack_on_inlined_code.html

The C Programming Language, Second Edition
by Brian W. Kernighan and Dennis M. Ritchie.
Prentice Hall, Inc., 1988.
ISBN 0-13-110362-8 (paperback), 0-13-110370-9 (hardback).

