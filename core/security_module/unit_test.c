/*!	\file unit_test.c
	\brief This file is used for unit test of functions in the security monitor.
*/

#include <monitor_util.h>
#include <guest_state.h>
#include <monitor_types.h>
#include <unit_test.h>
#include <guest_IA32ePage.h>
#ifdef CONFIG_BITVISOR
	#include <printf.h>
#endif

void test_start()
{
	int result;
	if(get_page_table_base_GPA())
	{
		printf("Unit test : %llx\n",gvaToGPA(0xffffffff810c7590,get_page_table_base_GPA()));
	// 	printf("Unit test start\n");

	// 	printf("Test 'gpaToHPA()'\n");
	// 	result = test_gpaToHPA();
	// 	printf("Test 'gpaToHPA()' end; result : %s\n",result?"success":"fail");		

	// 	printf("Test 'test_logicalCPUStatus()'\n");
	// 	result = test_logicalCPUStatus();
	// 	printf("Test 'test_logicalCPUStatus()' end; result : %s\n",result?"success":"fail");				

	// 	printf("Test 'test_traverseGuestPages()'\n");
	// 	result = test_traverseGuestPages();

	// 	printf("Test 'test_traverseGuestPages()' end; result : %s\n",result?"success":"fail");
	// 
	}

}

int test_gpaToHPA()
{
	GPA_t input;
	HPA_t output;
	HPA_t expected;
	input = (GPA_t)get_page_table_base_GPA();
	input |= 0xCDE;
	output = gpaToHPA(input, 0);
#ifdef CONFIG_BITVISOR
	expected = input;
	return expected == output;
#else
	expected = 0;
	return 0;
#endif
}

int test_traverseGuestPages()
{
	GPA_t cr3GPA;
	cr3GPA = get_page_table_base_GPA();
	printf("Guest CR3 : %llx\n",cr3GPA);
	printf("EPT_PML4_ENTRY_MASK : %llx\n",EPT_PML4_ENTRY_MASK);
	//traverseGuestPages(cr3GPA, 0);	

	return 1;
}

int test_logicalCPUStatus()
{
	U64_t value_IA32_EFER = 0xFD;
	value_IA32_EFER = monitor_vmcs_read(FIELD_ENCODING_IA32_EFER);
	printf("IA32_EFER value : %llx\n",value_IA32_EFER);
	return 1;
}