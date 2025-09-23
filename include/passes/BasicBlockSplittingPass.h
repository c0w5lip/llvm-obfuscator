#pragma once

#include "llvm/IR/PassManager.h"

namespace llvm {
    class ModulePass;

    struct BasicBlockSplittingPass : PassInfoMixin<BasicBlockSplittingPass> {
        PreservedAnalyses run(Module &M, ModuleAnalysisManager &);

        static bool isRequired() {
            return true;
        }
    };
}
