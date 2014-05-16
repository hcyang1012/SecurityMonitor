/**	\file eventHandler.c
	\brief Catching & handling of memory-related events.
 *
 */

#include <eventHandler.h>
#include <monitor_types.h>
#include <guest_state.h>
#include <memory_ownership_table.h>
#include <unit_test.h>
#include <monitor_util.h>
#include <guest_IA32ePage.h>

 #ifdef CONFIG_BITVISOR
 int protecting = 0;
 #endif
void hypercall_handler()
{
	GPA_t pagetable_start_GPA;
	pagetable_start_GPA = get_page_table_base_GPA();

	if(pagetable_start_GPA)
	{
		printf("Start protection\n");
		printf("Current CR3 value : %llx\n",get_page_table_base_GPA());
		//protecting = 1;
		protectCurrentApplication();	
		printf("Start end\n");
		//test_start();
	}
}
void interrupt_handler()
{
	int isUserToKernel = 0;
	U64_t csSelector;
	char privilegeLevel;
	csSelector = monitor_vmcs_read(FIELD_ENCODING_GUEST_CS_SELECTOR);
	privilegeLevel = csSelector & 0x3;
	if(privilegeLevel > 0)
	{
		isUserToKernel = 1;
	}

	if(isUserToKernel){
		struct protected_application_t *currentProtectedApplication;
		currentProtectedApplication = getCurrentProtectedApplication();
		if(currentProtectedApplication)
		{
			GPA_t cr3GPA;
			GPA_t gpa;
			save_guest_status(&(currentProtectedApplication->guest_sensitive_stats));	
			currentProtectedApplication->guest_sensitive_stats.RIP = monitor_vmcs_read(FIELD_ENCODING_GUEST_RIP);
			
			gpa = gvaToGPA(currentProtectedApplication->guest_sensitive_stats.RIP, get_page_table_base_GPA());
			currentProtectedApplication->guest_sensitive_stats.RIP_GPA = gpa;
			printf("\n---------------------------------------------------\n");
			debug();
			printf("User to kernel due to interrupt : %llx(%llx)\n",currentProtectedApplication->guest_sensitive_stats.RIP_GPA, currentProtectedApplication->guest_sensitive_stats.RIP);
			printf("Close %llx - %llx\n",currentProtectedApplication->guest_sensitive_stats.RIP,gpa);
			printf("---------------------------------------------------\n");
			//clear_guest_status();
			cr3GPA = get_page_table_base_GPA();
			traverseGuestPages(currentProtectedApplication->owner_VM, currentProtectedApplication->owner_APP, cr3GPA, closePage);
			setCurrentProtectedApplication(NULL);
			//openSystemCallHandler();
		}
	}

}