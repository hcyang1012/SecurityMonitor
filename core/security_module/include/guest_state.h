/**	\file guest_state.h
*	\brief High-level facilities to get guest VM's information.
*
*	\def CR3_MASK
*	\brief
*	A mask which is used to extract the base physical address of page table from CR3 control register value\n
*	See __Intel's architecture manual VOL3, Chapter 2.5 : Control Registers__.
*
*	\def EPTP_MASK
*	\brief
*	A mask which is used to extract the base physical address of page table from EPTP field in a VMCS\n
*	See __Intel's architecture manual VOL3, Chapter 28.2.2 : EPT Translation Mechanism__.
*
*	\fn GPA_t get_page_table_base_GPA() {get_page_table_base_GPA}
*	\brief Get the start address of page table.
*	\warning The returned address is __guest physical address (GPA)__.
*
*	\fn HPA_t get_ept_base_HPA()
*	\brief Get the start address of extended page table.
*	\warning The returned address is __host physical address (HPA)__.
*
*	\fn void traverseGuestPages(const VMID_t vmid, const APPID_t appID, const GPA_t startGPAofPageTable, void* (*do_something)(const VMID_t vmID, const APPID_t appID, GPA_t gpa))
*	\brief Traverse all pages that start from startGPAofPageTable and do something for each page.
*	
*	Detail.{#traverseGuestPages}
*	This function does two things:
*	1. This function finds all pages of a process whose page table starts from the input *startGPAofPageTable*.
*	2. For each page, if needed, call a function named *do_something* passing VMID, APPID, and GPA of a page as a parameter. What the function does is depends on the caller.
*	\note In this implementation, the security monitor is assuming that the guest is using IA32e paging mode.
*
*
*/
#ifndef __GUEST_STATE_H__
#define __GUEST_STATE_H__
#include <monitor_types.h>
#include <vmcs.h>

#define CR3_MASK (~((U64_t)0xFFF))
#define EPTP_MASK (~((U64_t)0xFFF))


GPA_t get_page_table_base_GPA();
HPA_t get_ept_base_HPA();

void traverseGuestPages(const VMID_t vmid, 				/**< [in] VM ID to be passed to the do_something() */
						const APPID_t appID,			/**< [in] APP ID to be passed to the do_something() */
						const GPA_t startGPAofPageTable, /**< [in] start GPA of a process's page table */
						void* (*do_something)(const VMID_t vmID, const APPID_t appID, GPA_t gpa) /**< [in] a function pointer that will be invoked for each page */
						);

#endif