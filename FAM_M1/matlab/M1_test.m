% compare the C simulation of the HLS with the calculation function of M1
% clear all;clc;
% Vivado_path = './Database/Vivado/';
% Inputsig = 'SquareCxn';
% sig = 'squarecxnoise';
Inputsig = 'Deepsig';
sig = 'deepsig';
Vivado_path = ['/mnt/tuna/SCDfunctions/hls/FAM_M1/Result/',Inputsig,'/'];
order = '16161616';
bits = str2double(order(1:2))-1;
bit_fft1 = str2double(order(3:4))-1;
bit_cm = str2double(order(5:6))-1;
bit_fft2 = str2double(order(7:8))-1;
Nc = 256;
Nf = 32;
Ns = 2048;
type = 'FAM';
filepath = [Vivado_path,type,sig,order,'.dat'];
data = load(filepath);
vdata = data(:,1)+data(:,2)*j;
Matlab_path = ['/mnt/RDS/Database/precision/Matlab_M1/',Inputsig,'/'];
filepath = [Matlab_path,Inputsig,type,order,'Amp100Norm02v4.mat'];
load(filepath)
% mdata = reshape(resultF.Input,[],1);
% rdata = reshape(result.Input,[],1);
% mdata = reshape(resultF.Windowing,[],1);
% rdata = reshape(result.Windowing,[],1);
% mdata = reshape(resultF.FirstFFT,[],1);
% rdata = reshape(result.FirstFFT,[],1);
% mdata = reshape(resultF.Multi,[],1);
% rdata = reshape(result.Multi,[],1);
% mdata = reshape(resultF.SecondFFT,[],1);
rdata = reshape(result.SecondFFT,[],1);

% avgS = var(reshape(result.Input,[],1));
% avgS = var(reshape(result.Windowing,[],1));
% avgS = var(reshape(result.FirstFFT,[],1));
% avgS = var(reshape(result.Multi,[],1));
avgS = var(reshape(result.SecondFFT,[],1));
avgNv = var(single(rdata)-single(vdata));
% avgNm = var(single(rdata)-single(mdata));
Ps = single(var(reshape(result.Input,[],1)));
% % --------------------------FAM-calculating------------------------
% CalN_win = 2^(-2*bits)/6;
% P_win = Ps/1.59^2;
% CalN_FFT1 = CalN_win/Nc + (2-(log2(Nc)+1.5)/Nc)/3 * 2^(-2*bit_fft1);
% P_FFT1 = P_win/Nc;
% CalN_CM = 2*P_FFT1*CalN_FFT1 + 2^(-2*bit_cm)/3;
% P_CM = P_FFT1^2;
% CalN_FFT2 = CalN_CM/Nf + (2-(log2(Nf)+1.5)/Nf)/3 * 2^(-2*bit_fft2);
% P_FFT2 = P_CM/Nf;

% % --------------------------SSCA-calculating------------------------
% CalN_win = 2^(-2*bits)/6;
% P_win = Ps/1.59^2;
% CalN_FFT1 = CalN_win/Nc + (2-(log2(Nc)+1.5)/Nc)/3 * 2^(-2*bit_fft1);
% P_FFT1 = P_win/Nc;
% CalN_CM = Ps*CalN_FFT1 + 2^(-2*bit_cm)/3;
% P_CM = P_FFT1*Ps;
% CalN_FFT2 = CalN_CM/Ns + (2-(log2(Nf)+1.5)/Nf)/3 * 2^(-2*bit_fft2);
% P_FFT2 = P_CM/Ns;

[PsignalN,PnoiseN,expN] = FAM_M1(Ps,Nc,Nf,bits,bit_fft1,bit_cm,bit_fft2);
disp(['The SQNR of simulation in vivado is: ', num2str(10*log10(single(avgS/avgNv)))]);
% disp(['The SQNR of simulation in matlab is: ', num2str(10*log10(single(avgS/avgNm)))]);
% disp(['The SQNR of Calculation is: ', num2str(10*log10(P_FFT2/CalN_FFT2))]);
disp(['The SQNR of Calculation is: ', num2str(expN)]);


