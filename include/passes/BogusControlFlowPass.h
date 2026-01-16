#pragma once

#include "llvm/IR/PassManager.h"

namespace llvm {
    class ModulePass;

    struct BogusControlFlowPass : PassInfoMixin<BogusControlFlowPass> {
        PreservedAnalyses run(Module &M, ModuleAnalysisManager &);

        static bool isRequired() {
            return true;
        }
    };
}
