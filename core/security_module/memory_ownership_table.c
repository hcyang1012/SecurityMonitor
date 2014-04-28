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
static struct protected_application_t protected_application_table[NUMBER_OF_PROTECTED_APPLICATIONS];
APPID_t currentRunningApplication = OWNER_OTHER;
APPID_t numberOfProtectedApplications = 0;
VMID_t numberOfVMs = 0;

U64_t target = 0;

struct protected_application_t *currentProtectedApplication = NULL;

void protectCurrentApplication()
{
	VMID_t newVMID;
	APPID_t newAPPID;
	GPA_t cr3GPA;

	allocateNewApplicationIdentifiers(&newVMID,&newAPPID);
	initializeNewProtectedApplication(newVMID,newAPPID);
	cr3GPA = get_page_table_base_GPA();
	traverseGuestPages(newVMID, newAPPID, cr3GPA, closePage);
	traverseGuestPages(newVMID, newAPPID, cr3GPA, openPage);
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
	if(gpaToHPA(gpa, 0, 0, 0, &targetEPTEntryHPA))
	{
		debug();
		printf("targetEPTEntryHPA : %llx\n",targetEPTEntryHPA);
	}
	if(targetEPTEntryHPA)
	{
		pTargetEPTEntry = mapHPAintoHVA(targetEPTEntryHPA, sizeof(EPT_ENTRY_t));
		if(!pTargetEPTEntry)
		{
			unmapHPAintoHVA((void*)pTargetEPTEntry,sizeof(EPT_ENTRY_t));
			debug();
			return 0;
		}

		if(changePageStatus(vmID, appID, gpa, CLOSED))
		{
			targetEPTEntry = *pTargetEPTEntry;
			targetEPTEntry &= (~(EPT_ATTRIBUTE_MASK));
			*pTargetEPTEntry = targetEPTEntry;
			if(!target) target = gpa;
			
		}
		else
		{
			//printf("Fail\n");
		}
		unmapHPAintoHVA((void*)pTargetEPTEntry,sizeof(EPT_ENTRY_t));			
		return 0;
	}
	debug();
	printf("Closing error %llx - %llx\n",gpa, targetEPTEntryHPA);

	return 0;
}

void* openPage(const VMID_t vmID, const APPID_t appID, GPA_t gpa)
{
	HPA_t targetEPTEntryHPA = 0;
	EPT_ENTRY_t *pTargetEPTEntry, targetEPTEntry;
	U8_t original_permission;
	gpaToHPA(gpa, 0, 0, 0, &targetEPTEntryHPA);
	if(targetEPTEntryHPA)
	{
		pTargetEPTEntry = mapHPAintoHVA(targetEPTEntryHPA, sizeof(EPT_ENTRY_t));
		if(!pTargetEPTEntry)
		{
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

void remapPage(const GPA_t gpa, void *newPage)
{
	HPA_t 	targetEPTPML4EntryHPA = 0,
			targetEPTPDPEntryHPA = 0,
			targetEPTPDEntryHPA = 0,
			targetEPTEntryHPA = 0;


	HPA_t newPageHPA;
	newPageHPA = virt_to_phys(newPage);
	gpaToHPA(gpa, &targetEPTPML4EntryHPA, &targetEPTPDPEntryHPA, &targetEPTPDPEntryHPA, &targetEPTEntryHPA);

	if(	targetEPTPML4EntryHPA && 
		targetEPTPDPEntryHPA &&
		targetEPTPDEntryHPA &&
		targetEPTEntryHPA)
	{
		EPT_ENTRY_t *pTargetEPTPML4Entry, 
					*pTargetEPTPDPEntry,
					*pTargetEPTPDEntry,
					*pTargetEPTEntry,
					*targetEPTPML4Entry, 
					*targetEPTPDPEntry,
					*targetEPTPDEntry,
					*targetEPTEntry;

		pTargetEPTPML4Entry = mapHPAintoHVA(targetEPTPML4EntryHPA, sizeof(EPT_ENTRY_t));
		pTargetEPTPDPEntry = mapHPAintoHVA(targetEPTPDPEntryHPA, sizeof(EPT_ENTRY_t));
		pTargetEPTPDEntry = mapHPAintoHVA(targetEPTPDEntryHPA, sizeof(EPT_ENTRY_t));
		pTargetEPTEntry = mapHPAintoHVA(targetEPTEntryHPA, sizeof(EPT_ENTRY_t));

		if(!pTargetEPTEntry)
		{
			return;
		}

		changePageStatus(0, 0, gpa, PARTIAL);

		unmapHPAintoHVA((void*)targetEPTPML4Entry, sizeof(EPT_ENTRY_t));
		unmapHPAintoHVA((void*)targetEPTPDPEntry, sizeof(EPT_ENTRY_t));
		unmapHPAintoHVA((void*)targetEPTPDEntry, sizeof(EPT_ENTRY_t));
		unmapHPAintoHVA((void*)targetEPTEntry, sizeof(EPT_ENTRY_t));		
		
	}

	return;
}

int changePageStatus(const VMID_t vmID, const APPID_t appID, const GPA_t gpa, const enum page_state_t state)
{
	HPA_t targetEPTEntryHPA;
	EPT_ENTRY_t *pTargetEPTEntry, targetEPTEntry;
	HPA_t hpa = gpaToHPA(gpa, 0, 0, 0, &targetEPTEntryHPA);
	U64_t index;
	U8_t pageAttribute;

	index = getMemoryOwnershipTableIndex(hpa);
	//printf("Index : %d / %d\n",index,NUMBER_OF_MEMORY_OWNERSHIP_TABLE_ENTRY);

	pTargetEPTEntry = mapHPAintoHVA(targetEPTEntryHPA, sizeof(EPT_ENTRY_t));
	if(!pTargetEPTEntry)
	{
		debug();
		unmapHPAintoHVA((void*)pTargetEPTEntry,sizeof(EPT_ENTRY_t));
		return 0;
	}
	targetEPTEntry = *pTargetEPTEntry;
	unmapHPAintoHVA((void*)pTargetEPTEntry,sizeof(EPT_ENTRY_t));

	pageAttribute = targetEPTEntry & EPT_ATTRIBUTE_MASK;


	if(index >= NUMBER_OF_MEMORY_OWNERSHIP_TABLE_ENTRY)
	{
		//Overflow
		debug();
		return 0;
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
	protected_application_table[appID].owner_VM = vmid;
	protected_application_table[appID].owner_APP = appID;
	setCurrentProtectedApplication(&protected_application_table[appID]);
}

struct protected_application_t *getCurrentProtectedApplication()
{
	int index;
	if(currentProtectedApplication != NULL)
	{
		return currentProtectedApplication;
	}
	else
	{
		GPA_t currentESP = getGuestRSP();
		for(index = 0 ; index < NUMBER_OF_PROTECTED_APPLICATIONS ; index++)
		{
			if(protected_application_table[index].guest_sensitive_stats.SP_User == currentESP)
			{
				debug();
				return &(protected_application_table[index]);
			}
		}
		return 0;
	}
}


struct protected_application_t *findProtectedApplicationFromRIP(const GPA_t source)
{
	int index;
	GPA_t currentRSP = getGuestRSP();
	debug();
	printf("Source : %llx\n",source);
	for(index = 0 ; index < NUMBER_OF_PROTECTED_APPLICATIONS ; index++)
	{
		GPA_t currentProtectedApplicationRIPGPA;
		currentProtectedApplicationRIPGPA = protected_application_table[index].guest_sensitive_stats.RIP_GPA;

		if(	currentProtectedApplicationRIPGPA == source &&
			currentRSP == protected_application_table[index].guest_sensitive_stats.SP_User)
		{
			debug();
			return &(protected_application_table[index]);
		}
	}
	return 0;		
}

void setCurrentProtectedApplication(struct protected_application_t *application)
{
	currentProtectedApplication = application;
}