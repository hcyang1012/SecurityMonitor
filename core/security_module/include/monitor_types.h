/**	\file monitor_types.h
	\brief Definitions of types that are used in this project.

	This header file defines several types that are used in this project.


*	\typedef typedef unsigned char	U8_t.
*	\brief A type definition for a unsigned 8-bits data.
*
* 	\typedef typedef unsigned short	U16_t.
*	\brief	A type definition for a unsigned 16-bits data.
*
*	\typedef typedef unsigned long	U32_t.
*	\brief	A type definition for a unsigned 32-bits data.
*
*	\typedef typedef unsigned long long	U64_t.
*	\brief	A type definition for a unsigned 64-bits data.
*
*	\typedef typedef U64_t GPA_t.
*	\brief	A type definition for a guest physical address.
*	> Note : The size of this type is __architecture-dependent__.
*
*	\typedef typedef U64_t	HPA_t
*	\brief A type definition for a host physical address.
*	> Note : The size of this type is __architecture-dependent__.
*
*	\typedef typedef HPA_t	EPTP_t
*	\brief A type definition for EPT Pointer (HPA).
*	> Note : EPTP_t is equal to HPA.
*
*	\typedef typedef EPTP_t	VMID_t
*	\brief A type definition for ID of a virtual machine.
*	> In this implementation, this type is equal to EPTP_t
*
*	\typedef typedef GPA_t	APPID_t
*	\brief A type definition for ID of a application in a virtual machine.
*	> In this implmentation, this type is equal to GPA_t.
*
*	\typedef typedef U64_t	EPT_ENTRY_t
*	\brief A type definition for EPT entry.
*
*	\typedef typedef U64_t	PGT_ENTRY_t
*	\brief A type definition for guest page table entry.
*/
#ifndef __MONITOR_TYPES_h__
#define __MONITOR_TYPES_h__
#ifdef CONFIG_BITVISOR
#include <spinlock.h>
#endif
typedef unsigned char	U8_t;
typedef unsigned short	U16_t;
typedef unsigned long	U32_t;
typedef unsigned long long	U64_t;

typedef U64_t	GVA_t;
typedef U64_t	GPA_t;
typedef U64_t	HPA_t;
typedef HPA_t	EPTP_t;

typedef EPTP_t	VMID_t;
typedef GPA_t	APPID_t;

typedef U64_t	EPT_ENTRY_t;
typedef U64_t	PGT_ENTRY_t;

#ifdef CONFIG_BITVISOR
typedef rw_spinlock_t SPIN_LOCK_t;
#endif
#endif