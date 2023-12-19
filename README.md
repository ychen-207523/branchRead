# branchRead
BranchRead is an LLVM Compiler Pass designed to analyze C programs. Its primary function is to identify and catalog all branching points within the C source code. This is achieved by generating a comprehensive dictionary, which maps each branch to its corresponding line number. Additionally, during the execution of the C program, BranchRead actively monitors and outputs the line number of the current target branch being executed. This enhances the visibility of program flow and aids in debugging and analysis.

## Prerequisite

### Ninja
1. `sudo apt-get update -y`
2. `sudo apt-get install -y ninja-build`

### LLVM Clang Opt
For this project, I am using the latest version available, which is LLVM-18.
- You can install LLVM-18 through the following commends:
`wget https://apt.llvm.org/llvm.sh`
`chmod +x llvm.sh`
`sudo ./llvm.sh 18`
- Then export path as following: `export PATH="/usr/lib/llvm-18/bin/:$PATH"`
- You can check the version by: `llvm-config --version` ` clang --version` `opt --version`
- The output should be similar as shown below:

- __Please note that installing the same version of LLVM as your local LLVM source file is crucial to avoid errors.__


## Compile LLVM
1. `git clone https://github.com/llvm/llvm-project.git`
2. `cd llvm-project`
3. `cmake -S llvm -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_PROJECTS="clang;lld"`
4. `ninja -C build check-llvm`
5. `ninja -C build`

## Compile new passes
1. Copy and Paste the whole directory into `llvm-project/llvm/lib/Transforms`
2. In the __CMakeLists.txt__ file, add `add_subdirectory(KeyPoints)`
3. In the llvm-project folder, use `ninja -C build`
4. The new pass should be successfully added.


## Test the new passes
1. In the testfile folder
2. Compile the test files with the following commends: (Use -O1 because it can output most desirable outcome in this project)
    1. `clang -O1 -g -emit-llvm test1.c -c -o test1.bc`
    2. `clang -O1 -g -emit-llvm test2.c -c -o test2.bc`
3. Transform the bc file with the new pass:
    1.  `opt -load-pass-plugin ~/llvm-project/build/lib/KeyPoints.so -passes=keyPoints -o test1-2.bc test1.bc`
    2.  `opt -load-pass-plugin ~/llvm-project/build/lib/KeyPoints.so -passes=keyPoints -o test2-2.bc test2.bc`
4. run the transformed bc file
    1.  `lli test1-2.bc`
    2.  `lli test2-2.bc`
5. The dictionary.txt file that contains all the branches will also be generated when transforming the bc file with the new pass in the testfile folder.

