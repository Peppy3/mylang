#ifndef SCANNER_H
#define SCANNER_H

#include <stddef.h>
#include <stdint.h>

#include "ParserFile.h"
#include "Token.h"

// moves the ParserFile forward and spits out a token
// line_pos should be initialised with zero (0) before first call
Token NextToken(ParserFile *file, size_t *line_pos);

#endif /* SCANNER_H */
