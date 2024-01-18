#include <fstream>
#include <ios>

extern "C" {

void trackCFG(const char *OutputDir, const char *ModuleName,
              const char *FunctionName, unsigned long Id) {
  std::ofstream(std::string{OutputDir} + "/" + std::string{ModuleName},
                std::ios::app)
      << FunctionName << " " << Id << "\n";
}
}
