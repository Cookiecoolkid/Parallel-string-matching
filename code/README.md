# Parallel String Matching Project

## 项目结构
```sh
.
├── code
│   ├── antivirus_brute_force.cpp
│   ├── antivirus_brute_force_parallel.cpp
│   ├── antivirus_kmp.cpp
│   ├── antivirus_kmp_parallel.cpp
│   ├── antivirus_trie.cpp
│   ├── antivirus_trie_parallel.cpp
│   ├── document_brute_force.cpp
│   ├── document_brute_force_parallel.cpp
│   ├── document_kmp.cpp
│   ├── document_kmp_parallel.cpp
│   ├── document_trie.cpp
│   └── document_trie_parallel.cpp
├── Makefile
├── result_document.txt
└── result_software.txt
```

## 编译项目

项目使用 `C++` 编写并且使用 `Makefile` 进行编译。
通过以下命令在 `Makefile` 所在目录（即项目根目录）编译所有目标：

```sh
make
```

如果希望在编译时启用 VERBOSE 宏以输出执行时间，可以使用以下命令：

```sh
make VERBOSE=1
```

使用以下命令清理编译生成的文件和目录：
    
```sh   
make clean
```

得到的可执行文件将会被放置在项目根目录下。

## 代码文件说明
antivirus_brute_force.cpp：使用暴力算法进行病毒检测。
antivirus_brute_force_parallel.cpp：使用并行暴力算法进行病毒检测。
antivirus_kmp.cpp：使用KMP算法进行病毒检测。
antivirus_kmp_parallel.cpp：使用并行KMP算法进行病毒检测。
antivirus_trie.cpp：使用Trie树进行病毒检测。
antivirus_trie_parallel.cpp：使用并行Trie树进行病毒检测。
document_brute_force.cpp：使用暴力算法进行文档匹配。
document_brute_force_parallel.cpp：使用并行暴力算法进行文档匹配。
document_kmp.cpp：使用KMP算法进行文档匹配。
document_kmp_parallel.cpp：使用并行KMP算法进行文档匹配。
document_trie.cpp：使用Trie树进行文档匹配。
document_trie_parallel.cpp：使用并行Trie树进行文档匹配。

- 针对场景一文档检索，对于使用了并行算法(_parallel后缀)的代码，以 `document_trie_parallel.cpp` 性能最为优秀，可以通过运行这个文件对应的可执行文件进行测试。
- 针对场景二病毒检测，对于使用了并行算法(_parallel后缀)的代码，以 `antivirus_trie_parallel.cpp` 性能最为优秀，可以通过运行这个文件对应的可执行文件进行测试。
- 可以选择 `make VERBOSE=1` 编译，运行时输出执行时间更便于复现检查。