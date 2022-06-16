function OutputSignal = FFTFloatv3(InputSignal)
%FFTDOUBLE Summary of this function goes here; Matched with FFTFixedv3
%   Detailed explanation goes here
S = double(InputSignal);
FFTn = length(S(:,1));                                                      % The number of FFT
k = length(S(1,:));                                                         % How many times
N = FFTn;
W=exp(-1*2j*pi*(0:N-1)/N);                                  
OutputSignal = zeros(FFTn,k);
p=nextpow2(FFTn);                                                           % checking the size of the input array
for i = 1:k
    s = bitrevorder(S(:,i));
    s=[s zeros(1,(2^p)-length(s))];                                             % complementing an array of zeros if necessary
    Stages = log2(N);
    
%--------------------------------DIF---------------------------------------    
%     Half=N/2;
%     for stage = 1:Stages
%         for index=0:(N/(2^(stage-1))):(N-1)
%             for n=0:Half-1
%  
%                 pos=n+index+1;                                                  % index of the data sample
%                 pow=(2^(stage-1))*n;                                            % part of power of the complex multiplier
%                 w = W(pow+1);
%                 a=s(pos)+s(pos+Half);                                           % 1-st part of the "butterfly" creating operation
%                 b=(s(pos)-s(pos+Half)).*w;                                      % 2-nd part of the "butterfly" creating operation
%                 
%                 s(pos)=a;                                                       % saving computation of the 1-st part
%                 s(pos+Half)=b;
%             end
%         end
%         Half=Half/2;
%     end

%--------------------------------DIT---------------------------------------
% shift at each stage
    Half=1;
    for stage = 1:Stages
        for index=0:(N/(2^(Stages-stage))):(N-1)
            for n=0:Half-1
 
                pos=n+index+1;                                                  % index of the data sample
                pow=(2^(Stages-stage))*n;                                       % part of power of the complex multiplier
                w = W(pow+1);
                a=(s(pos)+s(pos+Half).*w);                                           % 1-st part of the "butterfly" creating operation
                b=(s(pos)-s(pos+Half).*w);                                      % 2-nd part of the "butterfly" creating operation

                s(pos)=a;                                                       % saving computation of the 1-st part
                s(pos+Half)=b;
            end
        end
        Half=Half*2;
    end
    OutputSignal(:,i) = s;
end


end

