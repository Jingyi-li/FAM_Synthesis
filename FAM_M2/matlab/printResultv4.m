function resultcell = printResultv4(result,resultF,bit,method,ifdisp)
%PRINTRESULT Summary of this function goes here
%   Detailed explanation goes here
bits=bit.windowing-1;
bit_fft1 = bit.firstFFT-1;
bit_multi = bit.ConjMulti-1;
bit_fft2 = bit.secondFFT-1;
nFFT1 = 256;
nFFT2 = 32;
nFFT3 = 2048;
ifdisp = ifdisp;

switch method
    
    case 'FAM'
        disp('---------------------------Print Result for FAM normilization at ConjuMulti Method---------------------------------------');
        disp(['bit.windowing = ' num2str(bit.windowing) ', bit.firstFFT = ' num2str(bit.firstFFT) ', bit.ConjMulti = ' num2str(bit.ConjMulti) ', bit.secondFFT = ' num2str(bit.secondFFT)]);
        if ifdisp==1
            disp('%%%%%Windowing%%%%%');
        end
        avgWind_s = var(reshape(result.Windowing,[],1));
        avgWind_n = var(reshape(result.Windowing-single(resultF.Windowing),[],1));
        calWind_n = 2^(-2*bits)/6;
        if ifdisp == 1
            disp(['The Calculation average variance error is: ' num2str(calWind_n)]);
            disp(['The average variance error is: ' num2str(avgWind_n) ' The average variance of signal is: ' num2str(avgWind_s)]);
            disp(['The simulation SQNR is: ' num2str(10*log10(avgWind_s/avgWind_n))]);
            disp(['The calculation SQNR is: ' num2str(10*log10(avgWind_s/calWind_n))]);
            
            disp('%%%%%FirstFFT%%%%%');
        end
        avgFFT1_s = var(reshape(result.FirstFFT,[],1));
        avgFFT1_n = var(reshape(result.FirstFFT-single(resultF.FirstFFT),[],1));
        if bits > bit_fft1
            calWind_n = calWind_n+(2^(-2*bit_fft1)/6-2^(-2*bits)/6);
        end
        calFFT1_n = 2^(-2*bit_fft1)/6*(nFFT1/6-1)+calWind_n*nFFT1;% to match with IP Core change from /3 to /6 by JYP
%         calFFT1_n = 2^(-2*bit_fft1)/3*(nFFT1/6-1)+calWind_n*nFFT1;
        calFFT1_s = avgWind_s*nFFT1;
        if ifdisp == 1
            disp(['The Calculation average variance error is: ' num2str(calFFT1_n) 'The Calculation average variance signal is: ' num2str(calFFT1_s)]);
            disp(['The average variance error is: ' num2str(avgFFT1_n) ' The average variance of signal is: ' num2str(avgFFT1_s)]);
            disp(['The simulation SQNR is: ' num2str(10*log10(avgFFT1_s/avgFFT1_n))]);
            disp(['The calculation SQNR is: ' num2str(10*log10(calFFT1_s/calFFT1_n))]);
            
            disp('%%%%%FirstFFTnormal%%%%%');
        end
        avgFFT1n_s = var(reshape(result.FirstFFTnormal,[],1));
        avgFFT1n_n = var(reshape(result.FirstFFTnormal-single(resultF.FirstFFTnormal),[],1));
        calFFT1n_n = 2^(-2*bit_fft1)/6+calFFT1_n*resultF.Scale.FirstFFTf^2;
%         calFFT1n_n = 2^(-2*bit_multi)/6+calFFT1_n*resultF.Scale.FirstFFT^2;
        calFFT1n_s = calFFT1_s*resultF.Scale.FirstFFTf^2;
        if ifdisp == 1
            disp(['The Calculation average variance error is: ' num2str(calFFT1n_n) 'The Calculation average variance signal is: ' num2str(calFFT1n_s)]);
            disp(['The average variance error is: ' num2str(avgFFT1n_n) ' The average variance of signal is: ' num2str(avgFFT1n_s)]);
            disp(['The simulation SQNR is: ' num2str(10*log10(avgFFT1n_s/avgFFT1n_n))]);
            disp(['The calculation SQNR is: ' num2str(10*log10(calFFT1n_s/calFFT1n_n))]);
            
            disp('%%%%%Conjugate Multiplication%%%%%');
        end
        if bit_fft1>bit_multi
            calFFT1n_n = calFFT1n_n+(2^(-2*bit_multi)/6-2^(-2*bit_fft1)/6);
        end
        avgMulti_s = var(reshape(result.Multi,[],1));
        avgMulti_n = var(reshape(result.Multi-single(resultF.Multi),[],1));
        calMulti_n = 2*(calFFT1n_n*calFFT1n_s)+2^(-2*bit_multi)/3;
        calMulti_s = (calFFT1n_s)*(calFFT1n_s);
        if ifdisp == 1
            disp(['The Calculation average variance error is: ' num2str(calMulti_n) 'The Calculation average variance signal is: ' num2str(calMulti_s)]);
            disp(['The average variance error is: ' num2str(avgMulti_n) ' The average variance of signal is: ' num2str(avgMulti_s)]);
            disp(['The simulation SQNR is: ' num2str(10*log10(avgMulti_s/avgMulti_n))]);
            disp(['The calculation SQNR is: ' num2str(10*log10(calMulti_s/calMulti_n))]);
            
            disp('%%%%%Conjugate Multiplication normal%%%%%');
        end
        avgMultin_s = var(reshape(result.Multinormal,[],1));
        avgMultin_n = var(reshape(result.Multinormal-single(resultF.Multinormal),[],1));
        calMultin_n = 2^(-2*bit_multi)/6+calMulti_n*resultF.Scale.Multif^2;
%         calMultin_n = 2^(-2*bit_fft2)/6+calMulti_n*resultF.Scale.Multi^2;
        calMultin_s = calMulti_s*resultF.Scale.Multif^2;
        if ifdisp == 1
            disp(['The Calculation average variance error is: ' num2str(calMultin_n) 'The Calculation average variance signal is: ' num2str(calMultin_s)]);
            disp(['The average variance error is: ' num2str(avgMultin_n) ' The average variance of signal is: ' num2str(avgMultin_s)]);
            disp(['The simulation SQNR is: ' num2str(10*log10(avgMultin_s/avgMultin_n))]);
            disp(['The calculation SQNR is: ' num2str(10*log10(calMultin_s/calMultin_n))]);
            
            disp('%%%%%SecondFFT%%%%%');
        end
        if bit_multi>bit_fft2
            calMultin_n = calMultin_n+(2^(-2*bit_fft2)/6-2^(-2*bit_multi)/6);
        end
        avgFFT2_s = var(reshape(result.SecondFFT,[],1));
        avgFFT2_n = var(reshape(result.SecondFFT-single(resultF.SecondFFT),[],1));
        calFFT2_n = 2^(-2*bit_fft2)/3*(nFFT2/6-1)+calMultin_n*nFFT2;
        calFFT2_s = calMultin_s*nFFT2;
        if ifdisp == 1
            disp(['The Calculation average variance error is: ' num2str(calFFT2_n) ' The Calculation average variance signal is: ' num2str(calFFT2_s)]);
            disp(['The average variance error is: ' num2str(avgFFT2_n) ' The average variance of signal is: ' num2str(avgFFT2_s)]);
            
            disp('%%%%%SecondFFTnormal%%%%%');
        end
        avgFFT2n_s = var(reshape(result.SecondFFTnormal,[],1));
        avgFFT2n_n = var(reshape(result.SecondFFTnormal-single(resultF.SecondFFTnormal),[],1));
        calFFT2n_n = 2^(-2*bit_fft2)/6+calFFT2_n*resultF.Scale.SecondFFTf^2;
        calFFT2n_s = calFFT2_s*resultF.Scale.SecondFFTf^2;
        if ifdisp == 1
            disp(['The Calculation average variance error is: ' num2str(calFFT2n_n) ' The Calculation average variance signal is: ' num2str(calFFT2n_s)]);
            disp(['The average variance error is: ' num2str(avgFFT2n_n) ' The average variance of signal is: ' num2str(avgFFT2n_s)]);
        end
        Ps = single(var(reshape(resultF.Input,[],1)));
        q1 = resultF.Scale.FirstFFT;
        q2 = resultF.Scale.Multi;
        q3 = resultF.Scale.SecondFFT;
        q1f = resultF.Scale.FirstFFTf;
        q2f = resultF.Scale.Multif;
        q3f = resultF.Scale.SecondFFTf;
        
        %         PsignalN = ((Ps/1.59^2*nFFT1)*q1^2)^2*nFFT2*q2^2*q3^2;
        %         PnoiseN = (2*nFFT1^2*nFFT2*q1^4*q2^2*q3^2*Ps)/(1.59^2*6)*2^(-2*bits)+(2*nFFT1*nFFT2*q1^2*q3^2*q2^2*Ps)/(1.59^2)*(q1^2*(nFFT1/6-1)+1/2)/(3)*2^(-2*bit_fft1)...
        %             + q3^2*nFFT2/3*(q2^2+1/2)*2^(-2*bit_multi) + (q3^2*(nFFT2/6-1)/(3) +1/6)*2^(-2*bit_fft2);
        %         expN = 10*log10(PsignalN/PnoiseN);
        
        [~,~,expN] = FAM_M2(Ps,q1f,q2f,q3f,nFFT1,nFFT2,bits,bit_fft1,bit_multi,bit_fft2);
        
        disp('----------------Result---------------------');
        disp(['The simulation SQNR is: ' num2str(10*log10(avgFFT2n_s/avgFFT2n_n))]);
        disp(['The calculation SQNR is: ' num2str(10*log10(calFFT2n_s/calFFT2n_n))]);
        disp(['The Result of simplify expression of SQNR with Normalization is: ' num2str(expN)]);
        
        resultcell = {bit.windowing,bit.firstFFT,bit.ConjMulti,bit.secondFFT,10*log10(avgFFT2n_s/avgFFT2n_n),10*log10(calFFT2n_s/calFFT2n_n),q1,q2,q3};
        
    case 'SSCA'
        Ps = single(var(reshape(resultF.Input,[],1)));
        disp('---------------------------Print Result SSCA---------------------------------------');
        disp(['bit.windowing = ' num2str(bit.windowing) ', bit.firstFFT = ' num2str(bit.firstFFT) ', bit.ConjMulti = ' num2str(bit.ConjMulti) ', bit.secondFFT = ' num2str(bit.secondFFT)]);
        if ifdisp == 1
            disp('%%%%%Windowing%%%%%');
        end
        avgWind_s = var(reshape(result.Windowing,[],1));
        avgWind_n = var(reshape(result.Windowing-single(resultF.Windowing),[],1));
        calWind_n = 2^(-2*bits)/6;
        if ifdisp == 1
            disp(['The Calculation average variance error is: ' num2str(calWind_n)]);
            disp(['The average variance error is: ' num2str(avgWind_n) ' The average variance of signal is: ' num2str(avgWind_s)]);
            disp(['The simulation SQNR is: ' num2str(10*log10(avgWind_s/avgWind_n))]);
            disp(['The calculation SQNR is: ' num2str(10*log10(avgWind_s/calWind_n))]);
            
            disp('%%%%%FirstFFT%%%%%');
        end
        avgFFT1_s = var(reshape(result.FirstFFT,[],1));
        avgFFT1_n = var(reshape(result.FirstFFT-single(resultF.FirstFFT),[],1));
        if bits > bit_fft1
            calWind_n = calWind_n+(2^(-2*bit_fft1)/6-2^(-2*bits)/6);
        end  
        calFFT1_n = 2^(-2*bit_fft1)/3*(nFFT1/6-1)+calWind_n*nFFT1;
        calFFT1_s = avgWind_s*nFFT1;
        if ifdisp == 1
            disp(['The Calculation average variance error is: ' num2str(calFFT1_n) 'The Calculation average variance signal is: ' num2str(calFFT1_s)]);
            disp(['The average variance error is: ' num2str(avgFFT1_n) ' The average variance of signal is: ' num2str(avgFFT1_s)]);
            disp(['The simulation SQNR is: ' num2str(10*log10(avgFFT1_s/avgFFT1_n))]);
            disp(['The calculation SQNR is: ' num2str(10*log10(calFFT1_s/calFFT1_n))]);
            %
            disp('%%%%%FirstFFTNormal%%%%%');
        end
        avgFFT1n_s = var(reshape(result.FirstFFTnormal,[],1));
        avgFFT1n_n = var(reshape(result.FirstFFTnormal-single(resultF.FirstFFTnormal),[],1));
        calFFT1n_n = 2^(-2*bit_fft1)/6+calFFT1_n*resultF.Scale.FirstFFTf^2;
        calFFT1n_s = calFFT1_s*resultF.Scale.FirstFFTf^2;
        if ifdisp == 1
            disp(['The Calculation average variance error is: ' num2str(calFFT1n_n) 'The Calculation average variance signal is: ' num2str(calFFT1n_s)]);
            disp(['The average variance error is: ' num2str(avgFFT1n_n) ' The average variance of signal is: ' num2str(avgFFT1n_s)]);
            disp(['The simulation SQNR is: ' num2str(10*log10(avgFFT1n_s/avgFFT1n_n))]);
            disp(['The calculation SQNR is: ' num2str(10*log10(calFFT1n_s/calFFT1n_n))]);
            
            
            disp('%%%%%Conjugate Multiplication%%%%%');
        end
        if bit_fft1>bit_multi
            calFFT1n_n = calFFT1n_n+(2^(-2*bit_multi)/6-2^(-2*bit_fft1)/6);
        end
        avgMulti_s = var(reshape(result.Multi,[],1));
        avgMulti_n = var(reshape(result.Multi-single(resultF.Multi),[],1));
%         calMulti_n = (2*calFFT1_n*calFFT1_s+2^(-2*bit_multi)/3);
%         calMulti_s = calFFT1_s*calFFT1_s;
        calMulti_n = calFFT1n_n*Ps+2^(-2*bit_multi)/3;
        calMulti_s = calFFT1n_s*Ps;
        if ifdisp == 1
            disp(['The Calculation average variance error is: ' num2str(calMulti_n) 'The Calculation average variance signal is: ' num2str(calMulti_s)]);
            disp(['The average variance error is: ' num2str(avgMulti_n) ' The average variance of signal is: ' num2str(avgMulti_s)]);
            disp(['The simulation SQNR is: ' num2str(10*log10(avgMulti_s/avgMulti_n))]);
            disp(['The calculation SQNR is: ' num2str(10*log10(calMulti_s/calMulti_n))]);
            
            disp('%%%%%Conjugate Multiplication normal%%%%%');
        end
        avgMultin_s = var(reshape(result.Multinormal,[],1));
        avgMultin_n = var(reshape(result.Multinormal-single(resultF.Multinormal),[],1));
%         calMultin_n = calMulti_n*resultF.Scale.Multif^2;
%         calMultin_n = 2^(-2*bit_fft2)/6+calMulti_n*resultF.Scale.Multif^2;
        calMultin_n = 2^(-2*bit_multi)/6+calMulti_n*resultF.Scale.Multif^2;
        calMultin_s = calMulti_s*resultF.Scale.Multif^2;
        if ifdisp == 1
            disp(['The Calculation average variance error is: ' num2str(calMultin_n) 'The Calculation average variance signal is: ' num2str(calMultin_s)]);
            disp(['The average variance error is: ' num2str(avgMultin_n) ' The average variance of signal is: ' num2str(avgMultin_s)]);
            disp(['The simulation SQNR is: ' num2str(10*log10(avgMultin_s/avgMultin_n))]);
            disp(['The calculation SQNR is: ' num2str(10*log10(calMultin_s/calMultin_n))]);
            
            
            disp('%%%%%SecondFFT%%%%%');
        end
        if bit_multi>bit_fft2
            calMultin_n = calMultin_n+(2^(-2*bit_fft2)/6-2^(-2*bit_multi)/6);
        end
        avgFFT2_s = var(reshape(result.SecondFFT,[],1));
        avgFFT2_n = var(reshape(result.SecondFFT-single(resultF.SecondFFT),[],1));
        calFFT2_n = 2^(-2*bit_fft2)/3*(nFFT3/6-1)+calMultin_n*nFFT3;
        calFFT2_s = calMultin_s*nFFT3;
        if ifdisp == 1
            disp(['The Calculation average variance error is: ' num2str(calFFT2_n) 'The Calculation average variance signal is: ' num2str(calFFT2_s)]);
            disp(['The average variance error is: ' num2str(avgFFT2_n) ' The average variance of signal is: ' num2str(avgFFT2_s)]);
            disp(['The simulation SQNR is: ' num2str(10*log10(avgFFT2_s/avgFFT2_n))]);
            disp(['The calculation SQNR is: ' num2str(10*log10(calFFT2_s/calFFT2_n))]);
            disp('%%%%%SecondFFTnormal%%%%%');
        end
        avgFFT2n_s = var(reshape(result.SecondFFTnormal,[],1));
        avgFFT2n_n = var(reshape(result.SecondFFTnormal-single(resultF.SecondFFTnormal),[],1));
        calFFT2n_n = 2^(-2*bit_fft2)/6+calFFT2_n*resultF.Scale.SecondFFT^2;
        calFFT2n_s = calFFT2_s*resultF.Scale.SecondFFT^2;
        if ifdisp == 1
            disp(['The Calculation average variance error is: ' num2str(calFFT2n_n) 'The Calculation average variance signal is: ' num2str(calFFT2n_s)]);
            disp(['The average variance error is: ' num2str(avgFFT2n_n) ' The average variance of signal is: ' num2str(avgFFT2n_s)]);
            disp(['The simulation SQNR is: ' num2str(10*log10(avgFFT2n_s/avgFFT2n_n))]);
            disp(['The calculation SQNR is: ' num2str(10*log10(calFFT2n_s/calFFT2n_n))]);
        end
        q1 = resultF.Scale.FirstFFT;
        q2 = resultF.Scale.Multi;
        q3 = resultF.Scale.SecondFFT;
        q1f = resultF.Scale.FirstFFTf;
        q2f = resultF.Scale.Multif;
        q3f = resultF.Scale.SecondFFTf;
        %         PsignalN = Ps^2*q1^2*q2^2*q3^2/1.59^2*nFFT1*nFFT3;
        %
        %         PnoiseN = Ps*q3^2*q2^2*q1^2*nFFT1*nFFT3/6*2^(-2*bits) ...
        %             + q3^2*Ps*nFFT3*q2^2*(q1^2*(nFFT1/6-1)+1/2)/3*2^(-2*bit_fft1)...
        %             + (q2^2+1/2)*q3^2*nFFT3/3*2^(-2*bit_multi) ...
        %             + (q3^2*(nFFT3/6-1)+1/2)/3*2^(-2*bit_fft2);
        %         expN1 = 10*log10(PsignalN/PnoiseN);
        [~,~,expN] = SSCA_M2(Ps,q1f,q2f,q3f,nFFT1,nFFT3,bits,bit_fft1,bit_multi,bit_fft2);
        
        
        disp('----------------Result---------------------');
        % disp(['The simulation SQNR is: ' num2str(SNRt(result.SecondFFT,resultF.SecondFFT))]);
        disp(['The simulation SQNR is: ' num2str(10*log10(avgFFT2n_s/avgFFT2n_n))]);
        disp(['The calculation SQNR is: ' num2str(10*log10(calFFT2n_s/calFFT2n_n))]);
        disp(['The Result of simplify expression of SQNR with Normalization is: ' num2str(expN)]);
        resultcell = {bit.windowing,bit.firstFFT,bit.ConjMulti,bit.secondFFT,10*log10(avgFFT2n_s/avgFFT2n_n),10*log10(calFFT2n_s/calFFT2n_n),q1,q2,q3};
end

