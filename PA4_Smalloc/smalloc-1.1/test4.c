#include <stdio.h>
#include "smalloc.h"

int 
main()
{
	void *p1, *p2, *p3, *p4, *p5;

	print_sm_containers();

	p1 = smalloc(300); 
	printf("smalloc(300)\n"); 
	print_sm_containers();

	p2 = smalloc(200); 
	printf("smalloc(200)\n"); 
	print_sm_containers();

	p3 = smalloc(100); 
	printf("smalloc(100)\n"); 
	print_sm_containers();

	sfree(p1); 
	printf("sfree(%p)\n", p1); 
	print_sm_containers();

	sfree(p2); 
	printf("sfree(%p)\n", p2); 
	print_sm_containers();

	sfree(p3); 
	printf("sfree(%p)\n", p3); 
	print_sm_containers();

	p1 = smalloc(110);
	printf("smalloc(110)\n");
	print_sm_containers();

	print_sm_uses();
}
