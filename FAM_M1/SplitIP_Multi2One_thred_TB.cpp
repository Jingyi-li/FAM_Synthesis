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

//void Multi2Multi(
//		hls::stream<AXI_inter> r_AXI_in1[Phalf],
//		hls::stream<AXI_inter> r_AXI_in2[Phalf],
//		hls::stream<AXI_out> r_AXI_out1[Phalf],
//		hls::stream<AXI_out> r_AXI_out2[Phalf]
//);
void Multi2One(
		hls::stream<AXI_SCD> r_AXI_in11[Phalf],
		hls::stream<AXI_SCD> r_AXI_in22[Phalf],
		hls::stream<AXI_SCD>& r_AXI_out
);

int main(int argc, char **argv) {
//	hls::stream<AXI_inter> r_in1[Phalf];
//	hls::stream<AXI_inter> r_in2[Phalf];
	hls::stream<AXI_SCD> r_out1[Phalf];
	hls::stream<AXI_SCD> r_out2[Phalf];
	hls::stream<AXI_SCD> r_out;

	//	FILE *input_file  = fopen("/home/jingyi/SCDfunctions/Paper/Synthesis_M2_SCD/SCDresult_RTL_reorder.dat","r");// Input signal file
	//	float a,b;
	//	for(int i=0;i<OUTLOOPS;i++){
	//		for (int j=0;j<Phalf;j++){
	//			fscanf(input_file,"%f,",&a);
	//			AXI_inter temp;
	//			temp.data = single_fixed_t4(a);
	////			printf("Data is %f", a);
	//			b = temp.data.to_float();
	//			if(i==OUTLOOPS-1){
	//				temp.last = true;
	//			}else{
	//				temp.last = false;
	//			}
	//			r_in1[j].write(temp);
	//		}
	//
	//		for (int j=0;j<Phalf;j++){
	//			fscanf(input_file,"%f,",&a);
	//			AXI_inter temp;
	//			temp.data = single_fixed_t4(a);
	//			if(i==OUTLOOPS-1){
	//				temp.last = true;
	//			}else{
	//				temp.last = false;
	//			}
	//			r_in2[j].write(temp);
	//		}
	//	}
	//
	//	for (int loops = 0; loops<OUTLOOPS; loops++){
	//		Multi2Multi(r_in1,r_in2,r_out1,r_out2);
	//	}


	//	for(int i=0;i<OUTLOOPS;i++){
	//		for (int j=0;j<Phalf;j++){
	//			AXI_out temp;
	//			if (r_out1[j].read_nb(temp)){
	//				printf("temp1[%d][%d]: value=%f, al = %d, fl = %d, last = %d\n", i,j,temp.data.value.to_float(), temp.data.al.to_int(),temp.data.fl.to_int(),temp.last);
	//			}
	//		}
	//
	//		for (int j=0;j<Phalf;j++){
	//			AXI_out temp;
	//			if (r_out2[j].read_nb(temp)){
	//				printf("temp2[%d][%d]: value=%f, al = %d, fl = %d, last = %d\n", i,j,temp.data.value.to_float(), temp.data.al.to_int(),temp.data.fl.to_int(),temp.last);
	//			}
	//		}
	//	}
	int nloops = Phalf/4;
	for (int loops = 0; loops<nloops; loops++){
		SCDconvert fconvert;
		AXI_SCD temp;
		for (int i = 0;i<Phalf;i++){
			fconvert.i.value = i%7/15.0;
			fconvert.i.al = i+1;
			fconvert.i.fl = i*2;
			temp.data = fconvert.f;
			if (loops<nloops-1){
				temp.last = 0;
			}
			if (loops==nloops-1){
				temp.last=0;
			}
			r_out1[i].write(temp);
			r_out2[i].write(temp);
		}
		for (int i = 0;i<Phalf;i++){
			fconvert.i.value = 0;
			fconvert.i.al = 0;
			fconvert.i.fl = 0;
			temp.data = fconvert.f;
			if (loops<nloops-1){
				temp.last = 0;
			}
			if (loops==nloops-1){
				temp.last=1;
			}
			r_out1[i].write(temp);
			r_out2[i].write(temp);
		}
	}



	for (int loops = 0; loops<OUTLOOPS; loops++){
		Multi2One(r_out1,r_out2,r_out);
	}

	for(int i=0;i<132;i++){
		AXI_SCD temp;
		SCDconvert fconvert;
		if (r_out.read_nb(temp)){
			fconvert.f = temp.data;
			printf("temp[%d]: value=%f, al = %d, fl = %d, last = %d\n", i,fconvert.i.value, fconvert.i.al,fconvert.i.fl,temp.last);
		}
	}
	return 0;
}
