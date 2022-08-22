clear all; clc
% Fig 13 FAM methods for DSP utilization at different conditions

load('./Data/uniBit_M1_M2.mat')
unibits_Dsp_M1 = [160,160,160,160,160,160,318,320,320,320,320,320,320,320,636,640]';
unibits_Dsp_M2 = [230,230,230,230,326,372,394,396,460,460,460,460,652]';
unibits_M1 = 14:29;
unibits_M2 = 14:26;
op_M1 = [28.19475609,280;77.0536262997094,558];

figure
hold on;
p1 = plot(unibits_M1, unibits_Dsp_M1, 'b*','DisplayName','FAM M1');
p2 = plot(unibits_M2, unibits_Dsp_M2, 'r*','DisplayName','FAM M2');
legend([p1,p2],'Location','best');
ylabel('Utilization of DSPs');
xlabel('Wordlength');
%%
figure
hold on;
p1 = plot(cell2mat(unitBit_M1(2:17,6)), unibits_Dsp_M1, 'b*','DisplayName','FAM M1');
p2 = plot(cell2mat(unitBit_M2(2:14,6)), unibits_Dsp_M2, 'r*','DisplayName','FAM M2');
p3 = plot(op_M1(:,1),op_M1(:,2),'bs','DisplayName','FAM M1 optimize');
p4 = plot(cell2mat(unitBit_M1(15,6)),unibits_Dsp_M1(14),'bO','DisplayName','Choosen for FAM M1');
p5 = plot(cell2mat(unitBit_M2(5,6)),unibits_Dsp_M2(4),'rO','DisplayName','Choosen for FAM M2');
legend([p1,p2,p3,p4,p5],'Location','best');
ylabel('Utilization of DSPs');
xlabel('SQNR');
