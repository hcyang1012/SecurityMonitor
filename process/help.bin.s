.section .processes
.long prc_help_name, 0, prc_help_bin, 0
.quad prc_help_bin_end - prc_help_bin
.quad +0
.data
prc_help_name: .string "help"
prc_help_bin: .incbin "process/help.bin"
prc_help_bin_end:
