#include "../../include/passes/ControlFlowFlatteningPass.h"

using namespace llvm;

PreservedAnalyses ControlFlowFlatteningPass::run(Module &M, ModuleAnalysisManager &) {
    errs() << "[+] Pass registered: ControlFlowFlatteningPass\n";
    for (Function &F : M) {
        // flatten...(F);
    }

    return PreservedAnalyses::none();
}
