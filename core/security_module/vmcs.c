/**	\file vmcs.c
*	\brief Get VM information by reading VMCS

	This file defines several functions that are used to do vmcs operations.
*/

#include <monitor_types.h>
#include <vmcs.h>
	#include "asm.h"

U64_t get_guest_cr3_value()
{
	U64_t	guest_cr3_value;
	guest_cr3_value = monitor_vmcs_read(FIELD_ENCODING_GUEST_CR3);
	return guest_cr3_value;
}

U64_t get_guest_eptp_value()
{
	U64_t	guest_eptp_value;
	guest_eptp_value = monitor_vmcs_read(FIELD_ENCODING_GUEST_EPTP_FULL);
	return guest_eptp_value;
}

U64_t monitor_vmcs_read(const U64_t field)
{
	U64_t value = 0xDF;
	asm volatile ("vmread %1,%0"
		      : "=rm" (value)
		      : "r" (field)
		      : "cc");
	return value;	
}