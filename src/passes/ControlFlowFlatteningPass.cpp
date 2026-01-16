w#include "../../include/passes/ControlFlowFlatteningPass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/Local.h" // For DemotePHIToStack
#include "llvm/Transforms/Utils/BasicBlockUtils.h" // For SplitBlock
#include <vector>
#include <map>

using namespace llvm;

PreservedAnalyses ControlFlowFlatteningPass::run(Module &M, ModuleAnalysisManager &) {
    errs() << "[+] Pass registered: ControlFlowFlatteningPass\n";

    for (Function &F : M) {
        if (F.isDeclaration() || F.empty()) {
            continue;
        }
        

    }

    return PreservedAnalyses::none();
}
