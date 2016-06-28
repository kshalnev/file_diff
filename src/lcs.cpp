#include "lcs.h"

size_t LongestCommonSubstring(std::vector<char> const & a, 
                              std::vector<char> const & b)
{
  if (a.empty() || b.empty())
    return 0;
  
  if (a.size() > b.size())
    return LongestCommonSubstring(b, a);

  size_t const w = a.size(); // x
  size_t const h = b.size(); // y

  std::vector<size_t> dp(w, 0);
  std::vector<size_t> dpprev(w, 0);

  size_t len = 0;
  size_t pos = 0; // pos on x

  for (size_t y = 0; y < h; ++y)
  {
    for (size_t x = 0; x < w; ++x)
    {
      if (a[x] == b[y])
      {
        dp[x] = (x == 0 || y == 0) ? 1 : (dpprev[x - 1] + 1);

        if (len < dp[x])
        {
          len = dp[x];
          pos = x - len + 1;
        }
      }
      else
      {
        dp[x] = 0;
      }
    }
    
    dpprev.swap(dp);
  }

  // the substring is vector<char>(a.begin() + pos, a.begin() + pos + len);
  return len;
}
