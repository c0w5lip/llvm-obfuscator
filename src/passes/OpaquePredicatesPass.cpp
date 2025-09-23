#include "../../include/passes/OpaquePredicatesPass.h"

using namespace llvm;

PreservedAnalyses OpaquePredicatesPass::run(Module &M, ModuleAnalysisManager &) {
    errs() << "[+] Pass registered: OpaquePredicatesPass\n";

    for (Function &F : M) {
        if (F.isDeclaration() || F.empty()) { // skip external and empty functions
            continue;
        }

        // addOpaquePredicates(F);
    }

    return PreservedAnalyses::none();
}
