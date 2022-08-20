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
//void init_quarter(int quarter_table[OUTLOOPS/2+Np])
//{
//	int i=0;
//	for(int ik = 0; ik<Np/2; ik++){
//		int K = int (ik/2)*2;
//		for(int jk = K;jk<Np-K;jk++){
//			int countn = ik*Np+jk;
//			quarter_table[i] = countn;
////			printf("value of countn is %d; i is %d\n",countn,i);
//			i +=1;
//		}
//	}
////	printf("value of I is %d\n",i);
//
//}
//int array_quarter(int idx)
//{
//#pragma HLS PIPELINE
//	int quarter_table[OUTLOOPS/2+Np];
//	init_quarter(quarter_table);
//	return quarter_table[idx];
//}
void Threshold_func(
		hls::stream<AXI_inter> r_AXI_in[Phalf],
		hls::stream<AXI_SCD> r_AXI_out[Phalf],
		int countn
){
#pragma HLS PIPELINE
//	int alabel = k+i-256;
//	int flabel = 8*(i-k-1);
	for (int j=0;j<Phalf;j++){
#pragma HLS UNROLL
		AXI_inter temp = r_AXI_in[j].read();
		//		printf("the value is %f",temp.data.to_float());
		if (temp.last||countn>=OUTLOOPS/2+Np){
			AXI_SCD outtemp;
			SCDconvert convert;
			convert.i.value = 0;
			convert.i.al = 0;
			convert.i.fl = 0;
			outtemp.data = convert.f;
			outtemp.last = temp.last;
//			printf("%f,%d,%d\n",float(temp.data),countn,j);
			r_AXI_out[j].write(outtemp);
		}else{
			if (temp.data>THRESHOLD){
				AXI_SCD outtemp;
				SCDconvert convert;
				convert.i.value = temp.data;
				convert.i.al = countn;
				convert.i.fl = j;
				outtemp.data = convert.f;
				outtemp.last = temp.last;
//				printf("%f,%d,%d\n",float(temp.data),countn,j);
				r_AXI_out[j].write(outtemp);
			}
		}
	}

}

void Multi2Multi(
		hls::stream<AXI_inter> r_AXI_in[STRIDE][Phalf],
		hls::stream<AXI_SCD> r_AXI_out[STRIDE][Phalf]
){
#pragma HLS PIPELINE
#pragma HLS INTERFACE axis register port=r_AXI_in
#pragma HLS INTERFACE axis register port=r_AXI_out
#pragma HLS INTERFACE s_axilite register port=return

	static int countn;
//	int count = array_quarter(countn);
//	int k = count/256;
//	int i = count%256;
	for (int ll=0;ll<STRIDE;ll++){
#pragma HLS UNROLL
		Threshold_func(r_AXI_in[ll],r_AXI_out[ll],countn+ll);
//		printf("i = %d,k = %d\n",i,k);
	}
	if (countn<OUTLOOPS/2+Np){
		countn +=STRIDE;
	}else{
		countn = STRIDE;
	}

}






