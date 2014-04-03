.section .processes
.long prc_log_name, 0, prc_log_bin, 0
.quad prc_log_bin_end - prc_log_bin
.quad +0
.data
prc_log_name: .string "log"
prc_log_bin: .incbin "process/log.bin"
prc_log_bin_end:
