#pragma once

#include "llvm/IR/PassManager.h"

namespace llvm {

enum class CFGPruningMode { Off, Profile, Prune };

class CFGTrackPass : public PassInfoMixin<CFGTrackPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // namespace llvm
