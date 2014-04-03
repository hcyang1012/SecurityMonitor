.section .processes
.long prc_storage_name, 0, prc_storage_bin, 0
.quad prc_storage_bin_end - prc_storage_bin
.quad +0
.data
prc_storage_name: .string "storage"
prc_storage_bin: .incbin "process/storage.bin"
prc_storage_bin_end:
