#ifndef __MEMORY_OWNERSHIP_TABLE_H__
#define __MEMORY_OWNERSHIP_TABLE_H__

#include <sec_types.h>

#define PHY_MEM_MAX_SIZE	((U64_t)8*1024*1024*1024)	//8GB
#define PAGE_SIZE			((U64_t)1024*4)				//4KB
#define MAX_NUMBER_OF_PAGES	((U64_t)PHY_MEM_MAX_SIZE / PAGE_SIZE)
#define NUMBER_OF_PML4_ENTRY						(1 << 9)
#define NUMBER_OF_PAGE_DIRECTORY_POINTER_ENTRY		(1 << 9)
#define NUMBER_OF_PAGE_DIRECTORY_ENTRY				(1 << 9)
#define ADDRESS_LENGTH_IN_BITS		(32)

#define PML4_SHIFT		(3)
#define PDPT_SHIFT		(3)
#define PD_SHIFT		(3)
#define PT_SHIFT		(3)


#define PAGE_ID_SHIFT		(12)

#define EPT_PML4_GPA_MASK	0xFF8000000000
#define EPT_PDPT_GPA_MASK	0x7FC0000000
#define EPT_PD_GPA_MASK 	0x3FE00000
#define EPT_PT_GPA_MASK 	0x1FF000
#define EPT_GPA_MASK 		0xFFF

#define EPT_PML4_GPA_SHIFT	(39)
#define EPT_PDPT_GPA_SHIFT	(30)
#define EPT_PD_GPA_SHIFT 	(21)
#define EPT_PT_GPA_SHIFT 	(12)

#define PAGE_DIRECTORY_POINTER_TABLE_ENTRY_MASK		0xFFFFFFFFFF000
#define PAGE_DIECTORY_ENTRY_MASK					0xFFFFFFFFFF000
#define PAGE_TABLE_ENTRY_MASK						0xFFFFFFFFFF000
#define PAGE_TABLE_MASK								0xFFFFFFFFFF000
#define PAGE_MASK 									0xFFFFFFFFFF000


#define GUEST_PAGE_MASK								0xFFFFF000

#define OWNER_VMM		0
#define OWNER_KERNEL	0

#define PD_INDEX_MAX	0x3FF
#define PT_INDEX_MAX	0x3FF

typedef struct 
{
	VMID_t ownerVM;
	APPID_t ownerApp;
	U8_t originalAttr;
} OWNERSHIPTABLE_t;


void initOwnershipTable();
//Traverse one VM's EPT & update memory ownership table
void updateOwnershipTableForOneVM(const VMID_t vmid, PHYADDR_t *eptPhyBaseAddr);
//Traverse EPT PML4
void traverseEPTPML4(const VMID_t vmid, PHYADDR_t *eptPhyBaseAddr);
//Travers one VM's EPT Page Directory Pointer Table
void traverseEPTPageDirectoryPointerTable(const VMID_t vmid, PHYADDR_t *currentEPTPageDirectoryPointerTableBasePhysicalAddress);
//Traverse one VM's EPT Page Directory
void traverseEPTPageDirectory(const VMID_t vmid, PHYADDR_t *currentEPTPageDirectoryBasePhysicalAddress);
//Traverse one VM's EPT Page Table
void traverseEPTPageTable(const VMID_t vmid, PHYADDR_t *currentEPTPageTableBasePhysicalAddress);
PHYADDR_t gpaToHPA(const GUEST_PHYADDR_t gpa, U64_t *pageTableEntry);
//Update all VM's memory ownership table by scanning entire EPT.
void updateOwnershipTable();
//Update one VM's memory ownership entry
void updateOwnerVMIDofEPTPage(const VMID_t vmid, const U64_t pageID, const U8_t originalAttr);
//Update one App's memory ownership entry
void updateOwnerAppIDofEPTPage(const VMID_t vmid, const APPID_t appID, const U64_t pageID);
// CR3Change handler
void cr3ChangedHandler();
//Traverse total guest pages
void traverseApplicationPageTable(const VMID_t vmID, const APPID_t appID, const GUEST_PHYADDR_t cr3PDGPA);
//Traverse guest page directories
void traverseGuestPD(const VMID_t vmID, const APPID_t appID, const GUEST_PHYADDR_t cr3PDGPA);
//Traverse guest page tables
void traversePT(const VMID_t vmID, const APPID_t appID, const U64_t currentPDEntry);
void updateOwnerAppIDofGuestPage(const VMID_t vmID, const APPID_t appID, const U64_t currentPTEntry);
U64_t getEPTEntryIndexOfGuestPage(const U64_t guestPTEntry, U64_t *eptEntryValue);
//Check if we want to protect currently running application.
BOOL_t applicationIsProtectionTarget(const APPID_t appID);
BOOL_t pageIsProtecting(U64_t pageID);
void addCurrentProcessToProctectingApp();
#endif