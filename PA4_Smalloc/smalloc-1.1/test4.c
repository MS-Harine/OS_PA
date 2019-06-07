#include <stdio.h>
#include "smalloc.h"

int 
main()
{
	void *p1, *p2, *p3, *p4, *p5;

	p1 = smalloc(1000); 
	printf("smalloc(1000)\n"); 

	p2 = smalloc(400); 
	printf("smalloc(400)\n"); 

	p3 = smalloc(1000); 
	printf("smalloc(1000)\n"); 

	p4 = smalloc(350); 
	printf("smalloc(350)\n"); 

	p5 = smalloc(1000); 
	printf("smalloc(1000)\n"); 

	sfree(p2); 
	printf("sfree(%p)\n", p2); 

	sfree(p4); 
	printf("sfree(%p)\n", p4); 
	print_sm_containers();

	p2 = smalloc(300);
	printf("smalloc(300)\n");
	print_sm_containers();

	print_sm_uses();
}
