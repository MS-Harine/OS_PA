#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include "smalloc.h" 

sm_container_ptr sm_first = NULL;
sm_container_ptr sm_last = NULL;
sm_container_ptr sm_unused_containers = NULL;
sm_container_ptr sm_before_hole_unused = NULL;

void sm_container_split(sm_container_ptr hole, size_t size) {
	sm_container_ptr remainder = hole->data + size;

	remainder->data = ((void *)remainder) + sizeof(sm_container_t);
	remainder->dsize = hole->dsize - size - sizeof(sm_container_t);
	remainder->status = Unused;
	remainder->next = hole->next;
	hole->next = remainder;

	if (hole == sm_last)
		sm_last = remainder;

	if (sm_before_hole_unused != NULL) {
		remainder->next_unused = hole->next_unused;
		sm_before_hole_unused->next_unused = remainder;
	}
	else {
		remainder->next_unused = sm_unused_containers->next_unused;
		sm_unused_containers = remainder;
	}
	hole->next_unused = NULL;
}

void * sm_retain_more_memory(int size) {
	sm_container_ptr hole = NULL;
	int pagesize = getpagesize();
	int n_pages = 0;

	n_pages = (sizeof(sm_container_t) + size + sizeof(sm_container_t)) / pagesize  + 1;
	hole = (sm_container_ptr) sbrk(n_pages * pagesize);
	if (hole == NULL)
		return NULL;

	hole->data = ((void *) hole) + sizeof(sm_container_t);
	hole->dsize = n_pages * getpagesize() - sizeof(sm_container_t);
	hole->status = Unused;
	hole->next = NULL;
	hole->next_unused = NULL;

	return hole;
}

void * smalloc(size_t size) {
	sm_container_ptr hole = NULL;
	sm_container_ptr itr = NULL;
	sm_container_ptr last_unused = NULL;
	size_t min_hole = INT_MAX;

	sm_before_hole_unused = NULL;
	for (itr = sm_unused_containers; itr != NULL; itr = itr->next_unused) {
		if (size == itr->dsize) {
			// a hole of the exact size
			if (last_unused != NULL)
				last_unused->next_unused = itr->next_unused;
			else
				sm_unused_containers = sm_unused_containers->next;

			itr->status = Busy;
			itr->next_unused = NULL;
			return itr->data;
		} else if (size + sizeof(sm_container_t) < itr->dsize) {
			// a hole large enought to split 
			if (itr->dsize < min_hole) {
				min_hole = itr->dsize;
				sm_before_hole_unused = last_unused;
				hole = itr;
			}
		}

		last_unused = itr;
	}

	if (hole == NULL) {
		sm_before_hole_unused = last_unused;
		hole = sm_retain_more_memory(size);

		if (hole == NULL)
			return NULL;

		if (sm_first == NULL) {
			sm_first = hole;
			sm_last = hole;
			sm_unused_containers = hole;
			hole->next = NULL;
		} else {
			sm_last->next = hole;
			sm_last = hole;
			hole->next = NULL;
		}
	}
	sm_container_split(hole, size);
	hole->dsize = size;
	hole->status = Busy;

	return hole->data;
}

void sfree(void * p) {
	sm_container_ptr itr = NULL, prev_hole = NULL, last = NULL;
	int size = 0;

	for (itr = sm_first; itr->next != NULL; itr = itr->next) {
		if (itr->data == p) {
			itr->status = Unused;
			break;
		}

		if (itr->status == Unused)
			prev_hole = itr;
	}

	if (prev_hole != NULL) {
		itr->next_unused = prev_hole->next_unused;
		prev_hole->next_unused = itr;
	}
	else {
		itr->next_unused = sm_unused_containers;
		sm_unused_containers = itr;
	}

	if (prev_hole == NULL)
		last = itr;
	else if (prev_hole->next == itr)
		last = itr = prev_hole;
	else
		last = itr;
	prev_hole = NULL;

	while (itr != NULL && itr->status == Unused) {
		size += (itr->dsize + sizeof(sm_container_t));
		prev_hole = itr;
		itr = itr->next;
	}

	if (size != 0) {
		size -= sizeof(sm_container_t);
		last->dsize = size;
		last->next_unused = prev_hole == NULL ? prev_hole : prev_hole->next_unused;
		last->next = itr;
	}
}

void print_sm_containers() {
	sm_container_ptr itr;
	int i = 0;

	printf("==================== sm_containers ====================\n");
	for (itr = sm_first; itr != NULL; itr = itr->next, i++) {
		char * s;
		printf("%3d:%p:%s:", i, itr->data, itr->status == Unused ? "Unused" : "  Busy");
		printf("%8d:", (int) itr->dsize);

		for (s = (char *) itr->data;
				s < (char *) itr->data + (itr->dsize > 8 ? 8 : itr->dsize);
				s++) 
			printf("%02x ", *s);
		printf("\n");
	}
	printf("=======================================================\n");
}

void print_unused() {
	sm_container_ptr itr = NULL;
	int i = 0;

	printf("======================= Unused ==========================\n");
	for (itr = sm_unused_containers; itr != NULL; itr = itr->next_unused, i++) {
		char *s;
		printf("%3d:%p:%s:", i, itr->data, itr->status == Unused ? "Unused" : "  Busy");
		printf("%8d:", (int)itr->dsize);
		
		for (s = (char *) itr->data;
				s < (char *) itr->data + (itr->dsize > 8 ? 8 : itr->dsize);
				s++) 
			printf("%02x ", *s);
		printf("\n");
	}
	printf("=========================================================\n");
}

void print_sm_uses() {
	sm_container_ptr itr = NULL;
	size_t retained = 0, using = 0, unused = 0;
	
	for (itr = sm_first; itr != NULL; itr = itr->next) {
		retained += (itr->dsize + sizeof(sm_container_t));
		if (itr->status == Busy)
			using += itr->dsize;
		else
			unused += itr->dsize;
	}

	printf("================= Current memory uses =================\n");
	printf("Allocated (retained) : %lu\n", retained);
	printf("Using (Busy) : %lu\n", using);
	printf("Unused (Unused) : %lu\n", unused);
	printf("=======================================================\n");
}
