.section .processes
.long prc_panic_name, 0, prc_panic_bin, 0
.quad prc_panic_bin_end - prc_panic_bin
.quad +0
.data
prc_panic_name: .string "panic"
prc_panic_bin: .incbin "process/panic.bin"
prc_panic_bin_end:
