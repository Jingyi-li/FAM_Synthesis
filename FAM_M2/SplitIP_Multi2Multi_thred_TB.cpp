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

void Multi2Multi(
		hls::stream<AXI_inter> r_AXI_in1[Phalf],
		hls::stream<AXI_inter> r_AXI_in2[Phalf],
		hls::stream<AXI_SCD> r_AXI_out1[Phalf],
		hls::stream<AXI_SCD> r_AXI_out2[Phalf]
);


int main(int argc, char **argv) {
	hls::stream<AXI_inter> r_in1[Phalf];
	hls::stream<AXI_inter> r_in2[Phalf];
	hls::stream<AXI_SCD> r_out1[Phalf];
	hls::stream<AXI_SCD> r_out2[Phalf];
	hls::stream<AXI_out> r_out;


	for (int n=0;n<2;n++){
		FILE *input_file  = fopen("/home/jingyi/SCDfunctions/Paper/Synthesis_M2_SCD/SCDresult_RTL_reorder.dat","r");// Input signal file
		float a,b;
		for(int i=0;i<OUTLOOPS/SIZE;i++){
			for (int j=0;j<Phalf;j++){
				fscanf(input_file,"%f,",&a);
				AXI_inter temp;
				temp.data = single_fixed_t4(a);
				//			printf("Data is %f", a);
				b = temp.data.to_float();
				temp.last = false;
				r_in1[j].write(temp);
			}

			for (int j=0;j<Phalf;j++){
				fscanf(input_file,"%f,",&a);
				AXI_inter temp;
				temp.data = single_fixed_t4(a);
				temp.last = false;
				r_in2[j].write(temp);
			}
		}
		fclose(input_file);
	}
	FILE *input_file  = fopen("/home/jingyi/SCDfunctions/Paper/Synthesis_M2_SCD/SCDresult_RTL_reorder.dat","r");// Input signal file
	float a,b;
	for(int i=0;i<OUTLOOPS/SIZE;i++){
		for (int j=0;j<Phalf;j++){
			fscanf(input_file,"%f,",&a);
			AXI_inter temp;
			temp.data = single_fixed_t4(a);
			//			printf("Data is %f", a);
			b = temp.data.to_float();
			if(i==OUTLOOPS/SIZE-1){
				temp.last = true;
			}else{
				temp.last = false;
			}
			r_in1[j].write(temp);
		}
		for (int j=0;j<Phalf;j++){
			fscanf(input_file,"%f,",&a);
			AXI_inter temp;
			temp.data = single_fixed_t4(a);
			if(i==OUTLOOPS/SIZE-1){
				temp.last = true;
			}else{
				temp.last = false;
			}
			r_in2[j].write(temp);
		}
	}
	fclose(input_file);



	for(int n=0;n<3;n++){
		for (int loops = 0; loops<OUTLOOPS/SIZE; loops++){
			Multi2Multi(r_in1,r_in2,r_out1,r_out2);
		}
	}



	for (int n=0; n<3;n++){
		for(int i=0;i<OUTLOOPS/SIZE;i++){
			for (int j=0;j<Phalf;j++){
				AXI_SCD temp;
				if (r_out1[j].read_nb(temp)){
					SCDconvert convert;
					convert.f = temp.data;
					if (j==8){
					printf("temp1[%d][%d]: value=%f, al = %d, fl = %d, last = %d\n", i,j,convert.i.value, convert.i.al,convert.i.fl,temp.last);
				}
				}
			}

			for (int j=0;j<Phalf;j++){
				AXI_SCD temp;
				if (r_out2[j].read_nb(temp)){
					SCDconvert convert;
					convert.f = temp.data;
//					printf("temp2[%d][%d]: value=%f, al = %d, fl = %d, last = %d\n", i,j,convert.i.value, convert.i.al,convert.i.fl,temp.last);
				}
			}
		}
	}
	return 0;
}
