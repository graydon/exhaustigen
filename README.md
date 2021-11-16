## Exhaustigen

This is a tiny (but delightful!) utility library for exhaustive testing.

It is based (directly) on the idea and code in the following blog post:

https://matklad.github.io//2021/11/07/generate-all-the-things.html

TL;DR: the idea is to write a test that is _similar_ to the sort of test one
writes against a PRNG -- an imperative test that just asks some "generator" to
create scalars, booleans, data vectors, random shuffles, etc. -- but to use a
special generator that has some interesting features:

  - It has a concept of being "done", so you can put it in a do-while loop
  - Every call to it requires an inclusive upper bound, which should be small
  - It _tracks its progress_ through the sequence of bounds
  - It _lazily extends_ the sequence of bounds as it's asked for more data

By threading such a generator through such an imperative test, and putting the
body of that test in a do-while loop, you can write straightforward code with
elaborate value-dependent nesting structure -- eg. generate value K in 0..N and
then value J in 0..K and so forth -- and it will automatically record and
enumerate the space of sequences meaningful to the code, re-running until it has
exhausted all possible paths/values.

### Example

```
#include "exhaustigen.h"
void test()
{
    exhaustigen::Gen g;
    do {
        size_t num_terriers = g.gen(5);
        if (num_terriers > 0)
        {
            auto all_terriers = create_terriers(num_terriers);
            auto terriers = g.gen_subset(all_terriers);
            while (!terriers.empty())
            {
                auto best = g.gen(terriers.size() - 1);
                auto worst = g.gen(best);
                auto best_terrier = terriers[best];
                auto worst_terrier = terriers[worst];
                if (worst_terrier.chase(best_terrier))
                {
                    terriers.erase(best_terrier);
                }
                else
                {
                    terriers.erase(worst_terrier);
                }
            }
        }
    } while (!g.is_done());
}
```

### Notes on use

The blog-post design uses a while loop (because Rust has no do-while) but I have
removed the bit of support for that since C++ has do-while. You actually have to
put it in a do-while loop; if you put it in a while loop header it will not run
the loop at all since it's empty and hasn't collected a run yet.

Obviously if this is to work at all you need your tests to be _deterministic_ in
terms of how each gen() call relates to the next. But that relationship can vary
on a value-by-value basis.

You do ultimately need some constant bounds to feed to outermost gen() calls,
and there is no magic here to avoid combinatorial explosion. Keep your bounds
small if you want them to finish running in your lifetime! Try numbers like 3 or
5 to start.

I threw in a few utility methods on the `Gen` class for generating combinations,
permutations, subsets and random vectors. Of course these are less-direct copies
from the blog post because C++ doesn't have Rust's delightful
iterator-combinator library. There are examples of use in `test.cpp`

### Analogy

One way to view the approach is that the generator is effectively storing a
stack of dynamic, implicit nested loop-counters for what _would_ be explicit
static nested loops (or dynamic recursive nested loops) in a manually-structured
exhaustive test, with each `gen()` call opening a new inner loop with a
fixed-at-opening loop count, and each `is_done()` call stepping forward whatever
the innermost unfinished loop is in the stack.

What makes it special is that in normal imperative code, each loop runs to
completion, they don't dynamically compose. In other words if loop A runs X
iterations and loop B runs Y iterations, you can't easily write a loop C
containing one step from A followed by one cycle through B, where C then runs
X*Y times over the nested-loop (product) space. You have to buffer the results
of A and B in memory, or manually nest the loop-bodies, or .. somehow
externalize their loop counters. Which is what this library does, in a very
general fashion. So you can stick calls to `gen_perm()` or `gen_subset()` (each
defined in terms of `gen()`) in the middle of your test, and the set of nested
loops that will be revisited on the next outermost `is_done()` dynamically
expands to include the space of permutations (or whatever) implied by all the
unfinished calls to `gen()`. It all Just Works.

(You could do something gnarly with coroutines or higher order functions as
well, but this approach is far simpler. This README is larger than the entire
implementation, including utility helpers.)

One might say this code is "obvious" or "trivial" because it's so small and
simple, but I've been programming professionally for 25 years and haven't seen
it before, and it seems super useful, so .. kudos to Aleksey Kladov, yet again!

## License

The code in the blog post doesn't have a license and I'm not really sure if
my close transliteration of it here is a derivative work or an original one,
but either way I'm trained to put copyright and license notices on things I
write, so: this code is copyright 2021 Graydon Hoare <graydon@pobox.com>
and licensed under the Apache 2.0 license.