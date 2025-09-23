#include "../../include/passes/InstructionSubstitutionPass.h"

using namespace llvm;

PreservedAnalyses InstructionSubstitutionPass::run(Module &M, ModuleAnalysisManager &) {
    errs() << "[+] Pass registered: InstructionSubstitutionPass\n";

    for (Function &F : M) {
        if (F.isDeclaration() || F.empty()) { // skip external and empty functions
            continue;
        }

        // addInstructionSubstitution(F);
    }

    return PreservedAnalyses::none();
}
