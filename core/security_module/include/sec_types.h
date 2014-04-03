#ifndef __SEC_TYPES_H__
#define __SEC_TYPES_H__

typedef unsigned char		U8_t;
typedef unsigned long		U32_t;
typedef unsigned long long	U64_t;

typedef U64_t				VIRTADDR_t;
typedef U64_t				PHYADDR_t;

typedef PHYADDR_t			GUEST_PHYADDR_t;

typedef PHYADDR_t			APPID_t;
typedef U64_t				VMID_t;

typedef U8_t				BOOL_t;

#define FALSE	0
#define TRUE	1
#endif