# mylang


## WARNING !! In early stages !!

Rough sketch of a spec in ``mylang.spec.txt``.

The specifications are open for discussion.
Talk to me directly or make a pull request for language ideas.

Build dependencies:

- Python 3.10 or later
- ISO C11 standard compliant C compiler


### How to build

A python build script is provided to build the compiler.
Run the python file with your favourite python interpreter.

	python3 -m build

There is some arguments you can pass to the build script
please run the script with ``--help`` for more information


### Running the damn thing

Right now the compiler takes in one file argument and nothing more.
It will spit out a token stream onto stdout and any internal errors on
stderr. I recommend looking at the output though ``less``

	./mylang test/test1.txt

