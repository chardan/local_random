**local_random** is a wrapper for the standard library's `<random>` facilities 
that provides thread-local seeding and a straightforward interface suitiable for
basic, non-cryptographic uses.

Mini-examples, generate a number between 0 and a maximum (inclusive):

``` c++
#include "local_random"

using namespace local_random;

// ...

int x = generate_random_number(10);
float y = generate_random_number(10.0);

auto rng = random_number_generator(10);
int z = rng();
```

...there are plenty of things you can tune, but that's the basic idea! Each 
function or function object has a thread-local engine that's seeded the
first time it's used. Reads from the PRNG are mutex-guarded.

Features:
	- self-seeding (per-thread)
	- thread-safe
	- flexible parameters
	- multiple interfaces (functions, classes)
	- straightforward use

Dependencies:

`C++17`

`Catch2` if you want to build the tests.

To build:

No build is necessary, the library is header only.

To build examples:

cd examples 
make

To build tests:

The unit tests use `Catch2`. The Makefile looks for Catch2 in CATCH2_DIR, which
defaults to "deps/Catch2/single_include" in the test directory.

A note on this project:

This is a fork of a random library developed for Ceph. I was the original designer
and implementer, but this project would not have been possible without generous 
input from members of the Ceph community (especially Casey Bodley (@cbodley) and 
Adam Emerson (@adamemerson)). 

I have forked this work in order to provide a standalone library suitable for uses 
outside of Ceph.
