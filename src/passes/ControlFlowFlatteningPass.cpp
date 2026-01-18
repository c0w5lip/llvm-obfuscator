#include "../../include/passes/ControlFlowFlatteningPass.h"
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


        // flattening erases the predecessor reference from flattened block
        // therefore, the phi nodes can't be resolved anymore
        // to fix it, we need to "demote" them (put them on the stack)
        std::vector<PHINode *> phiNodesToDemote;
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                if (auto *phiNode = dyn_cast<PHINode>(&I)) { // if we encounter a phi node
                    phiNodesToDemote.push_back(phiNode);
                }
            }
        }

        for (auto *phiNode : phiNodesToDemote) {
            // demote phi node; insert 'alloca' at the start of the function
            DemotePHIToStack(phiNode, F.begin()->getTerminator());
        }



        // gather basic blocks
        std::vector<BasicBlock *> basicBlocksToFlatten;
        for (BasicBlock &BB : F) {
            basicBlocksToFlatten.push_back(&BB);
        }

        // skip trivial functions
        if (basicBlocksToFlatten.size() <= 1) {
            continue;
        }

        errs() << "[*] ControlFlowFlatteningPass: Flattening function '" << F.getName() << "'\n";



        // get the entry block and remove it from the blocks to flatten
        BasicBlock *entryBasicBlock = basicBlocksToFlatten[0];
        basicBlocksToFlatten.erase(basicBlocksToFlatten.begin());

        // if the said entry block ends in a conditional jump,
        // we split it to ensure we have a clean block to insert our code into
        if (entryBasicBlock->getTerminator()->getNumSuccessors() > 1) {
            BasicBlock *newBasicBlock = SplitBlock(entryBasicBlock, entryBasicBlock->getTerminator());
            basicBlocksToFlatten.insert(basicBlocksToFlatten.begin(), newBasicBlock);
        }




        IRBuilder<> builder(F.getContext());

        BasicBlock *loopEntry = BasicBlock::Create(F.getContext(), "loop_entry", &F, entryBasicBlock);
        BasicBlock *loopEnd = BasicBlock::Create(F.getContext(), "loop_end", &F, entryBasicBlock);
        // where the switch goes if it breaks (back to loopEntry)
        BasicBlock *switchDefault = BasicBlock::Create(F.getContext(), "swtich_default", &F, entryBasicBlock);


        loopEntry->moveAfter(entryBasicBlock);
        loopEnd->moveAfter(loopEntry);
        switchDefault->moveAfter(loopEnd);


        // jump from entry to dispatcher
        entryBasicBlock->getTerminator()->eraseFromParent();
        BranchInst::Create(loopEntry, entryBasicBlock); // entryBasicBlock -> loopEntry
    

        // create switchVariable (n) in the entryBasicBlock
        builder.SetInsertPoint(entryBasicBlock->getTerminator());
        AllocaInst *switchVariable = builder.CreateAlloca(builder.getInt32Ty(), nullptr, "switchVariable");

        // create a map for BB->Identifier
        uint32_t i = 1;
        std::map<BasicBlock *, uint32_t> basicBlockToIndentifier;
        for (BasicBlock *BB : basicBlocksToFlatten) {
            basicBlockToIndentifier[BB] = i++;
        }

        // we set the initial value for out switchVariable as the first identifier
        builder.CreateStore(builder.getInt32(basicBlockToIndentifier[basicBlocksToFlatten[0]]), switchVariable);



        // build the dispatcher
        builder.SetInsertPoint(loopEntry);
        LoadInst *loadState = builder.CreateLoad(builder.getInt32Ty(), switchVariable, "switchVariable");
        SwitchInst *switchInstruction = builder.CreateSwitch(loadState, switchDefault, basicBlocksToFlatten.size());

        // close loop
        BranchInst::Create(loopEntry, switchDefault); // switchDefault -> loopEntry
        BranchInst::Create(loopEntry, loopEnd); // loopEnd -> loopEntry


        // populate switch statement with blocks
        for (BasicBlock *BB : basicBlocksToFlatten) {
            switchInstruction->addCase(builder.getInt32(basicBlockToIndentifier[BB]), BB);

            Instruction *terminator = BB->getTerminator();

            // ignore returns
            if (isa<ReturnInst>(terminator)) {
                continue;
            }

            
            builder.SetInsertPoint(terminator);


            // handle branches
            if (auto *branch = dyn_cast<BranchInst>(terminator)) {
                // if the last instruction of block is conditional,
                // we compute the condition and set switchVariable accordingly
                if (branch->isConditional()) {
                    Value *condition = branch->getCondition();

                    BasicBlock *trueSuccessor = branch->getSuccessor(0);
                    BasicBlock *falseSucessor = branch->getSuccessor(1);

                    Value *nextSwitchVariableValue = builder.CreateSelect(
                        condition,
                        builder.getInt32(basicBlockToIndentifier[trueSuccessor]),
                        builder.getInt32(basicBlockToIndentifier[falseSucessor])
                    );

                    builder.CreateStore(nextSwitchVariableValue, switchVariable);
                } else { // not conditional, we just set switchVariable to the next block Identifier
                    builder.CreateStore(builder.getInt32(basicBlockToIndentifier[branch->getSuccessor(0)]), switchVariable);
                }

                builder.CreateBr(loopEnd); // BB -> loopEnd
                terminator->eraseFromParent(); // remove the terminator
            }
        }


    }

    return PreservedAnalyses::none();
}
