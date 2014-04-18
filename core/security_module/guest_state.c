/**	\file guest_state.c
*	\brief High-level facilities to get guest VM's information.
*/
#include <guest_state.h>
#include <vmcs.h>
#include <guest_IA32ePage.h>
#include <monitor_util.h>
#include <memory_ownership_table.h>

#ifdef CONFIG_BITVISOR
#include <current.h>
#endif

GPA_t syscallHandlerAddress = 0;

GPA_t get_page_table_base_GPA()
{
	GPA_t cr3Value;
	cr3Value = get_guest_cr3_value();
	cr3Value = cr3Value & CR3_MASK;
	return cr3Value;
}


GPA_t get_ept_base_HPA()
{
	GPA_t eptpValue;
	eptpValue = get_guest_eptp_value();
	eptpValue = eptpValue & EPTP_MASK;

	return eptpValue;
}

void traverseGuestPages(const VMID_t vmid, const APPID_t appID, const GPA_t startGPAofPageTable, void* (*do_something)(const VMID_t vmID, const APPID_t appID, GPA_t gpa))
{
	traverseIA32ePages(vmid, appID, startGPAofPageTable, do_something);
}

void saveSystemCallHandlerAddress(const GVA_t addr)
{
	GPA_t syscallHandlerAddressGPA;
	syscallHandlerAddressGPA = gvaToGPA(addr, get_page_table_base_GPA());
	syscallHandlerAddress = syscallHandlerAddressGPA;	
	closePage(0,0, syscallHandlerAddress);
}

GPA_t getSystemCallHandlerGPA()
{
	//printf("syscallHandlerAddress : %llx\n",syscallHandlerAddress);
	return syscallHandlerAddress;
}

void save_guest_status(struct guest_sensitive_stats *guest_status)
{
	#ifdef CONFIG_BITVISOR
	current->vmctl.read_general_reg (GENERAL_REG_RAX, (unsigned long*)&(guest_status->RAX));
	current->vmctl.read_general_reg (GENERAL_REG_RBX, (unsigned long*)&(guest_status->RBX));
	current->vmctl.read_general_reg (GENERAL_REG_RCX, (unsigned long*)&(guest_status->RCX));
	current->vmctl.read_general_reg (GENERAL_REG_RDX, (unsigned long*)&(guest_status->RDX));
	current->vmctl.read_general_reg (GENERAL_REG_RDI, (unsigned long*)&(guest_status->RDI)); 
	current->vmctl.read_general_reg (GENERAL_REG_RSI, (unsigned long*)&(guest_status->RSI));
	current->vmctl.read_general_reg (GENERAL_REG_RBP, (unsigned long*)&(guest_status->RBP));
	current->vmctl.read_general_reg (GENERAL_REG_RSP, (unsigned long*)&(guest_status->RSP));
	current->vmctl.read_general_reg (GENERAL_REG_R8, (unsigned long*)&(guest_status->R8));
	current->vmctl.read_general_reg (GENERAL_REG_R9, (unsigned long*)&(guest_status->R9));
	current->vmctl.read_general_reg (GENERAL_REG_R10, (unsigned long*)&(guest_status->R10));
	current->vmctl.read_general_reg (GENERAL_REG_R11, (unsigned long*)&(guest_status->R11));
	current->vmctl.read_general_reg (GENERAL_REG_R12, (unsigned long*)&(guest_status->R12));
	current->vmctl.read_general_reg (GENERAL_REG_R13, (unsigned long*)&(guest_status->R13));
	current->vmctl.read_general_reg (GENERAL_REG_R14, (unsigned long*)&(guest_status->R14));
	current->vmctl.read_general_reg (GENERAL_REG_R15, (unsigned long*)&(guest_status->R15));

	guest_status->ES_selector = monitor_vmcs_read(FIELD_ENCODING_GUEST_ES_SELECTOR);
	guest_status->CS_selector = monitor_vmcs_read(FIELD_ENCODING_GUEST_CS_SELECTOR);
	guest_status->SS_selector = monitor_vmcs_read(FIELD_ENCODING_GUEST_SS_SELECTOR);
	guest_status->DS_selector = monitor_vmcs_read(FIELD_ENCODING_GUEST_DS_SELECTOR);
	guest_status->FS_selector = monitor_vmcs_read(FIELD_ENCODING_GUEST_FS_SELECTOR);
	guest_status->GS_selector = monitor_vmcs_read(FIELD_ENCODING_GUEST_GS_SELECTOR);

	monitor_vmcs_read(FIELD_ENCODING_GUEST_ES_LIMIT);
	guest_status->CS_limit = monitor_vmcs_read(FIELD_ENCODING_GUEST_CS_LIMIT);
	guest_status->SS_limit = monitor_vmcs_read(FIELD_ENCODING_GUEST_SS_LIMIT);
	guest_status->DS_limit = monitor_vmcs_read(FIELD_ENCODING_GUEST_DS_LIMIT);
	guest_status->FS_limit = monitor_vmcs_read(FIELD_ENCODING_GUEST_FS_LIMIT);
	guest_status->GS_limit = monitor_vmcs_read(FIELD_ENCODING_GUEST_GS_LIMIT);

	guest_status->ES_access_right = monitor_vmcs_read(FIELD_ENCODING_GUEST_ES_ACCESS_RIGHT);
	guest_status->CS_access_right = monitor_vmcs_read(FIELD_ENCODING_GUEST_CS_ACCESS_RIGHT);
	guest_status->SS_access_right = monitor_vmcs_read(FIELD_ENCODING_GUEST_SS_ACCESS_RIGHT);
	guest_status->DS_access_right = monitor_vmcs_read(FIELD_ENCODING_GUEST_DS_ACCESS_RIGHT);
	guest_status->FS_access_right = monitor_vmcs_read(FIELD_ENCODING_GUEST_FS_ACCESS_RIGHT);
	guest_status->GS_access_right = monitor_vmcs_read(FIELD_ENCODING_GUEST_GS_ACCESS_RIGHT);

	guest_status->ES_base = monitor_vmcs_read(FIELD_ENCODING_GUEST_ES_BASE);
	guest_status->CS_base = monitor_vmcs_read(FIELD_ENCODING_GUEST_CS_BASE);
	guest_status->SS_base = monitor_vmcs_read(FIELD_ENCODING_GUEST_SS_BASE);
	guest_status->DS_base = monitor_vmcs_read(FIELD_ENCODING_GUEST_DS_BASE);
	guest_status->FS_base = monitor_vmcs_read(FIELD_ENCODING_GUEST_FS_BASE);
	guest_status->GS_base = monitor_vmcs_read(FIELD_ENCODING_GUEST_GS_BASE);	
	#endif
}

void clear_guest_status()
{
	#ifdef CONFIG_BITVISOR
	current->vmctl.write_general_reg (GENERAL_REG_RAX,0);
	current->vmctl.write_general_reg (GENERAL_REG_RBX,0);
	current->vmctl.write_general_reg (GENERAL_REG_RCX,0);
	current->vmctl.write_general_reg (GENERAL_REG_RDX,0);
	current->vmctl.write_general_reg (GENERAL_REG_RDI,0); 
	current->vmctl.write_general_reg (GENERAL_REG_RSI,0);
	current->vmctl.write_general_reg (GENERAL_REG_RBP,0);
	current->vmctl.write_general_reg (GENERAL_REG_RSP,0);
	current->vmctl.write_general_reg (GENERAL_REG_R8, 0);
	current->vmctl.write_general_reg (GENERAL_REG_R9, 0);
	current->vmctl.write_general_reg (GENERAL_REG_R10,0);
	current->vmctl.write_general_reg (GENERAL_REG_R11,0);
	current->vmctl.write_general_reg (GENERAL_REG_R12,0);
	current->vmctl.write_general_reg (GENERAL_REG_R13,0);
	current->vmctl.write_general_reg (GENERAL_REG_R14,0);
	current->vmctl.write_general_reg (GENERAL_REG_R15,0);

	monitor_vmcs_write(FIELD_ENCODING_GUEST_ES_SELECTOR,0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_CS_SELECTOR,0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_SS_SELECTOR,0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_DS_SELECTOR,0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_FS_SELECTOR,0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_GS_SELECTOR,0);

	monitor_vmcs_write(FIELD_ENCODING_GUEST_ES_LIMIT, 0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_CS_LIMIT, 0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_SS_LIMIT, 0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_DS_LIMIT, 0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_FS_LIMIT, 0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_GS_LIMIT, 0);

	monitor_vmcs_write(FIELD_ENCODING_GUEST_ES_ACCESS_RIGHT, 0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_CS_ACCESS_RIGHT, 0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_SS_ACCESS_RIGHT, 0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_DS_ACCESS_RIGHT, 0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_FS_ACCESS_RIGHT, 0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_GS_ACCESS_RIGHT, 0);

	monitor_vmcs_write(FIELD_ENCODING_GUEST_ES_BASE, 0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_CS_BASE, 0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_SS_BASE, 0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_DS_BASE, 0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_FS_BASE, 0);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_GS_BASE, 0);
	#endif	
}

void restore_guest_status(struct guest_sensitive_stats *guest_status)
{
	#ifdef CONFIG_BITVISOR
	current->vmctl.write_general_reg (GENERAL_REG_RAX, (guest_status->RAX));
	current->vmctl.write_general_reg (GENERAL_REG_RBX, (guest_status->RBX));
	current->vmctl.write_general_reg (GENERAL_REG_RCX, (guest_status->RCX));
	current->vmctl.write_general_reg (GENERAL_REG_RDX, (guest_status->RDX));
	current->vmctl.write_general_reg (GENERAL_REG_RDI, (guest_status->RDI)); 
	current->vmctl.write_general_reg (GENERAL_REG_RSI, (guest_status->RSI));
	current->vmctl.write_general_reg (GENERAL_REG_RBP, (guest_status->RBP));
	current->vmctl.write_general_reg (GENERAL_REG_RSP, (guest_status->RSP));
	current->vmctl.write_general_reg (GENERAL_REG_R8,  (guest_status->R8));
	current->vmctl.write_general_reg (GENERAL_REG_R9,  (guest_status->R9));
	current->vmctl.write_general_reg (GENERAL_REG_R10, (guest_status->R10));
	current->vmctl.write_general_reg (GENERAL_REG_R11, (guest_status->R11));
	current->vmctl.write_general_reg (GENERAL_REG_R12, (guest_status->R12));
	current->vmctl.write_general_reg (GENERAL_REG_R13, (guest_status->R13));
	current->vmctl.write_general_reg (GENERAL_REG_R14, (guest_status->R14));
	current->vmctl.write_general_reg (GENERAL_REG_R15, (guest_status->R15));

	monitor_vmcs_write(FIELD_ENCODING_GUEST_ES_SELECTOR, guest_status->ES_selector);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_CS_SELECTOR, guest_status->CS_selector);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_SS_SELECTOR, guest_status->SS_selector);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_DS_SELECTOR, guest_status->DS_selector);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_FS_SELECTOR, guest_status->FS_selector);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_GS_SELECTOR, guest_status->GS_selector);

	monitor_vmcs_write(FIELD_ENCODING_GUEST_ES_LIMIT, guest_status->ES_limit);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_CS_LIMIT, guest_status->CS_limit);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_SS_LIMIT, guest_status->SS_limit);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_DS_LIMIT, guest_status->DS_limit);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_FS_LIMIT, guest_status->FS_limit);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_GS_LIMIT, guest_status->GS_limit);

	monitor_vmcs_write(FIELD_ENCODING_GUEST_ES_ACCESS_RIGHT, guest_status->ES_access_right);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_CS_ACCESS_RIGHT, guest_status->CS_access_right);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_SS_ACCESS_RIGHT, guest_status->SS_access_right);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_DS_ACCESS_RIGHT, guest_status->DS_access_right);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_FS_ACCESS_RIGHT, guest_status->FS_access_right);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_GS_ACCESS_RIGHT, guest_status->GS_access_right);

	monitor_vmcs_write(FIELD_ENCODING_GUEST_ES_BASE, guest_status->ES_base);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_CS_BASE, guest_status->CS_base);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_SS_BASE, guest_status->SS_base);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_DS_BASE, guest_status->DS_base);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_FS_BASE, guest_status->FS_base);
	monitor_vmcs_write(FIELD_ENCODING_GUEST_GS_BASE, guest_status->GS_base);
	#endif	
}

GVA_t getTSSGVA()
{
	return monitor_vmcs_read(FIELD_ENCODING_GUEST_TR_BASE);
}

GVA_t getKernelESPGVA()
{
	GVA_t tssGVA = getTSSGVA();
	GPA_t tssGPA = gvaToGPA(tssGVA, get_page_table_base_GPA());
	HPA_t tssHPA = gpaToHPA(tssGPA, 0);
	GVA_t *pTSS = mapHPAintoHVA(tssHPA,sizeof(HPA_t));
	GVA_t kernelESPGVA = *pTSS;
	unmapHPAintoHVA(pTSS,sizeof(HPA_t));
	return kernelESPGVA;
}