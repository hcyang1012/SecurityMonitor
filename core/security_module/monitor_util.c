/*!	\file monitor_util.c
	\brief This file includes some implementations of some utility functions.

*/

#include <monitor_util.h>
#include <monitor_ept.h>
#include <guest_state.h>
#include <monitor_types.h>

#ifdef CONFIG_BITVISOR
	#include <mm.h>
	#include <printf.h>

#endif

SPIN_LOCK_t ept_spinlock = 0xFF;

HPA_t gpaToHPA(const GPA_t gpa, HPA_t *eptPML4EntryHPA, HPA_t *eptPDPEntryHPA, HPA_t *eptPDEntryHPA, HPA_t *eptEntryHPA)
{

	HPA_t eptBaseHPA;
	HPA_t currentEPT_PML4_Entry_HPA;
	EPT_ENTRY_t *pCurrentEPT_PML4_Entry, currentEPT_PML4_Entry;

	HPA_t eptBase_PDP_HPA;
	HPA_t currentEPT_PDP_Entry_HPA;
	EPT_ENTRY_t *pCurrentEPT_PDP_Entry, currentEPT_PDP_Entry;

	HPA_t eptBase_PD_HPA;
	HPA_t currentEPT_PD_Entry_HPA;
	EPT_ENTRY_t *pCurrentEPT_PD_Entry, currentEPT_PD_Entry;	

	HPA_t eptBase_PT_HPA;
	HPA_t currentEPT_PT_Entry_HPA;
	EPT_ENTRY_t *pCurrentEPT_PT_Entry, currentEPT_PT_Entry;		

	HPA_t pageFrameHPA;

	if(eptPML4EntryHPA)
	{
		*eptPML4EntryHPA = 0;	
	}

	if(eptPDPEntryHPA)
	{
		*eptPDPEntryHPA = 0;	
	}

	if(eptPDEntryHPA)
	{
		*eptPDEntryHPA = 0;	
	}

	if(eptEntryHPA) 
	{
		*eptEntryHPA = 0;
	}


	eptBaseHPA = get_ept_base_HPA();
	currentEPT_PML4_Entry_HPA = eptBaseHPA | ((gpa & EPT_PML4_GPA_MASK) >> EPT_PML4_GPA_SHIFT);
	pCurrentEPT_PML4_Entry = (EPT_ENTRY_t*)mapHPAintoHVA(currentEPT_PML4_Entry_HPA,sizeof(EPT_ENTRY_t));
	if(!pCurrentEPT_PML4_Entry)
	{
		{
			extern int protecting;
			if(protecting && eptEntryHPA != 0)
			{
				debug();
				printf("eptEntryHPA : %llx\n",eptEntryHPA);
			}
		
		}
		return 0;
	}
	currentEPT_PML4_Entry = *pCurrentEPT_PML4_Entry;
	if(eptPML4EntryHPA)
	{
		*eptPML4EntryHPA = currentEPT_PML4_Entry_HPA;	
	}	
	unmapHPAintoHVA((void*)pCurrentEPT_PML4_Entry,sizeof(EPT_ENTRY_t));
	if(!currentEPT_PML4_Entry)
	{
		
		{
			extern int protecting;
			if(protecting && eptEntryHPA != 0)
			{
				debug();
				printf("eptEntryHPA : %llx\n",eptEntryHPA);
			}
		
		}
		return 0;
	}

	eptBase_PDP_HPA = currentEPT_PML4_Entry & EPT_PML4_ENTRY_MASK;
	currentEPT_PDP_Entry_HPA = eptBase_PDP_HPA | ((gpa & EPT_PDP_GPA_MASK) >> EPT_PDP_GPA_SHIFT);
	pCurrentEPT_PDP_Entry = (EPT_ENTRY_t*)mapHPAintoHVA(currentEPT_PDP_Entry_HPA,sizeof(EPT_ENTRY_t));
	if(!pCurrentEPT_PDP_Entry)
	{
		
		{
			extern int protecting;
			if(protecting && eptEntryHPA != 0)
			{
				debug();
				printf("eptEntryHPA : %llx\n",eptEntryHPA);
			}
		
		}
		return 0;
	}
	currentEPT_PDP_Entry = *pCurrentEPT_PDP_Entry;
	if(eptPDPEntryHPA)
	{
		*eptPDPEntryHPA = currentEPT_PDP_Entry_HPA;	
	}	
	unmapHPAintoHVA((void*)pCurrentEPT_PDP_Entry,sizeof(EPT_ENTRY_t));
	if(!currentEPT_PDP_Entry)
	{
		{
			extern int protecting;
			if(protecting && eptEntryHPA != 0)
			{
				debug();
				printf("eptEntryHPA : %llx\n",eptEntryHPA);
			}
		
		}		

		return 0;
	}	

	eptBase_PD_HPA = currentEPT_PDP_Entry & EPT_PDP_ENTRY_MASK;
	currentEPT_PD_Entry_HPA = eptBase_PD_HPA | ((gpa & EPT_PD_GPA_MASK) >> EPT_PD_GPA_SHIFT);
	pCurrentEPT_PD_Entry = (EPT_ENTRY_t*)mapHPAintoHVA(currentEPT_PD_Entry_HPA,sizeof(EPT_ENTRY_t));
	if(!pCurrentEPT_PD_Entry)
	{		
		
		{
			extern int protecting;
			if(protecting && eptEntryHPA != 0)
			{
				debug();
				printf("eptEntryHPA : %llx\n",eptEntryHPA);
			}
		
		}
		return 0;
	}	
	currentEPT_PD_Entry = *pCurrentEPT_PD_Entry;
	if(eptPDEntryHPA)
	{
		*eptPDEntryHPA = currentEPT_PD_Entry_HPA;	
	}	
	unmapHPAintoHVA((void*)pCurrentEPT_PD_Entry,sizeof(EPT_ENTRY_t));
	if(!currentEPT_PD_Entry)
	{	
		{
			extern int protecting;
			if(protecting && eptEntryHPA != 0)
			{
				debug();
				printf("eptEntryHPA : %llx\n",eptEntryHPA);
			}
		
		}		
		return 0;
	}
	eptBase_PT_HPA = currentEPT_PD_Entry & EPT_PD_ENTRY_MASK;
	currentEPT_PT_Entry_HPA = eptBase_PT_HPA | ((gpa & EPT_PT_GPA_MASK) >> EPT_PT_GPA_SHIFT);
	pCurrentEPT_PT_Entry = (EPT_ENTRY_t*)mapHPAintoHVA(currentEPT_PT_Entry_HPA,sizeof(EPT_ENTRY_t));

	if(!pCurrentEPT_PT_Entry)
	{
		{
			extern int protecting;
			if(protecting && eptEntryHPA != 0)
			{
				debug();
				printf("eptEntryHPA : %llx\n",eptEntryHPA);
			}
		
		}
		return 0;
	}
	
	currentEPT_PT_Entry = *pCurrentEPT_PT_Entry;		
	if(eptEntryHPA)
	{
		{
			extern int protecting;
			if(protecting && eptEntryHPA != 0)
			{
				debug();
				printf("eptEntryHPA(%llx) = %llx\n",eptEntryHPA, currentEPT_PT_Entry_HPA);

			}
		
		}
		*eptEntryHPA = currentEPT_PT_Entry_HPA;	
	}
	unmapHPAintoHVA((void*)pCurrentEPT_PT_Entry,sizeof(EPT_ENTRY_t));	
	if(!currentEPT_PT_Entry)
	{
		{
			extern int protecting;
			if(protecting && eptEntryHPA != 0)
			{
				debug();
				printf("eptEntryHPA : %llx\n",eptEntryHPA);
			}
		
		}
		return 0;
	}
	pageFrameHPA = ((currentEPT_PT_Entry & EPT_PT_ENTRY_MASK) | (gpa & EPT_GPA_MASK));
	

	return pageFrameHPA;

}


void *mapHPAintoHVA(const HPA_t hpa, const U64_t size)
{
	#ifdef CONFIG_BITVISOR
	void *hva = 0;
	hva = mapmem_hphys (hpa, size, MAPMEM_WRITE);	
	return hva;
	#else
		return 0;
	#endif
}

void unmapHPAintoHVA(void* hva, const U64_t size)
{
	#ifdef CONFIG_BITVISOR
		unmapmem (hva, size);
	#else
		//Noop
	#endif
}

void monitor_memset(char *src, const int size, const char value)
{
	int index;
	for(index = 0 ; index < size ; index++)
	{
		src[index] = value;
	}
}

HPA_t virt_to_phys(const void* hva)
{
	#ifdef CONFIG_BITVISOR
	return (HPA_t)(hva - VMM_START_VIRT);
	#else
	return 0;
	#endif
}

