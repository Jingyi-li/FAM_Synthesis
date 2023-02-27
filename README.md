# FAM_Synthesis
This work corresponds to the ACM Transactions on Reconfigurable Technology and Systems Paper: https://dl.acm.org/doi/abs/10.1145/3567429
## Citation
If you want to use this code please cite our paper:

```
@article{10.1145/3567429,
author = {Li, Carol Jingyi and Li, Xiangwei and Lou, Binglei and Jin, Craig T. and Boland, David and Leong, Philip H.W.},
title = {Fixed-Point FPGA Implementation of the FFT Accumulation Method for Real-Time Cyclostationary Analysis},
year = {2022},
publisher = {Association for Computing Machinery},
address = {New York, NY, USA},
issn = {1936-7406},
url = {https://doi.org/10.1145/3567429},
doi = {10.1145/3567429},
abstract = {The spectral correlation density (SCD) is an important tool in cyclostationary signal detection and classification. Even using efficient techniques based on the fast Fourier transform (FFT), real-time implementations are challenging because of the high computational complexity. A key dimension for computational optimization lies in minimizing the wordlength employed. In this paper, we analyze the relationship between wordlength and signal-to-quantization noise in fixed-point implementations of the SCD function. A canonical SCD estimation algorithm, the FFT accumulation method (FAM) using fixed-point arithmetic is studied. We derive closed-form expressions for SQNR and compare them at wordlengths ranging from 14 to 26 bits. The differences between the calculated SQNR and bit-exact simulations are less than 1 dB. Furthermore, an HLS-based FPGA design is implemented on a Xilinx Zynq UltraScale+ XCZU28DR-2FFVG1517E RFSoC. Using less than 25% of the logic fabric on the device, it consumes 7.7 W total on-chip power and has a power efficiency of 12.4 GOPS/W, which is an order of magnitude improvement over an Nvidia Tesla K40 graphics processing unit (GPU) implementation. In terms of throughput, it achieves 50 MS/sec, which is a speedup of 1.6 over a recent optimized FPGA implementation.},
note = {Just Accepted},
journal = {ACM Trans. Reconfigurable Technol. Syst.},
month = {oct},
keywords = {FAM, HLS, SCD, FPGAs., quantization error}
}
```
and cite it using the following DOI: 10.5281/zenodo.7679339
Please also reference the included `citation.cff` file for more information.
## Results
Results of the paper can be reproduced from FAM_Synthesis/Figures
### Figure 6 & 12
Run FAM_Synthesis/Figure/Result_sqnr.m

![image](https://media.github.sydney.edu.au/user/6712/files/acf3da80-ed7d-11ec-97ad-ed8c1193110f)

### Figure 13
Run FAM_Synthesis/Figures/Result_check.m

![image](https://media.github.sydney.edu.au/user/6712/files/90f03900-ed7d-11ec-95bb-b4a2098cf3d5)

### Figure 14
Run FAM_Synthesis/Figures/Result_dsp.m

![image](https://media.github.sydney.edu.au/user/6712/files/c8f77c00-ed7d-11ec-92c4-6aa21ad33958)

### Figure 17
Run FAM_Synthesis/Figures/Result_utilize.m
![Screenshot from 2022-08-22 18-17-29](https://user-images.githubusercontent.com/33167403/185887529-108292c5-da90-4d80-bc83-02c39a53fc9b.png)

## Table 3
Run FAM_Synthesis/Figures/Result_opti.m

![image](https://media.github.sydney.edu.au/user/6712/files/a6b22e00-ed7e-11ec-8b98-249df1c3649b)

