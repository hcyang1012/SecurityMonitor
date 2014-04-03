.section .processes
.long prc_idman_name, 0, prc_idman_bin, 0
.quad prc_idman_bin_end - prc_idman_bin
.quad +0
.data
prc_idman_name: .string "idman"
prc_idman_bin: .incbin "process/idman.bin"
prc_idman_bin_end:
