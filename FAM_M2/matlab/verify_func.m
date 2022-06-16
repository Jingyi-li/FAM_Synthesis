function verify_func(bitW,bitF1,bitCM,bitF2)
%% Initialization
Type = 'FAM';
path_cr = './Data';


fnames = {'Deepsig','Sine','SineCx','SineCxn','Square','SquareCx','SquareCxn','Triangle'};
idx_sig = 1;

Amp = 1;
ifdisp = 0;
idxfile = 'idx5_unitbits.mat';
load(idxfile);

%% Input signal

fname = fnames{idx_sig}
index = 2;

path = [path_cr,'/',fname];

switch fname
    case 'Deepsig'
        disp('Generate signal Deepsig');
        temp = load('DeepSig.mat').a(index,:,:);
        temp=fi(Amp*(single(temp)./single(max(abs(temp)))),1,16,15);
        x = temp(:,:,1)+temp(:,:,2)*1i;
        x = [x x];
    case 'Sine'
        disp('Generate signal Sine');
        t = (0:1/2240:1);
        x = fi(Amp*sin(2*pi*200*t),1,16,15);
    case 'SineCx'
        disp('Generate signal SineComplex');[path '/' filename '.mat']
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


bit.input = bitW;
bit.windowing = bitW;
bit.firstFFT = bitF1;
bit.ConjMulti = bitCM;
bit.secondFFT = bitF2;


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
    %save([path '/' filename '.mat'],'bit','result','resultF');
    disp(['-----------' 'save' filename '-----------']);
    result_temp = printResultv4(result,resultF,bit,Type,ifdisp);
end
%% Save result_temp
Ps = single(var(reshape(resultF.Input,[],1)));
q1 = resultF.Scale.FirstFFT;
q2 = resultF.Scale.Multi;
q3 = resultF.Scale.SecondFFT;
q1f = resultF.Scale.FirstFFTf;
q2f = resultF.Scale.Multif;
q3f = resultF.Scale.SecondFFTf;
nFFT1 = 256;
nFFT2 = 32;
[~,~,expN] = FAM_M2(Ps,q1f,q2f,q3f,nFFT1,nFFT2,bit.windowing-1,bit.firstFFT-1,bit.ConjMulti-1,bit.secondFFT-1);
file = fopen(['../Result/MR/FAM_M2_FullSCD_SQNR_' num2str(bit.windowing) num2str(bit.firstFFT) num2str(bit.ConjMulti) num2str(bit.secondFFT) '.txt'],'w');

fprintf(file,'The simulation SQNR is: %f dB \n ', result_temp{5});
fprintf(file,'The calculation SQNR is: %f dB \n ', result_temp{6});
fprintf(file,'The Result of simplify expression of SQNR with Normalization is: %f dB \n ', expN);
fclose(file);

%% Generate full floating result for testbench
file = fopen(['../Result/deepsig_float_result_full' num2str(bit.windowing) num2str(bit.firstFFT) num2str(bit.ConjMulti) num2str(bit.secondFFT) '.dat'],'w');
xtemp = result.SecondFFTnormal;
[idxr,idxc] = size(xtemp);
idxrs = [1:8,25:32];
for i = 1:idxc
    for j = 1:idxr
        fprintf(file,'%.*f,',9,real(xtemp(j,i)));
        fprintf(file,'%.*f,',9,imag(xtemp(j,i)));
    end
    fprintf(file,'\n');
end
fclose(file);

%% Generate The SparseSCD

file = fopen(['../Result/sparse_half/sparse_SCD_float_half' num2str(bit.windowing) num2str(bit.firstFFT) num2str(bit.ConjMulti) num2str(bit.secondFFT) '.dat'],'w');
xtemp = result.SecondFFTnormal;
[idxr,idxc] = size(xtemp);
idxrs = [1:8,25:32];
temp = zeros(idxr/2,idxc);
for i = 0:(idxc/2)-1
    for j = 0:15
        temp = abs(xtemp(idxrs(j+1),i+1))^2;
        if temp>0.05
            kk = floor(i/256);
            ii = mod(i,256);
            fprintf(file,'%.*f,%d,%d\n',9,temp,(kk+ii-256),8*(ii-kk-1)+j);
        end
    end
end
file = fopen(['../Result/sparse_full/sparse_SCD_float_full' num2str(bit.windowing) num2str(bit.firstFFT) num2str(bit.ConjMulti) num2str(bit.secondFFT) '.dat'],'w');
xtemp = result.SecondFFTnormal;
[idxr,idxc] = size(xtemp);
idxrs = [1:8,25:32];
temp = zeros(idxr,idxc);
for i = 0:idxc-1
    for j = 0:15
        temp = abs(xtemp(idxrs(j+1),i+1))^2;
        if temp>0.05
            kk = floor(i/256);
            ii = mod(i,256);
            fprintf(file,'%.*f,%d,%d\n',9,temp,(kk+ii-256),8*(ii-kk-1)+j);
        end
    end
end
end



