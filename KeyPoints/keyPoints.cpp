#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/ADT/SmallVector.h"

#include "llvm/Analysis/LoopInfo.h"
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>


using namespace llvm;
using namespace std;

namespace {
    struct KeyPointsPass : PassInfoMixin<KeyPointsPass> {
        // The Count for the branch
        int branchCount = 1;
        // The map to store the information
        // The key is a vector including: File name, current branch, and the true branch or the false branch
        // The value is the branch number
        map<vector<string>, string> myMap;
        PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
            // Get the context for this function for IR builder later
            LLVMContext &Ctx = F.getContext();
            IRBuilder<> Builder(Ctx);
            Module *M = F.getParent();
            // Ensure 'printf' is present in the module
            FunctionCallee printfFunc = M->getOrInsertFunction(
                    "printf", FunctionType::get(Builder.getInt32Ty(), {Builder.getInt8PtrTy()}, true)
            );
            // if 'printf' function cannot be defined, this pass would fail, so it is better to close it
            if (!printfFunc) {
                errs() << "Failed to insert 'printf' function in module\n";
                return PreservedAnalyses::all();
            }
            // Get the entry block of this function
            BasicBlock &entry = F.getEntryBlock();
            // And first safe insertion point so the printf would not cause any problem after the insertion
            auto insertionPoint = entry.getFirstInsertionPt();
            Builder.SetInsertPoint(&entry, insertionPoint);
            // Convert the value of a function pointer to string
            ostringstream addressStream;
            addressStream << &F;
            string funcAddressStr = addressStream.str();
            // The output formula for the function name
            auto formatStr = Builder.CreateGlobalStringPtr("%s\n");
            string funcNameStr = "*func_" + funcAddressStr;
            // Create the function Name pointer as a global string pointer so bc file can print out the function name
            auto funcNamePtr = Builder.CreateGlobalStringPtr(funcNameStr);
            // Create a call to insert the print function
            Builder.CreateCall(printfFunc, { formatStr, funcNamePtr });
            // These two are helper method
            // Print out the function pointer value and function name while compiling the original file
            errs() << "*func_" << &F << '\n';
            errs() << F.getName() << '\n';
            // Go through all the instructions in all the blocks
            for (BasicBlock &BB : F) {
                for(Instruction &I: BB) {
                    // if one instruction is a branch instruction
                    if (auto *branchInst = dyn_cast<BranchInst>(&I)) {
                        // create vector for true and false string as a key for the map
                        vector<string> currentTrueString;
                        vector<string> currentFalseString;
                        // when it is conditional
                        if (branchInst->isConditional()) {
                            // call the printSourceLine and printSourceFile to get the current line and file
                            int branchLine = printSourceLine(I);
                            StringRef branchFile = printSourceFile(I);
                            Instruction *trueBranchInst = branchInst->getSuccessor(0)->getFirstNonPHIOrDbg(); // The "true" branch
                            Instruction *falseBranchInst = branchInst->getSuccessor(1)->getFirstNonPHIOrDbg(); // The "false" branch
                            // get the true line number and false line number
                            int trueLine = printSourceLine(*trueBranchInst);
                            int falseLine = printSourceLine(*falseBranchInst);

                            // start to build map
                            currentTrueString.push_back(branchFile.str());
                            currentFalseString.push_back(branchFile.str());

                            currentTrueString.push_back(to_string(branchLine));
                            currentFalseString.push_back(to_string(branchLine));

                            currentTrueString.push_back(to_string(trueLine));
                            currentFalseString.push_back(to_string(falseLine));
                            // when the current vector is not found in the map
                            // add the vector and current branch count to the map
                            if (myMap.find(currentTrueString) == myMap.end()) {
                                string currentBranch = "br_" + to_string(branchCount);
                                branchCount = branchCount + 1;
                                myMap[currentTrueString] = currentBranch;
                            }
                            if (myMap.find(currentFalseString) == myMap.end()) {
                                string currentBranch = "br_" + to_string(branchCount);
                                branchCount = branchCount + 1;
                                myMap[currentFalseString] = currentBranch;}
                            // map building finished
                            errs()  << myMap[currentTrueString] << '\n';
                            errs()  << myMap[currentFalseString] << '\n';
                            // this part is to find add 'printf' instruction
                            for (int succ = 0; succ < branchInst->getNumSuccessors(); ++succ) {
                                BasicBlock *succBB = branchInst->getSuccessor(succ);
                                if (!succBB) continue;  // Skip if successor is null
                                // Set the insertion point for the IRBuilder to the first instruction
                                // of the successor basic block
                                Builder.SetInsertPoint(&*succBB->getFirstInsertionPt());
                                // Determine which branch (true or false) we are dealing with
                                // and get the corresponding branch information string
                                vector<string> currentBranchString = succ == 0 ? currentTrueString : currentFalseString;
                                // Check if the current branch information string exists in the map
                                if (myMap.find(currentBranchString) != myMap.end()) {
                                    // retrieve the current branch
                                    string branchId = myMap[currentBranchString];
                                    // Create a global string pointer for the branch ID with a newline character
                                    auto branchIdStr = Builder.CreateGlobalStringPtr(branchId + "\n");
                                    // insert the 'printf' function
                                    Builder.CreateCall(printfFunc, { formatStr, branchIdStr });
                                }
                            }

                        }
                    }
                }
            }

            return PreservedAnalyses::none();
        }
        // called when the pass is finished running
        ~KeyPointsPass() {
            // create an output file
            std::ofstream outputFile("dictionary.txt");
            // when the file is not able to open, print out the message
            if (!outputFile.is_open()) {
                errs() << "Failed to open dictionary.txt for writing\n";
                return;
            }
            // print out all the entries in the map
            for (const auto &entry : myMap) {
                outputFile << entry.second << ": " << entry.first[0] << ", " << entry.first[1] << ", " << entry.first[2] << '\n';
            }
            // close the file when the printing is done
            outputFile.close();
        }
        /**
         * The method to get the line number
         * @param I current instruction
         * @return the line number
         */
        int printSourceLine(Instruction &I) {
            const llvm::DebugLoc &debugInfo = I.getDebugLoc();
            int line = debugInfo->getLine();
            return line;
        }
        /**
         * The method to get the file name
         * @param I I current instruction
         * @return the file name
         */
        StringRef printSourceFile(Instruction &I) {
            const llvm::DebugLoc &debugInfo = I.getDebugLoc();
            StringRef fileName = debugInfo->getFilename();
            return fileName;
        }
    };


}  // end of anonymous namespace

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return {
            LLVM_PLUGIN_API_VERSION, "KeyPoints", LLVM_VERSION_STRING,
            [](PassBuilder &PB) {
                PB.registerVectorizerStartEPCallback(
                        [](llvm::FunctionPassManager &PM, OptimizationLevel Level) {
                            PM.addPass(KeyPointsPass());
                        });
                PB.registerPipelineParsingCallback(
                        [](StringRef Name, FunctionPassManager &FPM,
                           ArrayRef<PassBuilder::PipelineElement>) {
                            if(Name == "keyPoints"){
                                FPM.addPass(KeyPointsPass());
                                return true;
                            }
                            return false;
                        }
                );
            }
    };
}
