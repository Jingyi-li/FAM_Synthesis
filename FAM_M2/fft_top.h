#ifndef _MY_FFT_
#define _MY_FFT_

#include "ap_fixed.h"
#include "hls_fft.h"
#include "types.h"
#include "auto.h"

// configurable params
const char FFT_INPUT_WIDTH                     = (F1f+7)/8*8; // default: 16
const char FFT_CONFIG_WIDTH                    = 8;//16;
const char FFT_NFFT_MAX                        = 8; // default: 10

const char FFT_OUTPUT_WIDTH                    = FFT_INPUT_WIDTH+FFT_NFFT_MAX+1;
const char ROUND_FFT_OUTPUT_WIDTH			   = (FFT_OUTPUT_WIDTH + 7)/8*8;
const int  FFT_LENGTH                          = 1 << FFT_NFFT_MAX; // 256
const char PHASE_FACTOR_WIDTH                  = F1f+2; // default: 16

using namespace std;

// FFT Struct Parameters
struct config1 : hls::ip_fft::params_t {
    static const unsigned ordering_opt = hls::ip_fft::natural_order; // bit_reversed_order
    static const unsigned config_width = FFT_CONFIG_WIDTH;
    static const unsigned arch_opt = hls::ip_fft::pipelined_streaming_io; // default
    static const unsigned scaling_opt = hls::ip_fft::unscaled; // default
    static const unsigned rounding_opt = hls::ip_fft::convergent_rounding;//convergent_rounding; // truncation
    static const unsigned butterfly_type = hls::ip_fft::use_xtremedsp_slices; // use_luts
    static const unsigned max_nfft = FFT_NFFT_MAX; // 8
    static const unsigned input_width = FFT_INPUT_WIDTH; // 18
    static const unsigned output_width = ROUND_FFT_OUTPUT_WIDTH; // 18
    static const unsigned phase_factor_width = PHASE_FACTOR_WIDTH; // 18
    static const unsigned type = hls::ip_fft::fixed_point;
};


typedef hls::ip_fft::config_t<config1> config_t;
typedef hls::ip_fft::status_t<config1> status_t;

//typedef ap_fixed<FFT_INPUT_WIDTH,1> data_t;
typedef ap_fixed<FFT_INPUT_WIDTH,1> data_in_t;
//typedef ap_fixed<FFT_OUTPUT_WIDTH,FFT_OUTPUT_WIDTH-FFT_INPUT_WIDTH+1> data_out_t;
typedef ap_fixed<ROUND_FFT_OUTPUT_WIDTH, ROUND_FFT_OUTPUT_WIDTH-FFT_INPUT_WIDTH+1> data_out_t;

#include <complex>
typedef std::complex<data_in_t> cmpxDataIn;
typedef std::complex<data_out_t> cmpxDataOut;

#endif
