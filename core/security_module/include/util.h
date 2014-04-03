#ifndef __UTIL_H__
#define __UTIL_H__

#include <sec_types.h>

void sec_memset(U8_t *dst, U8_t value, U32_t size);
void sec_showDataofPhysicalAddress(PHYADDR_t *physicalAddress);

#endif