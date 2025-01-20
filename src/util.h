#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>

#include "ParserFile.h"
#include "Token.h"

void print_token(FILE *stream, ParserFile *src, Token *token);

#endif /* UTIL_H_ */
