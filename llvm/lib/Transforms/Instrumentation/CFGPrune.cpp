#include "llvm/Transforms/Instrumentation/CFGPrune.h"

#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;

namespace {

cl::opt<std::string>
    OutputDir("cfg-pruning-profile-dir",
              cl::desc("Specify the directory where profiles will be stored "
                       "(default: current directory)"),
              cl::value_desc("direstory path"), cl::init(""));

Function *addTrackingFunctionDecl(Module &M) {
  auto *FT =
      FunctionType::get(Type::getVoidTy(M.getContext()),
                        {
                            Type::getInt8Ty(M.getContext())->getPointerTo(),
                            Type::getInt8Ty(M.getContext())->getPointerTo(),
                            Type::getInt8Ty(M.getContext())->getPointerTo(),
                            Type::getInt64Ty(M.getContext()),
                        },
                        false);
  return Function::Create(FT, Function::ExternalLinkage, "trackCFG", M);
}

} // namespace

PreservedAnalyses CFGTrackPass::run(Module &M, ModuleAnalysisManager &MAM) {
  if (OutputDir.empty()) {
    llvm_unreachable("CFGTrack: Output directory is not specified");
  }
  auto *TrackingFunc = addTrackingFunctionDecl(M);
  auto *ModuleName =
      llvm::ConstantDataArray::getString(M.getContext(), M.getName(), true);
  auto *GVModule = new GlobalVariable(M, ModuleName->getType(), true,
                                      GlobalValue::ExternalLinkage, ModuleName,
                                      "module_name");
  auto *OutputDirectory =
      llvm::ConstantDataArray::getString(M.getContext(), OutputDir, true);
  auto *GVOutputDir = new GlobalVariable(M, OutputDirectory->getType(), true,
                                         GlobalValue::PrivateLinkage,
                                         OutputDirectory, "output_dir");
  IRBuilder<> Builder(M.getContext());
  for (auto &F : M) {
    uint64_t ID = 0;
    auto *FuncName =
        llvm::ConstantDataArray::getString(M.getContext(), F.getName(), true);
    auto *GVFuncName = new GlobalVariable(M, FuncName->getType(), true,
                                          GlobalValue::PrivateLinkage, FuncName,
                                          "func_name_" + F.getName());
    for (auto &BB : F) {
      Builder.SetInsertPoint(&BB, BB.getFirstInsertionPt());
      Builder.CreateCall(TrackingFunc, {
                                           GVOutputDir,
                                           GVModule,
                                           GVFuncName,
                                           Builder.getInt64(ID++),
                                       });
    }
  }
  return PreservedAnalyses::all();
}
