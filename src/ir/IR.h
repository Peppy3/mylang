#ifndef IR_IR_H_
#define IR_IR_H_

#include <stddef.h>
#include <stdint.h>

#include <stdio.h>

#include <CompileUnit.h>

/*
idea: 
	use a stack based approach and store local variables / structs on the stack
	push / pop and load / store (stack relative) operations to access them
	values for func args are passed on the stack (first arg on top)
	use two regs for operations
*/

#define X(name) IrOp_##name,
typedef enum IrOp {
#include <ir/IrOpsMacro.h>
IR_OP_ENUM_END
} IrOp;
#undef X

typedef struct IrInstr {
	size_t op;
	union {
		int64_t iarg;
		uint64_t uarg;
		double farg;
	};
} IrInstr;

typedef struct IR {
	size_t length;
	IrInstr *program;
} IR;

#endif /* IR_IR_H_ */

