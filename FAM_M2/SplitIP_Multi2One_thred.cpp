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

void Multi2One_func(
		hls::stream<AXI_SCD> r_AXI_out[STRIDE][Phalf],
		hls::stream<AXI_value>& r_AXI_out_value,
		hls::stream<AXI_label>& r_AXI_out_al,
		hls::stream<AXI_label>& r_AXI_out_fl
){
//	static bool last[STRIDE][Phalf]={
//			{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
//			{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false}
//	};
	static bool last[STRIDE][Phalf];
#pragma HLS ARRAY_PARTITION variable=last1 complete dim=0

	//	AXI_SCD temp;
	static int countn = 0;
	//	SCDconvert convert;
	//	convert.i.value=0;
	//	convert.i.al=0;
	//	convert.i.fl=0;

	for(int i = 0;i<Phalf;i++){
#pragma HLS PIPELINE
		for (int ll=0;ll<STRIDE;ll++){
			if (~last[ll][i]){

				AXI_SCD temp;
				if(r_AXI_out[ll][i].read_nb(temp)){
					if (temp.data==0){
						last[ll][i] = true;
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
								for (int lll = 0;lll<STRIDE;lll++){
									last[lll][j] = false;
								}
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
}
void Multi2One(
		hls::stream<AXI_SCD> r_AXI_in[STRIDE][Phalf],
		hls::stream<AXI_value>& r_AXI_out_value,
		hls::stream<AXI_label>& r_AXI_out_al,
		hls::stream<AXI_label>& r_AXI_out_fl

){
#pragma HLS PIPELINE
#pragma HLS INTERFACE axis register port=r_AXI_in
#pragma HLS INTERFACE axis register port=r_AXI_out_value
#pragma HLS INTERFACE axis register port=r_AXI_out_al
#pragma HLS INTERFACE axis register port=r_AXI_out_fl
#pragma HLS INTERFACE s_axilite register port=return

	Multi2One_func(r_AXI_in,r_AXI_out_value,r_AXI_out_al,r_AXI_out_fl);
}

