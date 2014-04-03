/*!	\file memory_ownership_table.h
	\brief Definition of data structrues & constants for memory ownership table.\n

	Details.

	This file is used to define some data structrues for memory ownership table.\n
	The memory ownership table is a table which is maintained by the security monitor.\n
	The purpose of this table is to associate each machine physical frame to an owner application.\n
	Each entry in this table has some attributes: frame index, owner VM ID, owner application ID, page state, original permission, original frame address, and integrity measurement\n
	1. Frame index:\n
	The security monitor protects an application's pages in machine-physical frame level. So this attributes indicates the index of the frame.\n
	Actually, this attribute is not presented in the entry; It is actually represented as a array index.\n
	2. Owner VM ID\n
	This attributes indicates the virtual machine that the owner application of a page is running on. In this implementation, __the base address of EPT(EPTP value in VMCS)__ is used for VM ID.	3. Owner application ID\n
	3. Owner Application ID\n
	This attributes indicates the owner application of a page. In this implementation, __the base address of guest page table of the application__ is used for application ID.
	\note	Therefore, each owner is identifed as VMID:APPID
	4. Page state\n
	Each page has its own state : unprotected, closed, opened, partialy-opened, and encrypted.
		- Unprotected : The security monitor doesn't care about the pages in this state. It is natural state.
		- Closed : When an application requests protection to the security monitor (via hypercall), all pages that the application owns are set to be in this state.
		In this state, the EPT entry of each page is set to No-read, No-write and No-execute.
		Therefore, whatever guest VM tries to do on pages in this state, EPT page violation will be occurred.
		Once an EPT violation is occurred, the security monitor changes a page's state from this state to one of the other states depending of the guest context.\n
		\note to recover the original attribute whan the page state is transited to opened state, the original attribute is saved in the original permission attribute of the entry.
		- Opened : A page can be in this state only when the owner application tries to use the page. In this state, the original permission of the corresponding EPT entry is recovered for the application to use the page freely.
		- Partialy-opened : Sometimes, the operating system kernel or hardware devices have to use the contents of memory that the protected application owns as a plaintext state. This state exists for this purpose.
		In this state, the security monitor allocates a new page that contains data that the kernel or device driver really need only. Then, the EPT entry is changed to point this newly allocated page.\n
		> Note: For recovery(that is, changing to other states), the original page frame address (this address is HPA) is saved in the original frame address attribute of the entry.
		- Encrypted : This state is exposed to other guest components except the owner application. In this state, the contents of the page is encrypted. That is, other application or operating system kernel can only read the encrypted data of the page.\n
		\note Once a page is set to be in this state, the integrity measurement value of the encrypted page contents is maintained in the integrity measurement attribute of the entry. This is to check whether the integrity of the page is maintained during the kernel or other application is running.

	\enum page_state_t
	\brief A enumeration for describing page state.

	\enum  ownerAppID_t
	\brief A enumeration for describing owner application of a page

	\enum ownerVMID_t	
	\brief A enumeration for describing owner virtual machine of a page

	\struct memory_ownership_table_entry_t
	\brief A structure-type definition for one memory ownership table entry.

	\def PAGE_SIZE
	\brief A constant that describes the size of on page frame.

	\def MAX_MEMORY_SIZE
	\brief A constant that describes the size of on total machine-physical memory.

	> In this implementation, this constant is fixed to 8 GB.

	\def NUMBER_OF_MEMORY_OWNERSHIP_TABLE_ENTRY
	\brief A constant that describes the number of entries that the memory ownership table has to contain.

	\def HPA_TO_INDEX_SHIFT
	\brief A constant which is used to change HPA to index of the memory ownership table.

	\fn	void protectCurrentApplication()
	\brief Start to protect of currently running process's pages.

	Details.

	When this function is invoked (by the hypercall handler), the security monitor starts to protect the pages of application that the issued the hypercall.\n
	For protection, this function does followings:
	1.  Allocate VM ID and application ID.
		The security monitor identifies the application by ID of virtual machine that running the application and the appplication's ID.\n
		So to start protection, this function first allocates new VMID and APP ID of the currently running process.\n
		\note In this implementation, VM ID is equal to start address (HPA) of EPT, and APP ID is equal to the start address (GPA) of the page table.
	2.  Traverse the process's all pages.
		Next step is to traverse all guest pages that the application owns. As traversing each page, this function translates the GPA of page into HPA.
		This translated HPA is used for index of memory ownership table. Once the HPA of the page is identified, the following fields of corresponding memory ownership table entry are set:
			- The owner VM ID and owner application ID is set to the values that are allocated from step 1.
			- The page state is set to *CLOSED* state.
			- The original permission attribute is the same with the attributes of the EPT entry(RWX permission field in the EPT).
			- The original page address is set to the translated HPA.
		After the entry is initialized correctly, the permission in EPT entry is set cleared. That is, if anyone tries to access the page, the EPT violation will be occurred.


	\fn void allocateNewApplicationIdentifiers(VMID_t *new_VMID, APPID_t *new_APPID)
	\brief Allocate new VM ID & application ID

	Details.

	This function allocates new ID of virtual machine and application.
	Actually, the new virtual machine ID is already generated because there may be another protected application. In this case, the same VMID will be returned instead of allocating new VMID.
	However, application ID always should be unique. Therefore, this function always allocates new application ID.
	\note In this implementation, VM ID is equal to start address (HPA) of EPT, and APP ID is equal to the start address (GPA) of the page table. Therefore, above conditions will be naturally satisfied.


	\fn VMID_t allocateNewVMID()
	\brief This function returns currently running VM's ID. If it does not exist, this function allocates new one and returns it.
	\return This function returns currently running VM's ID. If it does not exist, this function allocates new one and returns it.

	Details.

	\note In this implementation, VM ID is equal to start address of VM's EPT. Therefore, this function extracts it from EPTP field in the VMCS.	

	\fn VMID_t allocateNewAppID()
	\brief This function returns currently running process's ID. For new process, this function allocates new one and returns it.
	\return This function returns currently running process's ID. For new process, this function allocates new one and returns it.

	Details.

	\note In this implementation, the APP ID is equal to start GPA of VM's page table. Therefore, this function extracts it from guest's current CR3 value.

	\fn void* closePage(const VMID_t vmID, const APPID_t appID, GPA_t gpa)
	\brief Set the a frame to closed state

	Details.

	This function changes a page frame's status to CLOSED status. By doing this, it also clears all access permissions in the EPT entry to prohibit all accesses to the closed page frame. 
	
	\fn void changePageStatus(const VMID_t vmID, const APPID_t appID, const GPA_t gpa, const enum page_state_t state)
	\brief change a page's status

	Details.

	This function is used to change state of a page frame

	\fn U64_t getMemoryOwnershipTableIndex(const HPA_t hpa)
	\brief A function which is used to change HPA to index of the memory ownership table.

	\fn struct memory_ownership_table_entry_t getMemoryOwnershipTableEntry(U64_t index)
	\brief A function which is used to get an entry of memory ownership table
*/
#ifndef __MEMORY_OWNERSHIP_TABLE_H__
#define __MEMORY_OWNERSHIP_TABLE_H__

#include <monitor_types.h>

enum page_state_t{
	UNPROTECTED = 0,	/**< unprotectd state */
	CLOSED,				/**< closed state */
	OPENED,				/**< opened state */
	PARTIAL,			/**< partialy-opened state */
	ENCRYPTED			/**< encrypted state */
};

enum ownerAppID_t{
	KERNEL = 0	/**< The owner is the guest kernel */
};

enum ownerVMID_t
{
	VMM = 0		/**< The owner is virtual machine monitor */
};

struct memory_ownership_table_entry_t
{
	VMID_t owner_VM;				/**< ID of VM that the owner application of the page is running on */
	APPID_t owner_APP;				/**< ID of the owner application of the page */
	enum page_state_t state;		/**< Page state */
	U8_t original_permission;		/**< Original permission of the page in the corresponding EPT entry */
	HPA_t original_page_address;	/**< The back-upped original page address storage for partialy-opened state */
};

#define PAGE_SIZE 4096
#define MAX_MEMORY_SIZE ((U64_t)17*1024*1024*1024)
#define NUMBER_OF_MEMORY_OWNERSHIP_TABLE_ENTRY (MAX_MEMORY_SIZE / PAGE_SIZE)
#define HPA_TO_INDEX_SHIFT 12


void protectCurrentApplication();
void allocateNewApplicationIdentifiers(	VMID_t *new_VMID, /**< [out] The newly allocated VM ID will be stored in this parameter. */
										APPID_t *new_APPID/**< [out] The newly allocated application ID will be stored in this parameter. */
										);

VMID_t allocateNewVMID();
APPID_t allocateNewAppID();

void* closePage(const VMID_t vmid, /**< [in] owner VMID of the page to close */
				const VMID_t appID, /**< [in] owner app ID of the page to close */
				GPA_t gpa /**< [in] GPA of page frame to close*/);
int changePageStatus(	const VMID_t vmid, /**< [in] owner VMID of the page to close */
						const VMID_t appID, /**< [in] owner app ID of the page to close */
						const GPA_t gpa,	/**< [in] GPA of page frame to change */
						const enum page_state_t state /**< [in] state value to change */
						);

U64_t getMemoryOwnershipTableIndex(const HPA_t hpa /*<< [in] HPA to change */);
struct memory_ownership_table_entry_t getMemoryOwnershipTableEntry(const U64_t index /*<< [in] index to an entry of memory ownership table */);

#endif