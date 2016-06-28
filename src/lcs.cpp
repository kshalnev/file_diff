#include "lcs.h"

size_t LongestCommonSubstring(std::vector<char> const & a, 
                              std::vector<char> const & b)
{
  if (a.empty() || b.empty())
    return 0;

  std::vector<char> const & s = (a.size() >= b.size()) ? b : a; // x
  std::vector<char> const & l = (a.size() >= b.size()) ? a : b; // y

  size_t const w = s.size(); // x
  size_t const h = l.size(); // y

  std::vector<size_t> curr(w, 0);
  std::vector<size_t> prev(w, 0);

  size_t len = 0;
  size_t pos = 0; // pos on x

  for (size_t y = 0; y < h; ++y)
  {
    const char ly = l[y];
    for (size_t x = 0; x < w; ++x)
    {
      size_t & cx = curr[x];
      if (ly == s[x])
      {
        if (x == 0 || y == 0)
            cx = 1;
        else
            cx = prev[x - 1] + 1;

        if (len < cx)
        {
          len = cx;
          pos = x - len + 1;
        }
      }
      else
      {
        cx = 0;
      }
    }
    prev.swap(curr);
  }

  // the substring is vector<char>(s.begin() + pos, s.begin() + pos + len);
  return len;
}
