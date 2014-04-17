/**	\file	vmcs.h
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

#define FIELD_ENCODING_GUEST_ES_SELECTOR		((U64_t) 0x00000800)
#define FIELD_ENCODING_GUEST_CS_SELECTOR		((U64_t) 0x00000802)
#define FIELD_ENCODING_GUEST_SS_SELECTOR		((U64_t) 0x00000804)
#define FIELD_ENCODING_GUEST_DS_SELECTOR		((U64_t) 0x00000806)
#define FIELD_ENCODING_GUEST_FS_SELECTOR		((U64_t) 0x00000808)
#define FIELD_ENCODING_GUEST_GS_SELECTOR		((U64_t) 0x0000080A)

#define FIELD_ENCODING_GUEST_ES_LIMIT		((U64_t) 0x00004800)
#define FIELD_ENCODING_GUEST_CS_LIMIT		((U64_t) 0x00004802)
#define FIELD_ENCODING_GUEST_SS_LIMIT		((U64_t) 0x00004804)
#define FIELD_ENCODING_GUEST_DS_LIMIT		((U64_t) 0x00004806)
#define FIELD_ENCODING_GUEST_FS_LIMIT		((U64_t) 0x00004808)
#define FIELD_ENCODING_GUEST_GS_LIMIT		((U64_t) 0x0000480A)

#define FIELD_ENCODING_GUEST_ES_ACCESS_RIGHT		((U64_t) 0x00004814)
#define FIELD_ENCODING_GUEST_CS_ACCESS_RIGHT		((U64_t) 0x00004816)
#define FIELD_ENCODING_GUEST_SS_ACCESS_RIGHT		((U64_t) 0x00004818)
#define FIELD_ENCODING_GUEST_DS_ACCESS_RIGHT		((U64_t) 0x0000480A)
#define FIELD_ENCODING_GUEST_FS_ACCESS_RIGHT		((U64_t) 0x0000480C)
#define FIELD_ENCODING_GUEST_GS_ACCESS_RIGHT		((U64_t) 0x0000480F)

#define FIELD_ENCODING_GUEST_ES_BASE		((U64_t) 0x00006800)
#define FIELD_ENCODING_GUEST_CS_BASE		((U64_t) 0x00006802)
#define FIELD_ENCODING_GUEST_SS_BASE		((U64_t) 0x00006804)
#define FIELD_ENCODING_GUEST_DS_BASE		((U64_t) 0x00006806)
#define FIELD_ENCODING_GUEST_FS_BASE		((U64_t) 0x00006808)
#define FIELD_ENCODING_GUEST_GS_BASE		((U64_t) 0x0000680A)


#define FIELD_ENCODING_GUEST_TR_BASE		((U64_t) 0x00006814)

U64_t get_guest_cr3_value();
U64_t get_guest_eptp_value();
U64_t monitor_vmcs_read(const U64_t field);
void monitor_vmcs_write(const U64_t field, const U32_t value);

#endif