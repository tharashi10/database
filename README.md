# database
### DevInfo
```cpp
$ sw_vers
ProductName:    macOS
ProductVersion: 12.2.1
BuildVersion:   21D62
---
$ g++ -v
Apple clang version 13.1.6 (clang-1316.0.21.2.5)
Target: arm64-apple-darwin21.3.0
Thread model: posix
InstalledDir: /Library/Developer/CommandLineTools/usr/bin
---
$ cmake --version
cmake version 3.24.2
```

### 
| ## | Detail | Summary |
|----|---|---|
| 01 | Introduction and Setting up the REPL | REPLの実装(Read execute-print Loop)<br>対話型インタプリタを実装する<br>Tokenizer(字句解析)は入力文字をToken列に分割する |
| 02 | World’s Simplest SQL Compiler and Virtual Machine |SQLコンパイラの実装<br>SQLコンパイラは文字列をパースする<br>そしてByteCode(中間コード)に変換し仮想マシンへ返す|
| 03 | An In-Memory, Append-Only, Single-Table Database ||
| 04 | Our First Tests (and Bugs) ||
| 05 | Persistence to Disk ||
| 06 | The Cursor Abstraction ||
| 07 | Introduction to the B-Tree ||
| 08 | B-Tree Leaf Node Format ||
| 09 | Binary Search and Duplicate Keys ||
| 10 | Splitting a Leaf Node ||
| 11 | Recursively Searching the B-Tree ||
| 12 | Scanning a Multi-Level B-Tree ||
| 13 | Updating Parent Node After a Split ||


### Database
- https://github.com/piotrku91?tab=repositories
- https://www.youtube.com/watch?v=gGxi500Q5uE
- https://cstack.github.io/db_tutorial/parts/part2.html
- https://scrapbox.io/ohbarye/Let%27s_Build_a_Simple_Database
- https://joseph-m-hansen.medium.com/c-on-jupyter-notebooks-97f2be68923a

### Sqlite
- https://play.google.com/store/books/details?id=9Z6IQQnX1JEC


### C++
- https://google.github.io/styleguide/cppguide.html
