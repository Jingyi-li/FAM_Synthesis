function [Sx,alphao,fo,result]=autofamFixedv4_M1(x,fs,df,dalpha,bitInput)
%   AUTOFAM(X,FS,DF,DALPHA) computes the spectral auto-
%   correlation density function estimate of the signals X
%   by using the FFT Accumulation Method(FAM). Make sure that
%   DF is much bigger than DALPHA in order to have a reliable estimate.
%
%   INPUTS:
%   X       - input column vector;
%   FS      - sampling rate;
%   DF      - desired frequency resolution;
%   DALPHA  - desired cyclic frequency resolution
%
%   OUTPUTS:
%   SX     - spectral correlation density function estimate;
%   ALPHAO  - cyclic frequency;
%   FO      - spectrum frequency;
%
% e.g. x = (1:2240)'; fs = 1000; df = 5; dalpha = 0.5; % Add by Louis
% or x = repmat(0:0.1:0.3, [1 560]); fs = 1000; df = 5; dalpha = 0.5; % Add by Louis

if nargin ~= 5
    error('Wrong number of arguments.');
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Definition of Parameters %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Np=pow2(nextpow2(fs/df));       %Number of input channels, defined
                                %by the desired frequency
                                %resolution (df) as follows:
                                %Np=fs/df, where fs is the original
                                %data sampling rate. It must be a
                                %power of 2 to avoid truncation or
                                %zero-padding in the FFT routines;
                                
L=Np/4;                         %Offset between points in the same
                                %column at consecutive rows in the
                                %same channelization matrix. It
                                %should be chosen to be less than
                                %or equal to Np/4;
                                
P=pow2(nextpow2(fs/dalpha/L));  %Number of rows formed in the
                                %channelization matrix, defined by
                                %the desired cyclic frequency
                                %resolution (dalpha) as follows:
                                %P=fs/dalpha/L. It must be a power
                                %of 2;
                                
N=P*L;                          %Total number of points in the
                                %input data

                                
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% bits setting for each block %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% bit.input = 16;
% bit.windowing = 16;
% bit.firstFFT = 16;
% bit.ConjMulti = 17;
% bit.secondFFT = 18;
bit = bitInput;
                                
%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Input Channelization %%
%%%%%%%%%%%%%%%%%%%%%%%%%%
%ylb: this is zero padding if necessary and taking truncating if not.
if length(x) < N
    x(N)=0;
elseif length(x) > N
    x=x(1:N);
end
% n = sqrt(2^-30/12)*randn(size(x));
% xt = x;
% x = xt+n;
% snr(xt,xt-x)
NN=(P-1)*L+Np;
xx=x;
xx(NN)=0;
xx=xx(:);
X=fi(zeros(Np,P),1,bit.input,bit.input-1);
for k=0:P-1
    X(:,k+1)=xx(k*L+1:k*L+Np);
end

% n = sqrt(2^-30/12)*randn(size(X));
% xt = X;
% X = xt+n;
% 
% disp('SQNR of Framing = ')
% snr(xt,xt-X)
result.Input = X;

%%%%%%%%%%%%%%%
%% Windowing %%
%%%%%%%%%%%%%%%
% a=hamming(Np);
a = [0.079999924,0.08013916,0.080558777,0.081256866,0.082231522,0.083486557,0.085018158,0.086826324,0.088907242,0.091264725,0.093893051,0.096792221,0.099962234,0.10339737,0.10709953,0.111063,0.11528778,0.11976814,0.124506,0.12949562,0.13473511,0.14021873,0.1459465,0.15191269,0.15811539,0.16454887,0.17121124,0.17809677,0.18520355,0.19252396,0.20005608,0.20779419,0.21573448,0.22387123,0.23220062,0.24071503,0.24941254,0.25828743,0.26733208,0.27654266,0.28591156,0.29543686,0.30510902,0.31492615,0.32487679,0.33496094,0.34516716,0.35549355,0.36593056,0.37647438,0.38711739,0.39785194,0.40867424,0.41957474,0.43054962,0.44159126,0.45269203,0.4638443,0.47504425,0.48628426,0.49755669,0.50885391,0.52017021,0.53149986,0.54283333,0.55416489,0.56548882,0.57679749,0.58808327,0.59934044,0.61055946,0.62173843,0.63286591,0.64393806,0.65494537,0.66588593,0.67674828,0.6875267,0.69821739,0.70881081,0.71930122,0.72968483,0.73995209,0.75009727,0.76011467,0.77000046,0.7797451,0.78934479,0.79879189,0.80808449,0.81721115,0.82617188,0.83495903,0.84356499,0.85198784,0.86022186,0.86826134,0.87610054,0.88373566,0.89116287,0.89837646,0.90537262,0.91214752,0.91869545,0.9250145,0.93109894,0.93694687,0.94255447,0.94791603,0.95302963,0.95789337,0.96250343,0.96685791,0.97095108,0.97478485,0.9783535,0.98165512,0.98468971,0.98745537,0.98994827,0.99216843,0.99411201,0.99578285,0.99717522,0.99829102,0.99912834,0.99968529,0.99996567,0.99996567,0.99968529,0.99912834,0.99829102,0.99717522,0.99578285,0.99411201,0.99216843,0.98994827,0.98745537,0.98468971,0.98165512,0.9783535,0.97478485,0.97095108,0.96685791,0.96250343,0.95789337,0.95302963,0.94791603,0.94255447,0.93694687,0.93109894,0.9250145,0.91869545,0.91214752,0.90537262,0.89837646,0.89116287,0.88373566,0.87610054,0.86826134,0.86022186,0.85198784,0.84356499,0.83495903,0.82617188,0.81721115,0.80808449,0.79879189,0.78934479,0.7797451,0.77000046,0.76011467,0.75009727,0.73995209,0.72968483,0.71930122,0.70881081,0.69821739,0.6875267,0.67674828,0.66588593,0.65494537,0.64393806,0.63286591,0.62173843,0.61055946,0.59934044,0.58808327,0.57679749,0.56548882,0.55416489,0.54283333,0.53149986,0.52017021,0.50885391,0.49755669,0.48628426,0.47504425,0.4638443,0.45269203,0.44159126,0.43054962,0.41957474,0.40867424,0.39785194,0.38711739,0.37647438,0.36593056,0.35549355,0.34516716,0.33496094,0.32487679,0.31492615,0.30510902,0.29543686,0.28591156,0.27654266,0.26733208,0.25828743,0.24941254,0.24071503,0.23220062,0.22387123,0.21573448,0.20779419,0.20005608,0.19252396,0.18520355,0.17809677,0.17121124,0.16454887,0.15811539,0.15191269,0.1459465,0.14021873,0.13473511,0.12949562,0.124506,0.11976814,0.11528778,0.111063,0.10709953,0.10339737,0.099962234,0.096792221,0.093893051,0.091264725,0.088907242,0.086826324,0.085018158,0.083486557,0.082231522,0.081256866,0.080558777,0.08013916,0.079999924];
a = fi(a,1,bit.windowing+2,bit.windowing+1);
XW=fi(diag(a)*X,1,bit.windowing,bit.windowing-1);
result.Windowing = XW;
% xw = diag(a)*xt;
% n = sqrt(2^-30/12)*randn(size(XW));
% XW = XW + n;
% disp('SQNR of Windowing = ')
% snr(XW,n)
% disp('SQNR of Previous Steps = ')
% snr(xw,xw-XW)
% XW=int16(XW); % Add by Louis

%%%%%%%%%%%%%%%
%% First FFT %%
%%%%%%%%%%%%%%%

XF1=FFTFixedv3_M1(XW,bit.firstFFT);
%These two statements perform the same operation
%as simply doing the new third operation
XF1=fftshift(XF1);
XF1=[XF1(:,P/2+1:P) XF1(:,1:P/2)];
% XF1=fftshift(XF1, 1);
result.FirstFFT = XF1;

%surf(log(abs(XF1)));
%title('FFT of data');

%%%%%%%%%%%%%%%%%%%%
%% Downconversion %%
%%%%%%%%%%%%%%%%%%%%

% This is doing a time shift to account for the shift in L on the data prior.  each column gets a shift of column # * L.

E=zeros(Np,P);
for k=-Np/2:Np/2-1
    for m=0:P-1
        E(k+Np/2+1,m+1)=exp(-i*2*pi*k*m*L/Np);
    end
end

XD=XF1.*E;

%all this operation does is take a transpose and 2 complex conjugates
%so the complex conjugate operations cancel each other out
%XD=conj(XD');
XD=transpose(XD);  
% Prior to this, each column was a set of time, column 1 was t = 0 ... 4, column 2 was t = 2 .... 6 etc.
% Now each row is time t = 0 .... 4, row 2 t = 2 .... 6 etc.

%figure;surf(log(abs(XD))); title('Down conversion Trans');

%%%%%%%%%%%%%%%%%%%%
%% Multiplication %%
%%%%%%%%%%%%%%%%%%%%

%ylb so what the multiplication is doing, it puts a copy of the FFT in, scaled by FFT(0) then again by FFT(1) etc for all time.  
% It's similar to the auto correlation of the FFT.
XM=fi(zeros(P,Np^2),1,bit.ConjMulti+1,bit.ConjMulti-1);


for k=1:Np
    for l=1:Np
        XM(:,(k-1)*Np+l) = (fi(real(XD(:,k)).*real(XD(:,l)),1,bit.ConjMulti,bit.ConjMulti-1)...
            +fi(imag(XD(:,k)).*imag(XD(:,l)),1,bit.ConjMulti,bit.ConjMulti-1))...
            +(fi(imag(XD(:,k)).*real(XD(:,l)),1,bit.ConjMulti,bit.ConjMulti-1)...
            -fi( real(XD(:,k)).*imag(XD(:,l)),1,bit.ConjMulti,bit.ConjMulti-1))*1j;

    end
end

result.Multi = XM;


%figure;surf(log(abs(XM))); title('Post Multiplication');
%%%%%%%%%%%%%%%%
%% Second FFT %%
%%%%%%%%%%%%%%%%

XF2=FFTFixedv3_M1(XM,bit.secondFFT);
XF2=fftshift(XF2);
XF2=[XF2(:,Np^2/2+1:Np^2) XF2(:,1:Np^2/2)];
result.SecondFFT = XF2;

%figure;surf(log(abs(XF2))); title('FFT 2');

% Here he is cutting out the high frequency and low frequency components, why?  
XF2=XF2(P/4+1:3*P/4, :); % XF2=XF2(P/4:3*P/4, :); % Changed by Louis

%Get the magnitude
M=fi(abs(XF2),1,16,15);
alphao=-1:1/N:1;
fo=-.5:1/Np:.5;
Sx=zeros(Np+1,2*N+1);

% The size of M  is (P/2, Np^2)
% The size of Sx is (Np+1, 2*N+1)

%%%%%%%%%%%%%%%%%%%
%% alpha profile %%
%%%%%%%%%%%%%%%%%%%
for k1=1:P/2 % k1=1:P/2+1 % Changed by Louis
    for k2=1:Np^2
        if rem(k2,Np)==0
            l=Np/2-1;
        else
            l=rem(k2,Np)-Np/2-1;
        end
	      
        k=ceil(k2/Np)-Np/2-1;
        p=k1-P/4-1;
        alpha=(k-l)/Np+(p-1)/L/P;
        f=(k+l)/2/Np;
        
        if alpha<-1 || alpha>1
            k2=k2+1;
        elseif f<-.5 || f>.5
            k2=k2+1;
        else
            kk=ceil(1+Np*(f+.5));
            ll=1+N*(alpha+1);
            Sx(kk,ll)=M(k1,k2);
        end
    end
end
result.Sx = Sx;
end
