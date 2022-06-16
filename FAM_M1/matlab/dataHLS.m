Inputsig = 'Deepsig';
order = '22222222';
type = 'FAM';
Matlab_path = ['/mnt/RDS/Database/precision/Matlab_M1/',Inputsig,'/'];
filepath = [Matlab_path,Inputsig,type,order,'Amp100Norm02v4.mat'];
load(filepath)

x = result.SecondFFT;
[idxr,idxc] = size(x);
idxrs = [1:8,25:32];
%% Generate full floating result for testbench
file = fopen('deepsig_float_result_full.dat','w');

for i = 1:idxc
    for j = 1:idxr
        fprintf(file,'%.*f,',9,real(x(j,i)));
        fprintf(file,'%.*f,',9,imag(x(j,i)));
    end
    fprintf(file,'\n');
end
fclose(file);
%% Generate floating result for testbench
file = fopen('deepsig_float_result.dat','w');

for i = 1:idxc
    for j = idxrs
        fprintf(file,'%.*f,',9,real(x(j,i)));
        fprintf(file,'%.*f,',9,imag(x(j,i)));
    end
    fprintf(file,'\n');
end
fclose(file);

%% Generating floating result of magnitude for testbench
file = fopen('deepsig_float_result_abs.dat','w');

for i = 1:idxc
    for j = idxrs
        temp = abs(x(j,i));
        fprintf(file,'%.*f,',9,temp);
    end
    fprintf(file,'\n');
end
fclose(file);

%% Generating floating result of magnitude for testbench
file = fopen('deepsig_float_result_mag.dat','w');

for i = 1:idxc
    for j = idxrs
        temp = abs(x(j,i))^2;
        fprintf(file,'%.*f,',9,temp);
    end
    fprintf(file,'\n');
end
fclose(file);