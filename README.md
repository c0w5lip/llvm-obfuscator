# llvm-obfuscator

This project provides basic samples implementing various obfuscation techniques in order to make reverse engineering and static analysis more difficult.

The passes can be used independently or in combination to achieve various levels of obfuscation. Have fun tweaking :]

*This project is built under **LLVM 17** and follows the [New Pass Manager](https://llvm.org/docs/NewPassManager.html) infrastructure.*

---

## Passes

- ~~Control Flow Flattening~~
- Instruction Substitution
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

### Usage

```bash
clang-17 -S -emit-llvm -O1 samples/is/is.c -o samples/is/is.ll

opt-17 -load-pass-plugin=build/src/LLVMObfuscator.so -passes="is" samples/is/is.ll -S -o samples/is/is_o.ll

clang-17 samples/is/is_o.ll -o samples/is/is_o.out
```

---

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

---

*For more information, see the [LLVM documentation](https://llvm.org/docs/).*
