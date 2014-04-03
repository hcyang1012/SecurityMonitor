.section .processes
.long prc_vpn_name, 0, prc_vpn_bin, 0
.quad prc_vpn_bin_end - prc_vpn_bin
.quad +0
.data
prc_vpn_name: .string "vpn"
prc_vpn_bin: .incbin "process/vpn.bin"
prc_vpn_bin_end:
