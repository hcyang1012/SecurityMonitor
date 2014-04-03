.section .processes
.long prc_serialtest_name, 0, prc_serialtest_bin, 0
.quad prc_serialtest_bin_end - prc_serialtest_bin
.quad +0
.data
prc_serialtest_name: .string "serialtest"
prc_serialtest_bin: .incbin "process/serialtest.bin"
prc_serialtest_bin_end:
