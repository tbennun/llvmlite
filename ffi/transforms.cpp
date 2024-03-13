#include "core.h"
#include "llvm/IR/Function.h"
#include "llvm-c/Core.h"
#include "llvm-c/Target.h"
#include "llvm-c/Transforms/PassManagerBuilder.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <llvm-c/Types.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Support/ErrorHandling.h>
#include <llvm/Transforms/Utils/CodeExtractor.h>

extern "C" {

API_EXPORT(LLVMPassManagerBuilderRef)
LLVMPY_PassManagerBuilderCreate() { return LLVMPassManagerBuilderCreate(); }

API_EXPORT(void)
LLVMPY_PassManagerBuilderDispose(LLVMPassManagerBuilderRef PMB) {
    LLVMPassManagerBuilderDispose(PMB);
}

API_EXPORT(void)
LLVMPY_PassManagerBuilderPopulateModulePassManager(
    LLVMPassManagerBuilderRef PMB, LLVMPassManagerRef PM) {
    LLVMPassManagerBuilderPopulateModulePassManager(PMB, PM);
}

API_EXPORT(unsigned)
LLVMPY_PassManagerBuilderGetOptLevel(LLVMPassManagerBuilderRef PMB) {
    llvm::PassManagerBuilder *pmb = llvm::unwrap(PMB);
    return pmb->OptLevel;
}

API_EXPORT(void)
LLVMPY_PassManagerBuilderSetOptLevel(LLVMPassManagerBuilderRef PMB,
                                     unsigned OptLevel) {
    LLVMPassManagerBuilderSetOptLevel(PMB, OptLevel);
}

API_EXPORT(unsigned)
LLVMPY_PassManagerBuilderGetSizeLevel(LLVMPassManagerBuilderRef PMB) {
    llvm::PassManagerBuilder *pmb = llvm::unwrap(PMB);
    return pmb->SizeLevel;
}

API_EXPORT(void)
LLVMPY_PassManagerBuilderSetSizeLevel(LLVMPassManagerBuilderRef PMB,
                                      unsigned SizeLevel) {
    LLVMPassManagerBuilderSetSizeLevel(PMB, SizeLevel);
}

API_EXPORT(int)
LLVMPY_PassManagerBuilderGetDisableUnrollLoops(LLVMPassManagerBuilderRef PMB) {
    llvm::PassManagerBuilder *pmb = llvm::unwrap(PMB);
    return pmb->DisableUnrollLoops;
}

API_EXPORT(void)
LLVMPY_PassManagerBuilderSetDisableUnrollLoops(LLVMPassManagerBuilderRef PMB,
                                               LLVMBool Value) {
    LLVMPassManagerBuilderSetDisableUnrollLoops(PMB, Value);
}

API_EXPORT(void)
LLVMPY_PassManagerBuilderUseInlinerWithThreshold(LLVMPassManagerBuilderRef PMB,
                                                 unsigned Threshold) {
    LLVMPassManagerBuilderUseInlinerWithThreshold(PMB, Threshold);
}

API_EXPORT(void)
LLVMPY_PassManagerBuilderPopulateFunctionPassManager(
    LLVMPassManagerBuilderRef PMB, LLVMPassManagerRef PM) {
    LLVMPassManagerBuilderPopulateFunctionPassManager(PMB, PM);
}

API_EXPORT(void)
LLVMPY_PassManagerBuilderSetLoopVectorize(LLVMPassManagerBuilderRef PMB,
                                          int Value) {
    llvm::PassManagerBuilder *pmb = llvm::unwrap(PMB);
    pmb->LoopVectorize = Value;
}

API_EXPORT(int)
LLVMPY_PassManagerBuilderGetLoopVectorize(LLVMPassManagerBuilderRef PMB) {
    llvm::PassManagerBuilder *pmb = llvm::unwrap(PMB);
    return pmb->LoopVectorize;
}

API_EXPORT(void)
LLVMPY_PassManagerBuilderSetSLPVectorize(LLVMPassManagerBuilderRef PMB,
                                         int Value) {
    llvm::PassManagerBuilder *pmb = llvm::unwrap(PMB);
    pmb->SLPVectorize = Value;
}

API_EXPORT(int)
LLVMPY_PassManagerBuilderGetSLPVectorize(LLVMPassManagerBuilderRef PMB) {
    llvm::PassManagerBuilder *pmb = llvm::unwrap(PMB);
    return pmb->SLPVectorize;
}

// Extracts a single basic block from a function using the CodeExtractor
// interface.
API_EXPORT(LLVMValueRef)
LLVMPY_ExtractBasicBlock(LLVMValueRef Func, LLVMValueRef BBlock) {

    llvm::Function *Fn = llvm::dyn_cast<llvm::Function>(llvm::unwrap(Func));
    if (!Fn)
        llvm::report_fatal_error((std::string(__FILE__) + ":" +
                                  std::to_string(__LINE__) +
                                  "Expected function")
                                     .c_str());

    llvm::BasicBlock *BB = llvm::dyn_cast<llvm::BasicBlock>(llvm::unwrap(BBlock));
    if (!BB)
        llvm::report_fatal_error((std::string(__FILE__) + ":" +
                                  std::to_string(__LINE__) +
                                  "Expected basic block")
                                     .c_str());

    llvm::CodeExtractorAnalysisCache CEAC{*Fn};
    llvm::CodeExtractor Extractor{{BB},
                                  /* DominatorTree */ nullptr,
                                  /* AggregateArgs */ false,
                                  /* BlockFrequencyInfo */ nullptr,
                                  /* BranchProbabilityInfo */ nullptr,
                                  /* AssumptionCache */ nullptr,
                                  /* AllowVarArgs */ false,
                                  /* AllowAlloca */ true,
                                  /* Suffix */ "bblock_extract"};
    llvm::Function *Outlined = Extractor.extractCodeRegion(CEAC);
    std::string FnName = Outlined->getName().str();
    // Remove '.' added by extractor in function name to avoid invalid function
    // names in C.
    std::replace(FnName.begin(), FnName.end(), '.', '_');
    Outlined->setName(FnName);
    if (Outlined == nullptr)
        llvm::report_fatal_error("extractCodeRegion failed, not eligible");
    return llvm::wrap(Outlined);
}

} // end extern "C"
