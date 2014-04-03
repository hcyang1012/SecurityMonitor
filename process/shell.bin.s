.section .processes
.long prc_shell_name, 0, prc_shell_bin, 0
.quad prc_shell_bin_end - prc_shell_bin
.quad +0
.data
prc_shell_name: .string "shell"
prc_shell_bin: .incbin "process/shell.bin"
prc_shell_bin_end:
