#ifndef __ZERO_PAGE_ALLOCATOR_H__
#define __ZERO_PAGE_ALLOCATOR_H__

#include <monitor_types.h>

#define NUMBER_OF_PAGES 1024
#define PAGE_SIZE (1024*4)

struct zero_page
{
	U8_t isMapped;	
};

void* allocateNewZeroPage();
int findUnmappedPage();
void initPagePool();
#endif