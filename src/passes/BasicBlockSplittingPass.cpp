#include "../../include/passes/BasicBlockSplittingPass.h"

using namespace llvm;

PreservedAnalyses BasicBlockSplittingPass::run(Module &M, ModuleAnalysisManager &) {
    errs() << "[+] Pass registered: BasicBlockSplittingPass\n";

    for (Function &F : M) {
        if (F.isDeclaration() || F.empty()) { // skip external and empty functions
            continue;
        }

        // splitBasicBlocks(F);
    }

    return PreservedAnalyses::none();
}