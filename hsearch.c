// hsearch.c by Suzanne Aldrich for CS121

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "hsearch.h"
#include "pqueue.h"

#define MAXIMUM 65536	// max number of nodes in pqueue

int Heuristic;
city* Cities;
int CityCount;
int nodes_expanded; 
int verbose = 0;
//float scale = 1.0;


void PrintNode(searchNode node) {
	int i;
	
	printf("Node: (");
	for (i=0;i<node->length;i++) {
		printf("%s", Cities[node->path[i]].name);
		if (i < node->length - 1) printf(", ");
	}
	printf(")\n");
}

int main(int argc, char *argv[])
{
	int i;
	FILE *fp;
	
	float x;
	float y;
	char name[100];
	
	searchNode solution;
	
	if(argc < 2) {
		printf("usage: hsearch datafile [0|1|2]\n");
		return(1);
	} else if (argc == 2) {
		Heuristic = 2;
	} else {
		//scale = atof(argv[2]);
	
		Heuristic =  atoi(argv[2]);
		if (Heuristic < 0 || Heuristic > 2) {
			printf("usage: hsearch datafile [0|1|2]\n");
			return(1);
		}
	}
	
	if (verbose) printf("open file: %s\n", argv[1]);
	if (verbose) printf("heuristic: %d\n\n", Heuristic);
	
	if( (fp = fopen(argv[1], "r")) == NULL) {
		printf("error: cannot open the file\n");
		return(1);
	}
	
	fscanf(fp, "%d", &CityCount);
	if (CityCount > MAX_CITIES) {
		printf("error: max city count exceeded\n");
		return(1);
	}
	
	Cities = (city*)malloc(CityCount * sizeof(city));
	assert(Cities);
	
	for(i=0; i<CityCount; i++)
	{
		fscanf(fp, "%s", name);
		fscanf(fp, "%f", &x);
		fscanf(fp, "%f", &y);
		
		strcpy(Cities[i].name, name);
		Cities[i].x = x;
		Cities[i].y = y;
		
		if (verbose) printf("City %d is %s: (%f, %f) \n", i, Cities[i].name, Cities[i].x, Cities[i].y);
	}
	
	fclose(fp);

	if (verbose) printf("\n");

	solution = ASearch();
	assert(solution);

	if (verbose) printf("\n");
			
	printf("Number of nodes expanded: %d\n", nodes_expanded);
	
	printf("Optimal tour: (");
	for (i=0;i<solution->length;i++) {
		printf("%s", Cities[solution->path[i]].name);
		if (i < solution->length - 1) printf(", ");
	}
	printf(")\n");
	
	printf("Cost: %f\n", solution->g);
	
	free(Cities);
	
	return(0);
}

searchNode ASearch() {

	heapptr Paths;
	searchNode initial, node;
	float f;
	searchNode* successors;
	int i, successorCount;
	
	if (verbose) printf("Entering ASearch\n");	
	
	Paths = heap_new(MAXIMUM, 1);

	initial = (searchNode)malloc(sizeof(searchNode));
	assert(initial);
	
	initial->path[0] = 0;
	initial->length = 1;
	initial->g = 0;
	initial->h = 0;
	
	successors = (searchNode*)malloc(CityCount * sizeof(searchNode));
	assert(successors);
	

	heap_insert(Paths, (void*)initial, eval(initial));
	
	if (verbose) printf("Inserting initial into heap\n");
	if (verbose) PrintNode(initial);
	if (verbose) printf("\n");
	
	while (!heap_empty(Paths)) {
		
		heap_pop(Paths, (void**)&node, &f);
		
		if (verbose) printf("Popping best from heap: f = %f\n", f);
		if (verbose) PrintNode(node);
		
		if (goaltest(node)) {
			
			if (verbose) printf("Nodes left in heap: %d\n", heap_length(Paths));
			heap_free(Paths);
			if (verbose) printf("Exiting ASearch\n");
			return node;
		}
		
		successorCount = expand(node, successors);

		free(node);

		for (i = 0; i < successorCount; i++) {
			
			if (verbose) printf("Inserting successor %d into heap\n", i);
			if (verbose) PrintNode(successors[i]);
			
			if (!heap_full(Paths)) {
				heap_insert(Paths, (void*)(successors[i]), eval(successors[i]));
			} else {
				printf("error: max heap elements exceeded\n");
				heap_free(Paths);
				if (verbose) printf("Exiting ASearch\n");
				return NULL;
			}
		}
		
		if (verbose) printf("\n");	
	}

	heap_free(Paths);
	
	if (verbose) printf("Exiting ASearch\n");	
	
	return(NULL);

}

int goaltest(searchNode node) {

	if (node->length == CityCount + 1) {

		if (verbose) printf("goaltest: GOAL!\n\n");	
	
		return 1;
		
	} else  {
	
		if (verbose) printf("goaltest: not a goal\n\n");	
	
		return 0;
	}
}

searchNode newNode(searchNode parent, int city) {
		
		searchNode node = (searchNode)malloc(sizeof(*node));
		
		*node = *parent;
		node->path[node->length++] = city;
		
		return node;
}

int expand(searchNode parent, searchNode* children) {

	int i, j, k;

	k = 0;	// k indexes through the children array
	
	for (i = 0; i < CityCount; i++) {			// foreach city in Cities
		for (j = 0; j < parent->length; j++) {	// foreach city in the parent's path
			 if (i == parent->path[j]) break;	// if the city is in the path, end loop early
		}
		if (j == parent->length) {				// if city isn't in path
			children[k++] = newNode(parent, i);
		}
	}
	
	if (k == 0) {
		children[k++] = newNode(parent, 0);
	}
	
	nodes_expanded++;

	if (verbose) printf("expand: expanded node #%d with pathlength %d has %d successors\n\n", nodes_expanded, parent->length, k);	
	
	return k;
}

float distance(int a, int b) {

	float dx, dy, answer;
	
	dx = Cities[a].x - Cities[b].x;
	dy = Cities[a].y - Cities[b].y;
	answer = sqrt(dx * dx + dy * dy);

	return (answer);
}

float pathlength(searchNode node) {

	int i;
	float cost = 0;
	
	for (i = 1; i < node->length; i++) {
		cost += distance(node->path[i], node->path[i - 1]);
	}
	
	return cost;
}

float eval(searchNode node) {
		
	node->g = pathlength(node);
	
	switch (Heuristic) {
		case 0: node->h = h0(node); break;
		case 1: node->h = h1(node); break;
		case 2: node->h = h2(node); break;
	}

	if (verbose) printf("eval: g = %f, h = %f\n\n", node->g, node->h);
	
	return (node->g + node->h);
}

/*	h = 0 (uniform cost search) */
float h0(searchNode node) {
	
	return 0;
	
}

/*	if pathlength < CityCount/2
	h = 2 * max(distance(start, every city)) - distance(start, last in path)
	else
	h = distance(start, last in path) */
float h1(searchNode node) {
	
	int i;
	int m = 0;
	float h, d;
	float p = distance(node->path[0], node->path[node->length - 1]); 
	float max = 0;
	
	if (node->length - 1 < CityCount/2) {
		for (i = 1; i < CityCount; i++) {
			d = distance(node->path[0], i);
			if (d > max) {
				max = d;
				m = i;
			}
		}
		if (verbose) printf("Furthest is %s\n", Cities[m].name);
		if (verbose) printf("h is 2 * %f - %f\n", max, p);
		h = 2 * max - p;
	} else {
		if (verbose) printf("h is %f\n", p);
		h = p;
	}
		
	return h;

}

enum { inPath, inTree, Free };

int nearestFreeNeighbor(int city, int* treecities, float* cost) {
	
	int i;
	int m = city;
	float d, min = 0;
	
	for (i = 0; i < CityCount; i++) if (treecities[i] == Free) {	// for every free city
		d = distance(city, i);
		if (min == 0 || d < min) {
			min = d;
			m = i;
		}
		if (verbose) printf("i = %d, d = %f, min = %f\n", i, d, min);
	}
	if (verbose) printf("Nearest to %s is %s with cost %f\n", Cities[city].name, Cities[m].name, min);
	*cost = min;
	return m;
}

int costToTree(int city, int* treecities) {
	
	int i;
	int m = city;
	float d, min = 0;
	
	for (i = 0; i < CityCount; i++) if (treecities[i] == inTree) {	// for every tree city
		d = distance(city, i);
		if (min == 0 || d < min) {
			min = d;
			m = i;
		}
		if (verbose) printf("i = %d, d = %f, min = %f\n", i, d, min);
	}
	if (verbose) printf("Nearest is %s with cost %f\n", Cities[m].name, min);
	return min;
}

/* find nearest neighbor to the tree, add to tree, and return cost of adding it */
float mstGrow(int* treecities) {
	
	int i, n, m = 0;
	float c, cost = 0;
	
	for (i = 0; i < CityCount; i++) if (treecities[i] == inTree) { 	// for every city in MST
		n = nearestFreeNeighbor(i, treecities, &c);
		
		if (cost == 0 || c < cost) {
			cost = c;
			m = n;
		}			
	}
	treecities[m] = inTree;
	if (verbose) printf("City added to MST is %s with a cost of %f\n", Cities[m].name, cost);
	return cost;
}

/*	h = cost of minimum spanning tree of unvisited nodes 
	  + minimum cost of attaching mst to path  */
float h2(searchNode node) {

	int i, j;
	float h;
	int* treecities = (int*)malloc(CityCount * sizeof(int));
	
	for (i = 0; i < CityCount; i++) {			
		for (j = 0; j < node->length; j++)	
			 if (i == node->path[j]) break;		// if the city is in the path, end loop early
		treecities[i] = (j == node->length) ? Free : inPath;
	}
	
	if (verbose) printf("Finding cost to tree from %s\n", Cities[node->path[node->length - 1]].name);
	treecities[nearestFreeNeighbor(node->path[node->length - 1], treecities, &h)] = inTree;
	if (verbose) printf("\n");
	
	if (verbose) printf("Finding cost of MST of unvisited cities\n");
	for (i = 0; i < CityCount - (node->length + 1); i++)
		h += mstGrow(treecities);
	if (verbose) printf("\n");
	
	//h += scale * costToTree(node->path[0], treecities);
	if (verbose) printf("Finding cost to tree from %s\n", Cities[node->path[0]].name);
	h += costToTree(node->path[0], treecities);
	if (verbose) printf("\n");
	
	free(treecities);
	return h;
	
}