load('/mnt/RDS/Database/precision/Matlab/Deepsig/DeepsigFAM16161616Amp100Norm02v4.mat');
unibits = 16;
bit.input = unibits;
bit.windowing = unibits;
bit.firstFFT = unibits;
bit.ConjMulti = unibits;
bit.secondFFT = unibits;
ifdisp = 0;
Type = 'FAM';
result_temp = printResultv4(result,resultF,bit,Type,ifdisp);

disp('%%%%%Target range SQNR%%%%%');
testfloat = result.SecondFFTnormal([1:8,25:32],:);
testfixed = resultF.SecondFFTnormal([1:8,25:32],:);
avgs = var(reshape(testfloat,[],1));
avgn = var(reshape(testfloat,[],1)-single(reshape(testfixed,[],1)));
disp(['The SQNR is: ' num2str(10*log10(avgs/avgn)) 'dB']);
disp(['The variance of signal is: ' num2str(avgs)]);
disp(['The variance of noise is: ' num2str(avgn)]);

disp('%%%%%Rest Part SQNR%%%%%');
testfloat = result.SecondFFTnormal(9:24,:);
testfixed = resultF.SecondFFTnormal(9:24,:);
avgs = var(reshape(testfloat,[],1));
avgn = var(reshape(testfloat,[],1)-single(reshape(testfixed,[],1)));
disp(['The SQNR is: ' num2str(10*log10(avgs/avgn)) 'dB']);
disp(['The variance of signal is: ' num2str(avgs)]);
disp(['The variance of noise is: ' num2str(avgn)]);


disp('%%%%%First Half SQNR%%%%%');
testfloat = result.SecondFFTnormal(1:16,:);
testfixed = resultF.SecondFFTnormal(1:16,:);
avgs = var(reshape(testfloat,[],1));
avgn = var(reshape(testfloat,[],1)-single(reshape(testfixed,[],1)));
disp(['The SQNR is: ' num2str(10*log10(avgs/avgn)) 'dB']);
disp(['The variance of signal is: ' num2str(avgs)]);
disp(['The variance of noise is: ' num2str(avgn)]);

disp('%%%%%Second Half SQNR%%%%%');
testfloat = result.SecondFFTnormal(17:32,:);
testfixed = resultF.SecondFFTnormal(17:32,:);
avgs = var(reshape(testfloat,[],1));
avgn = var(reshape(testfloat,[],1)-single(reshape(testfixed,[],1)));
disp(['The SQNR is: ' num2str(10*log10(avgs/avgn)) 'dB']);
disp(['The variance of signal is: ' num2str(avgs)]);
disp(['The variance of noise is: ' num2str(avgn)]);
