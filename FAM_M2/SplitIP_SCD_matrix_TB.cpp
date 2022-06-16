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
		hls::stream<AXI_inter> r_AXI_out1[Phalf],
		hls::stream<AXI_inter> r_AXI_out2[Phalf]
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
	hls::stream<AXI_inter> r_out1[Phalf];
	hls::stream<AXI_inter> r_out2[Phalf];
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
			stream_i_in.data =  a;
			stream_q_in.data =  b;
			stream_i_in.last = false;
			stream_q_in.last = false;
			i_in.write(stream_i_in);
			q_in.write(stream_q_in);
		}if (i==P_*L_-1){
			fscanf(input_file,"%f,%f",&a,&b);
			stream_i_in.data = a;
			stream_q_in.data = b;
			stream_i_in.last = true;
			stream_q_in.last = true;
//			printf("%f,%f\n",(float)stream_i_in.data,(float)stream_q_in.data);
//			printf("%d,%d\n",(int)stream_i_in.last,(int)stream_q_in.last);
			i_in.write(stream_i_in);
			q_in.write(stream_q_in);
		}

	}
	fclose(input_file);

//	model_SCD(i_in,q_in,i_out,q_out);

//	for(int i=0;i<Np;i++){
//		for(int j=0;j<P;j++){
//			single_fixed_t2 tempi = i_out.read();
//			single_fixed_t2 tempq = q_out.read();
////			if (j==15){
//				printf("i_out[%d][%d] = %f, q_out[%d][%d] = %f\n",i,j,tempi.to_float(), i,j,tempq.to_float());
////			}
//		}
//	}


	model_SCD(i_in,q_in,r_out1,r_out2);
	FILE *outFile = fopen("/home/jingyi/SCDfunctions/Paper/Synthesis_M2_SCD/SCDresult.dat","w");// Result file save path
	FILE *resultFile = fopen("/home/jingyi/SCDfunctions/Paper/Synthesis_M2_SCD/src/matlab/deepsig_float_result_mag.dat","r");
	FILE *lastFile = fopen("/home/jingyi/SCDfunctions/Paper/Synthesis_M2_SCD/SCDlast.dat","w");
	printf("Save file \n");
	float c;// c is the floating result from matlab

	int outbit = 10;
	AXI_inter rtemp1;
	AXI_inter rtemp2;
	float noise_sum=0, signal_sum=0;
	for (int i = 0; i < OUTLOOPS/SIZE; i++) {
		for (int j = 0; j < Phalf; j++) {
			rtemp1 = r_out1[j].read();
			double_fixed_Mag temp1 = (double_fixed_Mag)rtemp1.data;
			fscanf(resultFile,"%f,",&c);
			noise_sum +=pow(sqrt(c)-sqrt((float)temp1),2);
			signal_sum += c;
			fprintf(outFile, "%.*f,", outbit,(float)temp1);
			fprintf(lastFile, "%d,", rtemp1.last);
//			if (i==3){
//				printf("q_out[%d][%d] = %f, result = %f\n",i, j, (float)temp1,c);
//			}
		}
		fprintf(outFile, "\n");
		for (int j = 0; j < Phalf; j++) {
			rtemp2 = r_out2[j].read();
			double_fixed_Mag temp2 = (double_fixed_Mag)rtemp2.data;
			fscanf(resultFile,"%f,",&c);
			noise_sum +=pow(sqrt(c)-sqrt((float)temp2),2);
			signal_sum += c;
			fprintf(outFile, "%.*f,",outbit,(float)temp2);
			fprintf(lastFile, "%d,", rtemp2.last);
//			if (i==3){
//				printf("q_out[%d][%d] = %f, result = %f\n",i, j, (float)temp2,c);
//			}
		}
		fprintf(outFile, "\n");
		fprintf(lastFile, "\n");
	}
	fclose(outFile);
	fclose(resultFile);
	fclose(lastFile);

	float SNR = 10*log10(signal_sum/noise_sum);
	printf("The SNR of the output is: %fdB", SNR);

	return 0;
}
