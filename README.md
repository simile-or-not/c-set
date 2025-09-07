# C Set Library
A simple set library for **C**. This library's sets work in a similar manner to C++ vectors: they can store any type, their elements can be accessed via the `[]` operator, and elements may be added or removed with simple library calls.

You can easily create a set like so:

```c
int* num_set = set_create();
```

You can store different data types in sets by simply modifying the pointer type:

```c
// set of chars
char* char_set = set_create();

// set of integers
int* int_set = set_create();

// you can even store structs!
struct foo* foo_set = set_create();
```

A set's elements can be accessed using the `[]` operator:

```c
char* char_set = set_create();

// add an item
set_add(&char_set, 'a');

// print the item from the set
printf("first item: %c\n", char_set[0]);
```

Sets require that their pointer type corresponds to the contents of the set (e.g. `int*` for a set containing `int`s); otherwise, you have to explicitly cast the set pointer to the correct type when accessing elements or making library calls:

```c
// recommended set use
int* foo = set_create();

set_add(&foo, 5);

// reassignment
foo[0] = 3;


// generic set (not recommended)
set bar = set_create();

set_add(&(int*)bar, 3);

((int*)bar)[0] = 5; // we have to cast to an int* so we can properly access set elements
```
Note: the `set` type is just an alias for `void*` defined in `set.h`.

# How It Works

These sets can be manipulated in a similar manner to C++ vectors; their elements can be accessed via the `[]` operator and elements may be added or removed through the use of simple library calls.

This works because these sets are stored directly alongside special header in memory, which keeps track of the set's size and capacity:

    +--------+-------------+
    | Header |   Set data  |
    +--------+-------------+
             |
             `-> Pointer returned to the user.

This design was inspired by anitrez's [Simple Dynamic Strings](https://github.com/antirez/sds/).

This library uses the preprocessor to perform compile-time type checks. The type checks are done using C23's `typeof` operator, which was actually implemented in some compilers before C23 (such as GCC and Clang). [Older versions of MSVC](https://learn.microsoft.com/en-us/cpp/c-language/typeof-c?view=msvc-170#requirements) do not support the `typeof` operator. See [Missing typeof Reference Sheet](#missing-typeof-reference-sheet) for info about set usage when `typeof` is not present.

If you're using this library in a C++ project, and `decltype` is supported, a macro substitute for `typeof` will automatically be applied.

# Usage

Just because these sets can be accessed and modified like regular arrays doesn't mean they should be treated the same in all cases.

Because of the hidden header data, these sets can only be properly freed by calling `set_free(set)`.

Since the header data is stored in the same location as the the set's elements, the entire set might be moved to a new location when its size is changed. This means the library calls that modify the set's capacity need to take the address of the set pointer, e.g `&set`, so it can be reassigned. This obviously means that you can't have copies of the same set pointer in multiple locations (unless the set's capacity is constant), but this issue can be easily remedied by indirectly referencing the set, either by referencing some kind of structure that contains the set pointer, or by referencing the set pointer itself.

Here is an example using a function that reassigns the set pointer:

```c
int* baz = set_create();

// adds the number "5" to the set
set_add(&baz, 5); // takes the address of the `int*` in case the pointer needs to be changed
```

This is another similarity to *Simple Dynamic Strings*, which has library calls that are a little more clear in their reassignment, e.g. `s = sdscat(s,"foo")`, but this functionality would break the `set_add` and `set_insert` macros, which already expand to an assignment expression. If you're using a compiler that supports the `typeof` operator, the set macros will perform compile-time checks that will produce an error when the set pointer is used incorrectly.

Some functions never move a set to a new location, so they just take a regular set pointer as an argument:

```c
int* age_set = set_create();

// get the set size
int num_ages = set_size(age_set); // just pass the `int*` without taking its address
```

If the set parameter for a function or macro is named `set`, you don't need to take the address, but if the parameter is named `set_addr`, then you do need to take it.

# Best Practices

Because of the differences between regular arrays and set, it's probably a good idea to try to distinguish them from one another.

One way to do this is to create an alias for various set types using `typedef`:

```c
typedef float* set_float; // set alias for float

set_float qux = set_create();
```

The *recommended* way to differentiate between sets and arrays is to simply name them differently, for example, an array of eggs could be named `eggs` while a set of eggs could be named `egg_set`.

# What About Structures?

If you have a set storing some kind of structure, you can't initialize new elements of the set like you would a variable, e.g. `{ a, b, c }`. To get around this, there's a set of special macros that allow for more control over element initialization.

Here is an example:

```c
typedef struct { ... } foo;

foo* foo_set = set_create();

// the lifetime of temp is not guaranteed to be long; don't use this pointer after initialization
foo* temp = set_add_dst(&foo_set);
temp->a = 1;
temp->b = 2;
temp->c = 3;
temp = NULL; // stop using temp now that the element is initialized
```

# Reference Sheet

Below is a cheat sheet for this library's functions and macros.

Some functions and macros take a normal set argument, e.g. `set`, while others can change the sets memory location and therefore require the address of the set pointer, e.g. `&set`. You should get a compile-time error if you use a set pointer incorrectly. Parameter names will also indicate whether or not to take the address of the set pointer.

| Action                                  | Code                                    | Changes set address?    |
|-----------------------------------------|-----------------------------------------|-------------------------|
| create a set                            | `type* set = set_create();`             | N/A                     |
| free a set                              | `set_free(set);`                        | N/A                     |
| add `item` to the set `set`             | `set_add(&set, item);`                  | yes                     |
| insert `item` into `set` at index `9`   | `set_insert(&set, 9, item)`             | yes                     |
| erase `4` items from `set` at index `3` | `set_erase(set, 3, 4);`                 | no (moves elements)     |
| remove item at index `3` from `set`     | `set_remove(set, 3);`                   | no (moves elements)     |
| get the number of items in `set`        | `int size = set_size(set);`             | no                      |
| get the storage capacity of `set`       | `int capacity = set_get_capacity(set);` | no                      |
| add `item` to the set `set`             | `type* temp = set_add_dst(&set);`       | yes                     |
| insert `item` into `set` at index `9`   | `type* temp = set_insert_dst(&set, 9);` | yes                     |
| reserve space for 255 items in `set`    | `set_reserve(&set, 255);`               | yes                     |
| make a copy of `set`                    | `type* set_copy = set_copy(set);`       | no                      |

# Missing typeof Reference Sheet

Because some compilers don't support the `typeof` operator, which is used for static type checks in some of this library's macros, you have to use a slightly different set of macros. Unfortunately, this also means some errors will be missed at compile time, so if you're getting runtime errors, make sure you are properly using `set` and `&set` for their corresponding calls.

| Action                                  | Code                                             | Changes set address?    |
|-----------------------------------------|--------------------------------------------------|-------------------------|
| add `item` to the set `set`             | `set_add(&set, type) = item;`                    | yes                     |
| insert `item` into `set` at index `9`   | `set_insert(&set, type, 9) = item;`              | yes                     |
| erase `4` items from `set` at index `3` | `set_erase(set, type, 3, 4);`                    | no (moves elements)     |
| remove item at index `3` from `set`     | `set_remove(set, type, 3);`                      | no (moves elements)     |
| add `item` to the set `set`             | `type* temp = set_add_dst(&set, type);`          | yes                     |
| insert `item` into `set` at index `9`   | `type* temp = set_insert_dst(&set, type, 9);`    | yes                     |
