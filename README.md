# llvm-obfuscator

This project provides basic samples implementing various obfuscation techniques in order to make reverse engineering and static analysis more difficult.

The passes can be used independently or in combination, in different order, as well as multiple times in a row on the same target to achieve various levels of obfuscation. Have fun tweaking :]



*This project is built under **LLVM 17** and follows the [New Pass Manager](https://llvm.org/docs/NewPassManager.html) infrastructure.*

---

## Passes

- Bogus Control Flow ("bcf")
    - Uses [Collatz conjecture](https://en.wikipedia.org/wiki/Collatz_conjecture) as Opaque predicate | Cao Y, Zhou Z, Zhuang Y. Advancing Code Obfuscation: Novel Opaque Predicate Techniques to Counter Dynamic Symbolic Execution. Computers, Materials & Continua, 2025, 84(1): 1545-1565. https://doi.org/10.32604/cmc.2025.062743
- Control Flow Flattening ("cff")
- Instruction Substitution ("is")
    - `add` instructions | Uses Mixed Boolean-Arithmetic

## TODO

- Allow user to choose depth (number of cycles)
- Implement some of the transformations used in [Trigress](https://tigress.wtf/) (?)
- Add CFG screenshots within this README

## Setup

### Build

```bash
mkdir build
cd build
cmake ..

cd ..
make -C build
```

### Usage example

```bash
# be mindful of optimization levels
clang-17 -S -emit-llvm -O0 samples/cff.c -o out/out.ll

# use -passes="bcf,cff,is" to use other passes (order matters)
opt-17 -load-pass-plugin=build/LLVMObfuscator.so -passes="bcf" out/out.ll -S -o out/out_obfuscated.ll

clang-17 out/out_obfuscated.ll -o out/out
```

---

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

---

*For more information, see the [LLVM documentation](https://llvm.org/docs/).*
