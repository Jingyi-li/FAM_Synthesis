// This File is work for FAM M2 and output of Second FFT. The Synthesis file of cmodel_v2.cpp
#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_math.h"
#include "hls_stream.h"

#include "exp_table.h"
#include "window.h"
#include "types.h"

#include "fft_top.h"
#include "fxp_sqrt_top.h"

using namespace hls;


//void test(
//		single_fixed_t i_in[Phalf],
//		single_fixed_t q_in[Phalf],
//		single_fixed_t out[Phalf]
//		){
//	for (int i=0;i<Phalf;i++){
//		out[i] = hls::sqrt(i_in[i]*i_in[i]+q_in[i]*q_in[i]);
//	}
//}
// Save input stream to an array in length of N




void model_SCD(
		hls::stream<AXI_in >& i_AXI_in,
		hls::stream<AXI_in >& q_AXI_in,
		hls::stream<AXI_in> r_AXI_out1[Phalf],
		hls::stream<AXI_in> r_AXI_out2[Phalf]
//		hls::stream<single_fixed_t2 >& i_out,
//		hls::stream<single_fixed_t2 >& q_out
){
#pragma HLS INTERFACE axis register port=i_AXI_in
#pragma HLS INTERFACE axis register port=q_AXI_in
#pragma HLS INTERFACE axis register port=r_AXI_out1
#pragma HLS INTERFACE axis register port=r_AXI_out2
#pragma HLS INTERFACE s_axilite register port=return

	for (int n=0;n<N;n++){
#pragma HLS PIPELINE
		AXI_in temp1 = i_AXI_in.read();
		AXI_in temp2 = q_AXI_in.read();
		for (int j=0;j<16;j++){
			for (int i=0;i<Phalf;i++){
#pragma HLS UNROLL
				r_AXI_out1[i].write(temp1);
				r_AXI_out2[i].write(temp2);
			}
		}
	}
}


void Multi2Multi(
		hls::stream<AXI_in> r_AXI_in1[Phalf],
		hls::stream<AXI_in> r_AXI_in2[Phalf],
		hls::stream<AXI_in> r_AXI_out1[Phalf],
		hls::stream<AXI_in> r_AXI_out2[Phalf]
){
#pragma HLS PIPELINE
#pragma HLS INTERFACE axis register port=r_AXI_in1
#pragma HLS INTERFACE axis register port=r_AXI_in2
#pragma HLS INTERFACE axis register port=r_AXI_out1
#pragma HLS INTERFACE axis register port=r_AXI_out2
#pragma HLS INTERFACE s_axilite register port=return

for (int i = 0; i<Phalf;i++){
#pragma HLS UNROLL
	r_AXI_out1[i].write(r_AXI_in1[i].read());
	r_AXI_out2[i].write(r_AXI_in2[i].read());
}
}


void Multi2One(
		hls::stream<AXI_in> r_AXI_in11[Phalf],
		hls::stream<AXI_in> r_AXI_in22[Phalf],
		hls::stream<AXI_in>& r_AXI_value,
		hls::stream<AXI_in>& r_AXI_al,
		hls::stream<AXI_in>& r_AXI_fl
){
#pragma HLS PIPELINE
#pragma HLS INTERFACE axis register port=r_AXI_in11
#pragma HLS INTERFACE axis register port=r_AXI_in22
#pragma HLS INTERFACE axis register port=r_AXI_value
#pragma HLS INTERFACE axis register port=r_AXI_al
#pragma HLS INTERFACE axis register port=r_AXI_fl
#pragma HLS INTERFACE s_axilite register port=return
#pragma DATAFLOW
	for (int i=0;i<Phalf-1;i++){
#pragma HLS UNROLL
		AXI_in temp1 = r_AXI_in11[i].read();
		AXI_in temp2 = r_AXI_in22[i].read();
	}
	r_AXI_value.write(r_AXI_in11[Phalf-1].read());
	AXI_in temp = r_AXI_in22[Phalf-1].read();
	r_AXI_al.write(temp);
	r_AXI_fl.write(temp);

}




