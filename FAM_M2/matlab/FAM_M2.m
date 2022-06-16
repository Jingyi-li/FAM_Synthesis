function [PsignalN,PnoiseN,expN] = FAM_M2(Ps,q1,q2,q3,nFFT1,nFFT2,bits,bit_fft1,bit_multi,bit_fft2)
%THEORYFAM Summary of this function goes here
%   Detailed explanation goes here
PsignalN = ((Ps/1.59^2*nFFT1)*q1^2)^2*nFFT2*q2^2*q3^2;
PnoiseN = (2*nFFT1^2*nFFT2*q1^4*q2^2*q3^2*Ps)/(1.59^2*6)*2^(-2*bits)+(2*nFFT1*nFFT2*q1^2*q3^2*q2^2*Ps)/(1.59^2)*(q1^2*(nFFT1/6-1)+1/2)/(3)*2^(-2*bit_fft1)...
    + q3^2*nFFT2/3*(q2^2+1/2)*2^(-2*bit_multi) + (q3^2*(nFFT2/6-1)/(3) +1/6)*2^(-2*bit_fft2);

calWind_n = 2^(-2*bits)/6;
if bits > bit_fft1
    calWind_n = calWind_n+(2^(-2*bit_fft1)/6-2^(-2*bits)/6);
end
calFFT1_n = 2^(-2*bit_fft1)/3*(nFFT1/6-1)+calWind_n*nFFT1;
calFFT1n_n = 2^(-2*bit_fft1)/6+calFFT1_n*q1^2;
if bit_fft1>bit_multi
    calFFT1n_n = calFFT1n_n+(2^(-2*bit_multi)/6-2^(-2*bit_fft1)/6);
end
calFFT1n_s = (Ps/1.59^2)*nFFT1*q1^2;
calMulti_n = 2*(calFFT1n_n*calFFT1n_s)+2^(-2*bit_multi)/3;
calMultin_n = 2^(-2*bit_multi)/6+calMulti_n*q2^2;
if bit_multi>bit_fft2
    calMultin_n = calMultin_n+(2^(-2*bit_fft2)/6-2^(-2*bit_multi)/6);
end
calFFT2_n = 2^(-2*bit_fft2)/3*(nFFT2/6-1)+calMultin_n*nFFT2;
calFFT2n_n = 2^(-2*bit_fft2)/6+calFFT2_n*q3^2;
PnoiseN2 = calFFT2n_n;

expN = 10*log10(PsignalN/PnoiseN);
end

