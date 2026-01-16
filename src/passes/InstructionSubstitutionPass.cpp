#include "../../include/passes/InstructionSubstitutionPass.h"

#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;


PreservedAnalyses InstructionSubstitutionPass::run(Module &M, ModuleAnalysisManager &) {
    errs() << "[+] Pass registered: InstructionSubstitutionPass\n";
    
    for (Function &F : M) {
        if (F.isDeclaration() || F.empty()) { // skip external and empty functions
            continue;
        } 

        for (BasicBlock &BB : F) {

            for (auto i = BB.begin(); i != BB.end();) {
                
                Instruction &I = *i++;

                if (auto BO = dyn_cast<BinaryOperator>(&I)) {
                    IRBuilder<> builder(BO);
                    LLVMContext &context = BO->getContext();
                    Type *type = Type::getInt32Ty(context);


                    if (BO->getOpcode() == Instruction::Add && BO->getType()->isIntegerTy(32)) {
                        Value *a = BO->getOperand(0);
                        Value *b = BO->getOperand(1);

                        // "a + b" -> "(a ^ b) + 2 * (a & b)"
                        Value *xorInst = builder.CreateXor(a, b, "xor_obf");
                        Value *andInst = builder.CreateAnd(a, b, "and_obf");
                        Value *shlInst = builder.CreateShl(andInst, ConstantInt::get(type, 1), "shl_obf");
                        Value *result = builder.CreateAdd(xorInst, shlInst, "add_obf");

                        BO->replaceAllUsesWith(result);
                        BO->eraseFromParent();

                        errs() << "[+] Obfuscated add instruction in function: " << F.getName() << "\n";
                        continue;
                    }

                    // TODO...
                    if (BO->getOpcode() == Instruction::Sub && BO->getType()->isIntegerTy(32)) {}
                }
            }
        }
    }

    return PreservedAnalyses::none();
}
