#ifndef PARSER_H_
#define PARSER_H_

#include <CompileUnit.h>

// parses the source file
// returns number of errors catched and -1 if there is a internal parser error
int parse(CompileUnit *unit);

#endif /* PARSER_H_ */

