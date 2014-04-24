#ifndef __SYSTEM_CALL_H__
#define __SYSTEM_CALL_H__

#include <monitor_types.h>
#include <guest_state.h>

#define MAX_SYSCALL_ARGS 6

struct syscall_metadata
{
	int number_of_args;
	int args_size[MAX_SYSCALL_ARGS];
	U8_t encryptBitmap;
};
void restore_register(const int argumentIndex, struct guest_sensitive_stats *guest_status);
void handleSystemCall(const int syscall_number, struct guest_sensitive_stats *guest_status);


#endif