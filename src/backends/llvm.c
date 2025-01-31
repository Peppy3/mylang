#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdlib.h>
#include <stdio.h>

#include <llvm-c/Core.h>
#include <llvm-c/Types.h>



void generate_llvm(void) {
	LLVMModuleRef module = LLVMModuleCreateWithName("hello_world");
	LLVMSetTarget(module, "aarch64-unknown-linux-gnu");

	LLVMTypeRef main_func_args[2] = {
		LLVMInt32Type(),
		LLVMPointerType(LLVMInt64Type(), 0)
	};

	LLVMTypeRef main_func = LLVMFunctionType(LLVMInt32Type(), main_func_args, 2, false);
	LLVMValueRef main_func_val = LLVMAddFunction(module, "main", main_func);

	LLVMBasicBlockRef entry = LLVMAppendBasicBlock(main_func_val, "entry");
	
	LLVMBuilderRef builder = LLVMCreateBuilder();
	LLVMPositionBuilderAtEnd(builder, entry);

	LLVMBuildRet(builder, LLVMConstInt(LLVMInt32Type(), 42, false));

	LLVMDisposeBuilder(builder);

	char *llvm_err = NULL;
	if (LLVMPrintModuleToFile(module, "out.ll", &llvm_err)) {
		printf("LLVM ERROR: %s\n", llvm_err);
		LLVMDisposeMessage(llvm_err);
	}

	LLVMDisposeModule(module);
}



