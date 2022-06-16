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

void Threshold_func(
		hls::stream<AXI_inter> r_AXI_in[Phalf],
		hls::stream<AXI_SCD> r_AXI_out[Phalf],
		int k,
		int i,
		int countn
){
#pragma HLS PIPELINE
	int alabel = k+i-256;
	int flabel = 8*(i-k-1);
	for (int j=0;j<Phalf;j++){
#pragma HLS UNROLL
		AXI_inter temp = r_AXI_in[j].read();
		//		printf("the value is %f",temp.data.to_float());
		if (temp.last||countn==FULLLOOPS/SIZE-2){
			AXI_SCD outtemp;
			SCDconvert convert;
			convert.i.value = 0;
			convert.i.al = 0;
			convert.i.fl = 0;
			outtemp.data = convert.f;
			outtemp.last = temp.last;
			r_AXI_out[j].write(outtemp);
		}else{
			if (temp.data>THRESHOLD){
				AXI_SCD outtemp;
				SCDconvert convert;
				convert.i.value = temp.data;
				convert.i.al = alabel;
				convert.i.fl = flabel+j;
				outtemp.data = convert.f;
				outtemp.last = temp.last;
				r_AXI_out[j].write(outtemp);
			}
		}
	}

}


void Multi2Multi(
		hls::stream<AXI_inter> r_AXI_in1[Phalf],
		hls::stream<AXI_inter> r_AXI_in2[Phalf],
		hls::stream<AXI_SCD> r_AXI_out1[Phalf],
		hls::stream<AXI_SCD> r_AXI_out2[Phalf]
){
#pragma HLS PIPELINE
#pragma HLS INTERFACE axis register port=r_AXI_in1
#pragma HLS INTERFACE axis register port=r_AXI_in2
#pragma HLS INTERFACE axis register port=r_AXI_out1
#pragma HLS INTERFACE axis register port=r_AXI_out2
#pragma HLS INTERFACE s_axilite register port=return

	static int countn;
	int k = countn/256;
	int i = countn%256;
	Threshold_func(r_AXI_in1,r_AXI_out1,k,i,countn);
	i +=1;
	Threshold_func(r_AXI_in2,r_AXI_out2,k,i,countn);
	if (countn<FULLLOOPS/SIZE-2){
		countn +=2;
	}else{
		countn = 0;
	}

}





