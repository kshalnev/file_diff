#include "diff.h"

size_t Diff(std::vector<char> const & a, 
            std::vector<char> const & b,
            DiffTraits const & traits) // a -> b
{
  const size_t w = a.size(); // x
  const size_t h = b.size(); // y

  const size_t vCopy = traits.OpeationCopy;
  const size_t vReplace = traits.OpeationReplace;
  const size_t vDelete = traits.OpeationDelete;
  const size_t vInsert = traits.OpeationInsert;

  std::vector<size_t> dpprev(w+1, vDelete);
  dpprev[0] = 0;

  std::vector<size_t> dp(w+1);

  for (size_t y = 1; y <= h; ++y) // b
  {
      dp[0] = y * vInsert;

      for (size_t x = 1; x <= w; ++x) // a
      {
          const bool equal = (a[x-1] == b[y-1]);
          const size_t val1 = dpprev[x-1] + (equal ? vCopy : vReplace);
          const size_t val2 = dpprev[x] + vInsert;
          const size_t val3 = dp[x-1] + vDelete;
          dp[x] = std::min(val1, std::min(val2, val3));
      }

      dpprev.swap(dp);
  }

  return dpprev[w];
}
