#include <stdio.h>
#include <string.h>
#include "nedtrie.h"

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

int main(void)
{
  foo_t a, b, c, *r;
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
  return 0;
}
