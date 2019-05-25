#ifndef __DETECTOR_FUNC_H__
#define __DETECTOR_FUNC_H__

#include "info.h"
#include "linked_list.h"
#include "graph.h"

void assign_mutex(AdjList *, data_t *, LinkedList *);
void expire_mutex(AdjList *, data_t *);
int find_cycle(AdjList *);

#endif
