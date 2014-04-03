#include <util.h>
#include <sec_types.h>
#include <vmcs.h>

void sec_memset(U8_t *dst, U8_t value, U32_t size)
{
	int index;
	for(index = 0 ; index < size ; index++)
	{
		dst[index] = value;
	}
}

void sec_showDataofPhysicalAddress(PHYADDR_t *physicalAddress)
{
	VIRTADDR_t *virtualAddress;
	virtualAddress = getVirtualAddressFromPhysiacalAddress(physicalAddress);
	printf("Physical Address(%llx)\tVirtual Address(%llx)\tData(%llx)\n",physicalAddress,virtualAddress,*virtualAddress);
}