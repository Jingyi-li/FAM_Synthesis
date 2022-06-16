% compare the C simulation of the HLS with the fixed point of Matlab
% clear all;clc;
% Vivado_path = './Database/Vivado/';

vivado = load("../../SCDresult.dat");
matlb = load("deepsig_float_result.dat");
vivado_data = zeros(65536,16);
matlb_data = zeros(65536,16);
for i = 1:16
    vivado_data(:,i) = vivado(:,i*2-1)+1i*vivado(:,i*2);
    matlb_data(:,i) = matlb(:,i*2-1)+1i*matlb(:,i*2);
end

vdata = reshape(vivado_data,[],1);
mdata = reshape(matlb_data,[],1);

avgS = var(reshape(matlb_data,[],1));
avgN = var(vdata-mdata);
disp(['The SQNR of simulation in vivado is: ', num2str(10*log10(single(avgS/avgN)))]);
%%
Inputsig = 'SquareCxn';
sig = 'squarecxnoise';
Vivado_path = ['/mnt/tuna/SCDfunctions/hls/SSCA_hls/Result/',Inputsig,'/'];
order = '23232323';
type = 'SSCA';
filepath = [Vivado_path,type,sig,order,'.dat'];
data = load(filepath);
vdata = data(:,1)+data(:,2)*1i;
Matlab_path = ['/mnt/RDS/Database/precision/Matlab/',Inputsig,'/'];
filepath = [Matlab_path,Inputsig,type,order,'Amp100Norm02v4.mat'];
load(filepath)
% mdata = reshape(resultF.Input,[],1);
% rdata = reshape(result.Input,[],1);
% mdata = reshape(resultF.Windowing,[],1);
% rdata = reshape(result.Windowing,[],1);
% mdata = reshape(resultF.FirstFFTnormal,[],1);
% rdata = reshape(result.FirstFFTnormal,[],1);
% mdata = reshape(resultF.Multinormal,[],1);
% rdata = reshape(result.Multinormal,[],1);
mdata = reshape(resultF.SecondFFTnormal,[],1);
rdata = reshape(result.SecondFFTnormal,[],1);

% avgS = var(reshape(result.Input,[],1));
% avgS = var(reshape(result.Windowing,[],1));
% avgS = var(reshape(result.FirstFFTnormal,[],1));
% avgS = var(reshape(result.Multinormal,[],1));
avgS = var(reshape(result.SecondFFTnormal,[],1));
avgNv = var(single(rdata)-single(vdata));
avgNm = var(single(rdata)-single(mdata));
avgNe = var(vdata-mdata);
disp(['The SQNR of simulation in matlab is: ', num2str(10*log10(single(avgS/avgNm)))]);
disp(['The SQNR of simulation in vivado is: ', num2str(10*log10(single(avgS/avgNv)))]);
disp(['The avg error between the matlab and vivado is: ', num2str(mean(abs(single(vdata-mdata))))]);


%%
clear all; clc
idxfile = 'idx4_result.mat';
load(idxfile);
result_save = {'bits_w','bits_F1','bits_CM','bits_F2', 'Matlab', 'Vivado','diff'};
Inputsig = 'SineCx';
sig = 'sinecomplex';
for i = 1:length(idx_result)
    
    bit.input = idx_result(i,1);
    bit.windowing = idx_result(i,1);
    bit.firstFFT = idx_result(i,2);
    bit.ConjMulti = idx_result(i,3);
    bit.secondFFT = idx_result(i,4);
    
    order = [num2str(bit.windowing), num2str(bit.firstFFT), num2str(bit.ConjMulti), num2str(bit.secondFFT)];
    disp(['Save:', num2str(order)]); 
    Vivado_path = ['/mnt/tuna/SCDfunctions/hls/SSCA_hls/Result/',Inputsig,'/'];
    type = 'SSCA';
    filepath = [Vivado_path,type,sig,order,'.dat'];
    data = load(filepath);
    vdata = data(:,1)+data(:,2)*j;
    Matlab_path = ['/mnt/RDS/Database/precision/Matlab/',Inputsig,'/'];
    filepath = [Matlab_path,Inputsig,type,order,'Amp100Norm02v4.mat'];
    load(filepath)

    mdata = reshape(resultF.SecondFFTnormal,[],1);
    rdata = reshape(result.SecondFFTnormal,[],1);
    

    avgS = var(reshape(result.SecondFFTnormal,[],1));
    avgNv = var(single(rdata)-single(vdata));
    avgNm = var(single(rdata)-single(mdata));
    avgNe = var(vdata-mdata);
    
%     disp(['The SQNR of simulation in matlab is: ', num2str(10*log10(single(avgS/avgNm)))]);
%     disp(['The SQNR of simulation in vivado is: ', num2str(10*log10(single(avgS/avgNv)))]);
%     disp(['The avg error between the matlab and vivado is: ', num2str(mean(abs(single(vdata-mdata))))]);
    result_save = [result_save;{num2str(bit.windowing), num2str(bit.firstFFT), num2str(bit.ConjMulti),...
        num2str(bit.secondFFT),num2str(10*log10(single(avgS/avgNm))),num2str(10*log10(single(avgS/avgNv))),...
        num2str(mean(abs(single(vdata-mdata))))}];
    
end


%%
clear all; clc
Inputsig = 'Deepsig';
sig = 'deepsig';
idxfile = ['/mnt/RDS/Database/precision/Matlab/',Inputsig,'/',Inputsig,'SSCAAmp100saveresult5_testv4.mat'];
load(idxfile);
result_save.vivado = {'bits_w','bits_F1','bits_CM','bits_F2', 'Matlab', 'Vivado','Calculation'};
idx_result = result_save.resultcell2(2:end,1:6);
for i = 1:length(idx_result)
    
    bit.input = cell2mat(idx_result(i,1));
    bit.windowing = cell2mat(idx_result(i,1));
    bit.firstFFT = cell2mat(idx_result(i,2));
    bit.ConjMulti = cell2mat(idx_result(i,3));
    bit.secondFFT = cell2mat(idx_result(i,4));
    
    order = [num2str(bit.windowing), num2str(bit.firstFFT), num2str(bit.ConjMulti), num2str(bit.secondFFT)];
    disp(['Save:', num2str(order)]); 
    Vivado_path = ['/mnt/tuna/SCDfunctions/hls/SSCA_hls/Result/',Inputsig,'/'];
    type = 'SSCA';
    filepath = [Vivado_path,type,sig,order,'.dat'];
    data = load(filepath);
    vdata = data(:,1)+data(:,2)*1j;
    Matlab_path = ['/mnt/RDS/Database/precision/Matlab/',Inputsig,'/'];
    filepath = [Matlab_path,Inputsig,type,order,'Amp100Norm02v4.mat'];
    load(filepath)

%     mdata = reshape(resultF.SecondFFTnormal,[],1);
    rdata = reshape(result.SecondFFTnormal,[],1);
    

    avgS = var(reshape(result.SecondFFTnormal,[],1));
    avgNv = var(single(rdata)-single(vdata));
%     avgNm = var(single(rdata)-single(mdata));
%     avgNe = var(vdata-mdata);
    
%     disp(['The SQNR of simulation in matlab is: ', num2str(10*log10(single(avgS/avgNm)))]);
%     disp(['The SQNR of simulation in vivado is: ', num2str(10*log10(single(avgS/avgNv)))]);
%     disp(['The avg error between the matlab and vivado is: ', num2str(mean(abs(single(vdata-mdata))))]);
    result_save.vivado = [result_save.vivado;{bit.windowing, bit.firstFFT, bit.ConjMulti,...
        bit.secondFFT,cell2mat(idx_result(i,5)),10*log10(single(avgS/avgNv)),...
        cell2mat(idx_result(i,6))}];
    
end