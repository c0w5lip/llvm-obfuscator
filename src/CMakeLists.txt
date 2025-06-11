add_llvm_pass_plugin(LLVMObfuscator
    passes/ControlFlowFlatteningPass.cpp
    passes/InstructionSubstitutionPass.cpp

    RegisterPasses.cpp
)
