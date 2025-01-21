#ifndef SCANNER_H
#define SCANNER_H

#include <stddef.h>
#include <stdint.h>

#include "ParserFile.h"
#include "Token.h"

// moves the ParserFile forward and spits out a token
Token NextToken(ParserFile *file);

#endif /* SCANNER_H */

