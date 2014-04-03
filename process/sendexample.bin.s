.section .processes
.long prc_sendexample_name, 0, prc_sendexample_bin, 0
.quad prc_sendexample_bin_end - prc_sendexample_bin
.quad +0
.data
prc_sendexample_name: .string "sendexample"
prc_sendexample_bin: .incbin "process/sendexample.bin"
prc_sendexample_bin_end:
