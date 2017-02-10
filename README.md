# Security Monitor on Bitvisor

# Introduction
[Bitvisor] (http://bitvisor.org/) is a good example of providing a secure, lightweight hardware usage model for a guest VM through a pass-through device driver. However, it doesn't protect security-sensitive data in memory of user application. For example, when the OS kernel is copromised, there is no way to protect leakage of user's data such as passwords from the malicious kernel.
To address this problem, I bring the model from the [Overshadow](http://xenon.stanford.edu/~talg/papers/ASPLOS08/overshadow-asplos08.ps) and implemented it on Bitvisor. As a result, even the malicious OS kernel or computer viruses try to access to the protected application's memory, the hypervisor protects these unauthorized accesses through memory virtualization.

# Design
On the bitvisor, 
