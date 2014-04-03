/*!	\file guest_IA32ePage.h
	\brief functions and constants to support IA32e paging mode of guest VM

	Details. 

	This file exists to support traversing of pages in IA32e mode.\n
	In IA32e mode, MMU uses 4 level paging: PML4, Page Directory Pointer(PDPT), Page Directory(PD), and Page Table(PT).\n 
	For details about how to traverse each level, see function for each level and __Intel Architecture Manual Chapter 4.2 : IA-32E Paging__.

	\def EPT_PML4_ENTRY_MASK
	\brief A mask that is used to extract base HPA of EPT PDP from EPT PML4 entry.

	Details.

	To get the HPA of a EPT PDP table entry, following information should be combined:
	1. __Bits 51:12 are from the PML4E__.
	2. Bits 11:3 are bits 38:30 of the linear address.
	3. Bits 2:0 are all 0.

	Therefore, bits 51:12 should be extracted from the PML4 entry. This macro is used for this purpose.


	\def EPT_PDP_ENTRY_MASK
	\brief A mask that is used to extract base HPA of EPT PDP from EPT PDP table entry.

	Details.

	To get the HPA of a EPT PD table entry, following information should be combined:
	1. __Bits 51:12 are from the PDP table entry__.
	2. Bits 11:3 are bits 29:21 of the linear address.
	3. Bits 2:0 are all 0.

	Therefore, bits 51:12 should be extracted from the PDP entry. This macro is used for this purpose.

	\def EPT_PD_ENTRY_MASK
	\brief A mask that is used to extract base HPA of EPT PT from EPT PD table entry.

	Details.

	To get the HPA of a EPT page table entry, following information should be combined:
	1. __Bits 51:12 are from the PD table entry__.
	2. Bits 11:3 are bits 29:21 of the linear address.
	3. Bits 2:0 are all 0.

	Therefore, bits 51:12 should be extracted from the PD entry. This macro is used for this purpose.	


	\def EPT_PT_ENTRY_MASK
	\brief A mask that is used to extract base HPA of page from EPT page table entry.

	Details.

	To get the HPA of a page, following information should be combined:
	1. __Bits 51:12 are from the page table entry__.
	2. Bits 11:3 are bits 29:21 of the linear address.
	3. Bits 2:0 are all 0.

	Therefore, bits 51:12 should be extracted from the page table entry. This macro is used for this purpose.		

	\def MAX_GUEST_PML4_INDEX
	\brief The maximum number of guest PML4 entries

	Details.

	This macro indicates the maximum number of guest PML4 entries. Actually, PML4 entry index is identified by bits 47:39 bits of the GPA, which is 9 bits length. 
	Therefore, the actual value of this macro is 0x1FF + 1, or 512.

	\def MAX_GUEST_PDP_INDEX
	\brief The maximum number of guest PDP table entries

	Details.

	This macro indicates the maximum number of guest PDP table entries. Actually, PDP entry index is identified by bits 38:30 bits of the GPA, which is 9 bits length. 
	Therefore, the actual value of this macro is 0x1FF + 1, or 512.

	\def MAX_GUEST_PD_INDEX
	\brief The maximum number of guest page directory (PD) entries

	Details.

	This macro indicates the maximum number of guest PD table entries. Actually, PD table entry index is identified by bits 29:21 bits of the GPA, which is 9 bits length. 
	Therefore, the actual value of this macro is 0x1FF + 1, or 512.		

	\def MAX_GUEST_PT_INDEX
	\brief The maximum number of guest page table (PT) entries

	Details.

	This macro indicates the maximum number of guest page table entries. Actually, page table entry index is identified by bits 29:21 bits of the GPA, which is 9 bits length. 
	Therefore, the actual value of this macro is 0x1FF + 1, or 512.		

	\def GUEST_PML4_INDEX_SHIFT
	\brief The left-shift value for PML4 entry.

	Details.

	The PML4 entry index is in bits 11:3 of the address of the PML4 entry HPA. Therefore, the index value should be 3 left-shifted. This macro indicates the needed shift size.

	\def GUEST_PDP_INDEX_SHIFT
	\brief The left-shift value for PDP table entry.

	Details.

	The PDP table entry index is in bits 11:3 of the address of the PDP table entry HPA. Therefore, the index value should be 3 left-shifted. This macro indicates the needed shift size.


	\def GUEST_PD_INDEX_SHIFT
	\brief The left-shift value for PD table entry.

	Details.

	The PD table entry index is in bits 11:3 of the address of the PD table entry HPA. Therefore, the index value should be 3 left-shifted. This macro indicates the needed shift size.

	\def GUEST_PT_INDEX_SHIFT
	\brief The left-shift value for page table entry.

	Details.

	The page table entry index is in bits 11:3 of the address of the page table entry HPA. Therefore, the index value should be 3 left-shifted. This macro indicates the needed shift size.

	\fn void traverseIA32ePages(const VMID_t vmid, const APPID_t appID, const GPA_t startGPAofPageTable, void* (*do_something)(const VMID_t vmID, const APPID_t appID, GPA_t gpa))
	\brief Traverse all pages based on IA32e paging mode. It traverse at PML4 level.
	
	Details.	

	This function travers a process's all pages based on IA32e paging mode.
	For the purpose of input parameters, see the [traverseGuestPages](@ref traverseGuestPages) function.\n
	This function especially exists for traversing PML4 page table entries. The GPA of each PML4 entry is calculated as followings:
	1. Bits 51:12 are from CR3. ( This value is from the parameter,startGPAofPageTable, and has been extracted by the function [get_page_table_base_GPA](@ref get_page_table_base_GPA) ).
	2. Bits 11:3 are bit 47:39 of the GPA.
	3. Bits 2:0 are all 0

	\fn void traverseIA32ePDPT(const VMID_t vmid, const APPID_t appID, const GPA_t startGPAofPDPT, void* (*do_something)(const VMID_t vmID, const APPID_t appID, GPA_t gpa))
	\brief Traverse all pages based on IA32e paging mode. It traverse at page directory pointer (PDP) level.
	
	Details.	

	This function travers a process's all pages based on IA32e paging mode.
	For the purpose of input parameters, see the [traverseGuestPages](@ref traverseGuestPages) function.\n
	This function especially exists for traversing PDP table entries. The GPA of each PDP table entry is calculated as followings:
	1. Bits 51:12 are from PML4 entry. ( This value is from the parameter,startGPAofPDPT).
	2. Bits 11:3 are bit 38:30 of the GPA.
	3. Bits 2:0 are all 0

	\fn void traverseIA32ePDT(const VMID_t vmid, const APPID_t appID, const GPA_t startGPAofPDT, void* (*do_something)(const VMID_t vmID, const APPID_t appID, GPA_t gpa))
	\brief Traverse all pages based on IA32e paging mode. It traverse at page directory (PD) level.
	
	Details.	

	This function travers a process's all pages based on IA32e paging mode.
	For the purpose of input parameters, see the [traverseGuestPages](@ref traverseGuestPages) function.\n
	This function especially exists for traversing PD table entries. The GPA of each PD table entry is calculated as followings:
	1. Bits 51:12 are from PDP table entry. ( This value is from the parameter,startGPAofPDT).
	2. Bits 11:3 are bit 29:21 of the GPA.
	3. Bits 2:0 are all 0

	\fn void traverseIA32ePT(const VMID_t vmid, const APPID_t appID, const GPA_t startGPAofPT, void* (*do_something)(const VMID_t vmID, const APPID_t appID, GPA_t gpa))
	\brief Traverse all pages based on IA32e paging mode. It traverse at page table (PT) level.
	
	Details.	

	This function travers a process's all pages based on IA32e paging mode.
	For the purpose of input parameters, see the [traverseGuestPages](@ref traverseGuestPages) function.\n
	This function especially exists for traversing page table entries. The GPA of each page table entry is calculated as followings:
	1. Bits 51:12 are from PD table entry. ( This value is from the parameter,startGPAofPT).
	2. Bits 11:3 are bit 20:12 of the GPA.
	3. Bits 2:0 are all 0

	After traverse, this function also calls ,if exist, the function do_something. This called function will do something using the input parameter, GPA of each page.

*/
#ifndef __IA32E_PAGE_H__
#define __IA32E_PAGE_H__

#include <monitor_types.h>


#define EPT_PML4_ENTRY_MASK		((U64_t)(~0xFFF))
#define EPT_PDP_ENTRY_MASK		((U64_t)(~0xFFF))
#define EPT_PD_ENTRY_MASK		((U64_t)(~0xFFF))
#define EPT_PT_ENTRY_MASK		((U64_t)(~0xFFF))
	
#define MAX_GUEST_PML4_INDEX	(0x1FF + 1)
#define MAX_GUEST_PDP_INDEX		(0x1FF + 1)
#define MAX_GUEST_PD_INDEX		(0x1FF + 1)
#define MAX_GUEST_PT_INDEX		(0x1FF + 1)

#define GUEST_PML4_INDEX_SHIFT	(3)
#define GUEST_PDP_INDEX_SHIFT	(3)
#define GUEST_PD_INDEX_SHIFT	(3)
#define GUEST_PT_INDEX_SHIFT	(3)



inline void traverseIA32ePages(const VMID_t vmid, 				/**< [in] VM ID to be passed to the do_something() */
						const APPID_t appID,			/**< [in] APP ID to be passed to the do_something() */
						const GPA_t startGPAofPageTable, /**< [in] start GPA of a process's PML4 table */
						void* (*do_something)(const VMID_t vmID, const APPID_t appID, GPA_t gpa) /**< [in] a function pointer that will be invoked for each page */
						);

inline void traverseIA32ePDPT(const VMID_t vmid, 				/**< [in] VM ID to be passed to the do_something() */
						const APPID_t appID,			/**< [in] APP ID to be passed to the do_something() */
						const GPA_t startGPAofPDPT, /**< [in] start GPA of a PDP table */
						void* (*do_something)(const VMID_t vmID, const APPID_t appID, GPA_t gpa) /**< [in] a function pointer that will be invoked for each page */
						);

inline void traverseIA32ePDT(const VMID_t vmid, 				/**< [in] VM ID to be passed to the do_something() */
						const APPID_t appID,			/**< [in] APP ID to be passed to the do_something() */
						const GPA_t startGPAofPDT, /**< [in] start GPA of a process's PD table */
						void* (*do_something)(const VMID_t vmID, const APPID_t appID, GPA_t gpa) /**< [in] a function pointer that will be invoked for each page */
						);

inline void traverseIA32ePT(const VMID_t vmid, 				/**< [in] VM ID to be passed to the do_something() */
						const APPID_t appID,			/**< [in] APP ID to be passed to the do_something() */
						const GPA_t startGPAofPT, /**< [in] start GPA of a process's page table */
						void* (*do_something)(const VMID_t vmID, const APPID_t appID, GPA_t gpa) /**< [in] a function pointer that will be invoked for each page */
						);
#endif