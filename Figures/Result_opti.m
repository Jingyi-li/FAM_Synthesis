clear all; clc
% For Table 2 Reduce wordlength with less impact on SQNR
Nc = 256;
Nf = 32;
Ns = 2048;
A1 = 2-(log2(Nc)+1.5)/Nc;
A2 = 2-(log2(Nf)+1.5)/Nf;
A3 = 2-(log2(Ns)+1.5)/Ns;
m1 = log2(Nc);
m2 = log2(Nf);
m3 = log2(Ns);

fnames = {'Deepsig','SineCxn','SquareCxn'};
types = {'FAM'};
methods = {'M1','M2'};
order = '14141414';
for idx_sig = 1:3
    for type = 1
        Matlab_path = ['./Data/',fnames{idx_sig},'/'];
        filepath = [Matlab_path,fnames{idx_sig},types{type},order,'Amp100Norm02v4.mat'];
        disp(fnames{idx_sig});
        load(filepath);
        q1 = resultF.Scale.FirstFFTf;
        q2 = resultF.Scale.Multif;
        q3 = resultF.Scale.SecondFFTf;
        P = single(var(reshape(resultF.Input,[],1)));
        for idx_m = 1:2
            method = [types{type},'_',methods{idx_m}];
            disp(method);
            switch method
                case 'FAM_M1'
                    disp('FAM_M1');
                    windowing = (2*P)/(6*Nc^2*Nf*1.59^2);
                    FFT1 = (2*P*A1)/(3*Nc*Nf*1.59^2);
                    CM = 1/(3*Nf);
                    FFT2 = A2/3;
                case 'FAM_M2'
                    disp('FAM_M2');
                    windowing = (Nc^2*Nf*P*q1^4*q2^2*q3^2)/(3*1.59^2);
                    FFT1 = (2*P*Nc*Nf*q1^2*q2^2*q3^2)*(q1^2*(2^(m1)/6-1)+0.5)/(3*1.59^2);
                    CM = (Nf*q3^2*(q2^2+0.5))/(3);
                    FFT2 = (q3^2*(2^(m2)/6-1)+0.5)/3;
                otherwise
                    disp('not suitable method');
                    
            end
            CM_bits = 0.5*log2(FFT2/CM);
            FFT1_bits = 0.5*log2(FFT2/FFT1);
            W_bits = 0.5*log2(FFT2/windowing);
            disp(['Conjugate Multi has less bits of ',num2str(CM_bits)]);
            disp(['FirstFFT has less bits of ',num2str(FFT1_bits)]);
            disp(['Windowing has less bits of ',num2str(W_bits)]);
        end
    end
end