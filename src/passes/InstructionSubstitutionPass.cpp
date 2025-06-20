#include "../../include/passes/InstructionSubstitutionPass.h"

#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

PreservedAnalyses InstructionSubstitutionPass::run(Module &M, ModuleAnalysisManager &) {
    errs() << "[+] Pass registered: InstructionSubstitutionPass\n";
    
    for (Function &F : M) {
        if (F.isDeclaration()) { // skip external functions
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


                        // A + B = (A ^ B) - 2*(~A & B)
                        Value *xor_ab = builder.CreateXor(a, b);
                        Value *not_a = builder.CreateNot(a);
                        Value *and_not_a_b = builder.CreateAnd(not_a, b);
                        Value *shl = builder.CreateShl(and_not_a_b, ConstantInt::get(type, 1));
                        Value *result = builder.CreateSub(xor_ab, shl);

                        BO->replaceAllUsesWith(result);
                        BO->eraseFromParent();

                        errs() << "[+] Obfuscated add in function: " << F.getName() << "\n";
                        continue;
                    }

                    if (BO->getOpcode() == Instruction::Sub && BO->getType()->isIntegerTy(32)) {
                        Value *a = BO->getOperand(0);
                        Value *b = BO->getOperand(1);

                        // A - B = (A ^ -B) + 2*(A & -B)
                        Value *minus_b = builder.CreateSub(ConstantInt::get(type, 0), b);
                        Value *a_xor_minus_b = builder.CreateXor(a, minus_b);
                        Value *a_and_minus_b = builder.CreateAnd(a, minus_b);
                        Value *shl = builder.CreateShl(a_and_minus_b, ConstantInt::get(type, 1));
                        Value *result = builder.CreateAdd(a_xor_minus_b, shl);

                        BO->replaceAllUsesWith(result);
                        BO->eraseFromParent();
                        
                        errs() << "[+] Obfuscated sub in function: " << F.getName() << "\n";
                        continue;
                    }


                    if (BO->getOpcode() == Instruction::Mul && BO->getType()->isIntegerTy(32)) {
                        Value *a = BO->getOperand(0);
                        Value *b = BO->getOperand(1);

                        Value *result = ConstantInt::get(type, 0);

                        for (unsigned i = 0; i < 32; ++i) {
                            Value *shift = builder.CreateLShr(b, ConstantInt::get(type, i));
                            Value *bit = builder.CreateAnd(shift, ConstantInt::get(type, 1));
                            Value *a_shift = builder.CreateShl(a, ConstantInt::get(type, i));
                            Value *masked = builder.CreateAnd(a_shift, builder.CreateSExt(bit, type));
                            result = builder.CreateAdd(result, masked);
                        }

                        BO->replaceAllUsesWith(result);
                        BO->eraseFromParent();

                        errs() << "[+] Obfuscated mul in function: " << F.getName() << "\n";
                        continue;
                    }



                }


            }
        }
    }
    

    return PreservedAnalyses::none();
}
