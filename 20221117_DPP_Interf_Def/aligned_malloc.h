//aligned_malloc.h
#ifndef ALIGNED_MALLOC_H
#define  ALIGNED_MALLOC_H

#include <stdio.h>
#include <stdlib.h>

void *aligned_malloc(size_t bytes, size_t alignment);
void aligned_free(void *p );
#endif
