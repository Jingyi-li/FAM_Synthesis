// This File is work for FAM M2 and output of Second FFT. The Synthesis file of cmodel_v2.cpp
#include "ap_int.h"
#include "ap_fixed.h"
//#include "hls_math.h"
#include <math.h>
#include "hls_stream.h"

#include "exp_table.h"
#include "window.h"
#include "types.h"

#include "fft_top.h"
#include "fxp_sqrt_top.h"

using namespace hls;

// ROM Lookup table
// Hamming window coefficient
void init_Hamming_table(exp_fixed_t3 hamming_table[Np])
{
	int i;
	for (i = 0; i < Np; i++) {//
		hamming_table[i] = (exp_fixed_t3) (0.54-0.46*cos(2*PI*(float)i/(Np-1)));
	}
}

single_fixed_t array_ROM_Hamming_init(single_fixed_t inval, dint_t idx)
{
#pragma HLS PIPELINE
	exp_fixed_t3 hamming_table[Np];
	init_Hamming_table(hamming_table);
	return inval * hamming_table[idx];
}
// FFT shift for First FFT
void init_outorder1_Np_table(dint_t outorder[Np]){
	int i;
	int bias  = int (Np/2);
	for (i = 0; i < Np; i++) {//
		if (i<bias){
			outorder[i] = i+bias;
		}
		else{
			outorder[i] = i-bias;
		}
	}
}

dint_t array_ROM_outorder1_init(dint_t idx){
#pragma HLS PIPELINE
	dint_t outorder[Np];
	init_outorder1_Np_table(outorder);
	return outorder[idx];
}
// bit-reorder for P-point FFT
void init_bitreorder_table(dint_t bitreorder_table[P])
{
//	int P_log2 = log2(P);
	for (int i = 0; i < P; i++) {
		int c=0;
		int b=i;
		for (int j=0;j<P_log2;j++){
			c = c + (b%2)* pow(2,P_log2-j-1);
			b = int (b/2);
		}
		bitreorder_table[i] = c;
	}
}
dint_t array_ROM_bitreorder_init(dint_t idx)
{
#pragma HLS PIPELINE
	dint_t bitreorder_table[P];
	init_bitreorder_table(bitreorder_table);
	return bitreorder_table[idx];
}
// twiddle factors for FFT
void init_Twiddle_i_table(exp_fixed_t2 twiddle_i_table[P/2])
{
	for (int i = 0; i < P/2; i++) {//
		twiddle_i_table[i] = (exp_fixed_t2) cos(-2*PI*i/P);
	}
}

double_fixed_t3 array_ROM_Twiddle_i_init(double_fixed_t3 inval, dint_t idx)
{
#pragma HLS PIPELINE
	exp_fixed_t2 twiddle_i_table[P/2];
	init_Twiddle_i_table(twiddle_i_table);
	return inval * twiddle_i_table[idx];
}
void init_Twiddle_q_table(exp_fixed_t2 twiddle_q_table[P/2])
{
	for (int i = 0; i < P/2; i++) {//
		twiddle_q_table[i] = (exp_fixed_t2) sin(-2*PI*i/P);
	}
}

double_fixed_t3 array_ROM_Twiddle_q_init(double_fixed_t3 inval, dint_t idx)
{
#pragma HLS PIPELINE
	exp_fixed_t2 twiddle_q_table[P/2];
	init_Twiddle_q_table(twiddle_q_table);
	return inval * twiddle_q_table[idx];
}

// FFT shift for First FFT
void init_outorder2_P_table(dint_t outorder[P]){
	int i;
	int bias  = int (P/2);
	for (i = 0; i < P; i++) {//
		if (i<bias){
			outorder[i] = i+bias;
		}
		else{
			outorder[i] = i-bias;
		}
	}
}

dint_t array_ROM_outorder2_init(dint_t idx){
#pragma HLS PIPELINE
	dint_t outorder[P];
	init_outorder2_P_table(outorder);
	return outorder[idx];
}

// Save input stream to an array in length of N
void save_input(
		stream<AXI_in >& i_in,
		stream<AXI_in >& q_in,
		single_fixed_t i_out[PARALLEL][LENGTH],
		single_fixed_t q_out[PARALLEL][LENGTH],
		bool &last
){
#pragma HLS ARRAY_PARTITION variable=i_out complete dim=1
#pragma HLS ARRAY_PARTITION variable=q_out complete dim=1
	for(int i=0;i<N;i++){
#pragma HLS PIPELINE
		AXI_in tempi = i_in.read();
		AXI_in tempq = q_in.read();
		for (int j=0;j<PARALLEL;j++){
#pragma HLS UNROLL
			int start = N/PARALLEL*j;
			if (start<=i && i<start+LENGTH){
				i_out[j][i-start] = single_fixed_t(tempi.data);
				q_out[j][i-start] = single_fixed_t(tempq.data);
			}
		}

		if (i==N-1){
//			printf("check %f,%f\n",i_out[i].to_float(),q_out[i].to_float());
			last = (tempi.last||tempq.last);
		}
	}
	for (int i=LENGTH-Np+L;i<LENGTH;i++){
		i_out[PARALLEL-1][i] = 0;
		q_out[PARALLEL-1][i] = 0;
	}
}

// Fram Function + Windowing function
// Fram: divide the signal into P arrays and each array has Np samples, stride over Np/4 samples of each array
// Window: Pass a hamming window filter
void fram_windowing(
		single_fixed_t i_in[LENGTH],
		single_fixed_t q_in[LENGTH],
		cmpxDataIn xn[Np],
		const unsigned order
){
	int io = order*Np/4;
	for(int i=0; i<Np; i++){
#pragma HLS PIPELINE
		data_in_t re, im;
		re = array_ROM_Hamming_init(i_in[io+i], i);
		im = array_ROM_Hamming_init(q_in[io+i], i);
		xn[i] = cmpxDataIn(re, im);
	}
}

/*
// Unscaled FFT IP core with natural input order
void fft_ip_256_array(
		cmpxDataIn xn[Np],
		cmpxDataOut xk[Np]
) {
	config_t fft_config;
	status_t fft_status;
	// Forward FFT
	fft_config.setDir(1); // Forward(1); Inverse(0)
	// Set FFT length to 256 => log2(256) = 8
	//	fft_config.setSch(0xAA); // [2 2 2 2]
#pragma HLS interface ap_fifo depth=256 port=xn
#pragma HLS interface ap_fifo depth=256 port=xk
#pragma HLS interface ap_fifo port=fft_config
	// #pragma HLS interface ap_fifo port=fft_status
#pragma HLS data_pack variable=xn
#pragma HLS data_pack variable=xk
#pragma HLS data_pack variable=fft_config

	// FFT IPq_fft_256_out
	hls::fft<config1>(xn, xk, &fft_status, &fft_config);
	// FFT-shift (shifting the zero-frequency component to the center of the array)
//	for (int i=0;i<Np;i++){
//		xk[i] = cmpxDataOut(xn[i]);
//	}
}
*/

// Frequency shift of the FFT
void ShiftFrequency(
		cmpxDataOut xk[Np],
		single_fixed_t1 i_out[Np],
		single_fixed_t1 q_out[Np]
){
	exp_fixed_t1 coef = coef_FF1;// coefficient to nomalize the value back to range [-1,1)
//	const unsigned outorder[Np] = {
//				128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,
//				0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127
//		};

	for(int i=0;i<Np;i++){
#pragma HLS PIPELINE
		double_fixed_t itemp = xk[i].real();
		double_fixed_t qtemp = xk[i].imag();
		dint_t idx = array_ROM_outorder1_init(i);
		i_out[idx] = itemp*coef;
		q_out[idx] = qtemp*coef;
	}
}

// Down Conversion + bit reorder for input second FFT
void DownConversion_array(
		single_fixed_t1 i_in[Np],
		single_fixed_t1 q_in[Np],
		single_fixed_t2 i_out[P/PARALLEL][Np],
		single_fixed_t2 q_out[P/PARALLEL][Np],
		const unsigned orderi,
		const unsigned order
){

	for (int i=0;i<Np;i++){
#pragma HLS PIPELINE
		int phase = order*i;
		switch(phase&3){
		case 0:
			i_out[orderi][i] = i_in[i];
			q_out[orderi][i] = q_in[i];
			break;
		case 1:
			i_out[orderi][i] = q_in[i];
			q_out[orderi][i] = -i_in[i];
			break;
		case 2:
			i_out[orderi][i] = -i_in[i];
			q_out[orderi][i] = -q_in[i];
			break;
		case 3:
			i_out[orderi][i] = -q_in[i];
			q_out[orderi][i] = i_in[i];
			break;
		}
//		printf("i_out[%d][%d]=%f,q_out[%d][%d]=%f\n",orderi,i,(float)i_out[orderi][i],orderi,i,(float)q_out[orderi][i]);
	}
}

// Combine unit function of First part
void preprocess(
		single_fixed_t i_in[LENGTH],
		single_fixed_t q_in[LENGTH],
		single_fixed_t2 i_out[P/PARALLEL][Np],
		single_fixed_t2 q_out[P/PARALLEL][Np],
		const unsigned orderi,
		const unsigned orderj
){
#pragma HLS DATAFLOW
	cmpxDataIn xn[Np];
	cmpxDataOut xk[Np];
#pragma HLS interface ap_fifo depth=FFT_LENGTH port=xn
#pragma HLS interface ap_fifo depth=FFT_LENGTH port=xk
#pragma HLS data_pack variable=xn
#pragma HLS data_pack variable=xk
	single_fixed_t1 i_fft[Np];
	single_fixed_t1 q_fft[Np];
	config_t fft_config;
	status_t fft_status;
	fft_config.setDir(1);
	int order = orderi+orderj*P/PARALLEL;
	fram_windowing(i_in,q_in,xn,orderi);
	//fft_ip_256_array(xn,xk);
	hls::fft<config1>(xn, xk, &fft_status, &fft_config);
	ShiftFrequency(xk,i_fft,q_fft);
	DownConversion_array(i_fft,q_fft,i_out,q_out,orderi,order);

}
void preprocess_parallel(
		single_fixed_t i_in[PARALLEL][LENGTH],
		single_fixed_t q_in[PARALLEL][LENGTH],
		single_fixed_t2 i_out[PARALLEL][P/PARALLEL][Np],
		single_fixed_t2 q_out[PARALLEL][P/PARALLEL][Np]
){
	for(int i=0;i<P/PARALLEL;i++){
		for (int j=0;j<PARALLEL;j++){
#pragma HLS UNROLL
#pragma HLS dependence variable=i_in inter false
#pragma HLS dependence variable=q_in inter false
#pragma HLS dependence variable=i_out inter false
#pragma HLS dependence variable=q_out inter false
			preprocess(i_in[j],q_in[j],i_out[j],q_out[j],i,j);
		}
	}
}

void save_output(
		single_fixed_t2 i_temp[PARALLEL][P/PARALLEL][Np],
		single_fixed_t2 q_temp[PARALLEL][P/PARALLEL][Np],
		single_fixed_t2 i_out[Np][P],
		single_fixed_t2 q_out[Np][P]
){
//	const unsigned outorder[P] = {
//					0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30,
//					1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31
//			};
	for(int i=0;i<Np;i++){
		for(int k=0;k<P/PARALLEL;k++){
#pragma HLS PIPELINE
			for(int j=0;j<PARALLEL;j++){
#pragma HLS UNROLL
				dint_t idx = array_ROM_bitreorder_init((dint_t) j*(P/PARALLEL)+k);
//				int idx = outorder[j*(P/PARALLEL)+k];
				i_out[i][idx] = i_temp[j][k][i];
				q_out[i][idx] = q_temp[j][k][i];
			}
		}
	}
}
// complex Demodulate output of First Part of SCD matrix
void complexDemodulate_array(
		stream<AXI_in >& i_in,
		stream<AXI_in >& q_in,
		single_fixed_t2 i_out[Np][P],
		single_fixed_t2 q_out[Np][P],
		bool &last
){
//#pragma HLS ARRAY_PARTITION variable=i_out complete dim=2
//#pragma HLS ARRAY_PARTITION variable=q_out complete dim=2
#pragma HLS DATAFLOW
	single_fixed_t i_array[PARALLEL][LENGTH];
	single_fixed_t q_array[PARALLEL][LENGTH];
#pragma HLS ARRAY_PARTITION variable=i_array complete dim=1
#pragma HLS ARRAY_PARTITION variable=q_array complete dim=1
	single_fixed_t2 i_temp[PARALLEL][P/PARALLEL][Np];
	single_fixed_t2 q_temp[PARALLEL][P/PARALLEL][Np];
#pragma HLS ARRAY_PARTITION variable=i_temp complete dim=1
#pragma HLS ARRAY_PARTITION variable=q_temp complete dim=1

	save_input(i_in,q_in,i_array,q_array,last);
	preprocess_parallel(i_array,q_array,i_temp,q_temp);
	save_output(i_temp,q_temp,i_out,q_out);
}

// Second Part
// cm_function does the conjugate multiplication of two P size of arrays
void cm_function(
		single_fixed_t2 ibufk[P],
		single_fixed_t2 qbufk[P],
		single_fixed_t2 ibufi[P],
		single_fixed_t2 qbufi[P],
		single_fixed_t3 ibuf[P],
		single_fixed_t3 qbuf[P]
){
#pragma HLS PIPELINE II=1
	exp_fixed_t4 coef = coef_CM;// coefficient to nomalize the value back to range [-1,1)
	for (int j = 0; j < P; j++) { // column loop (both)
#pragma HLS UNROLL
		double_fixed_t2 ii_tmp = ibufk[j] * ibufi[j];
		double_fixed_t2 qq_tmp = qbufk[j] * qbufi[j];
		double_fixed_t2 iq_tmp = ibufk[j] * qbufi[j];
		double_fixed_t2 qi_tmp = qbufk[j] * ibufi[j];
		double_fixed_t2 i_tmp = (ii_tmp + qq_tmp);
		double_fixed_t2 q_tmp = (qi_tmp - iq_tmp);
		ibuf[j] = i_tmp*coef;
		qbuf[j] = q_tmp*coef;
	}
}

void fft_32_function(
		single_fixed_t3 i_in[P],
		single_fixed_t3 q_in[P],
		hls::stream<AXI_inter> out[Phalf],
		const bool mylast
){
//#pragma HLS ARRAY_PARTITION variable=i_in complete dim=1
//#pragma HLS ARRAY_PARTITION variable=q_in complete dim=1
//#pragma HLS ARRAY_PARTITION variable=out complete dim=1
#pragma HLS PIPELINE
	exp_fixed_t2 coef = coef_FF2;// coefficient to nomalize the value back to range [-1,1)
	double_fixed_t3 ibuf[P_log2][P];
#pragma HLS ARRAY_PARTITION variable=ibuf complete dim=2
	double_fixed_t3 qbuf[P_log2][P];
#pragma HLS ARRAY_PARTITION variable=qbuf complete dim=2



	Stage1:for (int i = 0; i < P; i += 2) {
#pragma HLS UNROLL
		//			#pragma HLS PIPELINE II=1
		//				single_fixed_t temp_i = ((ibuf[j+halfsize] * exp_table_32_i[k]) - (qbuf[j+halfsize] * exp_table_32_q[k])) >> 14;
		//				single_fixed_t temp_q = ((ibuf[j+halfsize] * exp_table_32_q[k]) + (qbuf[j+halfsize] * exp_table_32_i[k])) >> 14;
		ibuf[0][i+1] = (i_in[i] - i_in[i+1]) ; // >> 1
		qbuf[0][i+1] = (q_in[i] - q_in[i+1]) ; // >> 1
		ibuf[0][i] = (i_in[i] + i_in[i+1]) ; // >> 1
		qbuf[0][i] = (q_in[i] + q_in[i+1]) ; // >> 1
	}

	int halfsize = 2;
	Stage2:for (int i = 0; i < P; i += 4) {
#pragma HLS UNROLL
		for (int j = i; j < (i+2); j++) {
#pragma HLS UNROLL

			if (j==i){
				//				single_fixed_t temp_q = ((ibuf[j+halfsize] * exp_table_32_q[k]) + (qbuf[j+halfsize] * exp_table_32_i[k])) >> 14;
				ibuf[1][j+halfsize] = (ibuf[0][j] - ibuf[0][j+halfsize]) ; // >> 1
				qbuf[1][j+halfsize] = (qbuf[0][j] - qbuf[0][j+halfsize]) ; // >> 1
				ibuf[1][j] = (ibuf[0][j] + ibuf[0][j+halfsize]) ; // >> 1
				qbuf[1][j] = (qbuf[0][j] + qbuf[0][j+halfsize]) ; // >> 1
			}else{
				ibuf[1][j+halfsize] = (ibuf[0][j] - qbuf[0][j+halfsize]) ; // >> 1
				qbuf[1][j+halfsize] = (qbuf[0][j] + ibuf[0][j+halfsize]) ; // >> 1
				ibuf[1][j] = (ibuf[0][j] + qbuf[0][j+halfsize]) ; // >> 1
				qbuf[1][j] = (qbuf[0][j] - ibuf[0][j+halfsize]) ; // >> 1
			}

		}
	}


	// size = 8
int tablestep;
	for (int idx_s = 2;idx_s<P_log2; idx_s++){
#pragma HLS UNROLL
		halfsize = halfsize*2;
		tablestep = P/2/halfsize;
		Stage3:for (int i = 0; i < P; i += halfsize*2) {
#pragma HLS UNROLL
			int k = 0;
			for (int j = i; j < (i+halfsize); j++) {
#pragma HLS UNROLL
				//			#pragma HLS PIPELINE II=1
				double_fixed_t3 temp_ii = array_ROM_Twiddle_i_init(ibuf[idx_s-1][j+halfsize],k);//double_fixed_t3 (ibuf_S2[j+halfsize]) * exp_table_32_i[k];
				double_fixed_t3 temp_qq = array_ROM_Twiddle_q_init(qbuf[idx_s-1][j+halfsize],k);//double_fixed_t3 (qbuf_S2[j+halfsize]) * exp_table_32_q[k];
				double_fixed_t3 temp_i = temp_ii - temp_qq;
				double_fixed_t3 temp_iq = array_ROM_Twiddle_q_init(ibuf[idx_s-1][j+halfsize],k);//double_fixed_t3 (ibuf_S2[j+halfsize]) * exp_table_32_q[k];
				double_fixed_t3 temp_qi = array_ROM_Twiddle_i_init(qbuf[idx_s-1][j+halfsize],k);//double_fixed_t3 (qbuf_S2[j+halfsize]) * exp_table_32_i[k];
				double_fixed_t3 temp_q = temp_iq + temp_qi;

				//				single_fixed_t temp_i = ((ibuf[j+halfsize] * exp_table_32_i[k]) - (qbuf[j+halfsize] * exp_table_32_q[k])) >> 14;
				//				single_fixed_t temp_q = ((ibuf[j+halfsize] * exp_table_32_q[k]) + (qbuf[j+halfsize] * exp_table_32_i[k])) >> 14;
				ibuf[idx_s][j+halfsize] = (ibuf[idx_s-1][j] - temp_i) ; // >> 1
				qbuf[idx_s][j+halfsize] = (qbuf[idx_s-1][j] - temp_q) ; // >> 1
				ibuf[idx_s][j] = (ibuf[idx_s-1][j] + temp_i) ; // >> 1
				qbuf[idx_s][j] = (qbuf[idx_s-1][j] + temp_q) ; // >> 1
				k += tablestep;
			}
		}
	}


//	const unsigned outorder[Phalf] = {
//					16,17,18,19,20,21,22,23,
//					8,9,10,11,12,13,14,15
//			};// we only need have size of output to SCD function
//#pragma HLS ARRAY_PARTITION variable=outorder complete dim=1

	Write:for (int i = 0; i < Phalf; i++) {// only focus on the middle part
#pragma HLS UNROLL
		single_fixed_t4 itemp,qtemp;
		if (i<Phalf/2){
#pragma HLS UNROLL
			itemp = ibuf[P_log2-1][i+Phalf]*coef;
			qtemp = qbuf[P_log2-1][i+Phalf]*coef;
		}else{
#pragma HLS UNROLL
			itemp = ibuf[P_log2-1][i]*coef;
			qtemp = qbuf[P_log2-1][i]*coef;
		}
		single_fixed_t4 iitemp = itemp*itemp;
		single_fixed_t4 qqtemp = qtemp*qtemp;
		AXI_inter temp;
		temp.data = iitemp+qqtemp;
		temp.last = mylast;
		out[i].write(temp);
	}
}



// combine of unit function of CM, FFT2 and Threshold.
void CMF_function(
		single_fixed_t2 i_in[P],
		single_fixed_t2 q_in[P],
		single_fixed_t2 i_inc[P],
		single_fixed_t2 q_inc[P],
		hls::stream<AXI_inter > myout[Phalf],
		bool mylast
		){
#pragma HLS PIPELINE II=1
	single_fixed_t3 ibuf_cm[P];
#pragma HLS ARRAY_PARTITION variable=ibuf_cm complete dim=1
	single_fixed_t3 qbuf_cm[P];
#pragma HLS ARRAY_PARTITION variable=qbuf_cm complete dim=1

	cm_function(i_in,q_in,i_inc,q_inc,ibuf_cm,qbuf_cm);
	fft_32_function(ibuf_cm,qbuf_cm,myout,mylast);
}

// Prepare for the CMR_thred_function and parallet 2 together
void cm_fft_test( //STRIDE
		single_fixed_t2 i_in[Np][P],
		single_fixed_t2 q_in[Np][P],
		hls::stream<AXI_inter > myout[STRIDE][Phalf],
		bool last
){
//#pragma HLS INLINE recursive
//#pragma HLS PIPELINE II=1

// Two Memories for I/Q stream
	single_fixed_t2 i_mem1[P];
#pragma HLS ARRAY_PARTITION variable=i_mem1 complete dim=1
	single_fixed_t2 q_mem1[P];
#pragma HLS ARRAY_PARTITION variable=q_mem1 complete dim=1

// Copy to Conju Stride
	single_fixed_t2 i_memc[STRIDE][Np/STRIDE][P];
#pragma HLS ARRAY_PARTITION variable=i_memc complete dim=3
#pragma HLS ARRAY_PARTITION variable=i_memc complete dim=1
	single_fixed_t2 q_memc[STRIDE][Np/STRIDE][P];
#pragma HLS ARRAY_PARTITION variable=q_memc complete dim=3
#pragma HLS ARRAY_PARTITION variable=q_memc complete dim=1

	for(int k = 0; k<Np;k+=STRIDE){
		int ks = k/STRIDE;
		for(int l = 0; l<P; l++){
#pragma HLS UNROLL
			for(int ll=0; ll<STRIDE;ll++){
#pragma HLS UNROLL
				i_memc[ll][ks][l] = i_in[k+ll][l];
				q_memc[ll][ks][l] = q_in[k+ll][l];
			}
		}
	}
	bool mylast = false;
	Self_Conjugate:
	for(int k = 0; k <Np/SIZE; k++){//256
	// TODO: set i_in[k] a new vector and pass to the CMF_thred_function_0
		prepare:for(int l = 0; l<P; l++){ // Try to remove this and only pass the input i_in and q_in to CMF_thred_funct but the II of Loop is not 1 due to the dependence of call
#pragma HLS UNROLL
			i_mem1[l] = i_in[k][l];
			q_mem1[l] = q_in[k][l];
		}
		paral_func:for(int i = 0; i < Np/STRIDE; i++){//64
#pragma HLS PIPELINE II=1
#pragma HLS DEPENDENCE variable=i_mem1 inter false
#pragma HLS DEPENDENCE variable=q_mem1 inter false
			if (k==Np/SIZE-1 && i==Np/STRIDE-1){mylast = last;}
			for(int ll=0;ll<STRIDE;ll++){
				CMF_function(i_mem1,q_mem1,i_memc[ll][i],q_memc[ll][i],myout[ll],mylast);
			}
		}
	}
}


void model_SCD(
		hls::stream<AXI_in >& i_AXI_in,
		hls::stream<AXI_in >& q_AXI_in,
		hls::stream<AXI_inter> r_AXI_out[STRIDE][Phalf]
//		hls::stream<single_fixed_t2 >& i_out,
//		hls::stream<single_fixed_t2 >& q_out
){
#pragma HLS INTERFACE axis register port=i_AXI_in
#pragma HLS INTERFACE axis register port=q_AXI_in
#pragma HLS INTERFACE axis register port=r_AXI_out
#pragma HLS INTERFACE s_axilite register port=return
#pragma HLS DATAFLOW
	single_fixed_t2 i_DC[Np][P];
	single_fixed_t2 q_DC[Np][P];
#pragma HLS ARRAY_PARTITION variable=i_DC complete dim=2
#pragma HLS ARRAY_PARTITION variable=q_DC complete dim=2
	static bool last = false;
	complexDemodulate_array(i_AXI_in,q_AXI_in,i_DC,q_DC,last);
	cm_fft_test(i_DC,q_DC,r_AXI_out,last);

}
