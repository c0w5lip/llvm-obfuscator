#pragma once

#include "llvm/IR/PassManager.h"

namespace llvm {
    class ModulePass;

    struct StringEncryptionPass : PassInfoMixin<StringEncryptionPass> {
        PreservedAnalyses run(Module &M, ModuleAnalysisManager &);

        static bool isRequired() {
            return true;
        }
    };
}
