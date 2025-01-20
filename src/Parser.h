#ifndef PARSER_H_
#define PARSER_H_

#include "ParserFile.h"
#include "Token.h"

// parses the source file
// returns number of errors catched and -1 if there is a internal parser error
int parse(char *source_path);

#endif /* PARSER_H_ */

