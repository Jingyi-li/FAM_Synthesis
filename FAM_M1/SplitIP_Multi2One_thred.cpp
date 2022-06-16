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
/*
void Threshold_func(
		hls::stream<AXI_inter> r_AXI_in[Phalf],
		hls::stream<AXI_out> r_AXI_out[Phalf],
		const int k,
		const int i
){
#pragma HLS PIPELINE
	int alabel = k+i-256;
	int flabel = 8*(i-k-1);
	for (int j=0;j<Phalf;j++){
#pragma HLS UNROLL
		AXI_inter temp = r_AXI_in[j].read();
		//		printf("the value is %f",temp.data.to_float());
		if (temp.last){
			if (temp.data>THRESHOLD){
				AXI_out outtemp;
				outtemp.data.value = temp.data;
				outtemp.data.al = alabel;
				outtemp.data.fl = flabel+j;
				outtemp.last = temp.last;
				r_AXI_out[j].write(outtemp);
			}else{
				AXI_out outtemp;
				outtemp.data.value = temp.data;
				outtemp.data.al = alabel;
				outtemp.data.fl = flabel+j;
				outtemp.last = temp.last;
				r_AXI_out[j].write(outtemp);
			}
		}else{
			if (temp.data>THRESHOLD){
				AXI_out outtemp;
				outtemp.data.value = temp.data;
				outtemp.data.al = alabel;
				outtemp.data.fl = flabel+j;
				outtemp.last = temp.last;
				r_AXI_out[j].write(outtemp);
			}
		}
	}

}


void Multi2Multi(
		hls::stream<AXI_inter> r_AXI_in1[Phalf],
		hls::stream<AXI_inter> r_AXI_in2[Phalf],
		hls::stream<AXI_out> r_AXI_out1[Phalf],
		hls::stream<AXI_out> r_AXI_out2[Phalf]
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
	Threshold_func(r_AXI_in1,r_AXI_out1,k,i);
	i +=1;
	Threshold_func(r_AXI_in2,r_AXI_out2,k,i);
	countn +=2;
}


 */

void Multi2One_func(
		hls::stream<AXI_SCD> r_AXI_out1[Phalf],
		hls::stream<AXI_SCD> r_AXI_out2[Phalf],
		hls::stream<AXI_value>& r_AXI_out_value,
		hls::stream<AXI_label>& r_AXI_out_al,
		hls::stream<AXI_label>& r_AXI_out_fl
){
	static bool last1[Phalf]={false,false,false,false,false,
			false,false,false,false,false,
			false,false,false,false,false,false};
#pragma HLS ARRAY_PARTITION variable=last1 complete dim=1
	static bool last2[Phalf]={false,false,false,false,false,
			false,false,false,false,false,
			false,false,false,false,false,false};
#pragma HLS ARRAY_PARTITION variable=last2 complete dim=1
	//	AXI_SCD temp;
	static int countn = 0;
	//	SCDconvert convert;
	//	convert.i.value=0;
	//	convert.i.al=0;
	//	convert.i.fl=0;

	for(int i = 0;i<Phalf;i++){
#pragma HLS PIPELINE
		if (~last1[i]){

			AXI_SCD temp;
			if(r_AXI_out1[i].read_nb(temp)){
				if (temp.data==0){
					last1[i] = true;
					countn++;
					if (countn==32){
						SCDconvert convertt;
						convertt.f=temp.data;
						AXI_value valuet;
						AXI_label alt;
						AXI_label flt;
						valuet.last = temp.last;
						valuet.value = convertt.i.value;
						alt.last = temp.last;
						alt.value = convertt.i.al;
						flt.last = temp.last;
						flt.value = convertt.i.fl;

						r_AXI_out_value.write(valuet);
						r_AXI_out_al.write(alt);
						r_AXI_out_fl.write(flt);
						countn=0;
						for (int j =0;j<Phalf;j++){
#pragma HLS UNROLL
							last1[j] = false;
							last2[j] = false;
						}
					}
				}else{
					SCDconvert convertt;
					convertt.f=temp.data;
					AXI_value valuet;
					AXI_label alt;
					AXI_label flt;
					valuet.last = temp.last;
					valuet.value = convertt.i.value;
					alt.last = temp.last;
					alt.value = convertt.i.al;
					flt.last = temp.last;
					flt.value = convertt.i.fl;

					r_AXI_out_value.write(valuet);
					r_AXI_out_al.write(alt);
					r_AXI_out_fl.write(flt);
				}
			}
		}
		if (~last2[i]){
			AXI_SCD temp;
			if(r_AXI_out2[i].read_nb(temp)){
				if (temp.data==0){
					last2[i] = true;
					countn++;
					if (countn==32){
						SCDconvert convertt;
						convertt.f=temp.data;
						AXI_value valuet;
						AXI_label alt;
						AXI_label flt;
						valuet.last = temp.last;
						valuet.value = convertt.i.value;
						alt.last = temp.last;
						alt.value = convertt.i.al;
						flt.last = temp.last;
						flt.value = convertt.i.fl;

						r_AXI_out_value.write(valuet);
						r_AXI_out_al.write(alt);
						r_AXI_out_fl.write(flt);
						countn=0;
						for (int j =0;j<Phalf;j++){
#pragma HLS UNROLL
							last1[j] = false;
							last2[j] = false;
						}
					}
				}else{
					SCDconvert convertt;
					convertt.f=temp.data;
					AXI_value valuet;
					AXI_label alt;
					AXI_label flt;
					valuet.last = temp.last;
					valuet.value = convertt.i.value;
					alt.last = temp.last;
					alt.value = convertt.i.al;
					flt.last = temp.last;
					flt.value = convertt.i.fl;

					r_AXI_out_value.write(valuet);
					r_AXI_out_al.write(alt);
					r_AXI_out_fl.write(flt);
				}
			}
		}
	}
}
void Multi2One(
		hls::stream<AXI_SCD> r_AXI_in11[Phalf],
		hls::stream<AXI_SCD> r_AXI_in22[Phalf],
		hls::stream<AXI_value>& r_AXI_out_value,
		hls::stream<AXI_label>& r_AXI_out_al,
		hls::stream<AXI_label>& r_AXI_out_fl

){
#pragma HLS PIPELINE
#pragma HLS INTERFACE axis register port=r_AXI_in11
#pragma HLS INTERFACE axis register port=r_AXI_in22
#pragma HLS INTERFACE axis register port=r_AXI_out_value
#pragma HLS INTERFACE axis register port=r_AXI_out_al
#pragma HLS INTERFACE axis register port=r_AXI_out_fl
#pragma HLS INTERFACE s_axilite register port=return

	Multi2One_func(r_AXI_in11,r_AXI_in22,r_AXI_out_value,r_AXI_out_al,r_AXI_out_fl);
}

