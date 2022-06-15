clear all; clc
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
        disp(filepath);
        load(filepath)
        q1 = resultF.Scale.FirstFFTf;
        q2 = resultF.Scale.Multif;
        q3 = resultF.Scale.SecondFFTf;
        P = single(var(reshape(resultF.Input,[],1)));
        for idx_m = 1:2
            method = [types{type},'_',methods{idx_m}];
            disp(method);
            result_p.(fnames{idx_sig}).(method) = {'unitbits','normcalculation'};
            for x = 14:26
                switch method
                    case 'FAM_M1'
                        [~,~,expN] = FAM_M1(P,Nc,Nf,x-1,x-1,x-1,x-1);
                    case 'FAM_M2'
                        [~,~,expN] = FAM_M2(P,q1,q2,q3,Nc,Nf,x-1,x-1,x-1,x-1);
                    otherwise
                        disp('not suitable method');
                        
                end
                result_p.(fnames{idx_sig}).(method) =[result_p.(fnames{idx_sig}).(method);{x, expN}];
            end
            
            
        end
    end
end

%%
figure; % figure 11
hold on
x = 14:26;
p1 = plot(x, cell2mat(result_p.Deepsig.FAM_M1(2:end,2)),'g','DisplayName','DeepSig-FAM-M1');
p2 = plot(x, cell2mat(result_p.SineCxn.FAM_M1(2:end,2)),'r','DisplayName','SineWave-FAM-M1');
p3 = plot(x, cell2mat(result_p.SquareCxn.FAM_M1(2:end,2)),'b','DisplayName','SquareWave-FAM-M1');
p7 = plot(x, cell2mat(result_p.Deepsig.FAM_M2(2:end,2)),'g--','DisplayName','DeepSig-FAM-M2');
p8 = plot(x, cell2mat(result_p.SineCxn.FAM_M2(2:end,2)),'r--','DisplayName','SineWave-FAM-M2');
p9 = plot(x, cell2mat(result_p.SquareCxn.FAM_M2(2:end,2)),'b--','DisplayName','SquareWave-FAM-M2');



p1=plot(nan, nan,'k','DisplayName','FAM-M1');
p2=plot(nan, nan,'k--','DisplayName','FAM-M2');


legend([p1,p2],'Location','best');
xlabel('Unit bit precision');
ylabel('SQNR (dB)');
%% figure 1
figure; 
hold on
x = 14:26;
plot(x, cell2mat(result_p.SineCxn.FAM_M1(2:end,2)),'r--','DisplayName','SineWave-FAM-M1');
plot(x, cell2mat(result_p.SquareCxn.FAM_M1(2:end,2)),'b--','DisplayName','SquareWave-FAM-M1');
p1=plot(nan, nan,'r--','DisplayName','SineWave');
p2=plot(nan, nan,'b--','DisplayName','SquareWave');
legend([p1,p2],'Location','best');
xlabel('Unit bit precision');
ylabel('SQNR (dB)');