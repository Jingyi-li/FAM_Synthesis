#include "hls_stream.h"
#include "ap_int.h"
#include "ap_fixed.h"
#include "math.h"

#include "exp_table.h"
#include "window.h"
#include "types.h"
#include "hls_fft.h"
#include "auto.h"

using namespace hls;


void model_SCD(
		hls::stream<AXI_in >& i_AXI_in,
		hls::stream<AXI_in >& q_AXI_in,
		hls::stream<AXI_in> r_AXI_out1[Phalf],
		hls::stream<AXI_in> r_AXI_out2[Phalf]
);

void Multi2One(
		hls::stream<AXI_in> r_AXI_in11[Phalf],
		hls::stream<AXI_in> r_AXI_in22[Phalf],
		hls::stream<AXI_in>& r_AXI_value,
		hls::stream<AXI_in>& r_AXI_al,
		hls::stream<AXI_in>& r_AXI_fl
);

void Multi2Multi(
		hls::stream<AXI_in> r_AXI_in1[Phalf],
		hls::stream<AXI_in> r_AXI_in2[Phalf],
		hls::stream<AXI_in> r_AXI_out1[Phalf],
		hls::stream<AXI_in> r_AXI_out2[Phalf]
);


//void model_SCD(
//		hls::stream<AXI_in >& i_AXI_in,
//		hls::stream<AXI_in >& q_AXI_in,
////		hls::stream<AXI_inter> r_AXI_out1[Phalf],
////		hls::stream<AXI_inter> r_AXI_out2[Phalf]
//		hls::stream<single_fixed_t2 >& i_out,
//		hls::stream<single_fixed_t2 >& q_out
//);

int main(int argc, char **argv) {
	//Setup input and output AXI stream
	AXI_in stream_i_in, stream_q_in;
	hls::stream<AXI_in > i_in("i_in_stream");
	hls::stream<AXI_in > q_in("q_in_stream");
	hls::stream<AXI_in> r_M2M1[Phalf];
	hls::stream<AXI_in> r_M2M2[Phalf];
	hls::stream<AXI_in> r_M2O1[Phalf];
	hls::stream<AXI_in> r_M2O2[Phalf];
	hls::stream<AXI_in> r_out_value;
	hls::stream<AXI_in> r_out_al;
	hls::stream<AXI_in> r_out_fl;
	hls::stream<single_fixed_t2 > i_out("i_out_stream");
	hls::stream<single_fixed_t2 > q_out("q_out_stream");

	int Np_ = Np;
	int L_ = L; // 64
	int P_ = P;

	int in_size = (P_-1)*L_ + Np_; // P*L = 2048 or (P-1)*L + Np = 2240
	int out_size = Np_*P_/2; // 4096

	FILE *input_file  = fopen(fileinput,"r");// Input signal file
	float a,b;// a is Signal.real b is Signal.image

	for (int i = 0; i <= (in_size); i++) { // 2241
		if (i < P_*L_-1) {
			fscanf(input_file,"%f,%f",&a,&b);
			stream_i_in.data = half(a);
			stream_q_in.data = half(b);
			stream_i_in.last = false;
			stream_q_in.last = false;
			i_in.write(stream_i_in);
			q_in.write(stream_q_in);
		}if (i==P_*L_-1){
			fscanf(input_file,"%f,%f",&a,&b);
			stream_i_in.data = half(a);
			stream_q_in.data = half(b);
			stream_i_in.last = true;
			stream_q_in.last = true;
			i_in.write(stream_i_in);
			q_in.write(stream_q_in);
		}

	}
	fclose(input_file);


	model_SCD(i_in,q_in,r_M2M1,r_M2M2);
	for (int loops = 0; loops<OUTLOOPS/SIZE; loops++){
		Multi2Multi(r_M2M1,r_M2M2,r_M2O1,r_M2O2);
	}
	for (int loops = 0; loops<OUTLOOPS/SIZE; loops++){
		Multi2One(r_M2O1,r_M2O2,r_out_value,r_out_al,r_out_fl);
	}
	for(int i=0;i<OUTLOOPS;i++){
		AXI_in temp;
		if (r_out_value.read_nb(temp)){
			printf("temp[%d]: value=%f, al = %f, fl = %f, last = %d\n", i,(float)temp.data, (float)r_out_al.read().data,(float)r_out_fl.read().data,temp.last);
		}
	}

	return 0;
}
