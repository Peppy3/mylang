from pprint import pprint

from lexer import Lexer

src: str = """
53.4
self
1.0e100
1.04
'\\n'
'\\0'
"\\\"this is a string\\""
aboba
++ +
-- -
() [] {}
.
-= +=
boo!
"""

def main() -> int:
	lexer = Lexer(src)
	
	for token in lexer:
		if token.type == "Invalid":
			print(f"Invalid token: {lexer.src[token.pos : token.pos + token.length]!r}")
		else:
			print(token)


if __name__ == "__main__": exit(main())

