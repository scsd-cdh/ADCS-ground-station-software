#include "adcs/config.h"

#include <cstdlib>

namespace adcs {

std::filesystem::path dataDir() {
  const std::filesystem::path root(ADCS_REPO_ROOT);
  const char *env = std::getenv("ADCS_DATA_DIR");
  std::filesystem::path dir = (env && *env) ? std::filesystem::path(env) : std::filesystem::path("output");
  if (dir.is_relative()) dir = root / dir;
  std::filesystem::create_directories(dir);
  return dir;
}

} // namespace adcs
