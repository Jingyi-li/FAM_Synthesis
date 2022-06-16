#!/usr/bin/env python
# coding: utf-8

# In[1]:


import scipy.io
import numpy as np
import os
import re
from ctypes import *  


# In[2]:


Signal = 'Deepsig'
signal = 'deepsig'
resultname = Signal+'FAMAmp100saveresult5_testv4.mat'
mat = scipy.io.loadmat(resultname)['result_save'][0,:][0][0][:,[0,1,2,3,6,7,8]]


# In[3]:


idxn = mat.shape[0]
idxi = mat.shape[1]
paramn = np.zeros((4,))
paramm = np.zeros((3,))
cpath = os.getcwd()


# In[4]:


def writefile(fname, n,m):
    fd = open(fname, 'w')
    L = ['#ifndef _AUTO_H_\n','#define _AUTO_H_\n',
         '#define Wf {}'.format(n[0]),'\n',
         '#define F1f {}'.format(n[1]),'\n',
         '#define CMf {}'.format(n[2]),'\n',
         '#define F2f {}'.format(n[3]),'\n',
         '#define fileinput "', cpath,'/',signal,'.dat"\n',
         '#define fileresult "', cpath,'/Result/',signal,'_float_result_full{}{}{}{}.dat"\n'.format(n[0],n[1],n[2],n[3]),
         '#define filename "',cpath,'/Result/',Signal,'/FAM',signal,'{}'.format(n[0]),
         '{}'.format(n[1]),'{}'.format(n[2]),'{}'.format(n[3]), '.dat"\n',
         '#define errorfile "',cpath,'/Result/SCDerror.dat"\n',
         '#define coef_FF1 {}'.format(m[0]),'\n',
         '#define coef_CM {}'.format(m[1]),'\n',
         '#define coef_FF2 {}'.format(m[2]),'\n',
         '#endif']
    fd.writelines(L)
    fd.close()
    
def writetcl(fname,n):
    fd = open(fname, 'w')
    L = ['open_project -reset {}'.format(n[0]),'\n',
         'set_top {}'.format(n[1]),'\n',
         'add_files SQNR_check.cpp \n',
         'add_files -tb SQNR_check_TB.cpp \n',
         'open_solution \"solution_{}_{}_{}{}{}{}\"'.format(n[2],n[3],n[4],n[5],n[6],n[7]),'\n',
         'set_part {xczu28dr-ffvg1517-2-e} \n',
         'create_clock -period {} -name default'.format(n[3]),'\n',
         'csim_design\n'
         #,
         #'csynth_design\n',
         #'cosim_design\n',
         #'export_design -format ip_catalog\n'
         ]
    fd.writelines(L)
    fd.close()
def execute_command(s):
    os.system(s)


# In[5]:


for n in range(1,14):
    for i in range(idxi):
        if i<4:
            paramn[i] = int(mat[n,i][0][0])
        else:
            paramm[i-4] = mat[n,i][0][0]
    paramn = paramn.astype(int)
    writefile('auto.h', paramn,paramm)
    os.system('matlab -nodisplay -r "cd(\''+cpath+'/matlab/\'); verify_func({},{},{},{});exit"'.format(paramn[0],paramn[1],paramn[2],paramn[3]))
    #sys.exit()
    periods= [3]
    board = 111
    for p in periods:
        paramp = ['FAM_M2', 'model', board, p, paramn[0], paramn[1], paramn[2], paramn[3]]
        tempfile = './FAM/solution_{}_{}_{}{}{}{}/impl/ip/xilinx_com_hls_model_1_0.zip'.format(paramp[2],paramp[3],paramn[0], paramn[1], paramn[2], paramn[3])
        if os.path.exists(tempfile):
            print('Skip solution_{}_{}_{}{}{}{}'.format(paramp[2],paramp[3],paramn[0], paramn[1], paramn[2], paramn[3]))
        else:
            print('Run solution_{}_{}_{}{}{}{}'.format(paramp[2],paramp[3],paramn[0], paramn[1], paramn[2], paramn[3]))
            writetcl('SQNR_check.tcl',paramp)
            execute_command('vivado_hls SQNR_check.tcl')   # replace with 'vivado_hls run_sim.tcl'
            execute_command('cp ./FAM_M2/solution_{}_{}_{}{}{}{}'.format(paramp[2],paramp[3],paramn[0], paramn[1], paramn[2], paramn[3])+'/csim/report/model_csim.log '+'./Result/MR/VHLS_{}_{}_{}{}{}{}'.format(paramp[2],paramp[3],paramn[0], paramn[1], paramn[2], paramn[3])+'.log')


# In[ ]:


execute_command('rm -rf FAM_M2')

