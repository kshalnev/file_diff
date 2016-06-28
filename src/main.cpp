#include <algorithm>
#include <iostream>
#include <iterator>
#include <fstream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

using namespace std;

/*
string LongestCommonSubstring(string const & a, string const & b)
{
  size_t const w = a.length(); // x
  size_t const h = b.length(); // y

  if (h == 0 || w == 0)
    return string();

  vector<vector<size_t>> dp(h, vector<size_t>(w, 0));

  size_t len = 0;
  size_t pos = 0; // pos on x

  for (size_t y = 0; y < h; ++y)
  {
    for (size_t x = 0; x < w; ++x)
    {
      if (a[x] == b[y])
      {
        if (x == 0 || y == 0)
            dp[y][x] = 1;
        else
            dp[y][x] = dp[y - 1][x - 1] + 1;

        if (len < dp[y][x])
        {
          len = dp[y][x];
          pos = x - len + 1;
        }
      }
      else
      {
        dp[y][x] = 0;
      }
    }
  }

  return string(a.begin() + pos, a.begin() + pos + len);
}
*/

size_t LongestCommonSubstring(vector<char> const & a, vector<char> const & b)
{
  if (a.empty() || b.empty())
  {
    // vector<char>();
    return 0;
  }

  vector<char> const & s = (a.size() >= b.size()) ? b : a; // x
  vector<char> const & l = (a.size() >= b.size()) ? a : b; // y

  size_t const w = s.size(); // x
  size_t const h = l.size(); // y

  vector<size_t> curr(w, 0);
  vector<size_t> prev(w, 0);

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

  // return vector<char>(s.begin() + pos, s.begin() + pos + len);
  return len;
}

/*
size_t diff(string const & a, string const & b) // a -> b
{
    size_t constexpr kCopy = 0;
    size_t constexpr kReplace = 1;
    size_t constexpr kDelete = 1;
    size_t constexpr kInsert = 1;

    const size_t w = a.size(); // x
    const size_t h = b.size(); // y

    vector<vector<size_t>> dp(h+1, vector<size_t>(w+1, 0));

    for (size_t x = 0; x <= w; ++x) // a
        dp[0][x] = x * kDelete;

    for (size_t y = 0; y <= h; ++y) // b
        dp[y][0] = y * kInsert;

    for (size_t y = 1; y <= h; ++y) // b
    {
        for (size_t x = 1; x <= w; ++x) // a
        {
            bool equal = (a[x-1] == b[y-1]);
            size_t valCopyOrReplace = dp[y-1][x-1] + (equal ? kCopy : kReplace);
            size_t valInsert = dp[y-1][x] + kInsert;
            size_t valDelete = dp[y][x-1] + kDelete;
            size_t val = min(valCopyOrReplace, min(valInsert, valDelete));
            dp[y][x] = val;
        }
    }

    return dp[h][w];
}
*/

struct DiffTraits
{
    enum {
        kCopy = 0,
        kReplace = 1,
        kDelete = 0,
        kInsert = 1
    };
};

template <typename Traits>
struct Diff
{
public:
    // Returns an editoral distance to transform string A to string B
    static size_t diff(string const & a, string const & b) // a -> b
    {
        return diff(vector<char>(a.begin(), a.end()),
                    vector<char>(b.begin(), b.end()));
    }
    // Returns an editoral distance to transform vector A to vector B
    static size_t diff(vector<char> const & a, vector<char> const & b) // a -> b
    {
        const size_t w = a.size(); // x
        const size_t h = b.size(); // y

        const size_t vCopy = Traits::kCopy;
        const size_t vReplace = Traits::kReplace;
        const size_t vDelete = Traits::kDelete;
        const size_t vInsert = Traits::kInsert;

        vector<size_t> dpprev(w+1, vDelete);
        dpprev[0] = 0;

        vector<size_t> dp(w+1);

        for (size_t y = 1; y <= h; ++y) // b
        {
            dp[0] = y * vInsert;

            for (size_t x = 1; x <= w; ++x) // a
            {
                const bool equal = (a[x-1] == b[y-1]);
                const size_t val1 = dpprev[x-1] + (equal ? vCopy : vReplace);
                const size_t val2 = dpprev[x] + vInsert;
                const size_t val3 = dp[x-1] + vDelete;
                dp[x] = min(val1, min(val2, val3));
            }

            dpprev.swap(dp);
        }

        return dpprev[w];
    }
};

class Processor
{
private:
  struct Result
  {
    size_t lcs = 0;
    size_t diff = 0;
    size_t difflen = 0;
  };
  struct Request
  {
    size_t id = 0;
    vector<char> const * file1 = nullptr;
    vector<char> const * file2 = nullptr;
    size_t start = 0;
    size_t len = 0;
    Result * result = nullptr;
  };
  static void log(const Request & req)
  {
    cout << "Block " << req.id
         << ", diff: " << req.result->diff
         << ", lcs: " << req.result->lcs
         << ", difflen: " << req.result->difflen
         << endl;
  }
  static void process(
               vector<char> const & file1, vector<char> const & file2,
               size_t start, size_t len,
               Result& res)
  {
      const vector<char> block1(file1.begin() + start, file1.begin() + start + len);
      const vector<char> block2(file2.begin() + start, file2.begin() + start + len);

      size_t diff = 0;
      for (size_t i = 0; i < len; ++i) {
          if (block1[i] != block2[i])
              ++diff;
      }

      size_t lcs = LongestCommonSubstring(block1, block2);
      size_t difflen = Diff<DiffTraits>::diff(block1, block2);

      res.lcs = lcs;
      res.diff = diff;
      res.difflen = difflen;
  }
  static void process(Request & req)
  {
    process(*req.file1, *req.file2, req.start, req.len, *req.result);
  }
  static void process(queue<Request> & requests, mutex & guard)
  {
    while (true)
    {
      Request req;
      {
        lock_guard<mutex> lg(guard);
        if (requests.empty()) break;
        req = requests.front();
        requests.pop();
      }
      process(req);
      {
        lock_guard<mutex> lg(guard);
        log(req);
      }
    }
  }
public:
  static void process(vector<char> const & file1,
                      vector<char> const & file2,
                      size_t blockSize = 65536)
  {
    size_t const size = min(file1.size(), file2.size());

    size_t const blocks = size / blockSize;

    queue<Request> requests;
    vector<Result> results(blocks);

    for (size_t i = 0, start = 0; i < blocks; ++i, start += blockSize)
    {
      Request req;
      req.id = i;
      req.file1 = &file1;
      req.file2 = &file2;
      req.start = start;
      req.len = blockSize;
      req.result = &results[i];
      requests.push(req);
    }

    std::mutex guard;
    vector<thread> t(thread::hardware_concurrency());
    for (size_t i = 1; i < t.size(); ++i) {
      t[i] = thread([&guard, &requests] { process(requests, guard); });
    }
    process(requests, guard);
    for (size_t i = 1; i < t.size(); ++i) t[i].join();

    Result total;
    for (size_t i = 0; i < results.size(); ++i) {
      total.lcs += results[i].lcs;
      total.diff += results[i].diff;
      total.difflen += results[i].difflen;
    }

    cout << "Total"
         << " diff: " << total.diff
         << ", lcs: " << total.lcs
         << ", difflen: " << total.difflen
         << endl;
  }
};

/*
void CheckBlocking(vector<char> const & file1, vector<char> const & file2)
{
    size_t constexpr kBlockSize = 65536;

    size_t const size = min(file1.size(), file2.size());

    size_t const blocks = size / kBlockSize;

    cout << "Block size: " << kBlockSize << ", " << blocks << " blocks" << endl;

    size_t totallcs = 0;
    size_t totaldifflen = 0;

    vector<char> block1;
    vector<char> block2;
    for (size_t i = 0, k = 0; i < size; i += kBlockSize, ++k)
    {
        size_t diff = 0;
        for (size_t j = i, jend = i + kBlockSize; j < jend; ++j) {
            if (file1[j] != file2[j])
                ++diff;
        }

        block1.clear();
        block2.clear();
        block1.insert(block1.end(), file1.begin() + i, file1.begin() + i + kBlockSize);
        block2.insert(block2.end(), file2.begin() + i, file2.begin() + i + kBlockSize);
        size_t blocklcs = 0; // LongestCommonSubstring(block1, block2);
        size_t difflen = Diff<DiffTraits>::diff(block1, block2);

        cout << "Block: " << k
             << ", diffs: " << diff
             // << ", lcs: " << blocklcs
             << ", difflen: " << difflen
             << endl;

        totallcs += blocklcs;
        totaldifflen += difflen;
    }

    cout << "Total"
         << " difflen: " << totaldifflen
         // << ", lcs: " << totallcs
         << endl;
}
*/

vector<char> readFile(string const & path)
{
    ifstream file(path);
    auto i = istreambuf_iterator<char>(file);
    auto iend = istreambuf_iterator<char>();
    return vector<char>(i, iend);
}

int main(int /* argc */, const char * /* argv */ [])
{
  const char * const path1 = "/Users/kshalnev/projects/mwm_diff/data/160602/Russia_Moscow.mwm";
  const char * const path2 = "/Users/kshalnev/projects/mwm_diff/data/160621/Russia_Moscow.mwm";

  cout << "Reading file " << path1 << endl;
  auto file1 = readFile(path1);
  cout << "File size is " << file1.size() << endl;

  cout << "Reading file " << path2 << endl;
  auto file2 = readFile(path2);
  cout << "File size is " << file2.size() << endl;

  // CheckBlocking(file1, file2);
  Processor::process(file1, file2);

  return 0;
}
