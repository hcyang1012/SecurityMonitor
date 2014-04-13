/*!	\file memory_ownership_table.c
	\brief Definition of data structrues & functions for memory ownership table.\n

	\var static struct memory_ownership_table_entry_t memory_ownership_table[NUMBER_OF_MEMORY_OWNERSHIP_TABLE_ENTRY]
	\brief The variable that indicates memory ownership table.

	Details.

	We call this array as __memory ownership table__. Each element in this array is associated to one machine frame, and array index indicates the frame number.
*/

#include <memory_ownership_table.h>
#include <guest_state.h>
#include <monitor_util.h>
#include <monitor_ept.h>

#ifdef CONFIG_BITVISOR
#include <mm.h>
#endif

static struct memory_ownership_table_entry_t memory_ownership_table[NUMBER_OF_MEMORY_OWNERSHIP_TABLE_ENTRY];

APPID_t currentRunningApplication = OWNER_OTHER;
APPID_t numberOfProtectedApplications = 0;
VMID_t numberOfVMs = 0;

void protectCurrentApplication()
{
	VMID_t newVMID;
	APPID_t newAPPID;
	GPA_t cr3GPA;

	allocateNewApplicationIdentifiers(&newVMID,&newAPPID);
	initializeNewProtectedApplication(&newVMID,&newAPPID);
	cr3GPA = get_page_table_base_GPA();
	traverseGuestPages(newVMID, newAPPID, cr3GPA, closePage);
}

void allocateNewApplicationIdentifiers(VMID_t *new_VMID, APPID_t *new_APPID)
{
	VMID_t allocatedVMID;
	APPID_t allocatedAppID;

	allocatedVMID = allocateNewVMID();
	*new_VMID = allocatedVMID;

	allocatedAppID = allocateNewAppID();
	*new_APPID = allocatedAppID;
}

VMID_t allocateNewVMID()
{
	VMID_t newVMID;
	newVMID = 1;
	numberOfVMs = 1;
	return newVMID;
}

APPID_t allocateNewAppID()
{
	APPID_t newAppID;
	newAppID = ++numberOfProtectedApplications;	
	return newAppID;
}

U64_t getMemoryOwnershipTableIndex(const HPA_t hpa)
{
	return hpa >> HPA_TO_INDEX_SHIFT;
}

void* closePage(const VMID_t vmID, const APPID_t appID, GPA_t gpa)
{
	HPA_t targetEPTEntryHPA = 0;
	EPT_ENTRY_t *pTargetEPTEntry, targetEPTEntry;
	printf("Close Page start : %llx %llx\n", gpa, gpaToHPA(gpa, &targetEPTEntryHPA));
	if(targetEPTEntryHPA)
	{
		pTargetEPTEntry = mapHPAintoHVA(targetEPTEntryHPA, sizeof(EPT_ENTRY_t));
		if(!pTargetEPTEntry)
		{
			printf("Error\n");
			return 0;
		}


		if(changePageStatus(vmID, appID, gpa, CLOSED))
		{
			printf("Close %llx\n",gpa);
			targetEPTEntry = *pTargetEPTEntry;
			targetEPTEntry &= (~(EPT_ATTRIBUTE_MASK));
			*pTargetEPTEntry = targetEPTEntry;
			unmapHPAintoHVA((void*)pTargetEPTEntry,sizeof(EPT_ENTRY_t));			
		}
		else
		{
			//printf("Fail\n");
		}
	}

	return 0;
}

void* openPage(const VMID_t vmID, const APPID_t appID, GPA_t gpa)
{
	HPA_t targetEPTEntryHPA = 0;
	EPT_ENTRY_t *pTargetEPTEntry, targetEPTEntry;
	U8_t original_permission;
	gpaToHPA(gpa, &targetEPTEntryHPA);
	if(targetEPTEntryHPA)
	{
		pTargetEPTEntry = mapHPAintoHVA(targetEPTEntryHPA, sizeof(EPT_ENTRY_t));
		if(!pTargetEPTEntry)
		{
			printf("Error\n");
			return 0;
		}

		original_permission = changePageStatus(vmID, appID, gpa, OPENED);

		targetEPTEntry = *pTargetEPTEntry;
		targetEPTEntry |= (original_permission);
		*pTargetEPTEntry = targetEPTEntry;
		unmapHPAintoHVA((void*)pTargetEPTEntry,sizeof(EPT_ENTRY_t));			
	}

	return 0;
}

int changePageStatus(const VMID_t vmID, const APPID_t appID, const GPA_t gpa, const enum page_state_t state)
{
	HPA_t targetEPTEntryHPA;
	EPT_ENTRY_t *pTargetEPTEntry, targetEPTEntry;
	HPA_t hpa = gpaToHPA(gpa, &targetEPTEntryHPA);
	U64_t index;
	U8_t pageAttribute;

	index = getMemoryOwnershipTableIndex(hpa);
	//printf("Index : %d / %d\n",index,NUMBER_OF_MEMORY_OWNERSHIP_TABLE_ENTRY);

	pTargetEPTEntry = mapHPAintoHVA(targetEPTEntryHPA, sizeof(EPT_ENTRY_t));
	if(!pTargetEPTEntry)
	{
		printf("Error : %s(%d)\n",__FILE__, __LINE__);
		return 0;
	}
	targetEPTEntry = *pTargetEPTEntry;
	unmapHPAintoHVA((void*)pTargetEPTEntry,sizeof(EPT_ENTRY_t));

	pageAttribute = targetEPTEntry & EPT_ATTRIBUTE_MASK;


	if(index >= NUMBER_OF_MEMORY_OWNERSHIP_TABLE_ENTRY)
	{
		printf("%s(%d) - Overflow : %d(%llx) / %d \n",__FILE__,__LINE__,index,hpa,NUMBER_OF_MEMORY_OWNERSHIP_TABLE_ENTRY);
	}

	if(	memory_ownership_table[index].owner_VM == 0 &&
		memory_ownership_table[index].owner_APP == 0)
	{
		memory_ownership_table[index].owner_VM = vmID;
		memory_ownership_table[index].owner_APP = appID;
	}
	memory_ownership_table[index].state = state;
	memory_ownership_table[index].original_page_address = 0;
	if(state == CLOSED)
	{
		memory_ownership_table[index].original_permission = pageAttribute;
		
	}

	return memory_ownership_table[index].original_permission;

}

struct memory_ownership_table_entry_t getMemoryOwnershipTableEntry(const U64_t index)
{
	return memory_ownership_table[index];
}

void initializeNewProtectedApplication(const VMID_t vmid, const APPID_t appID)
{
	
}