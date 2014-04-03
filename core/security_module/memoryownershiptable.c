#include <memoryownershiptable.h>
#include <util.h>
#include <vmcs.h>

#ifdef CONFIG_BITVISOR
	#include <printf.h>
#endif


PHYADDR_t targetApplicationPageTableBasePhysicalAddress = 0;
OWNERSHIPTABLE_t ownershipTable[MAX_NUMBER_OF_PAGES];
U64_t gpaToHPACache[PD_INDEX_MAX * PT_INDEX_MAX];
APPID_t protectingPageTable[PD_INDEX_MAX * PT_INDEX_MAX];
BOOL_t stop = FALSE;

//Initialize memory ownership table
void initOwnershipTable()
{
	U32_t index;
	U32_t primaryProcessorBasedVMExecutionControlsVector;

	for(index = 0 ; index < MAX_NUMBER_OF_PAGES ; index++)
	{
		sec_memset(ownershipTable + index,0,sizeof(OWNERSHIPTABLE_t));
		ownershipTable[index].ownerVM = OWNER_VMM;
		ownershipTable[index].ownerApp = OWNER_KERNEL;
	}
	sec_memset(gpaToHPACache,0,sizeof(U32_t) * PD_INDEX_MAX * PT_INDEX_MAX);

	primaryProcessorBasedVMExecutionControlsVector = sec_getPrimaryProcessorBasedVMExecutionControlsVector();
}

//Traverse one VM's EPT & update memory ownership table
void updateOwnershipTableForOneVM(const VMID_t vmid, PHYADDR_t *eptPhyBaseAddr)
{
	traverseEPTPML4(vmid,eptPhyBaseAddr);
}

//Traverse EPT PML4
void traverseEPTPML4(const VMID_t vmid, PHYADDR_t *eptPhyBaseAddr)
{
	int eptPML4Index;
	for(eptPML4Index = 0 ; eptPML4Index < NUMBER_OF_PML4_ENTRY ; eptPML4Index++)
	{
		PHYADDR_t *currentPML4EntryPhyAddr;
		VIRTADDR_t *currentPML4EntryVirtAddr;
		U64_t currentPML4Entry;

		currentPML4EntryPhyAddr = ((U64_t)eptPhyBaseAddr | ((eptPML4Index & 0xFF) << PML4_SHIFT));
		currentPML4EntryVirtAddr = getVirtualAddressFromPhysiacalAddress(currentPML4EntryPhyAddr);
		currentPML4Entry = *currentPML4EntryVirtAddr;
		if(currentPML4Entry)
		{
			PHYADDR_t *currentEPTPageDirectoryPointerTableBasePhysicalAddress;
			currentEPTPageDirectoryPointerTableBasePhysicalAddress = currentPML4Entry & PAGE_DIRECTORY_POINTER_TABLE_ENTRY_MASK;
			if(currentEPTPageDirectoryPointerTableBasePhysicalAddress)
			{
				traverseEPTPageDirectoryPointerTable(vmid, currentEPTPageDirectoryPointerTableBasePhysicalAddress);				
			}
		}
	}

}


//Travers one VM's EPT Page Directory Pointer Table
void traverseEPTPageDirectoryPointerTable(const VMID_t vmid, PHYADDR_t *currentEPTPageDirectoryPointerTableBasePhysicalAddress)
{
	int eptPDPTIndex;
	for(eptPDPTIndex = 0 ; eptPDPTIndex < NUMBER_OF_PAGE_DIRECTORY_POINTER_ENTRY ; eptPDPTIndex++)
	{
		PHYADDR_t *currentEPTPDPTEntryPhyAddr;
		VIRTADDR_t *currentEPTPDPTEntryVirtAddr;
		U64_t currentPDPTEntry;

		currentEPTPDPTEntryPhyAddr = ((U64_t)currentEPTPageDirectoryPointerTableBasePhysicalAddress | ((eptPDPTIndex & 0xFF) << PDPT_SHIFT));
		currentEPTPDPTEntryVirtAddr = getVirtualAddressFromPhysiacalAddress(currentEPTPDPTEntryPhyAddr);
		currentPDPTEntry = *currentEPTPDPTEntryVirtAddr;
		if(currentPDPTEntry)
		{
			PHYADDR_t *currentEPTPageDirectoryBasePhysicalAddress;
			currentEPTPageDirectoryBasePhysicalAddress = currentPDPTEntry & PAGE_DIECTORY_ENTRY_MASK;
			//sec_showDataofPhysicalAddress(currentEPTPageDirectoryBasePhysicalAddress);
			if(currentEPTPageDirectoryBasePhysicalAddress)
			{
				traverseEPTPageDirectory(vmid, currentEPTPageDirectoryBasePhysicalAddress);				
			}

		}
	}	
}

//Traverse one VM's EPT Page Directory
void traverseEPTPageDirectory(const VMID_t vmid, PHYADDR_t *currentEPTPageDirectoryBasePhysicalAddress)
{
	int eptPDIndex;
	for(eptPDIndex = 0 ; eptPDIndex < NUMBER_OF_PAGE_DIRECTORY_ENTRY ; eptPDIndex++)
	{
		PHYADDR_t *currentEPTPDEntryPhyAddr;
		VIRTADDR_t *currentEPTPDEntryVirtAddr;
		U64_t currentPDEntry;

		currentEPTPDEntryPhyAddr = ((U64_t)currentEPTPageDirectoryBasePhysicalAddress | ((eptPDIndex & 0xFF) << PD_SHIFT));
		currentEPTPDEntryVirtAddr = getVirtualAddressFromPhysiacalAddress(currentEPTPDEntryPhyAddr);
		currentPDEntry = *currentEPTPDEntryVirtAddr;
		if(currentPDEntry)
		{
			PHYADDR_t *currentEPTPageTableBasePhysicalAddress;
			currentEPTPageTableBasePhysicalAddress = currentPDEntry & PAGE_TABLE_ENTRY_MASK;
			if(currentEPTPageTableBasePhysicalAddress)
			{
				traverseEPTPageTable(vmid, currentEPTPageTableBasePhysicalAddress);				
			}

		}
	}	
}

//Traverse one VM's EPT Page Table
void traverseEPTPageTable(const VMID_t vmid, PHYADDR_t *currentEPTPageTableBasePhysicalAddress)
{
	int eptPTIndex;
	for(eptPTIndex = 0 ; eptPTIndex < NUMBER_OF_PAGE_DIRECTORY_ENTRY ; eptPTIndex++)
	{
		PHYADDR_t *currentEPTPTEntryPhyAddr;
		VIRTADDR_t *currentEPTPTEntryVirtAddr;
		U64_t currentPTEntry;

		currentEPTPTEntryPhyAddr = ((U64_t)currentEPTPageTableBasePhysicalAddress | ((eptPTIndex & 0xFF) << PT_SHIFT));
		currentEPTPTEntryVirtAddr = getVirtualAddressFromPhysiacalAddress(currentEPTPTEntryPhyAddr);
		currentPTEntry = *currentEPTPTEntryVirtAddr;
		if(currentPTEntry)
		{
			U64_t pageID;
			U8_t originalAttr;
			pageID = currentPTEntry & PAGE_TABLE_MASK;
			pageID >>= PAGE_ID_SHIFT;
			originalAttr = currentPTEntry & (~PAGE_TABLE_MASK);
			updateOwnerVMIDofEPTPage(vmid, pageID, originalAttr);
		}
	}	
}

PHYADDR_t gpaToHPA(const GUEST_PHYADDR_t gpa, PHYADDR_t *pageTableEntryPhysicalAddress)
{
	PHYADDR_t eptPML4BasePhysicalAddress;
	PHYADDR_t eptPML4EntryPhysicalAddress;
	VIRTADDR_t *eptPML4EntryVirtualAddress;
	U64_t	selectedEPTPML4Entry;

	PHYADDR_t eptPDPTBasePhysicalAddress;
	PHYADDR_t eptPDPTEntryPhysicalAddress;
	VIRTADDR_t *eptPDPTEntryVirtualAddress;
	U64_t selectedEPTPDPTEntry;

	PHYADDR_t eptPDBasePhysicalAddress;
	PHYADDR_t eptPDEntryPhysicalAddress;
	VIRTADDR_t *eptPDEntryVirtualAddress;
	U64_t selectedEPTPDEntry;

	PHYADDR_t eptPTBasePhysicalAddress;
	PHYADDR_t eptPTEntryPhysicalAddress;
	VIRTADDR_t *eptPTEntryVirtualAddress;
	U64_t selectedEPTPTEntry;

	PHYADDR_t hpa = 0;
	//Traverse EPT PML
	sec_getEPTPhyAddr(&eptPML4BasePhysicalAddress);
	eptPML4EntryPhysicalAddress = (eptPML4BasePhysicalAddress | (((gpa & EPT_PML4_GPA_MASK) >> EPT_PML4_GPA_SHIFT) << PML4_SHIFT));
	eptPML4EntryVirtualAddress =  mapHPAIntoHVA(eptPML4EntryPhysicalAddress,sizeof(U64_t));//getVirtualAddressFromPhysiacalAddress(eptPML4EntryPhysicalAddress);
	selectedEPTPML4Entry = *eptPML4EntryVirtualAddress;
	unmapHPAFromHVA(eptPML4EntryVirtualAddress,sizeof(U64_t));
	if(!selectedEPTPML4Entry)
	{
		return 0;
	}


	
	//Traverse EPT PDPT
	eptPDPTBasePhysicalAddress = selectedEPTPML4Entry & PAGE_DIRECTORY_POINTER_TABLE_ENTRY_MASK;
	eptPDPTEntryPhysicalAddress = (eptPDPTBasePhysicalAddress | (((gpa & EPT_PDPT_GPA_MASK) >> EPT_PDPT_GPA_SHIFT) << PDPT_SHIFT));
	eptPDPTEntryVirtualAddress =  mapHPAIntoHVA(eptPDPTEntryPhysicalAddress,sizeof(U64_t));//getVirtualAddressFromPhysiacalAddress(eptPDPTEntryPhysicalAddress);
	selectedEPTPDPTEntry = *eptPDPTEntryVirtualAddress;
	unmapHPAFromHVA(eptPDPTEntryVirtualAddress,sizeof(U64_t));
	if(!selectedEPTPDPTEntry)
	{
		return 0;
	}
	
	//Traverse EPT PD
	eptPDBasePhysicalAddress = selectedEPTPDPTEntry & PAGE_DIECTORY_ENTRY_MASK;
	eptPDEntryPhysicalAddress = (eptPDBasePhysicalAddress | (((gpa & EPT_PD_GPA_MASK) >> EPT_PD_GPA_SHIFT) << PD_SHIFT));
	eptPDEntryVirtualAddress =  mapHPAIntoHVA(eptPDEntryPhysicalAddress,sizeof(U64_t));//getVirtualAddressFromPhysiacalAddress(eptPDEntryPhysicalAddress);
	selectedEPTPDEntry = *eptPDEntryVirtualAddress;
	unmapHPAFromHVA(eptPDEntryVirtualAddress,sizeof(U64_t));
	if(!selectedEPTPDEntry)
	{
		return 0;
	}	
	
	//Traverse EPT PT
	eptPTBasePhysicalAddress = selectedEPTPDEntry & PAGE_TABLE_ENTRY_MASK;
	eptPTEntryPhysicalAddress = (eptPTBasePhysicalAddress | (((gpa & EPT_PT_GPA_MASK) >> EPT_PT_GPA_SHIFT) << PT_SHIFT));
	eptPTEntryVirtualAddress =  mapHPAIntoHVA(eptPTEntryPhysicalAddress,sizeof(U64_t));//getVirtualAddressFromPhysiacalAddress(eptPTEntryPhysicalAddress); 
	selectedEPTPTEntry = *eptPTEntryVirtualAddress;
	if(pageTableEntryPhysicalAddress)
	{
		*pageTableEntryPhysicalAddress = eptPTEntryPhysicalAddress;
	}
	unmapHPAFromHVA(eptPTEntryVirtualAddress,sizeof(U64_t));			
	if(!selectedEPTPTEntry)
	{
		return 0;
	}
	
	//Calculate final HPA
	hpa = ((selectedEPTPTEntry & PAGE_MASK) | (gpa & EPT_GPA_MASK));
	
	return hpa;
}

//Update all VM's memory ownership table by scanning entire EPT.
void updateOwnershipTable()
{
	PHYADDR_t *eptPhyAddr;
	U32_t physicalAddressWidth;
	sec_getEPTPhyAddr(&eptPhyAddr);
	//printf("SSLAB : EPT Physical Addr(%d) : %llx\n",sizeof(eptPhyAddr), eptPhyAddr);
	//printf("SSLAB : EPT Traverse...\n");

	//get physical address width through cpuid
	physicalAddressWidth = sec_getPhysicalAddressWidth();
	//printf("SSLAB : physicalAddressWidth : %d\n",physicalAddressWidth);
	//printf("\n----------------------------------------------------------------------\n");
	updateOwnershipTableForOneVM(0, eptPhyAddr);
}


//Update one VM's memory ownership entry
void updateOwnerVMIDofEPTPage(const VMID_t vmid, const U64_t pageID, const U8_t originalAttr)
{
	if(ownershipTable[pageID].ownerVM  != OWNER_VMM)
	{
		//printf("Already allocated Page : %x\n",pageID);
	}
	ownershipTable[pageID].ownerVM = vmid;
	ownershipTable[pageID].originalAttr = originalAttr;
}

//Update one App's memory ownership entry
void updateOwnerAppIDofEPTPage(const VMID_t vmid, const APPID_t appID, const U64_t pageID)
{
	ownershipTable[pageID].ownerVM  = vmid;
	ownershipTable[pageID].ownerApp = appID;
	ownershipTable[pageID].originalAttr = 0x7;
}

void addCurrentProcessToProctectingApp()
{
	PHYADDR_t *currentCR3Value;
	sec_getCR3Value(&currentCR3Value);
	printf("currentCR3Value : %llx\n",currentCR3Value);;
	if(currentCR3Value)
	{
		GUEST_PHYADDR_t cr3PDGPA = (U64_t)currentCR3Value & CR3_MASK;
		APPID_t appIDToSwitch = cr3PDGPA;
		targetApplicationPageTableBasePhysicalAddress = appIDToSwitch;
		printf("SSLAB : Target application ID initialized (%llx)\n",appIDToSwitch);

	}	
}

// CR3Change handler
void cr3ChangedHandler()
{
	U64_t cr3ValueToWrite;
	cr3ValueToWrite = sec_getCR3ValueToWrite();
	if(cr3ValueToWrite)
	{
		GUEST_PHYADDR_t cr3PDGPA = cr3ValueToWrite & CR3_MASK;

		APPID_t appIDToSwitch = cr3PDGPA;
		if(appIDToSwitch == targetApplicationPageTableBasePhysicalAddress)
		{
			//Start scanning the page table of application to protect
			traverseApplicationPageTable(0, appIDToSwitch, cr3PDGPA);
		}	

	}

}

//Traverse total guest pages
void traverseApplicationPageTable(const VMID_t vmID, const APPID_t appID, const GUEST_PHYADDR_t cr3PDGPA)
{
	traverseGuestPD(vmID, appID, cr3PDGPA);
}

//Traverse guest page directories
void traverseGuestPD(const VMID_t vmID, const APPID_t appID, const GUEST_PHYADDR_t cr3PDGPA)
{
	int pdIndex;
	for(pdIndex = 0 ; pdIndex < PD_INDEX_MAX ; pdIndex++)
	{
		GUEST_PHYADDR_t currentPDEntryGPA = 0;
		PHYADDR_t currentPDEntryHPA = 0;
		VIRTADDR_t *currentPDEntryHVA = 0;
		U64_t currentPDEntry;

		currentPDEntryGPA = (cr3PDGPA | (pdIndex << PD_SHIFT));
		currentPDEntryHPA = gpaToHPA(currentPDEntryGPA,0);
		currentPDEntryHVA = mapHPAIntoHVA(currentPDEntryHPA,sizeof(U64_t));
		if(currentPDEntryHVA)
		{
			currentPDEntry = *currentPDEntryHVA;
			traversePT(vmID,appID,currentPDEntry);			
		}
		unmapHPAFromHVA(currentPDEntryHVA,sizeof(U64_t));	
		
	}
}

//Traverse guest page tables
void traversePT(const VMID_t vmID, const APPID_t appID, const U64_t currentPDEntry)
{
	int ptIndex;
	for(ptIndex = 0 ; ptIndex < PT_INDEX_MAX ; ptIndex++)
	{
		GUEST_PHYADDR_t currentPTEntryGPA = 0;
		PHYADDR_t currentPTEntryHPA = 0;
		VIRTADDR_t *currentPTEntryHVA = 0;
		U64_t currentPTEntry;
		

		currentPTEntryGPA = ((currentPDEntry &  GUEST_PAGE_MASK) | (ptIndex << PT_SHIFT));

		if(gpaToHPACache[(currentPTEntryGPA >> PAGE_ID_SHIFT)])//Already owned guest page
		{
			continue;
		}
		currentPTEntryHPA = gpaToHPA(currentPTEntryGPA, 0);
		if(currentPTEntryHPA)
		{
			gpaToHPACache[(currentPTEntryGPA >> PAGE_ID_SHIFT)] = currentPTEntryHPA;
			currentPTEntryHVA = mapHPAIntoHVA(currentPTEntryHPA,sizeof(U64_t));
			if(currentPTEntryHVA)
			{
				currentPTEntry = *currentPTEntryHVA;
				updateOwnerAppIDofGuestPage(vmID, appID, currentPTEntry);
			}
			unmapHPAFromHVA(currentPTEntryHVA,sizeof(U64_t));
		}

	}
}


void updateOwnerAppIDofGuestPage(const VMID_t vmID, const APPID_t appID, const U64_t currentPTEntry)
{
	U64_t eptEntryOfCurrentPage;
	PHYADDR_t eptEntryPhysicalAddress;
	eptEntryOfCurrentPage = getEPTEntryIndexOfGuestPage(currentPTEntry, &eptEntryPhysicalAddress);
	if(eptEntryOfCurrentPage)
	{
		U64_t *eptEntryVirtualAddress;
		U64_t eptEntryValue;
		eptEntryVirtualAddress = mapHPAIntoHVA(eptEntryPhysicalAddress,sizeof(U64_t));
		if(eptEntryVirtualAddress)
		{
			U64_t currentPageID;
			eptEntryValue = *eptEntryVirtualAddress;
			currentPageID = eptEntryValue >> PAGE_ID_SHIFT;
			protectingPageTable[currentPageID] = appID;
			printf("(Before)currentPageID : %llx\n",currentPageID);
			printf("(Before)eptEntryValue : %llx\n",eptEntryValue);
			eptEntryValue &= (~0x7);
			*eptEntryVirtualAddress = eptEntryValue;
			printf("(After)eptEntryValue : %llx\n",eptEntryValue);
			unmapHPAFromHVA(eptEntryVirtualAddress,sizeof(U64_t));
			updateOwnerAppIDofEPTPage(vmID,appID, eptEntryOfCurrentPage);		
		}		
	}


}

U64_t getEPTEntryIndexOfGuestPage(const U64_t guestPTEntry, U64_t *eptEntryValue)
{
	GUEST_PHYADDR_t currentGuestPageGPA;
	U64_t eptpBaseAddr;
	U64_t pageID;
	currentGuestPageGPA = guestPTEntry & GUEST_PAGE_MASK;

	pageID = gpaToHPA(currentGuestPageGPA, eptEntryValue);
	pageID >>= PAGE_ID_SHIFT;

	return pageID;
}


BOOL_t pageIsProtecting(U64_t pageID)
{
	return protectingPageTable[pageID];
}