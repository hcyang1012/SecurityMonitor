/*!	\file monitor_ept.h
	\brief This file declares some EPT-related functions and defines EPT-related constants.

	\def EPT_PML4_ENTRY_MASK
	\brief A mask used to extract base HPA of page directory pointer (PDP) table. 

	Details.

	This mask is used for converting GPA to HPA. To be specific, to get the HPA of PDP entry from EPT PML4 entry, following information should be combined:

	- Bits 63:52 are all 0
	- __Bits 51:12 are from the EPT PML4 entry__.
	- Bits 11:3 are bits 38:30 of the guest-physical address.
	- Bits 2:0 are all 0.

	Therefore, we need to extract bits 51:12 of the EPT PML4 entry. This macro is used for this purpose.

	\def EPT_PDP_ENTRY_MASK
	\brief A mask used to extract base HPA of page directory (PD) table. 

	Details.

	This mask is used for converting GPA to HPA. To be specific, to get the HPA of PD entry from EPT PDP entry, following information should be combined:

	- Bits 63:52 are all 0
	- __Bits 51:12 are from the EPT PDP table entry__.
	- Bits 11:3 are bits 29:21 of the guest-physical address.
	- Bits 2:0 are all 0.

	Therefore, we need to extract bits 51:12 of the EPT PDP table entry. This macro is used for this purpose.

	\def EPT_PD_ENTRY_MASK
	\brief A mask used to extract base HPA of page table (PT). 

	Details.

	This mask is used for converting GPA to HPA. To be specific, to get the HPA of PT entry from EPT PD entry, following information should be combined:

	- Bits 63:52 are all 0
	- __Bits 51:12 are from the EPT PD table entry__.
	- Bits 11:3 are bits 20:12 of the guest-physical address.
	- Bits 2:0 are all 0.

	Therefore, we need to extract bits 51:12 of the EPT PD table entry. This macro is used for this purpose.

	\def EPT_PT_ENTRY_MASK
	\brief A mask used to extract base HPA of page table (PT). 

	Details.

	This mask is used for converting GPA to HPA. To be specific, to get the HPA from a GPA, following information should be combined:

	- Bits 63:52 are all 0
	- __Bits 51:12 are from the EPT PT table entry__.
	- Bits 11:3 are bits 20:12 of the guest-physical address.
	- Bits 2:0 are all 0.

	Therefore, we need to extract bits 51:12 of the EPT PT table entry. This macro is used for this purpose.			

	\def EPT_PML4_GPA_MASK 
	\brief A mask used to extract HPA of EPT entry from GPA.

	Details.

	This mask is used for converting GPA to HPA. To be specific, to get the HPA of EPTP entry, following information should be combined:

	- Bits 63:52 are all 0
	- Bits 51:12 are from the EPTP.
	- Bits 11:3 are __bits 47:39 of the guest-physical address__.
	- Bits 2:0 are all 0.

	Therefore, we need to extract bits 47:39 of the guest-physical address. This macro is used for this purpose.


	\def EPT_PDP_GPA_MASK 
	\brief A mask used to extract HPA of EPT entry from GPA.

	Details.

	This mask is used for converting GPA to HPA. To be specific, to get the HPA of PDP entry from EPT PML4 entry, following information should be combined:

	- Bits 63:52 are all 0
	- Bits 51:12 are from the EPT PML4 entry.
	- Bits 11:3 are __bits 38:30 of the guest-physical address__.
	- Bits 2:0 are all 0.

	Therefore, we need to extract bits 38:30 of the GPA. This macro is used for this purpose.

	\def EPT_PD_GPA_MASK 
	\brief A mask used to extract HPA of EPT entry from GPA.

	Details.

	This mask is used for converting GPA to HPA. To be specific, to get the HPA of PD entry from EPT PDP table entry, following information should be combined:

	- Bits 63:52 are all 0
	- Bits 51:12 are from the EPT PML4 entry.
	- Bits 11:3 are __bits 29:21 of the guest-physical address__.
	- Bits 2:0 are all 0.

	Therefore, we need to extract bits 29:21 of the GPA. This macro is used for this purpose.

	\def EPT_PT_GPA_MASK 
	\brief A mask used to extract HPA of EPT entry from GPA.

	Details.

	This mask is used for converting GPA to HPA. To be specific, to get the HPA of PT entry from EPT PD table entry, following information should be combined:

	- Bits 63:52 are all 0
	- Bits 51:12 are from the EPT PML4 entry.
	- Bits 11:3 are __bits 20:12 of the guest-physical address__.
	- Bits 2:0 are all 0.

	Therefore, we need to extract bits 20:12 of the GPA. This macro is used for this purpose.

	\def EPT_GPA_MASK
	\brief A mask used to extract HPA of EPT entry from GPA.

	Details.

	This mask is used for converting GPA to HPA. To be specific, to get the HPA of GPA from EPT page table entry, following information should be combined:
	- Bits 51:12 are from the page table entry.
	- __Bits 11:0 are from the original GPA__.

	Therefore, we need to extract bits 11:00 of the GPA. This macro is used for this purpose.	

	\def EPT_PML4_GPA_SHIFT
	\brief A right-shift value for EPT-PML4 entry address calculation.

	Details.
	As mentiond in [EPT_PML4_GPA_MASK](@ref EPT_PML4_GPA_MASK), bits 47:39 of the guest-physical address is bits 11:3 of the EPT PML4 entry address.
	Therefore, the bits from the GPA should be shifted, and this macro is used for this purpose.


	\def EPT_PDP_GPA_SHIFT
	\brief A right-shift value for EPT-PDP entry address calculation.

	Details.
	As mentiond in [EPT_PDP_GPA_MASK](@ref EPT_PDP_GPA_MASK), bits 38:30 of the guest-physical address is bits 11:3 of the EPT PDP entry address.
	Therefore, the bits from the GPA should be shifted, and this macro is used for this purpose.

	\def EPT_PD_GPA_SHIFT
	\brief A right-shift value for EPT-PD entry address calculation.

	Details.
	As mentiond in [EPT_PD_GPA_MASK](@ref EPT_PD_GPA_MASK), bits 29:21 of the guest-physical address is bits 11:3 of the EPT PD entry address.
	Therefore, the bits from the GPA should be shifted, and this macro is used for this purpose.	

	\def EPT_PT_GPA_SHIFT
	\brief A right-shift value for EPT-PT entry address calculation.

	Details.
	As mentiond in [EPT_PT_GPA_MASK](@ref EPT_PT_GPA_MASK), bits 20:12 of the guest-physical address is bits 11:3 of the EPT PD entry address.
	Therefore, the bits from the GPA should be shifted, and this macro is used for this purpose.		

	\def EPT_ATTRIBUTE_MASK
	\brief A mask to extract page attribute from EPT entry
	The bits [2:0] indicate R/W/X attribute of a page. This mask is used to extract these properties
*/

#ifndef __MONITOR_EPT_H__
#define __MONITOR_EPT_H__

#include <monitor_types.h>

#define EPT_PML4_ENTRY_MASK ((U64_t)0xFFFFFFFFFF000)
#define EPT_PDP_ENTRY_MASK 	((U64_t)0xFFFFFFFFFF000)
#define EPT_PD_ENTRY_MASK 	((U64_t)0xFFFFFFFFFF000)
#define EPT_PT_ENTRY_MASK	((U64_t)0xFFFFFFFFFF000)
#define EPT_GPA_MASK		(0xFFF)

#define EPT_PML4_GPA_MASK ((U64_t)0xFF8000000000)
#define EPT_PDP_GPA_MASK ((U64_t)0x7FC0000000)
#define EPT_PD_GPA_MASK ((U64_t)0x3FE00000)
#define EPT_PT_GPA_MASK ((U64_t)0x1FF000)

#define EPT_PML4_GPA_SHIFT (39 - 3)
#define EPT_PDP_GPA_SHIFT (30 - 3)
#define EPT_PD_GPA_SHIFT (21 - 3)
#define EPT_PT_GPA_SHIFT (12 - 3)

#define EPT_ATTRIBUTE_MASK 0x7

#endif