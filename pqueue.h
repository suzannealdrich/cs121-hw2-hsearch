/* A module for priority queues .. christopher manning .. dec 1996 */

typedef float heapvalue;
typedef void *heapdata;

struct heapitem {
  heapvalue score;
  heapdata  datum;
};

typedef struct heapstruct {
  struct heapitem *base;
  unsigned maxsize;
  unsigned size;
  char reversed;
} heap, *heapptr;


#define ELEMENT(h, i) ((h) -> base + (i))


#ifdef PQUEUE_NO_MACROS
unsigned heap_length(heapptr h);
/* the number of elements in the heap */
int heap_empty(heapptr h);
/* whether the heap is empty */
int heap_full(heapptr h);
/* whether the heap is full (and cannot store more items) */
heapvalue heap_max(heapptr h);
/* the value of the maximum/minimum value on the heap */
void heap_clear(heapptr h);
/* makes the heap empty */
#else
#define heap_length(h) ((h) -> size)
#define heap_empty(h) ((h) -> size == 0)
#define heap_full(h) ((h) -> size == (h) -> maxsize)
#define heap_max(h) ((h) -> base -> score)
#define heap_clear(h) ((h) -> size = 0)
#endif

heapptr heap_new(unsigned maxsize, char reversed);
/* Create a new empty heap which can store a maximum of maxsize items.
   If reversed == 1, smallest score is best; else biggest score is best */

void heap_free(heapptr hptr);
/* This frees the heap data structures, but not the contents of items */

void heap_maximum(heapptr h, heapdata *element, heapvalue *score);
/* Returns the value and score of the maximum/minimum value on the heap
   without removing it. */

void heap_insert(heapptr h, heapdata element, heapvalue score);
/* Add the given element into the heap with score score */

void heap_pop(heapptr h, heapdata *element, heapvalue *score);
/* Pop and return the value and score of the maximum/minimum value on the
   heap */

void heap_pop_and_push(heapptr h, heapdata newelement, heapvalue newscore,
		       heapdata *oldelement, heapvalue *oldscore);

/* Replace maximum item in heap with new supplied item.
   (This combo operation is here just for performance reasons. */

void heap_incorporate(heapptr in, heapptr out);

/* Add all the contents of heap in into heap out */
