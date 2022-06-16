clear all; close all;
% Plot the result figure12 a and b

x = 14:26;

load('./Data/MatlabvsVivado/Deepsig_FAM_matlab_Vivado.mat')
result.input1.FAM = result_save.vivado;
load('./Data/MatlabvsVivado/SineCxn_FAM_matlab_Vivado.mat')
result.input2.FAM = result_save.vivado;
load('./Data/MatlabvsVivado/SquareCxn_FAM_matlab_Vivado.mat')
result.input3.FAM = result_save.vivado;

result.input1 = computeAvg(result.input1);
result.input2 = computeAvg(result.input2);
result.input3 = computeAvg(result.input3);
%% plot vivado vs calculation
figure;
hold on
p1 = plot(x, result.input1.FAMavgvivado(:,2),'go','DisplayName','FAM-Deepsig-vivado');
p3 = plot(x, result.input2.FAMavgvivado(:,2),'ro','DisplayName','FAM-SineCx-vivado');
p5 = plot(x, result.input3.FAMavgvivado(:,2),'bo','DisplayName','FAM-SquareCx-vivado');
p7 = plot(x, result.input1.FAMavgcal(:,2),'g--','DisplayName','FAM-Deepsig-Cal');
p9 = plot(x, result.input2.FAMavgcal(:,2),'r--','DisplayName','FAM-SineCx-Cal');
p11 = plot(x, result.input3.FAMavgcal(:,2),'b--','DisplayName','FAM-SquareCx-Cal');


p1=plot(nan, nan,'k--','DisplayName','FAM-method-theory');
p7 = plot(nan, nan,'ko','DisplayName','FAM-method-vivado');
legend([p1,p7],'Location','best');
xlabel('Average bits');
ylabel('SQNR (dB) of vivado and theory');
%% plot errors
figure;
hold on
p1 = plot(x, abs(result.input1.FAMavg(:,2)),'go','DisplayName','FAM-Deepsig');
p3 = plot(x, abs(result.input2.FAMavg(:,2)),'ro','DisplayName','FAM-SineCx');
p5 = plot(x, abs(result.input3.FAMavg(:,2)),'bo','DisplayName','FAM-SquareCx');
p7 = plot(x, zeros(length(x)),'-k','DisplayName','0dB');
p8 = plot(x, ones(length(x))*0.3,'k:','DisplayName','Range-in-0.3dB');

p1=plot(nan, nan,'ko','DisplayName','FAM-method');
p7 = plot(nan, nan,'-k','DisplayName','0dB');
p8 = plot(nan, nan,'k:','DisplayName','+-0.3dB');
legend([p1,p7,p8],'Location','best');
xlabel('Average bits');
ylabel('SQNR errors (dB) of vivado and theory');

%%
function output = computeAvg(input)
  
    temp = input.FAM(2:end,1:7);
    temp_avg = cell2mat({(cell2mat(temp(:,1))+cell2mat(temp(:,2))+cell2mat(temp(:,3))+cell2mat(temp(:,4)))/4,(double(cell2mat(temp(:,7)))-double(cell2mat(temp(:,6))))});
    temp_avg_vivado = cell2mat({(cell2mat(temp(:,1))+cell2mat(temp(:,2))+cell2mat(temp(:,3))+cell2mat(temp(:,4)))/4,double(cell2mat(temp(:,6)))});
    temp_avg_cal = cell2mat({(cell2mat(temp(:,1))+cell2mat(temp(:,2))+cell2mat(temp(:,3))+cell2mat(temp(:,4)))/4,double(cell2mat(temp(:,7)))});
    
    x = unique(temp_avg(:,1));
    avg = zeros(length(x),2);
    avg_vivado = zeros(length(x),2);
    avg_cal = zeros(length(x),2);
    avg(:,1) = x;
    avg_vivado(:,1) = x;
    avg_cal(:,1) = x;
    for i=x'
        idx = find(temp_avg(:,1)==i);
        avg(find(avg(:,1)==i),2) = sum(temp_avg(idx,2))/length(idx);
        avg_vivado(find(avg_vivado(:,1)==i),2) = sum(temp_avg_vivado(idx,2))/length(idx);
        avg_cal(find(avg_cal(:,1)==i),2) = sum(temp_avg_cal(idx,2))/length(idx);
    end
    input.FAMavg = avg;
    input.FAMavgvivado = avg_vivado;
    input.FAMavgcal = avg_cal;
    output = input;
end