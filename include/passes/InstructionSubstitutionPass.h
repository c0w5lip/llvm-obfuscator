#pragma once

#include "llvm/IR/PassManager.h"

namespace llvm {
    class ModulePass;

    struct InstructionSubstitutionPass : PassInfoMixin<InstructionSubstitutionPass> {
        PreservedAnalyses run(Module &M, ModuleAnalysisManager &);

        static bool isRequired() {
            return true;
        }
    };
}
