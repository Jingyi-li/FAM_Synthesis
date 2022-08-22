
unibits_LUT_M2 = [20.05,17.70,20.07,21.19,26.18,29.71,36.74 ,34.95,36.04 ,25.45,26.18,27.47,31.52]';
unibits_FF_M2 = [7.72,7.87,8.79,9.69,12.52,13.78 ,15.95,14.49,15.59,14.12,14.62,16.26,17.47]';
unibits_BRAM_M2 = [14.72,14.72,15,15.09,15.09,21.34 ,21.34,21.62,21.62,21.90,21.90,22.22,22.22]';
unibits_Dsp_M2 = [21.70,21.75,21.86,22,22,22,22.05,31.04,34.04,43.07 ,43.16,43.49,61.47]';
unibits_M2 = 14:26;


figure
hold on;

p1 = plot(unibits_M2, unibits_Dsp_M2, 'b','DisplayName','DSPs');
p2 = plot(unibits_M2, unibits_LUT_M2, 'g','DisplayName','LUTs');
p3 = plot(unibits_M2, unibits_FF_M2, 'r','DisplayName','FFs');
p4 = plot(unibits_M2, unibits_BRAM_M2, 'c','DisplayName','BRAMs');
legend([p1,p2,p3,p4],'Location','best');
ylabel('Utilization Rate (%)');
xlabel('Wordlength');


unibits_LUT_M1 = [18.57, 14.80,16.94,17.93,19.2,23.46,30.07,33.12,31.16,21.52, 21.90,23.36,23.42]';
unibits_FF_M1 = [6.97,6.76,7.49,7.76,8.22,9.65,12.21,12.59,12.72,11.15,11.46,11.65,11.92]';
unibits_BRAM_M1 = [14.63,14.63,14.91,15.05,15.05,21.30 ,21.30,21.57,21.57,21.85,21.85,22.13,22.13]';
unibits_Dsp_M1 = [17.16,17.16,17.23,17.46,17.46,17.46,17.46,17.46,26.45,33.99,33.99,34.41,34.41]';
unibits_M1 = 14:26;


figure
hold on;

p1 = plot(unibits_M1, unibits_Dsp_M1, 'b','DisplayName','DSPs');
p2 = plot(unibits_M1, unibits_LUT_M1, 'g','DisplayName','LUTs');
p3 = plot(unibits_M1, unibits_FF_M1, 'r','DisplayName','FFs');
p4 = plot(unibits_M1, unibits_BRAM_M1, 'c','DisplayName','BRAMs');
legend([p1,p2,p3,p4],'Location','best');
ylabel('Utilization Rate (%)');
xlabel('Wordlength');
ylim([0 70])
