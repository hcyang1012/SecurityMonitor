/*!	\file monitor_util.h
	\brief This file exists for supporting some uiility functions.

	\fn void *mapHPAintoHVA(const HPA_t hpa, const U64_t size)
	\brief This function maps host physical address into hypervisor's virtual memory.
	\return This function returns mapped host virtual memory

	Details. {mapHPAintoHVA}

	This function is used to convert host physical address to host virtual address.
	During converting, depending on the base hypervisor solution, the HVA may be newly allocated. In this case, this function allocats new virtual address by invoking __HYPERVISOR_DEPENDENT ALLOCATION FUNCTION__.\n
	\note In this implementation that uses Bitvisor, mapmem_hphys() Bitvisor function is used to allocate new virtual memory. Therefore, this __MUST BE UNALLOCATED__ after usage by invoking [unmapHPAintoHVA] (@ref mapHPAintoHVA) function

	\fn void unmapHPAintoHVA(void* hva, const U64_t size)
	\brief This function maps host physical address into hypervisor's virtual memory.

	Details. {unmapHPAintoHVA}

	Depending on the implementation of [mapHPAintoHVA](@ref mapHPAintoHVA) function, a mapped virtual memory should be deallocated after its use. This function exist for this purpose.
	\note In this implmentation that uses Bitvisor, mapmem_hphys() Bitvisor function is used to deallocate the virtual memory.

	\fn HPA_t gpaToHPA(const GPA_t gpa, HPA_t *eptEntryHPA)
	\brief This function converts guest physical address to host physical address.
	\return	This function returns converted HPA, and HPA of EPT entry of returned HPA.

	Detail.

	This function converts guest physical address (GPA) to host physical address (HPA).\n
	To do this, this function traverses the current guest's page table from PML4 to page table.\n
	And, it also returns HPA of EPT entry of converted HPA.\n
	For more information, see __Intel's Architecture Manual Vol. 3, Chapter 28.2.2 : EPT Tranlation Mechanism__.

*/

#ifndef __MONITOR__UTIL_H__
#define __MONITOR__UTIL_H__


#include <monitor_types.h>

void *mapHPAintoHVA(const HPA_t hpa, /**< [in] Host physical address to map. */
					const U64_t size /**< [in] The size of the virtual memory to be allocated. */
					);

void unmapHPAintoHVA(void* hva, /**< [in] Host virtual address to unmap */ 
					const U64_t size  /**< [in] The size of the allocated virtual memory speficied in the [mapHPAintoHVA](@ref mapHPAintoHVA) function.*/
					);

HPA_t gpaToHPA(const GPA_t gpa, /**< [in] Guest physical address to convert */
				HPA_t *eptEntryHPA /**< [out] HPA of EPT entry of the returned HPA */
				);

void monitor_memset(char *src, const int size, const char value);
#endif