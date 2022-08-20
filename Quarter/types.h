#ifndef _MY_TYPES_
#define _MY_TYPES_

#include "ap_int.h"
#include "ap_fixed.h"
#include "auto.h"
#include <iostream>
#include "hls_stream.h"

#define EXP_W 18
#define EXP_I 1
#define IQ_W 16
#define IQ_I 1
#define PI 3.14159265358979323846


#define STRIDE 2
#define Np 256//16
#define L Np/4
#define P 32//8
#define P_log2 5
#define Phalf P/2 // half size of PL
#define N 2048//32 // the length of input data
#define STRIDE_N Np*Np/STRIDE
#define OUTLOOPS FULLLOOPS/STRIDE
#define FULLLOOPS Np*Np
#define THRESHOLD 0.05
#define OUTBIT 20
#define SIZE 2
#define QUARTER 2 // 2 means quarter size

#define PARALLEL 4
#define LENGTH L*(P/PARALLEL-1)+Np

//small size:
//#define STRIDE 2
//#define Np 16
//#define L Np/4
//#define P 32
//#define Phalf P/2 // half size of PL
//#define N 128 // the length of input data
//#define STRIDE_N Np*Np/STRIDE
//#define OUTLOOPS Np*Np/2
//#define THRESHOLD 0.05
//#define OUTBIT 20

typedef unsigned char dint_t;
typedef ap_fixed<18, 1, AP_RND, AP_SAT> exp_fixed_t; // exponential coefficients <18, 1>
typedef ap_fixed<F1f+2, 1, AP_RND, AP_SAT> exp_fixed_t1; // exponential coefficients for First FFT
typedef ap_fixed<F2f+2, 1, AP_RND, AP_SAT> exp_fixed_t2; // exponential coefficients for Second FFT
typedef ap_fixed<Wf+2, 1, AP_RND, AP_SAT> exp_fixed_t3; // exponential coefficients <18, 1>
typedef ap_fixed<CMf+3, 2, AP_RND, AP_SAT> exp_fixed_t4; // exponential coefficients <18, 1>
/********** Preprocess data type **********/
// i_error_max = 0.000031,  q_error_max = 0.000031, RMSE = 0.000000 (ap_fixed<24,2>, ap_fixed<16,1>)
// i_error_max = 0.000008,  q_error_max = 0.000008, RMSE = 0.000000 (ap_fixed<24,2>, ap_fixed<18,1>)
typedef ap_fixed<16, 1, AP_RND, AP_SAT> single_fixed_t6; // Input <16, 1> or <18, 1>
typedef ap_fixed<Wf, 1, AP_RND, AP_SAT> single_fixed_t; // Input <16, 1> or <18, 1>

typedef ap_fixed<F1f+8, 9, AP_RND, AP_SAT> double_fixed_t; // 256-pt FFT intermediate <32, 2>
typedef ap_fixed<F1f, 1, AP_RND, AP_SAT> single_fixed_t1; // 256-pt FFT <24, 1>

typedef ap_fixed<F1f+1, 2, AP_RND, AP_SAT> double_fixed_t1; // Down Conversion intermediate <32, 2>
typedef ap_fixed<F1f, 1, AP_RND, AP_SAT> single_fixed_t2; // Down Conversion <24, 1>


/********** SCD Matrix data type **********/
// i_error_max = 0.000031,  q_error_max = 0.000031, RMSE = 0.000000 (ap_fixed<24,2>, ap_fixed<16,1>)
// i_error_max = 0.000008,  q_error_max = 0.000008, RMSE = 0.000000 (ap_fixed<24,2>, ap_fixed<18,1>)
typedef ap_fixed<CMf+1, 2, AP_RND, AP_SAT> double_fixed_t2; // Conjugate Mult intermediate <32, 2>
typedef ap_fixed<CMf, 1, AP_RND, AP_SAT> single_fixed_t3; // Conjugate Mult <24, 1>

typedef ap_fixed<F2f+5, 6, AP_RND, AP_SAT> double_fixed_t3; // 32-pt FFT intermediate <32, 2>
typedef ap_fixed<F2f, 1, AP_RND, AP_SAT> single_fixed_t4; // 32-pt FFT <24, 1>
typedef ap_fixed<F2f+1, 2, AP_RND, AP_SAT> double_fixed_Mag; // 32-pt FFT <24, 1>
// verified!
typedef ap_fixed<24, 2, AP_RND, AP_SAT> double_fixed_t4; // alpha_sqr <32, 2>
typedef ap_fixed<16, 1, AP_RND, AP_SAT> single_fixed_t5; // alpha_out <24, 1>

typedef ap_fixed<12, 12, AP_RND, AP_SAT> int_12; // SCD matrix Frequency label 2^12 = 4096
typedef ap_fixed<9, 9, AP_RND, AP_SAT> int_9; // SCD matrix Alpha Frequency label 2^9 = 512

typedef struct myDataSCD{
	single_fixed_t4 value;
	int_12 fl;
	int_9 al;
} myDataSCD;
typedef struct myDataSCDv2{
	float value;
	short int fl;
	short int al;
} myDataSCDv2;

typedef struct AXI_in{
	float data;
    bool last;
} AXI_in;

typedef struct AXI_in_check{
//	single_fixed_t6 data;
	float data;
    bool last;
} AXI_in_check;

typedef struct AXI_inter{
	single_fixed_t4 data;
    bool last;
} AXI_inter;

typedef struct AXI_out{
	myDataSCD data;
    bool last;
} AXI_out;
typedef struct AXI_SCD{
	long int data;
    bool last;
} AXI_SCD;
typedef struct AXI_value{
	float value;
    bool last;
} AXI_value;
typedef struct AXI_label{
	short int value;
    bool last;
} AXI_label;

typedef union {
	myDataSCDv2 i;
	long int f;
} SCDconvert;
typedef hls::stream<AXI_in> AXIS_IN;
typedef hls::stream<AXI_out> AXIS_OUT;
/*** sqrt parameters
#include "hls_dsp.h"
const int DataFormat = hls::CORDIC_FORMAT_USIG_INT;
const int InputWidth = 30;
const int OutputWidth = 16; // Output width for integer square root must be (InputWidth/2)+1
const int RoundMode = hls::CORDIC_ROUND_TRUNCATE;

void sqrt_top(const hls::sqrt_input<InputWidth, DataFormat>::in &x,
              hls::sqrt_output<OutputWidth, DataFormat>::out &sqrtX);
***/
//extern void Threshold_function_FIFO_1(//one input and one output
//		single_fixed_t4 i_in[PLhalf],
//		single_fixed_t4 q_in[PLhalf],
//		const int k,
//		const int i,
//		hls::stream<myDataSCD > myout[PLhalf]
//);
#endif
