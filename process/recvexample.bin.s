.section .processes
.long prc_recvexample_name, 0, prc_recvexample_bin, 0
.quad prc_recvexample_bin_end - prc_recvexample_bin
.quad +0
.data
prc_recvexample_name: .string "recvexample"
prc_recvexample_bin: .incbin "process/recvexample.bin"
prc_recvexample_bin_end:
