#include "../../include/passes/BogusControlFlowPass.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/Cloning.h"

#include <vector>

using namespace llvm;

PreservedAnalyses BogusControlFlowPass::run(Module &M, ModuleAnalysisManager &) {
    errs() << "[+] Executing: BogusControlFlowPass\n";

    LLVMContext &Ctx = M.getContext();

    for (Function &F : M) {
        if (F.isDeclaration() || F.empty()) {
            continue;
        }

        // We first need to gather every basic bloc prior to modifying them because we will be adding blocks
        std::vector<BasicBlock*> basicBlocks;

        for (BasicBlock &BB : F) {
            basicBlocks.push_back(&BB);
        }

        for (BasicBlock *BB : basicBlocks) {
            // "Note that this only works on well formed basic blocks (must have a terminator)"
            // -> we need to check that the block is well formed, which is the case if getTerminator() returns true
            Instruction *terminator = BB->getTerminator();
            if (!terminator) {
                continue;
            }

            BasicBlock *coreBB = BB->splitBasicBlock(terminator);


            // we clone the core logic
            // todo: add junk mutation (?)
            ValueToValueMapTy VMap;
            BasicBlock *junkBB = CloneBasicBlock(coreBB, VMap, "junk_bb", &F);


            // "an unconditional branch is added to the original BB"
            // -> we need to remove this branch
            BB->getTerminator()->eraseFromParent();

            // we generate an opaque predicate: 
            // now we add the opaque predicate to the entry
            IRBuilder<> entryBBIRB(BB);



            // --- collatz opaque predicate
            BasicBlock *collatzHeaderBB = BasicBlock::Create(Ctx, "collatz.header", &F);
            BasicBlock *collatzBodyBB = BasicBlock::Create(Ctx, "collatz.body", &F);
            BasicBlock *collatzExitBB = BasicBlock::Create(Ctx, "collatz.exit", &F);

            entryBBIRB.CreateBr(collatzHeaderBB);
            entryBBIRB.SetInsertPoint(collatzHeaderBB);

            // jump to body if n != 1, jump to exit otherwise
            
            PHINode *phiNode = entryBBIRB.CreatePHI(Type::getInt32Ty(Ctx), 2, "n");
            phiNode->addIncoming(ConstantInt::get(Type::getInt32Ty(Ctx), 1337), BB); // arbitrary const

            Value *notOne = entryBBIRB.CreateICmpNE(phiNode, entryBBIRB.getInt32(1));
            entryBBIRB.CreateCondBr(notOne, collatzBodyBB, collatzExitBB);

            entryBBIRB.SetInsertPoint(collatzBodyBB);

            // (n % 2 == 0)
            Value *isEven = entryBBIRB.CreateICmpEQ(
                entryBBIRB.CreateURem(phiNode, entryBBIRB.getInt32(2)), 
                entryBBIRB.getInt32(0)
            );  

            // If even: n / 2
            Value *evenValue = entryBBIRB.CreateUDiv(phiNode, entryBBIRB.getInt32(2));
            // If odd: 3n + 1
            Value *oddValue = entryBBIRB.CreateAdd(
                entryBBIRB.CreateMul(phiNode, entryBBIRB.getInt32(3)), 
                entryBBIRB.getInt32(1)
            );

            Value *NextVal = entryBBIRB.CreateSelect(isEven, evenValue, oddValue);
            phiNode->addIncoming(NextVal, collatzBodyBB);
            entryBBIRB.CreateBr(collatzHeaderBB);


            entryBBIRB.SetInsertPoint(collatzExitBB);

            Value *isOne = entryBBIRB.CreateICmpEQ(phiNode, entryBBIRB.getInt32(1));
            entryBBIRB.CreateCondBr(isOne, coreBB, junkBB);
            

            junkBB->getTerminator()->eraseFromParent();
            IRBuilder<> junkBBIRB(junkBB);
            junkBBIRB.CreateBr(coreBB);
        }

    }

    return PreservedAnalyses::none();
}