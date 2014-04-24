#include <system_call.h>
#include <guest_state.h>

#ifdef CONFIG_BITVISOR
#include <current.h>
#endif
struct syscall_metadata syscall_metatata_table[] = 
{
{0,},
};


void handleSystemCall(const int syscall_number, struct guest_sensitive_stats *guest_status)
{
	struct syscall_metadata *targetSystemCall = &(syscall_metatata_table[syscall_number]);
	int number_of_args;
	int index;
	number_of_args = targetSystemCall->number_of_args;

	for(index = 0 ; index < number_of_args ; index++)
	{
		int currentArgument = targetSystemCall->args_size[index];
		if(currentArgument == 0)
		{
			restore_register(index, guest_status);
		}
		else if(currentArgument == -1)
		{
			continue;
		}
		else
		{

		}
	}
}


/* From (linux)/arch/x86/kernel/entry_64.S
 * Register setup:
 * rax  system call number
 * rdi  arg0
 * rcx  return address for syscall/sysret, C arg3
 * rsi  arg1
 * rdx  arg2
 * r10  arg3 	(--> moved to rcx for C)
 * r8   arg4
 * r9   arg5
 * r11  eflags for syscall/sysret, temporary for C
 * r12-r15,rbp,rbx saved by C code, not touched.
 *
 * Interrupts are off on entry.
 * Only called from user space.
 *
 * XXX	if we had a free scratch register we could save the RSP into the stack frame
 *      and report it properly in ps. Unfortunately we haven't.
 *
 * When user can change the frames always force IRET. That is because
 * it deals with uncanonical addresses better. SYSRET has trouble
 * with them due to bugs in both AMD and Intel CPUs.
 */

void restore_register(const int argumentIndex, struct guest_sensitive_stats *guest_status)
{
	switch(argumentIndex)
	{
		case 0:
			current->vmctl.write_general_reg (GENERAL_REG_RDI, (guest_status->RDI)); 
			break;
		case 1:
			current->vmctl.write_general_reg (GENERAL_REG_RSI, (guest_status->RSI));
			break;
		case 2:
			current->vmctl.write_general_reg (GENERAL_REG_RDX, (guest_status->RDX));
			break;
		case 3:
			current->vmctl.write_general_reg (GENERAL_REG_R10, (guest_status->R10));
			break;
		case 4:
			current->vmctl.write_general_reg (GENERAL_REG_R8,  (guest_status->R8));
			break;
		case 5:
	current->vmctl.write_general_reg (GENERAL_REG_R9,  (guest_status->R9));
			break;
	}
}