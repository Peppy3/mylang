#include <stddef.h>
#include <stdint.h>

#include <stdio.h>

#include <CompileUnit.h>
#include <ir/IR.h>

#define X(name) #name,
static const char *op_name[] = {
#include <ir/IrOpsMacro.h>
NULL
};
#undef X

void Ir_print_instr(FILE *stream, const IrInstr instr) {
	const char *op;
	
	if (instr.op > IR_OP_ENUM_END) {
		op = "invalid";
	}
	else {
		op = op_name[instr.op];
	}

	fprintf(stream, "%s: {%ld, %lu, %lf}\n",
		op, instr.iarg, instr.uarg, instr.farg);
}

void Ir_dump(FILE *stream, CompileUnit *unit) {
	for (size_t i = 0; i < unit->ir.length; i++) {
		Ir_print_instr(stream, unit->ir.program[i]);
	}
}

