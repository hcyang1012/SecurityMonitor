.section .processes
.long prc_sendint_name, 0, prc_sendint_bin, 0
.quad prc_sendint_bin_end - prc_sendint_bin
.quad +0
.data
prc_sendint_name: .string "sendint"
prc_sendint_bin: .incbin "process/sendint.bin"
prc_sendint_bin_end:
