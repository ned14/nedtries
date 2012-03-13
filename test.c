#include <stdio.h>
#include <string.h>
#include "nedtrie.h"

/* Include the Mersenne twister */
#if !defined(__cplusplus_cli) && (defined(_M_X64) || defined(__x86_64__) || (defined(_M_IX86) && _M_IX86_FP>=2) || (defined(__i386__) && defined(__SSE2__)))
#define HAVE_SSE2 1
#endif
#define MEXP 19937
#include "SFMT.c"


typedef struct foo_s foo_t;
struct foo_s {
  NEDTRIE_ENTRY(foo_s) link;
  size_t key;
};
typedef struct foo_tree_s foo_tree_t;
NEDTRIE_HEAD(foo_tree_s, foo_s);
static foo_tree_t footree;

size_t fookeyfunct(const foo_t *r)
{
  return r->key;
}

NEDTRIE_GENERATE(static, foo_tree_s, foo_s, link, fookeyfunct, NEDTRIE_NOBBLEZEROS(foo_tree_s))

#ifdef __cplusplus
struct keyfunct : public std::unary_function<int, size_t>
{
    size_t operator()(const int &) const
    {
    return 5;
    }
};
#endif

int main(void)
{
  foo_t a, b, c, *r;

  printf("General workout of the C API ...\n");
  NEDTRIE_INIT(&footree);
  a.key=2;
  NEDTRIE_INSERT(foo_tree_s, &footree, &a);
  b.key=6;
  NEDTRIE_INSERT(foo_tree_s, &footree, &b);
  r=NEDTRIE_FIND(foo_tree_s, &footree, &b);
  assert(r==&b);
  c.key=5;
  r=NEDTRIE_NFIND(foo_tree_s, &footree, &c);
  assert(r==&b); /* NFIND finds next largest. Invert the key function (i.e. 1-key) to find next smallest. */
  NEDTRIE_REMOVE(foo_tree_s, &footree, &a);
  NEDTRIE_FOREACH(r, foo_tree_s, &footree)
  {
    printf("%p, %u\n", (void *) r, (unsigned) r->key);
  }
  assert(!NEDTRIE_PREV(foo_tree_s, &footree, &b));
  assert(!NEDTRIE_NEXT(foo_tree_s, &footree, &b));

#ifdef __cplusplus
  printf("General workout of the C++ API ...\n");
  assert(keyfunct()(78)==5);
  using namespace nedtries;
  trie_map<int, int, keyfunct> map;
  trie_multimap<int, int, keyfunct> multimap;
  map.insert(78);
  multimap.insert(78);
  map.insert(79); // Replaces 78 with 79
  multimap.insert(79); // Pushes the existing 78 back so 79 is now in front
  assert(map.size()==1);
  assert(multimap.size()==2);
  assert(79==*map.find(5));
  trie_multimap<int, int, keyfunct>::const_iterator it=multimap.find(5);
  assert(79==*it);
  ++it;
  assert(78==*it);
#endif

  /* From https://github.com/ned14/nedtries/issues/5 */
  printf("Testing bug from https://github.com/ned14/nedtries/issues/5 (Nfind does not work in all cases) ...\n");
  NEDTRIE_INIT(&footree);
  a.key=0x10; /* 16 = 10000 */
  NEDTRIE_INSERT(foo_tree_s, &footree, &a);
  b.key=0x18; /* 24 = 11000 */
  NEDTRIE_INSERT(foo_tree_s, &footree, &b);
  c.key=0x11; /* 17 = 10001 */
  r=NEDTRIE_NFIND(foo_tree_s, &footree, &c);
  assert(r==&b); /* This was failing under the bug */

  NEDTRIE_INIT(&footree);
  a.key=52; /* 110100 */
  NEDTRIE_INSERT(foo_tree_s, &footree, &a);
  b.key=40; /* 101000 */
  NEDTRIE_INSERT(foo_tree_s, &footree, &b);
  c.key=25; /* 011001 */
  r=NEDTRIE_NFIND(foo_tree_s, &footree, &c);
  assert(r==&b); /* This was failing under the bug */

  /* To make sure the above never happens again */
  init_gen_rand(1234);
  {
    int n;
    for(n=0; n<(1<<22); n++)
    {
      foo_t *max, *min;
      NEDTRIE_INIT(&footree);
      a.key=gen_rand32() & 0xff;
      NEDTRIE_INSERT(foo_tree_s, &footree, &a);
      do { b.key=gen_rand32() & 0xff; } while(b.key==a.key);
      NEDTRIE_INSERT(foo_tree_s, &footree, &b);
      c.key=gen_rand32() & 0xff;
      r=NEDTRIE_NFIND(foo_tree_s, &footree, &c);
      max=(a.key > b.key) ? &a : &b;
      min=(a.key < b.key) ? &a : &b;
      if(c.key==a.key)
      {
        assert(r==&a);
      }
      else if(c.key==b.key)
      {
        assert(r==&b);
      }
      else if(c.key>a.key && c.key>b.key)
      {
        assert(r==0);
      }
      else if(c.key>min->key && c.key<max->key)
      {
        assert(r==max);
      }
      else
      {
        assert(r==min);
      }
    }
  }
  return 0;
}
