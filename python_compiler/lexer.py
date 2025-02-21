from pprint import pprint

class Token:
	def __init__(self, token_type, pos, length):
		self.type = token_type
		self.pos = pos
		self.length = length

	def __repr__(self):
		return f"Token(type = {self.type}, pos = {self.pos}, length = {self.length})"

class Lexer:
	def __init__(self, src: str):
		self.src = src
		self.pos = 0
		self.token_start = 0

	def __iter__(self):
		return self
	
	def _make_token(self, token_type):
		return Token(token_type, self.token_start, self.pos - self.token_start)

	def _get_char(self):
		ch = self.src[self.pos]
		self.pos += 1
		return ch

	def _peek_char(self):
		return self.src[self.pos]

	def is_eof(self):
		return self.pos >= len(self.src)

	def scan_escape(self):
		# Only support single char escape right now (hex and such later)
		ch = self._get_char()
		if ch in ('0', 'a', 'b', 'e', 'f', 'n', 'r', 't', 'v', '\\', '\'', '"'):
			return True
		else:
			return False

	
	def scan_multiline_comment(self):
		ch = None
		nesting = 1

		while nesting > 0:
			ch = self._get_char()

			if self.is_eof():
				return

			if ch == '*' and self._peek_char() == '/':
				nesting -= 1
			elif ch == '/' and self._peek_char() == '*':
				nesting += 1

		# capture the final '/'
		self._get_char()


	def scan_single_line_comment(self):
		while True:
			ch = self._get_char();
			if ch == '\n' or self.is_eof():
				return

	def scan_identifier(self):
		ch = self._peek_char()
		while ch.isalnum() or ch == '_':
			self.pos += 1
			ch = self._peek_char()

	def scan_number(self):
		# TODO: hex
		ch = self._peek_char()

		while ch.isdecimal():
			self.pos += 1
			ch = self._peek_char()
		
		if ch == '.':
			self.pos += 1;
			ch = self._peek_char()
			if not ch.isdecimal():
				return self._make_token("Invalid")

			while ch.isdecimal():
				self.pos += 1
				ch = self._peek_char()

			if ch != 'e':
				return self._make_token("FloatLiteral")

			self.pos += 1
			ch = self._peek_char()

			if not ch.isdecimal():
				return self._make_token("Invalid")

			while ch.isdecimal():
				self.pos += 1;
				ch = self._peek_char()

			return self._make_token("FloatLiteral")
		else:
			return self._make_token("IntegerLiteral")

	def scan_char(self):
		ch = self._get_char()

		if ch == '\\' and not self.scan_escape():
			return self._make_token("Invalid")
		elif not ch.isprintable():
			return self._make_token("Invalid")

		# capture the ending '\''
		ch = self._get_char()
		if ch != '\'':
			return self._make_token("Invalid")

		return self._make_token("CharLiteral")

	def scan_string(self):
		ch = self._get_char()

		while ch != '"':
			if not ch.isprintable():
				return self._make_token("Invalid")

			if ch == '\\' and not self.scan_escape():
				return TOKEN_invalid

			ch = self._get_char()

		return self._make_token("StringLiteral")

	def __next__(self):
		while True:
			if self.is_eof(): raise StopIteration

			self.token_start: int = self.pos
			ch = self._get_char()

			if ch.isspace():
				continue
			if ch == '/' and self._peek_char() == '/':
				self.scan_single_line_comment()
				continue
			elif ch == '/' and self._peek_char() == '*':
				self.pos += 1 # pos++ because we already know it's valid
				self.scan_multiline_comment()
				continue

			elif ch.isalpha() or ch == '_':
				self.scan_identifier()
				return self._make_token("Identifier")

			elif ch.isdecimal():
				return self.scan_number()

			elif ch == '(':
				return self._make_token("LeftParen")
			elif ch == ')':
				return self._make_token("RightParen")
			elif ch == '[':
				return self._make_token("LeftSquare")
			elif ch == ']':
				return self._make_token("RightSquare")
			elif ch == '{':
				return self._make_token("LeftCurly")
			elif ch == '}':
				return self._make_token("RightCurly")
			
			elif ch == '\'':
				return self.scan_char()
			elif ch == '"':
				return self.scan_string()

			elif ch == '+' and self._peek_char() == '+':
				self.pos += 1
				return self._make_token("Incement")
			elif ch == '+' and self._peek_char() == '=':
				self.pos += 1
				return self._make_token("AssignAdd")
			elif ch == '+':
				return self._make_token("Addition")

			elif ch == '-' and self._peek_char() == '-':
				self.pos += 1
				return self._make_token("Decrement")
			elif ch == '-' and self._peek_char() == '=':
				self.pos += 1
				return self._make_token("AssignSub")
			elif ch == '-':
				return self._make_token("Subtraction")

			elif ch == '.':
				return self._make_token("Period")

			else:
				return self._make_token("Invalid")


