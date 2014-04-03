/*! \file unit_test.h
	\brief This file declares functions for unit test of functions in the security module.

	\fn void test_start();
	\brief A function to start unit test

	Details.

	This function starts unit testing. To be specific, this function invokes test_* functions.\n
	These test_* functions returns boolean (actually, integer) value. Based on the test result, this function prints the testing output. 

	\fn int test_gpaToHPA();
	\brief A testing function for [gpaToHPA](@ref gpaToHPA)

	Details.

	This function tests [gpaToHPA](@ref gpaToHPA) function. The specification of this function is as follows:
	1. Input value : GPA of currently running process's page table base.
	2. Expected output : In bitvisor, the GPA is mapped the same as the HPA. Therefore, the output should be the same with the input value.


	\fn int test_traverseGuestPages();
	\brief A testing function for [traverseGuestPages](@ref traverseGuestPages)

	Details.

	This function tests [traverseGuestPages](@ref traverseGuestPages) function. The purpose of this function is to check whether this function traverses all pages of the currently running process.\n
	This function has no automatic testing method. So developers have to verify the printed results.
	For it this function prints GPA of each page by traversing the pages using [printPageGPA](@ref printPageGPA) function.

	\fn void* printPageGPA(GPA_t pageGPA)
	\brief A helper function for [test_traverseGuestPages](@ref test_traverseGuestPages)

	Details.
	This funcion exists to help the [test_traverseGuestPages](@ref test_traverseGuestPages) function. For each page traverse, this function is called to print the GPA of each page.

	\fn int test_logicalCPUStatus()
	\brief A testing function to see the logical CPU configuration.

	Details.

	This test is always success. Instead, this function prints logical cpu's configuration.
*/

#ifndef __UNIT_TEST_H__
#define __UNIT_TEST_H__

#include <monitor_types.h>

void test_start();
int test_gpaToHPA();
int test_traverseGuestPages();
void* printPageGPA(GPA_t pageGPA);
int test_logicalCPUStatus();
#endif