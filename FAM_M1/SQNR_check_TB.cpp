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
		hls::stream<AXI_inter> r_AXI_out1i[P],
		hls::stream<AXI_inter> r_AXI_out1q[P],
		hls::stream<AXI_inter> r_AXI_out2i[P],
		hls::stream<AXI_inter> r_AXI_out2q[P]
);

int main(int argc, char **argv) {
//Setup input and output AXI stream
	AXI_in stream_i_in, stream_q_in;
	hls::stream<AXI_in > i_in("i_in_stream");
	hls::stream<AXI_in > q_in("q_in_stream");
	hls::stream<AXI_inter> r_out1i[P];
	hls::stream<AXI_inter> r_out1q[P];
	hls::stream<AXI_inter> r_out2i[P];
	hls::stream<AXI_inter> r_out2q[P];

	int Np_ = Np;
	int L_ = L; // 64
	int P_ = P;

	int in_size = (P_-1)*L_ + Np_; // P*L = 2048 or (P-1)*L + Np = 2240
	int out_size = Np_*P_/2; // 4096

	FILE *input_file  = fopen(fileinput,"r");// Input signal file
	float a,b,c,d;// a is Signal.real b is Signal.image

	for (int i = 0; i <= (in_size); i++) { // 2241
		if (i < P_*L_-1) {
			fscanf(input_file,"%f,%f",&a,&b);
			stream_i_in.data = single_fixed_t6 (a);
			stream_q_in.data = single_fixed_t6 (b);
			stream_i_in.last = false;
			stream_q_in.last = false;
			i_in.write(stream_i_in);
			q_in.write(stream_q_in);
		}if (i==P_*L_-1){
			fscanf(input_file,"%f,%f",&a,&b);
			stream_i_in.data = single_fixed_t6 (a);
			stream_q_in.data = single_fixed_t6 (b);
			stream_i_in.last = true;
			stream_q_in.last = true;
			i_in.write(stream_i_in);
			q_in.write(stream_q_in);
		}

	}
	fclose(input_file);

	model_SCD(i_in,q_in,r_out1i,r_out1q,r_out2i,r_out2q);

	FILE *outFile = fopen("/home/jingyi/SCDfunctions/Paper/Synthesis_M1_SCD/SCDresult.dat","w");// Result file save path
	FILE *error_file = fopen(errorfile,"w");
	FILE *result_file = fopen(fileresult,"r");
//	FILE *resultFile = fopen("/home/jingyi/Desktop/Synthesis_M2_SCD/src/matlab/deepsig_float_result_mag.dat","r");
	printf("Save file \n");
//	float c,d;// c is the floating result from matlab

	float error_avg = 0, signal_avg = 0, sum_is = 0, sum_qs = 0, sum_ir=0, sum_qr=0;
	AXI_inter rtemp1i;
	AXI_inter rtemp1q;
	AXI_inter rtemp2i;
	AXI_inter rtemp2q;
	float noise_sum=0, signal_sum=0;
	for (int i = 0; i < 32768; i++) {
		for (int j = 0; j < P; j++) {
			rtemp1i = r_out1i[j].read();
			rtemp1q = r_out1q[j].read();
			single_fixed_t4 temp1i = rtemp1i.data;
			single_fixed_t4 temp1q = rtemp1q.data;
//			fscanf(resultFile,"%f,%f,",&c,&d);
			fscanf(result_file,"%f,%f,",&a,&b);
			sum_is += a;
			sum_qs += b;
			float itemp_err = (float)temp1i - a;
			float qtemp_err = (float)temp1q - b;
			fprintf(error_file, "%.*f,%.*f,", OUTBIT,(float)itemp_err,OUTBIT,(float)qtemp_err);
			sum_ir += itemp_err;
			sum_qr += qtemp_err;

			fprintf(outFile, "%.*f,%.*f,", OUTBIT,(float)temp1i,OUTBIT,(float)temp1q);
//			if (i==3){
//				printf("i_out[%d] = %.*f, q_out[%d] = %.*f\n",j, OUTBIT,(float)temp1i,j,OUTBIT, (float)temp1q);
//			}
		}
		fprintf(outFile, "\n");
		for (int j = 0; j < P; j++) {
			rtemp2i = r_out2i[j].read();
			rtemp2q = r_out2q[j].read();
			single_fixed_t4 temp2i = rtemp2i.data;
			single_fixed_t4 temp2q = rtemp2q.data;
//			ffscanf(resultFile,"%f,%f,",&c,&d);
			fscanf(result_file,"%f,%f,",&a,&b);
			sum_is += a;
			sum_qs += b;
			float itemp_err = (float)temp2i - a;
			float qtemp_err = (float)temp2q - b;
			fprintf(error_file, "%.*f,%.*f,", OUTBIT,(float)itemp_err,OUTBIT,(float)qtemp_err);
			sum_ir += itemp_err;
			sum_qr += qtemp_err;
			fprintf(outFile, "%.*f,%.*f,", OUTBIT,(float)temp2i,OUTBIT,(float)temp2q);
//			if (i==3){
//				printf("i_out[%d] = %.*f, q_out[%d] = %.*f\n",j+16, OUTBIT,(float)temp2i,j+16,OUTBIT, (float)temp2q);
//			}
		}
		fprintf(outFile, "\n");
	}
	fclose(outFile);
	fclose(error_file);
//	fclose(resultFile);

	float avg_is = sum_is/65536/32;
	float avg_qs = sum_qs/65536/32;
	float avg_ir = sum_ir/65536/32;
	float avg_qr = sum_qr/65536/32;
	printf( "Results are good \n");
	sum_is = 0, sum_qs = 0, sum_ir = 0, sum_qr = 0;
	FILE *result_file2 = fopen(fileresult,"r");
	FILE *error_file2 = fopen(errorfile,"r");
	for (int i = 0; i < 65536; i++) {
		for (int j = 0; j < 32; j++) {
			fscanf(result_file2,"%f,%f,",&a,&b);
			fscanf(error_file2,"%f,%f,",&c,&d);
			sum_is += pow(a-avg_is,2);
			sum_qs += pow(b-avg_qs,2);
			sum_ir += pow(c-avg_ir,2);
			sum_qr += pow(d-avg_qr,2);
		}
		}
	fclose(result_file2);
	fclose(error_file2);
	float result = 10*log10((sum_is+sum_qs)/(sum_ir+sum_qr));
	printf( "The SQNR of output is: %f dB\n", result);

//	float SNR = 10*log10(signal_sum/noise_sum);
//	printf("The SNR of the output is: %fdB", SNR);
	return 0;
}
