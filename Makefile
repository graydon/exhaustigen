test: test.cpp exhaustigen.hpp
	clang++ -O2 -o $@ $<

format:
	clang-format -i test.cpp exhaustigen.hpp