/**	\file	VMCS.h
*	\brief	Definition of constants related to VMCS

*	#	Details
*	This file defines several constants that are used to read a value from and write a value to the VMCS.\n
*	Most of these constnts are field encodings in VMCS.\n
*	Read __Intel's architecture manual Volume 3, APPENDIX B : Field Encoding in VMCS.__\n
*
*
*	\def FIELD_ENCODING_GUEST_CR3
*	\brief GUEST CR3 field encoding.
*
*	\def FIELD_ENCODING_GUEST_EPTP_FULL
*	\brief GUEST EPT pointer field (full) encoding.
*
*	\def FIELD_ENCODING_IA32_EFER
*	\brief GUEST IA32_EFER (full) encoding.
*
*	\fn U64_t get_guest_cr3_value()
*	\brief get the value of the guest CR3 control register
*	\return	This function returns guest's CR3 control register value.
*
*	Details.
*
*	This function returns the value of CR3 control register by reading VMCS.\n
*	\warning The return value of this function does not specifies the exact address of the page directory.\n
*	> Bits [11:0] of the return value are used to specify some information. Read __Intel's architecture manual Vol. 3, Chapter 2.5 : Control Registers__. 
*
*	\fn U64_t get_guest_eptp_value()
*	\brief get the value of guest's EPTP field in the VMCS
*	\returns This function returns the value of EPT pointer (EPTP) by reading a guest's VMCS.
*
*	Details.
*
*	This function returns the value of EPT pointer (EPTP) by reading a guest's VMCS.
*	\warning The return value of this function does not specifies the exact address of the EPT.\n
*	> Bits [11:0] of the return value are used to specify some information. Read __Intel's architecture manual Vol. 3, Chapter 24.6.11 : Extended-Page-Table Pointer(EPTP)__. 
*
*	\fn U64_t monitor_vmcs_read(const U64_t field)
*	\brief get a value in the VMCS
*	\param field The input value that specifies the VMCS field that to be read.
*	\return This function returns a value which is read from the VMCS field value specified by the input .
*/

#ifndef __VMCS_H__
#define __VMCS_H__

#include <monitor_types.h>

#define FIELD_ENCODING_GUEST_CR3		((U64_t) 0x00006802)
#define FIELD_ENCODING_GUEST_EPTP_FULL	((U64_t) 0x0000201A)
#define FIELD_ENCODING_IA32_EFER		((U64_t) 0x00002806)

U64_t get_guest_cr3_value();
U64_t get_guest_eptp_value();
U64_t monitor_vmcs_read(const U64_t field);

#endif