# llvm-obfuscator

This project provides basic samples implementing various obfuscation techniques in order to make reverse engineering and static analysis more difficult.

The passes can be used independently or in combination, in different order, as well as multiple times in a row on the same target to achieve various levels of obfuscation. Have fun tweaking :]

*This project is built under **LLVM 17** and follows the [New Pass Manager](https://llvm.org/docs/NewPassManager.html) infrastructure.*

---

## Passes

- ~~Control Flow Flattening~~ ("cff")
    - seed-based?
- Instruction Substitution ("is")
    - add
    - sub
    - mul
    - ~~div~~

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
clang-17 -S -emit-llvm -O0 samples/cff.c -o samples/cff.ll

# NOTE: this overwrites the original .ll
opt-17 -load-pass-plugin=build/LLVMObfuscator.so -passes="cff" samples/cff.ll -S -o samples/cff.ll

clang-17 samples/cff.ll -o samples/cff
```

---

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

---

*For more information, see the [LLVM documentation](https://llvm.org/docs/).*
