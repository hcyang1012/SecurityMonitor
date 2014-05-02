/*
 * Copyright (c) 2007, 2008 University of Tsukuba
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the University of Tsukuba nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "asm.h"
#include "constants.h"
#include "current.h"
#include "exint_pass.h"
#include "initfunc.h"
#include "int.h"
#include "string.h"

#ifdef CONFIG_SSLAB
 #include <guest_IA32ePage.h>
 #include <monitor_util.h>
 #include <memory_ownership_table.h>
 #include <vmcs.h>
#endif

static void exint_pass_int_enabled (void);
static void exint_pass_default (int num);
static void exint_pass_hlt (void);

static struct exint_func func = {
	exint_pass_int_enabled,
	exint_pass_default,
	exint_pass_hlt,
};

static void
exint_pass_int_enabled (void)
{
	current->vmctl.exint_pending (false);
	current->vmctl.exint_pass (false);
}

static void
exint_pass_default (int num)
{
	current->vmctl.generate_external_int (num);
}

static void
exint_pass_hlt (void)
{
	do_exint_pass ();
}

void
do_exint_pass (void)
{
	ulong rflags;
	int num;

	#ifdef CONFIG_SSLAB
		int isUserToKernel = 0;
		U64_t csSelector;
		char privilegeLevel;
		csSelector = monitor_vmcs_read(FIELD_ENCODING_GUEST_CS_SELECTOR);
		privilegeLevel = csSelector & 0x3;
		if(privilegeLevel > 0)
		{
			isUserToKernel = 1;
		}
	#endif
	current->vmctl.read_flags (&rflags);
	if (rflags & RFLAGS_IF_BIT) { /* if interrupts are enabled */
		num = do_externalint_enable ();
		if (num >= 0)
			current->exint.exintfunc_default (num);
		current->vmctl.exint_pending (false);
		current->vmctl.exint_pass (false);

		#ifdef CONFIG_SSLAB
		if(isUserToKernel){
			struct protected_application_t *currentProtectedApplication;
			currentProtectedApplication = getCurrentProtectedApplication();
			if(currentProtectedApplication)
			{
				GPA_t cr3GPA;
				save_guest_status(&(currentProtectedApplication->guest_sensitive_stats));	
				debug();
				currentProtectedApplication->guest_sensitive_stats.RIP = monitor_vmcs_read(FIELD_ENCODING_GUEST_RIP);
				currentProtectedApplication->guest_sensitive_stats.RIP_GPA = gvaToGPA(currentProtectedApplication->guest_sensitive_stats.RIP, get_page_table_base_GPA());
				printf("currentProtectedApplication->guest_sensitive_stats.RIP_GPA : %llx\n",currentProtectedApplication->guest_sensitive_stats.RIP_GPA);
				//clear_guest_status();
				cr3GPA = get_page_table_base_GPA();
				traverseGuestPages(currentProtectedApplication->owner_VM, currentProtectedApplication->owner_APP, cr3GPA, closePage);
				setCurrentProtectedApplication(NULL);
				openSystemCallHandler();
			}
		}
		#endif		
	} else {
		current->vmctl.exint_pending (true);
		current->vmctl.exint_pass (true);
	}	
}

static void
exint_pass_init (void)
{
	memcpy ((void *)&current->exint, (void *)&func, sizeof func);
}

INITFUNC ("pass0", exint_pass_init);
