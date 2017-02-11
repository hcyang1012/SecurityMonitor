# Security Monitor on Bitvisor

# Introduction
[Bitvisor] (http://bitvisor.org/) is a good example of providing a secure, lightweight hardware usage model for a guest VM through a pass-through device driver. However, it doesn't protect security-sensitive data in memory of user application. For example, when the OS kernel is copromised, there is no way to protect leakage of user's data such as passwords from the malicious kernel.

To address this problem, I bring the model from the [Overshadow](http://xenon.stanford.edu/~talg/papers/ASPLOS08/overshadow-asplos08.ps) and implemented it on Bitvisor. As a result, even the malicious OS kernel or computer viruses try to access to the protected application's memory, the hypervisor protects these unauthorized accesses through memory virtualization.

# Design
## From para-virtualization to Full-virtualization.
On the bitvisor, I implemented the overshadow's memory protection model in core/security_module(https://github.com/hcyang1012/SecurityMonitor/tree/master/core/security_module) directory. The model itself is very similar to the original one. However, the main difference is that I change the model to be run on full-virtualization, not para-virtualization. 

To be specific, to protect user application's memory from other stakeholders including OS kernel, Overshadow requires to install library that connects between a protected application and the hypervisor. However, in practice, this approach is too complicated to adopt in the real world.

Instead, To address this problem, this security monitor is based on full-virtualization. It means that, instead of tracing the guest VM's state via pre-installed library, the security monitor in Hypervisor monitors all accesses to the memory areas which are owned by the security-sensitive user application using *Extended Page Table (EPT)*. 

## Extended Page Table (EPT)
Actually, EPT is the feature in x86 VT-x for memory virtualization. EPT is the hightest-level page table that virtualizes physical memory to the guest VM, like the original page table virtualizaes the same on to user application.
(To be continue..)
