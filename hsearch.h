// hsearch.h by Suzanne Aldrich for CS121

#define MAX_NAME_LENGTH 64
#define	MAX_CITIES	16

typedef struct {
	char name[MAX_NAME_LENGTH];
	float x;
	float y;
} city;

typedef struct {
	float g, h;
	int length;
	int path[MAX_CITIES + 1];
} searchNodeData, *searchNode; 

searchNode ASearch();
int goaltest(searchNode node);
int expand(searchNode parent, searchNode* children);
float eval(searchNode node);
float h0(searchNode node);
float h1(searchNode node);
float h2(searchNode node);
