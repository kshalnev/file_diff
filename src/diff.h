#pragma once

#include <vector>

struct DiffTraits
{
  size_t OpeationCopy = 0;
  size_t OpeationReplace = 1;
  size_t OpeationDelete = 0;
  size_t OpeationInsert = 1;
};

// Returns an editoral distance to transform vector A to vector B
size_t Diff(std::vector<char> const & a, 
            std::vector<char> const & b,
            DiffTraits const & traits = DiffTraits());
