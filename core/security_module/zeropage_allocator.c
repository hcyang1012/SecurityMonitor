#include <zeropage_allocator.h>
#include <monitor_util.h>
#include <monitor_types.h>

extern char *PAGE_POOL_START;

struct zero_page zero_page_pool[NUMBER_OF_PAGES];


void initPagePool()
{
	int zero_page_index;
	for(zero_page_index = 0 ; zero_page_index < NUMBER_OF_PAGES ; zero_page_index++)
	{
		monitor_memset((void*)&zero_page_pool[zero_page_index],sizeof(struct zero_page),0);
	}
}

void* allocateNewZeroPage()
{

	int index;
	index = findUnmappedPage();
	if(index != -1)
	{
		zero_page_pool[index].isMapped = 1;
		monitor_memset((void*)PAGE_POOL_START + index * PAGE_SIZE,PAGE_SIZE,0);
		return (void*)(PAGE_POOL_START + index * PAGE_SIZE);		
	}
	return 0;
}

int findUnmappedPage()
{
	int index;
	for(index = 0 ; index < NUMBER_OF_PAGES ; index++)
	{
		if(zero_page_pool[index].isMapped == 0)
		{
			return index;
		}
	}
	return -1;	
}