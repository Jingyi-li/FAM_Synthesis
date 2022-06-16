function [PsignalN,PnoiseN,expN] = FAM_M1(Ps,nFFT1,nFFT2,bits,bit_fft1,bit_multi,bit_fft2)
%THEORYFAM Summary of this function goes here
%   Detailed explanation goes here
m1 = log2(nFFT1);
m2 = log2(nFFT2);
A1 = 2-(m1+1.5)/nFFT1;
A2 = 2-(m2+1.5)/nFFT2;

PsignalN = (Ps/(1.59^2*nFFT1))^2/nFFT2;
% PnoiseN = (2*Ps)/(1.59^2*6*nFFT1^2*nFFT2)*2^(-2*bits)...
%     +(2*Ps*A1)/(1.59^2*3*nFFT1*nFFT2)*2^(-2*bit_fft1)...
%     + 1/(3*nFFT2)*2^(-2*bit_multi)...
%     + A2/3*2^(-2*bit_fft2);

calWind_n = 2^(-2*bits)/6;
if bits > bit_fft1
    calWind_n = calWind_n+(2^(-2*bit_fft1)/6-2^(-2*bits)/6);
end
calFFT1_n = 2^(-2*bit_fft1)/3*(2-(log2(nFFT1)+1.5)/nFFT1)+calWind_n/nFFT1;
if bit_fft1>bit_multi
    calFFT1_n = calFFT1_n+(2^(-2*bit_multi)/6-2^(-2*bit_fft1)/6);
end
calFFT1n_s = Ps/(1.59^2*nFFT1);
calMulti_n = 2*(calFFT1_n*calFFT1n_s)+2^(-2*bit_multi)/3;
if bit_multi>bit_fft2
    calMulti_n = calMulti_n+(2^(-2*bit_fft2)/6-2^(-2*bit_multi)/6);
end
calFFT2_n = 2^(-2*bit_fft2)/3*(2-(log2(nFFT2)+1.5)/nFFT2)+calMulti_n/nFFT2;
PnoiseN = calFFT2_n;

expN = 10*log10(PsignalN/PnoiseN);
end

