#pragma once

#include <string>
#include <vector>

#include "job.h"
#include "warning.h"

namespace algorithm
{

std::vector<warning> algorithm(std::vector<algorithm::job>& jobs, const std::vector<std::string>& targets);

} // namespace algorithm

