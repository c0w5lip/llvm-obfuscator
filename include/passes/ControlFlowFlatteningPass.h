#pragma once

#include "llvm/IR/PassManager.h"

namespace llvm {
    class ModulePass;

    struct ControlFlowFlatteningPass : PassInfoMixin<ControlFlowFlatteningPass> {
        PreservedAnalyses run(Module &M, ModuleAnalysisManager &);

        static bool isRequired() {
            return true;
        }
    };
}
