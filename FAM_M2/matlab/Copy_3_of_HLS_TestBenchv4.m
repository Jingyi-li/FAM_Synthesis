clear all; close all;clc
%% Initialization
Check_result = 0;
Type = 'FAM';
% path = '/home/jingyi/Documents/Projects/FFT/Code/sandboxResult/SCAsim';
% path = '/mnt/drsftps/DataResult/HLS';
% path_cr = './Database/Matlab';/mnt/RDS/Database/precision/Matlab
path_cr = '/mnt/RDS/Database/precision/Matlab';


fnames = {'Deepsig','Sine','SineCx','SineCxn','Square','SquareCx','SquareCxn','Triangle'};
idx_sig = 1;

Amp = 1;
ifdisp = 1;
idxfile = 'idx5_unitbits.mat';
load(idxfile);

%% Input signal

fname = fnames{idx_sig}
index = 2;

path = [path_cr,'/',fname];

switch fname
    case 'Deepsig'
        disp('Generate signal Deepsig');
        temp = load('/home/jingyi/Documents/Projects/FFT/Code/SCDsim/DeepSig.mat').a(index,:,:);
        temp=fi(Amp*(single(temp)./single(max(abs(temp)))),1,16,15);
        x = temp(:,:,1)+temp(:,:,2)*1i;
        x = [x x];
    case 'Sine'
        disp('Generate signal Sine');
        t = (0:1/2240:1);
        x = fi(Amp*sin(2*pi*200*t),1,16,15);
    case 'SineCx'
        disp('Generate signal SineComplex');
        t = (0:1/2240:1);
        x = fi((Amp*sin(2*pi*200*t)+Amp*sin(2*pi*300*t)*1j),1,16,15);
    case 'SineCxn'
        disp('Generate signal SineComplex with noise');
%         t = (0:1/2240:1);
%         x = (Amp*sin(2*pi*200*t)+Amp*sin(2*pi*300*t)*1j);
%         x = fi(awgn(x,10,'measured'),1,16,15);
        load('./sinecxnoise.dat');
        x = sinecxnoise(:,1)'+sinecxnoise(:,2)'*1j;
        x = fi(x,1,16,15);
    case 'Square'
        disp('Generate signal Square');
        t = (0:1/2240:1);
        x = fi(Amp*square(2*pi*200*t),1,16,15);
    case 'SquareCx'
        disp('Generate signal SquareComplex');
        t = (0:1/2240:1);
        x = fi((Amp*square(2*pi*200*t)+Amp*square(2*pi*300*t)*1j),1,16,15);
    case 'SquareCxn'
        disp('Generate signal SquareComplex with noise');
%         t = (0:1/2240:1);
%         x = (Amp*square(2*pi*200*t)+Amp*square(2*pi*300*t)*1j);
%         x = fi(awgn(x,10,'measured'),1,16,15);
        load('./squarecxnoise.dat');
        x = squarecxnoise(:,1)'+squarecxnoise(:,2)'*1j;
        x = fi(x,1,16,15);
    case 'Triangle'
        disp('Generate signal Triangle');
        t = (0:1/2240:1);
        x = fi(Amp*sawtooth(2*pi*200*t,1/2),1,16,15);
    otherwise
        disp('Does not init this input signal');
        
end

result_save = {};

disp(['---------------going to print DataSet' num2str(index) '----------------------------'])
fs = 1000; df = 5; dalpha = 0.5; %df 5 10 2.5 dalpha 0.5 1 0.25
result_save.(['resultcell',num2str(index)]) = {'bits_w','bits_F1','bits_CM','bits_F2', 'normsimulation', 'normcalculation','q1','q2','q3'};

for i = 3:length(idx_result)
    
        bit.input = idx_result(i,1);
        bit.windowing = idx_result(i,1);
        bit.firstFFT = idx_result(i,2);
        bit.ConjMulti = idx_result(i,3);
        bit.secondFFT = idx_result(i,4);
%     bit.input = 14;
%     bit.windowing = 14;
%     bit.firstFFT = 14;
%     bit.ConjMulti = 22;
%     bit.secondFFT = 14;
%     
    
    if 0<=index && index<10
        filename = [fname Type num2str(bit.windowing) num2str(bit.firstFFT) num2str(bit.ConjMulti) num2str(bit.secondFFT) 'Amp' num2str(100*Amp) 'Norm0' num2str(index) 'v4'];
    else
        filename = [fname Type num2str(bit.windowing) num2str(bit.firstFFT) num2str(bit.ConjMulti) num2str(bit.secondFFT) 'Amp' num2str(100*Amp) 'Norm' num2str(index) 'v4'];
    end
    
    
    listing = dir(path);
    exist = 0;
    for l = 3:length(listing)
        if strfind(listing(l).name, [filename '.mat'])
            load([path '/' listing(l).name]);
            result_temp = printResultv4(result,resultF,bit,Type,ifdisp);
            exist =1;
        end
    end
    
    if exist == 0
        switch Type
            case 'FAM'
                % normalization:
                % FFT and CM has increaseing integer bit to avoid
                % overflow
                [SxF,alphaoF,foF,resultF]=autofamFixedv4(x,fs,df,dalpha,bit);
                [Sx,alphao,fo,result]=autofamv4(x,fs,df,dalpha,bit,resultF.Scale);
            case 'SSCA'
                [SxF,alphaoF,foF,resultF]=autosscaFixedNormv4(x,fs,df,dalpha,bit);
                [Sx,alphao,fo,result]=autosscaSingleNormv4(x,fs,df,dalpha,bit,resultF.Scale);
        end
        save([path '/' filename '.mat'],'bit','result','resultF');
        disp(['-----------' 'save' filename '-----------']);
        result_temp = printResultv4(result,resultF,bit,Type,ifdisp);
    end
    
    result_save.(['resultcell',num2str(index)]) = [result_save.(['resultcell',num2str(index)]);result_temp];
end

% save(['test/' fname Type 'Amp' num2str(Amp*100) 'saveresult' idxfile(4) '_testv4.mat'],'result_save');
save([path '/' fname Type 'Amp' num2str(Amp*100) 'saveresult' idxfile(4) '_testv4.mat'],'result_save');


