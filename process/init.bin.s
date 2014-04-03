.section .processes
.long prc_init_name, 0, prc_init_bin, 0
.quad prc_init_bin_end - prc_init_bin
.quad +0
.data
prc_init_name: .string "init"
prc_init_bin: .incbin "process/init.bin"
prc_init_bin_end:
