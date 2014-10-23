/* A module for priority queues .. christopher manning .. dec 1996 */
/* see pqueue.h for documentation of functions */

#include <stdlib.h>
#include <stdio.h>
#if defined(USE_VMALLOC) || defined(VMFL)
#include <vmalloc.h>
#endif
#include "pqueue.h"

#define PARENT(x) ((x-1)/2)
#define LEFT(x) (x*2+1)
#define RIGHT(x) (x*2+2)

/* PQueues work out better for arrays beginning with 1! Then can use
    define PARENT(x) (x >> 1)
    define LEFT(x) (x << 1)
    define RIGHT(x) ((x << 1) | 1) but that's not C arrays.... */


heapptr heap_new(unsigned maxsize, char reversed)

{
  struct heapitem *hiptr;
  heapptr h;

  if ((hiptr = (struct heapitem *) calloc(maxsize, sizeof(struct heapitem)))
      			== NULL)
    return NULL;
  if ((h = (heapptr) malloc(sizeof(heap))) == NULL)
    return NULL;

  h -> base = hiptr;
  h -> maxsize = maxsize;
  h -> size = 0;
  h -> reversed = reversed;

  return h;
}


void heap_free(heapptr hptr)

/* This frees the heap data structures, but not the contents of items */

{
  free(hptr -> base);
  free(hptr);
}


#ifdef PQUEUE_NO_MACROS

void heap_clear(heapptr h)

{
  h -> size = 0;
}


unsigned heap_length(heapptr h)

{
  return h -> size;
}


int heap_empty(heapptr h)

{
  return h -> size == 0;
}


int heap_full(heapptr h)

{
  return h -> size == h -> maxsize;
}


heapvalue heap_max(heapptr h)

{
  if (h -> size < 1)
    fprintf(stderr, "Heap underflow\n");

  return ELEMENT(h, 0) -> score;
}

#endif /* PQUEUE_NO_MACROS */


/* Resurrect the crucial heap property.  On the assumption that
   LEFT(i) and RIGHT(i) satisfy the heap property, make sure that
   the heap at i satisfies this property by possibly percolating
   the element at i downwards.  I've replaced the obvious recursive
   formulation with an iterative one in an attempt to gain speed, as
   this is the bottleneck of a heap implementation.			*/


void heapify(heapptr h, unsigned i)

{
  unsigned l, r, largest, current;
  heapvalue largestscore;
  heapdata tempdata;

  largest = i;
  largestscore = ELEMENT(h, largest) -> score;

  do
    {
      current = largest;

      l = LEFT(current);
      if (l < heap_length(h))
	if (h -> reversed)
	  {
	    if (ELEMENT(h, l) -> score < largestscore)
	      {
		largest = l;
		largestscore = ELEMENT(h, l) -> score;
	      }
	  }
	else
	  {
	    if (ELEMENT(h, l) -> score > largestscore)
	      {
		largest = l;
		largestscore = ELEMENT(h, l) -> score;
	      }
	  }

      r = RIGHT(current);
      if (r < heap_length(h))
	if (h -> reversed)
	  {
	    if (ELEMENT(h, r) -> score < largestscore)
	      largest = r;
	  }
	else
	  {
	    if (ELEMENT(h, r) -> score > largestscore)
	      largest = r;
	  }

      if (largest != current)
	{
	  /* Swap largest and current. */
	  largestscore = ELEMENT(h, current) -> score;
	  tempdata = ELEMENT(h, current) -> datum;
	  ELEMENT(h, current) -> score = ELEMENT(h, largest) -> score;
	  ELEMENT(h, current) -> datum = ELEMENT(h, largest) -> datum;
	  ELEMENT(h, largest) -> score = largestscore;
	  ELEMENT(h, largest) -> datum = tempdata;
	}
    }
  while (largest != current);
}


/* the "obvious recursive formulation"

void heapify(heapptr h, unsigned i)

{
  unsigned l, r, largest;
  struct heapitem temp;

  l = LEFT(i);
  r = RIGHT(i);

  largest = i;
  if (l < heap_length(h))
    if (h -> reversed)
      {
	if (ELEMENT(h, l) -> score < ELEMENT(h, i) -> score)
	  largest = l;
      }
    else
      {
	if (ELEMENT(h, l) -> score > ELEMENT(h, i) -> score)
	  largest = l;
      }
  if (r < heap_length(h))
    if (h -> reversed)
      {
	if (ELEMENT(h, r) -> score < ELEMENT(h, largest) -> score)
	  largest = r;
      }
    else
      {
	if (ELEMENT(h, r) -> score > ELEMENT(h, largest) -> score)
	  largest = r;
      }
  
  if (largest != i)
    {
      temp = *ELEMENT(h, largest);
      *ELEMENT(h, largest) = *ELEMENT(h, i);
      *ELEMENT(h, i) = temp;
      heapify(h, largest);
    }
}

*/


void heap_incorporate(heapptr in, heapptr out)

/* Add the contents of heap in into heap out */

{
  int i;

  for (i = 0; i < heap_length(in); i++)
    heap_insert(out, ELEMENT(in, i) -> datum, ELEMENT(in, i) -> score);
}


void heap_maximum(heapptr h, heapdata *element, heapvalue *score)

{
  if (h -> size < 1)
    fprintf(stderr, "Heap underflow\n");

  *element = ELEMENT(h, 0) -> datum;
  *score = ELEMENT(h, 0) -> score;
}


void heap_insert(heapptr h, heapdata element, heapvalue score)

{
  unsigned i;

  if (h -> size < h -> maxsize)
    {
      i = h -> size;
      h -> size++;
      if (h -> reversed)
	while (i > 0 && ELEMENT(h, PARENT(i)) -> score > score)
	  {
	    *ELEMENT(h, i) = *ELEMENT(h, PARENT(i));
	    i = PARENT(i);
	  }
      else
	while (i > 0 && ELEMENT(h, PARENT(i)) -> score < score)
	  {
	    *ELEMENT(h, i) = *ELEMENT(h, PARENT(i));
	    i = PARENT(i);
	  }
      ELEMENT(h, i) -> datum = element;
      ELEMENT(h, i) -> score = score;
    }
  else
    fprintf(stderr, "Heap overflow\n");
}


void heap_pop_and_push(heapptr h, heapdata newelement, heapvalue newscore,
		       heapdata *oldelement, heapvalue *oldscore)

{
  *oldelement = ELEMENT(h, 0) -> datum;
  *oldscore = ELEMENT(h, 0) -> score;
  ELEMENT(h, 0) -> datum = newelement;
  ELEMENT(h, 0) -> score = newscore;
  heapify(h, 0);
}


void heap_pop(heapptr h, heapdata *element, heapvalue *score)

{
  if (h -> size < 1)
    fprintf(stderr, "Heap underflow\n");

  *element = ELEMENT(h, 0) -> datum;
  *score = ELEMENT(h, 0) -> score;
  h -> size--;
  *ELEMENT(h, 0) = *ELEMENT(h, h -> size);
  heapify(h, 0);
}


#ifdef MAIN

main()

{
  heapptr t, h, p;
  char *value;
  float score;

  h = heap_new(20, 0);

  heap_insert(h, "word", 15);
  heap_insert(h, "word2", 13);
  heap_insert(h, "word3", 9);
  heap_insert(h, "word4", 5);
  heap_insert(h, "word5", 12);
  heap_insert(h, "word6", 8);
  heap_insert(h, "word7", 7);
  heap_insert(h, "word8", 4);
  heap_insert(h, "word9", 19);

  p = heap_new(20, 1);

  printf("SHOULD PRINT ELEMENTS vals: 19 15 13 12 9 8 7 5 4\n");
  while ( ! heap_empty(h))
    {
      heap_pop(h, (heapdata *) &value, &score);
      printf("%s %f\n", value, score);
      heap_insert(p, value, score);
    }
  printf("\n");

  printf("SHOULD PRINT REVERSED ELEMENTS vals: 4 5 7 8 9 12 13 15 19\n");
  while ( ! heap_empty(p))
    {
      heap_pop(p, (heapdata *) &value, &score);
      printf("%s %f\n", value, score);
      heap_insert(h, value, score);
    }
  printf("\n");

  printf("SHOULD PRINT REVERSED TWICE: 4 4 5 5 7 7, etc.\n");
  heap_incorporate(h, p);
  heap_incorporate(h, p);
  while ( ! heap_empty(p))
    {
      heap_pop(p, (heapdata *) &value, &score);
      printf("%s %f\n", value, score);
    }

  t = heap_new(10, 0);
  heap_insert(t, "singleton", 10);
  heap_pop(t, (heapdata *) &value, &score);
  printf("Singleton test: SHOULD BE: singleton 10\n");
  printf("%s %f\n", value, score);
  printf("heap length is %d (should be 0)\n", heap_length(t));
  heap_pop(t, (heapdata *) &value, &score);
  printf("Singleton test: SHOULD HAVE UNDERFLOW ERRORED AS EMPTY\n");
}

#endif
