#include "../include/passes/BogusControlFlowPass.h"
#include "../include/passes/ControlFlowFlatteningPass.h"
#include "../include/passes/InstructionSubstitutionPass.h"

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "LLVMObfuscator", "v1.0", [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback([](StringRef Name, ModulePassManager &MPM, ArrayRef<PassBuilder::PipelineElement>) {

                if (Name == "bcf") {
                    MPM.addPass(BogusControlFlowPass());
                    return true;
                }

                if (Name == "cff") {
                    MPM.addPass(ControlFlowFlatteningPass());
                    return true;
                }

                if (Name == "is") {
                    MPM.addPass(InstructionSubstitutionPass());
                    return true;
                }
                
                return false;
            });
        }
    };
}
