#include "exhaustigen.hpp"
#include <cmath>
#include <iostream>

void print_vec(std::vector<size_t> const &v) {
  for (auto const &e : v) {
    std::cout << ' ' << e;
  }
  std::cout << std::endl;
}

int main() {
  std::vector<size_t> in{33, 55, 77};

  {
    std::cout << "sequences:" << std::endl;
    exhaustigen::Gen g;
    do {
      std::vector<size_t> v = g.gen_vec(3, 3);
      print_vec(v);
    } while (!g.is_done());
  }

  {
    std::cout << "combinations:" << std::endl;
    exhaustigen::Gen g;
    do {
      std::vector<size_t> v = g.gen_comb(in);
      print_vec(v);
    } while (!g.is_done());
  }

  {
    std::cout << "permutations:" << std::endl;
    exhaustigen::Gen g;
    do {
      std::vector<size_t> v = g.gen_perm(in);
      print_vec(v);
    } while (!g.is_done());
  }

  {
    std::cout << "subsets:" << std::endl;
    exhaustigen::Gen g;
    do {
      std::vector<size_t> v = g.gen_subset(in);
      print_vec(v);
    } while (!g.is_done());
  }

  return 0;
}