.section .processes
.long prc_debug_name, 0, prc_debug_bin, 0
.quad prc_debug_bin_end - prc_debug_bin
.quad +0
.data
prc_debug_name: .string "debug"
prc_debug_bin: .incbin "process/debug.bin"
prc_debug_bin_end:
