#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MX 1000000

int compare(const void *a, const void *b)
{
	long long num1 = *(long long *)a;
	long long num2 = *(long long *)b;

	if (num1 < num2)
		return 1;
	
	if (num1 > num2)
		return -1;

	return 0;
}

int compare2(const void *a, const void *b)
{
	int num1 = *(int *)a;
	int num2 = *(int *)b;

	if (num1 < num2)
		return 1;

	if (num1 > num2)
		return -1;

	return 0;
}


long long arr[100000];



int record[100000];

int data[100000];


typedef struct graphnode {
	int vertex;
	struct graphnode *link;
} node;

typedef struct graphtype {
	int n;
	node* adjlist[100000];
} type;

void insert(type *g, int u, int v)
{
	node *node = malloc(sizeof(node));
	node->vertex = v;
	node->link = g->adjlist[u];
	g->adjlist[u] = node;

}

void init(type *g, int a)
{
	int v;
	
	for (v = 0; v < a; v++)
		g->adjlist[v] = NULL;

}




int main()
{
	type *g = malloc(sizeof(type));
	
	int large = 0;
	
	int a, b;
	scanf("%d %d", &a, &b);

	

	init(g, a);


	for (int i = 0; i < b; i++)
	{
		int c, d;
		scanf("%d %d", &c, &d);
		arr[c]++;
		arr[d]++;
		insert(g, c, d);
		insert(g, d, c);
	}
	
	

	for (int i = 0; i < a; i++)
	{
		record[i] = arr[i];
		arr[i] = ((long long)arr[i] * MX) + i;
	}

	qsort(arr, sizeof(arr) / sizeof(long long), sizeof(long long), compare);

	int i = 0;
	
	while (i != a)
	{
		int real = arr[i] % MX;
		node *temp = g->adjlist[real];
		
		int largenumber = 0;

		while (temp != NULL)
		{
			if (data[temp->vertex] > largenumber && record[temp->vertex] > record[real])
				largenumber = data[temp->vertex];
			temp = temp->link;
		}

		if (largenumber == 0)
			data[real] = 1;
		else
			data[real] = largenumber + 1;

		i++;
	}
	
	qsort(data, sizeof(data) / sizeof(int), sizeof(int), compare2);

	printf("%d", data[0]);
	

	return 0;

}
