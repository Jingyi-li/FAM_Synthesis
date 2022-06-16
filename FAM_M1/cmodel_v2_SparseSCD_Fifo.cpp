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

// Save input stream to an array in length of N
void save_input(
		stream<single_fixed_t6 >& i_in,
		stream<single_fixed_t6 >& q_in,
		single_fixed_t i_out[N],
		single_fixed_t q_out[N]
){
	for(int i=0;i<N;i++){
#pragma HLS PIPELINE
		i_out[i] = i_in.read();
		q_out[i] = q_in.read();
	}
}

// Fram Function + Windowing function
// Fram: divide the signal into P arrays and each array has Np samples, stride over Np/4 samples of each array
// Window: Pass a hamming window filter
void fram_windowing(
		single_fixed_t i_in[N],
		single_fixed_t q_in[N],
		cmpxDataIn xn[Np],
		const unsigned order
){
	int io = order*Np/4;
	for(int i=0; i<Np; i++){
#pragma HLS PIPELINE
		if (io+i<N){
			data_in_t re, im;
			re = i_in[io+i] * hamming_matlab[i];
			im = q_in[io+i] * hamming_matlab[i];
			xn[i] = cmpxDataIn(re, im);
		}else{
			data_in_t re, im;
			re = 0;
			im = 0;
			xn[i] = cmpxDataIn(re, im);
		}
	}
}

// Unscaled FFT IP core with natural input order
void fft_ip_256_array(
		cmpxDataIn xn[Np],
		cmpxDataOut xk[Np],
		const unsigned order
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
}

// Frequency shift of the FFT
void ShiftFrequency(
		cmpxDataOut xk[Np],
		single_fixed_t1 i_out[Np],
		single_fixed_t1 q_out[Np]
){
	double_fixed_t coef = coef_FF1;// coefficient to nomalize the value back to range [-1,1)
	const unsigned outorder[Np] = {
				128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,
				0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127
		};
	for(int i=0;i<Np;i++){
#pragma HLS PIPELINE
		double_fixed_t itemp = xk[i].real();
		double_fixed_t qtemp = xk[i].imag();
		i_out[outorder[i]] = itemp*coef;
		q_out[outorder[i]] = qtemp*coef;
	}
}

// Down Conversion + bit reorder for input second FFT
void DownConversion_array(
		single_fixed_t1 i_in[Np],
		single_fixed_t1 q_in[Np],
		single_fixed_t2 i_out[Np][P],
		single_fixed_t2 q_out[Np][P],
		const unsigned order
){
	const unsigned outorder[P] = {
				0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30,
				1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31
		};
	for (int i=0;i<Np;i++){
#pragma HLS PIPELINE

		int phase = order*i;
		if (phase%4 == 0) { // 2*k*PI: cos = 1, sin = 0.
			i_out[i][outorder[order]] = i_in[i];
			q_out[i][outorder[order]] = q_in[i];
		}
		if (phase%4 == 1) { // (2*k+1)/2*PI: cos = 0, sin = 1.
			i_out[i][outorder[order]] = q_in[i];
			q_out[i][outorder[order]] = -i_in[i];
		}
		if (phase%4 == 2) { // (2*k+1)/2*PI: cos = 0, sin = 1.
			i_out[i][outorder[order]] = -i_in[i];
			q_out[i][outorder[order]] = -q_in[i];
		}
		if (phase%4 == 3) { // (2*k+1)/2*PI: cos = 0, sin = 1.
			i_out[i][outorder[order]] = -q_in[i];
			q_out[i][outorder[order]] = i_in[i];
		}
	}
}


// Combine unit function of First part
void preprocess(
		single_fixed_t i_in[N],
		single_fixed_t q_in[N],
		single_fixed_t2 i_out[Np][P],
		single_fixed_t2 q_out[Np][P],
		const unsigned order
){
#pragma HLS DATAFLOW
	cmpxDataIn xn[Np];
	cmpxDataOut xk[Np];
	single_fixed_t1 i_fft[Np];
	single_fixed_t1 q_fft[Np];

	fram_windowing(i_in,q_in,xn,order);
	fft_ip_256_array(xn,xk,order);
	ShiftFrequency(xk,i_fft,q_fft);
	DownConversion_array(i_fft,q_fft,i_out,q_out,order);

}

// complex Demodulate output of First Part of SCD matrix
void complexDemodulate_array(
		stream<single_fixed_t6 >& i_in,
		stream<single_fixed_t6 >& q_in,
		single_fixed_t2 i_out[Np][P],
		single_fixed_t2 q_out[Np][P]
//		stream<single_fixed_t2 >& i_out_st,
//		stream<single_fixed_t2 >& q_out_st
){
	single_fixed_t i_array[N];
	single_fixed_t q_array[N];
	save_input(i_in,q_in,i_array,q_array);
	for(int i=0;i<P;i++){
		preprocess(i_array,q_array,i_out,q_out,i);
	}
//	for (int i=0;i<PL;i++){
//		for (int j=0;j<NpL;j++){
//			i_out_st.write(i_out[j][i]);
//			q_out_st.write(q_out[j][i]);
//		}
//	}
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
		double_fixed_Mag out[Phalf]
){
//#pragma HLS ARRAY_PARTITION variable=i_in complete dim=1
//#pragma HLS ARRAY_PARTITION variable=q_in complete dim=1
//#pragma HLS ARRAY_PARTITION variable=out complete dim=1
#pragma HLS PIPELINE
	exp_fixed_t2 coef = coef_FF2;// coefficient to nomalize the value back to range [-1,1)
	double_fixed_t3 ibuf_S1[P];
#pragma HLS ARRAY_PARTITION variable=ibuf_S1 complete dim=1
	double_fixed_t3 qbuf_S1[P];
#pragma HLS ARRAY_PARTITION variable=qbuf_S1 complete dim=1
	double_fixed_t3 ibuf_S2[P];
#pragma HLS ARRAY_PARTITION variable=ibuf_S2 complete dim=1
	double_fixed_t3 qbuf_S2[P];
#pragma HLS ARRAY_PARTITION variable=qbuf_S2 complete dim=1
	double_fixed_t3 ibuf_S3[P];
#pragma HLS ARRAY_PARTITION variable=ibuf_S3 complete dim=1
	double_fixed_t3 qbuf_S3[P];
#pragma HLS ARRAY_PARTITION variable=qbuf_S3 complete dim=1
	double_fixed_t3 ibuf_S4[P];
#pragma HLS ARRAY_PARTITION variable=ibuf_S4 complete dim=1
	double_fixed_t3 qbuf_S4[P];
#pragma HLS ARRAY_PARTITION variable=qbuf_S4 complete dim=1
	double_fixed_t3 ibuf_S5[P];
#pragma HLS ARRAY_PARTITION variable=ibuf_S5 complete dim=1
	double_fixed_t3 qbuf_S5[P];
#pragma HLS ARRAY_PARTITION variable=qbuf_S5 complete dim=1

	//size = 2
	int halfsize = 1;
	int tablestep = 16;

	Stage1:for (int i = 0; i < P; i += 2) {
		#pragma HLS UNROLL
		int k = 0;
		for (int j = i; j < (i+halfsize); j++) {
#pragma HLS UNROLL
//			#pragma HLS PIPELINE II=1
			double_fixed_t3 temp_ii = double_fixed_t3 (i_in[j+halfsize]) * exp_table_32_i[k];
			double_fixed_t3 temp_qq = double_fixed_t3 (q_in[j+halfsize]) * exp_table_32_q[k];
			double_fixed_t3 temp_i = temp_ii - temp_qq;
			double_fixed_t3 temp_iq = double_fixed_t3 (i_in[j+halfsize]) * exp_table_32_q[k];
			double_fixed_t3 temp_qi = double_fixed_t3 (q_in[j+halfsize]) * exp_table_32_i[k];
			double_fixed_t3 temp_q = temp_iq + temp_qi;

			//				single_fixed_t temp_i = ((ibuf[j+halfsize] * exp_table_32_i[k]) - (qbuf[j+halfsize] * exp_table_32_q[k])) >> 14;
			//				single_fixed_t temp_q = ((ibuf[j+halfsize] * exp_table_32_q[k]) + (qbuf[j+halfsize] * exp_table_32_i[k])) >> 14;
			ibuf_S1[j+halfsize] = (i_in[j] - temp_i) ; // >> 1
			qbuf_S1[j+halfsize] = (q_in[j] - temp_q) ; // >> 1
			ibuf_S1[j] = (i_in[j] + temp_i) ; // >> 1
			qbuf_S1[j] = (q_in[j] + temp_q) ; // >> 1
			k += tablestep;
		}
	}

	// size = 4
	halfsize = 2;
	tablestep = 8;

	Stage2:for (int i = 0; i < P; i += 4) {
		#pragma HLS UNROLL
		int k = 0;
		for (int j = i; j < (i+halfsize); j++) {
#pragma HLS UNROLL
//			#pragma HLS PIPELINE II=1
			double_fixed_t3 temp_ii = double_fixed_t3 (ibuf_S1[j+halfsize]) * exp_table_32_i[k];
			double_fixed_t3 temp_qq = double_fixed_t3 (qbuf_S1[j+halfsize]) * exp_table_32_q[k];
			double_fixed_t3 temp_i = temp_ii - temp_qq;
			double_fixed_t3 temp_iq = double_fixed_t3 (ibuf_S1[j+halfsize]) * exp_table_32_q[k];
			double_fixed_t3 temp_qi = double_fixed_t3 (qbuf_S1[j+halfsize]) * exp_table_32_i[k];
			double_fixed_t3 temp_q = temp_iq + temp_qi;

			//				single_fixed_t temp_i = ((ibuf[j+halfsize] * exp_table_32_i[k]) - (qbuf[j+halfsize] * exp_table_32_q[k])) >> 14;
			//				single_fixed_t temp_q = ((ibuf[j+halfsize] * exp_table_32_q[k]) + (qbuf[j+halfsize] * exp_table_32_i[k])) >> 14;
			ibuf_S2[j+halfsize] = (ibuf_S1[j] - temp_i) ; // >> 1
			qbuf_S2[j+halfsize] = (qbuf_S1[j] - temp_q) ; // >> 1
			ibuf_S2[j] = (ibuf_S1[j] + temp_i) ; // >> 1
			qbuf_S2[j] = (qbuf_S1[j] + temp_q) ; // >> 1
			k += tablestep;
		}
	}

	// size = 8
	halfsize = 4;
	tablestep = 4;

	Stage3:for (int i = 0; i < P; i += 8) {
		#pragma HLS UNROLL
		int k = 0;
		for (int j = i; j < (i+halfsize); j++) {
#pragma HLS UNROLL
//			#pragma HLS PIPELINE II=1
			double_fixed_t3 temp_ii = double_fixed_t3 (ibuf_S2[j+halfsize]) * exp_table_32_i[k];
			double_fixed_t3 temp_qq = double_fixed_t3 (qbuf_S2[j+halfsize]) * exp_table_32_q[k];
			double_fixed_t3 temp_i = temp_ii - temp_qq;
			double_fixed_t3 temp_iq = double_fixed_t3 (ibuf_S2[j+halfsize]) * exp_table_32_q[k];
			double_fixed_t3 temp_qi = double_fixed_t3 (qbuf_S2[j+halfsize]) * exp_table_32_i[k];
			double_fixed_t3 temp_q = temp_iq + temp_qi;

			//				single_fixed_t temp_i = ((ibuf[j+halfsize] * exp_table_32_i[k]) - (qbuf[j+halfsize] * exp_table_32_q[k])) >> 14;
			//				single_fixed_t temp_q = ((ibuf[j+halfsize] * exp_table_32_q[k]) + (qbuf[j+halfsize] * exp_table_32_i[k])) >> 14;
			ibuf_S3[j+halfsize] = (ibuf_S2[j] - temp_i) ; // >> 1
			qbuf_S3[j+halfsize] = (qbuf_S2[j] - temp_q) ; // >> 1
			ibuf_S3[j] = (ibuf_S2[j] + temp_i) ; // >> 1
			qbuf_S3[j] = (qbuf_S2[j] + temp_q) ; // >> 1
			k += tablestep;
		}
	}

	// size = 16
	halfsize = 8;
	tablestep = 2;

	Stage4:for (int i = 0; i < P; i += 16) {
		#pragma HLS UNROLL
		int k = 0;
		for (int j = i; j < (i+halfsize); j++) {
#pragma HLS UNROLL
//			#pragma HLS PIPELINE II=1
			double_fixed_t3 temp_ii = double_fixed_t3 (ibuf_S3[j+halfsize]) * exp_table_32_i[k];
			double_fixed_t3 temp_qq = double_fixed_t3 (qbuf_S3[j+halfsize]) * exp_table_32_q[k];
			double_fixed_t3 temp_i = temp_ii - temp_qq;
			double_fixed_t3 temp_iq = double_fixed_t3 (ibuf_S3[j+halfsize]) * exp_table_32_q[k];
			double_fixed_t3 temp_qi = double_fixed_t3 (qbuf_S3[j+halfsize]) * exp_table_32_i[k];
			double_fixed_t3 temp_q = temp_iq + temp_qi;

			//				single_fixed_t temp_i = ((ibuf[j+halfsize] * exp_table_32_i[k]) - (qbuf[j+halfsize] * exp_table_32_q[k])) >> 14;
			//				single_fixed_t temp_q = ((ibuf[j+halfsize] * exp_table_32_q[k]) + (qbuf[j+halfsize] * exp_table_32_i[k])) >> 14;
			ibuf_S4[j+halfsize] = (ibuf_S3[j] - temp_i) ; // >> 1
			qbuf_S4[j+halfsize] = (qbuf_S3[j] - temp_q) ; // >> 1
			ibuf_S4[j] = (ibuf_S3[j] + temp_i) ; // >> 1
			qbuf_S4[j] = (qbuf_S3[j] + temp_q) ; // >> 1
			k += tablestep;
		}
	}

	// size = 32
	halfsize = 16;
	tablestep = 1;

	Stage5:for (int i = 0; i < P; i += 32) {
		#pragma HLS UNROLL
		int k = 0;
		for (int j = i; j < (i+halfsize); j++) {
#pragma HLS UNROLL
//			#pragma HLS PIPELINE II=1
			double_fixed_t3 temp_ii = double_fixed_t3 (ibuf_S4[j+halfsize]) * exp_table_32_i[k];
			double_fixed_t3 temp_qq = double_fixed_t3 (qbuf_S4[j+halfsize]) * exp_table_32_q[k];
			double_fixed_t3 temp_i = temp_ii - temp_qq;
			double_fixed_t3 temp_iq = double_fixed_t3 (ibuf_S4[j+halfsize]) * exp_table_32_q[k];
			double_fixed_t3 temp_qi = double_fixed_t3 (qbuf_S4[j+halfsize]) * exp_table_32_i[k];
			double_fixed_t3 temp_q = temp_iq + temp_qi;

			//				single_fixed_t temp_i = ((ibuf[j+halfsize] * exp_table_32_i[k]) - (qbuf[j+halfsize] * exp_table_32_q[k])) >> 14;
			//				single_fixed_t temp_q = ((ibuf[j+halfsize] * exp_table_32_q[k]) + (qbuf[j+halfsize] * exp_table_32_i[k])) >> 14;
			ibuf_S5[j+halfsize] = (ibuf_S4[j] - temp_i) ; // >> 1
			qbuf_S5[j+halfsize] = (qbuf_S4[j] - temp_q) ; // >> 1
			ibuf_S5[j] = (ibuf_S4[j] + temp_i) ; // >> 1
			qbuf_S5[j] = (qbuf_S4[j] + temp_q) ; // >> 1
			k += tablestep;
		}
	}

//	const unsigned outorder[Phalf] = {
//					16,17,18,19,20,21,22,23,
//					8,9,10,11,12,13,14,15
//			};// we only need have size of output to SCD function
//#pragma HLS ARRAY_PARTITION variable=outorder complete dim=1
//
//	Write:for (int i = 0; i < Phalf; i++) {// only focus on the middle part
//#pragma HLS UNROLL
//		single_fixed_t4 itemp = ibuf_S5[outorder[i]]*coef;
//		single_fixed_t4 qtemp = qbuf_S5[outorder[i]]*coef;
//		out[i] = itemp*itemp+qtemp*qtemp;
//	}

	Write:for (int i = 8; i < Phalf+8; i++) {// only focus on the middle part
#pragma HLS UNROLL
		if (i<16){
			single_fixed_t4 itemp = ibuf_S5[i]*coef;
			single_fixed_t4 qtemp = qbuf_S5[i]*coef;
			out[i] = itemp*itemp+qtemp*qtemp;
		}else{
			single_fixed_t4 itemp = ibuf_S5[i]*coef;
			single_fixed_t4 qtemp = qbuf_S5[i]*coef;
			out[i-16] = itemp*itemp+qtemp*qtemp;
		}

	}
}


// Afer the Second FFT, only save the value whose I^2+Q^2 > THRESHOLD
// save the value and location in SCD matrix
void Threshold_function_FIFO(//one input and one output
		double_fixed_Mag myin[Phalf],
		const int k,
		const int i,
		hls::stream<myDataSCD > myout[Phalf]
){
//#pragma HLS ARRAY_PARTITION variable=q_in complete dim=1
//#pragma HLS ARRAY_PARTITION variable=i_in complete dim=1
//#pragma HLS INTERFACE axis port=myout

#pragma HLS PIPELINE
	single_fixed_t4 thred = THRESHOLD;
	int alabel = k+i-256;
	int flabel = 8*(i-k-1);

	for(int l=0; l<Phalf; l++){
#pragma HLS UNROLL
		if(myin[l]>=thred){
			myDataSCD mytemp;
			mytemp.value = myin[l];
			mytemp.al = alabel; //alpha frequency label
			mytemp.fl = flabel+l; //frequency label
			myout[l].write(mytemp);
		}
	}
}


// combine of unit function of CM, FFT2 and Threshold.
void CMF_thred_function(
		single_fixed_t2 i_in[P],
		single_fixed_t2 q_in[P],
		single_fixed_t2 i_inc[P],
		single_fixed_t2 q_inc[P],
		const int k,
		const int i,
		hls::stream<myDataSCD > myout[Phalf]
		){
#pragma HLS PIPELINE II=1
	single_fixed_t3 ibuf_cm[P];
#pragma HLS ARRAY_PARTITION variable=ibuf_cm complete dim=1
	single_fixed_t3 qbuf_cm[P];
#pragma HLS ARRAY_PARTITION variable=qbuf_cm complete dim=1
	double_fixed_Mag buf_fft[Phalf];
#pragma HLS ARRAY_PARTITION variable=buf_fft complete dim=1

	cm_function(i_in,q_in,i_inc,q_inc,ibuf_cm,qbuf_cm);
	fft_32_function(ibuf_cm,qbuf_cm,buf_fft);
	Threshold_function_FIFO(buf_fft,k,i,myout);
}

// Prepare for the CMR_thred_function and parallet 2 together
void cm_fft_test_v2( //STRIDE
		single_fixed_t2 i_in[Np][P],
		single_fixed_t2 q_in[Np][P],
		hls::stream<myDataSCD > myout0[Phalf],
		hls::stream<myDataSCD > myout1[Phalf],
		bool *out_last

){
//#pragma HLS INLINE recursive
//#pragma HLS PIPELINE II=1

// Two Memories for I/Q stream
	single_fixed_t2 i_mem1[P];
#pragma HLS ARRAY_PARTITION variable=i_mem1 complete dim=1
	single_fixed_t2 q_mem1[P];
#pragma HLS ARRAY_PARTITION variable=q_mem1 complete dim=1

// Copy to Conju Stride
	single_fixed_t2 i_memc[Np/STRIDE][P];
#pragma HLS ARRAY_PARTITION variable=i_memc complete dim=2
	single_fixed_t2 q_memc[Np/STRIDE][P];
#pragma HLS ARRAY_PARTITION variable=q_memc complete dim=2
	single_fixed_t2 i_memc1[Np/STRIDE][P];
#pragma HLS ARRAY_PARTITION variable=i_memc1 complete dim=2
	single_fixed_t2 q_memc1[Np/STRIDE][P];
#pragma HLS ARRAY_PARTITION variable=q_memc1 complete dim=2

	for(int k = 0; k<Np;k+=STRIDE){
		int ks = k/STRIDE;
		for(int l = 0; l<P; l++){
#pragma HLS UNROLL
			i_memc[ks][l] = i_in[k][l];
			q_memc[ks][l] = q_in[k][l];
			i_memc1[ks][l] = i_in[k+1][l];
			q_memc1[ks][l] = q_in[k+1][l];

		}
	}
	*out_last = false;
	Self_Conjugate:
	for(int k = 0; k <Np; k++){//256
	// TODO: set i_in[k] a new vector and pass to the CMF_thred_function_0
		prepare:for(int l = 0; l<P; l++){ // Try to remove this and only pass the input i_in and q_in to CMF_thred_funct but the II of Loop is not 1 due to the dependence of call
#pragma HLS UNROLL
			i_mem1[l] = i_in[k][l];
			q_mem1[l] = q_in[k][l];
		}
		paral_func:for(int i = 0; i < Np; i+=STRIDE){//64
#pragma HLS PIPELINE II=1
#pragma HLS DEPENDENCE variable=i_mem1 inter false
#pragma HLS DEPENDENCE variable=q_mem1 inter false
			int ii = i/STRIDE;
			CMF_thred_function(i_mem1,q_mem1,i_memc[ii],q_memc[ii],k,i,myout0);
			CMF_thred_function(i_mem1,q_mem1,i_memc1[ii],q_memc1[ii],k,i+1,myout1);

		}
	}
	*out_last = true;
}

// Third part of SCD function, change 32 stream out to one out
int Transfer_multi2one_v2(
		hls::stream<myDataSCD > in0[Phalf],
		hls::stream<myDataSCD > in1[Phalf],
		stream<myDataSCD >& out
){
	int countn = 0;
	myDataSCD temp;
//TODO How we will know when to get the data for second signal??

	for (int i=0;i<800;i++){

	for (int l=0; l<Phalf; l++){
#pragma HLS PIPELINE
		if (in0[l].read_nb(temp)){
			out.write(temp);
			countn +=1;
		}
	}
	for (int l=0; l<Phalf; l++){
#pragma HLS PIPELINE
		if (in1[l].read_nb(temp)){
			out.write(temp);
			countn+=1;
		}
	}
	}
	return countn;
}


void model_SCD(
		hls::stream<AXI_in >& i_AXI_in,
		hls::stream<AXI_in >& q_AXI_in,
		hls::stream<AXI_out>& r_AXI_out
){
#pragma HLS INTERFACE axis register port=i_AXI_in
#pragma HLS INTERFACE axis register port=q_AXI_in
#pragma HLS INTERFACE axis register port=r_AXI_out
#pragma HLS DATAFLOW
	hls::stream<myDataSCD > myout0[Phalf];
	hls::stream<myDataSCD > myout1[Phalf];
	hls::stream<single_fixed_t6> i_in;
	hls::stream<single_fixed_t6> q_in;
	hls::stream<myDataSCD> out;
		single_fixed_t2 i_DC[Np][P];
		single_fixed_t2 q_DC[Np][P];
#pragma HLS ARRAY_PARTITION variable=i_DC complete dim=2
#pragma HLS ARRAY_PARTITION variable=q_DC complete dim=2
		for (int ii = 0;ii<2048;ii++){
			AXI_in i_temp=i_AXI_in.read();
			AXI_in q_temp=q_AXI_in.read();
			i_in.write(i_temp.data);
			q_in.write(q_temp.data);
		}
		AXI_out r_temp;
		bool out_last;
	complexDemodulate_array(i_in,q_in,i_DC,q_DC);
	cm_fft_test_v2(i_DC,q_DC,myout0,myout1, &out_last);
	int countn = Transfer_multi2one_v2(myout0,myout1,out);
	output:for (int i=0;i<countn;i++){
		if (i==countn-1){
			r_temp.last = true;
		}
		else{r_temp.last = false;}
		r_temp.data = out.read();
		r_AXI_out.write(r_temp);
	}
}
