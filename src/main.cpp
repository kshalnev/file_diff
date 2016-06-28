#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#include "diff.h"
#include "lcs.h"
#include "processor.h"

void Process(std::vector<char> const & file1,
             std::vector<char> const & file2)
{
  size_t const blockSize = 65536;

  size_t const size = std::min(file1.size(), file2.size());

  size_t const blocks = size / blockSize;

  std::cout << blocks << " blocks, block size is " << blockSize << std::endl;

  size_t totallcs = 0;
  size_t totaldiff = 0;

  std::mutex guard;

  Processor p;
  for (size_t i = 0; i < blocks; ++i)
  {
    p.Add([&file1, &file2, i, blockSize, &totallcs, &totaldiff, &guard]
    {
        std::vector<char> block1(file1.begin() + i * blockSize,
                                 file1.begin() + i * blockSize + blockSize);
        std::vector<char> block2(file2.begin() + i * blockSize,
                                 file2.begin() + i * blockSize + blockSize);

        size_t lcs = LongestCommonSubstring(block1, block2);
        size_t diff = Diff(block1, block2);

        totallcs += lcs;
        totaldiff += diff;

        std::lock_guard<std::mutex> lg(guard);
        std::cout << "Block " << i << ": lcs: " << lcs << ", diff: " << diff
                  << " (" << static_cast<int>(100.0 * diff / blockSize) << "%)"
                  << std::endl;
    });
  }
  p.Process();

  std::cout << "Total lcs: " << totallcs << ", diff: " << totaldiff << std::endl;
}

std::vector<char> ReadFile(std::string const & path)
{
    std::ifstream file(path);
    auto i = std::istreambuf_iterator<char>(file);
    auto iend = std::istreambuf_iterator<char>();
    return std::vector<char>(i, iend);
}

int main(int /* argc */, const char * /* argv */ [])
{
  const char * const path1 = "../data/160602/Russia_Moscow.mwm";
  const char * const path2 = "../data/160621/Russia_Moscow.mwm";

  std::cout << "Reading file " << path1 << std::endl;

  auto file1 = ReadFile(path1);

  std::cout << "File size is " << file1.size() << std::endl;

  std::cout << "Reading file " << path2 << std::endl;

  auto file2 = ReadFile(path2);

  std::cout << "File size is " << file2.size() << std::endl;

  Process(file1, file2);

  return 0;
}
