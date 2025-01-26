# mylang


## WARNING !! In early stages !!

The specifications are open for discussion.
Talk to me directly or make an issue for language ideas.

Right now this compiler is only for posix

Build dependencies:

- ISO C11 standard compliant C compiler
- cmake
- favourite build tool that works with cmake

### How to build

Use cmake to configure and build the project

	mkdir build && cd build
 	cmake .. -DCMAKE_BUILD_TYPE=Release
  	make

### Simple hello world

```
linkname "main";

import std.io as io;

pub main: () -> int {
	io.printf("Hello world!\n");
	return 0;
}

```

### Running the damn thing

Right now the compiler takes in one file argument and nothing more.
It will dump out an ast parsed from the file if it did not get any parse errors.

I recommend looking at the output though less as

	./mylang ../test/test2.txt

