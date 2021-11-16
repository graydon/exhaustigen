#pragma once

// This is a self-contained "exhaustive sequence generator" class based on
// https://matklad.github.io//2021/11/07/generate-all-the-things.html
//
// Since C++ does have a do-while loop, this class is designed to be called in
// a do-while loop. Using it at the head of a while loop will do nothing.
//
// The code in the blog has no license, and this may or may not count as a
// derivative work, but if not it's copyright Graydon Hoare <graydon@pobox.com>
// and published under the ASL2 license. Please enjoy and reuse it!

#include <cstdint>
#include <vector>
namespace exhaustigen {
class Gen {
  std::vector<std::pair<size_t, size_t>> v;
  size_t p{0};

public:
  bool is_done() {
    for (auto i = v.rbegin(); i != v.rend(); ++i) {
      if (i->first < i->second) {
        i->first += 1;
        v.erase(i.base(), v.end());
        p = 0;
        return false;
      }
    }
    return true;
  }

  size_t gen(size_t inclusive_upper_bound) {
    if (p == v.size()) {
      v.emplace_back(0, 0);
    }
    p += 1;
    auto &pair = v.at(p - 1);
    pair.second = inclusive_upper_bound;
    return pair.first;
  }

  /////////////////////////////////
  // Utility methods
  /////////////////////////////////

  bool flip() { return gen(1) == 1; }

  std::vector<size_t> gen_vec(size_t len_bound, size_t elt_bound) {
    std::vector<size_t> r(gen(len_bound), 0);
    for (auto &v : r) {
      v = gen(elt_bound);
    }
    return r;
  }

  template <typename T> std::vector<T> gen_comb(std::vector<T> const &in) {
    std::vector<T> r;
    size_t const sz = in.size();
    if (sz > 0) {
      size_t const n = gen(sz);
      for (size_t i = 0; i < n; ++i) {
        r.emplace_back(in.at(gen(sz - 1)));
      }
    }
    return r;
  }

  template <typename T> std::vector<T> gen_perm(std::vector<T> const &in) {
    std::vector<T> r;
    size_t const sz = in.size();
    if (sz > 0) {
      std::vector<size_t> idxs;
      for (size_t i = 0; i < sz; ++i) {
        idxs.emplace_back(i);
      }
      while (!idxs.empty()) {
        auto ix = idxs.begin() + gen(idxs.size() - 1);
        r.emplace_back(in.at(*ix));
        idxs.erase(ix);
      }
    }
    return r;
  }

  template <typename T> std::vector<T> gen_subset(std::vector<T> const &in) {
    std::vector<T> r;
    for (auto const &i : in) {
      if (flip()) {
        r.emplace_back(i);
      }
    }
    return r;
  }
};
} // namespace exhaustigen