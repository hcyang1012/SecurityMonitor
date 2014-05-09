/**	\file eventHandler.h
*	\brief
*	Catching & handling of memory-related events.
*
*	\fn void hypercall_handler()
*	\brief
*	This function handles hypercall invoked from guest application.\n
*	The goal of this function is to start protection of the caller application's memory.\n
*	For this goal, this function does followings:
*	- Check whether the guest's CR3 control register has a non-zero value.(It means that the guest is running in protected mode, therefore, it uses paging mechanism.)
*	- If the register has a non-zero value, extract the start address of currently running process's page table. This address is GPA.
*	- The security monitor uses the address to identify an application. So this function regards the address as a ID of the caller application.
*	- Traverse the application's all pages to protect them. The traversed pages are registered to memory ownership table.

*	\return This function has no return value
*
*/
#ifndef __EVENT_HANDLER_H__
#define __EVENT_HANDLER_H__

void hypercall_handler();
void interrupt_handler();
#endif