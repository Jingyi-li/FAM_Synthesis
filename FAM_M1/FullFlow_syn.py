#!/usr/bin/env python
# coding: utf-8

# In[1]:


import scipy.io
import numpy as np
import os
import re
import time
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


# In[13]:


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
         '#endif']
    fd.writelines(L)
    fd.close()
    
def writetcl(fname,n):
    fd = open(fname, 'w')
    L = ['open_project -reset {}'.format(n[0]),'\n',
         'set_top {}'.format(n[1]),'\n',
         'add_files {} \n'.format(n[8]),
         #'add_files -tb {} \n'.format(n[9]),
         'open_solution \"solution_{}_{}_{}{}{}{}\"'.format(n[2],n[3],n[4],n[5],n[6],n[7]),'\n',
         'set_part {xczu28dr-ffvg1517-2-e} \n',
         'create_clock -period {} -name default'.format(n[3]),'\n',
         #'csim_design\n',
         'csynth_design\n',
         #'cosim_design\n',
         'export_design -format ip_catalog\n'
         ]
    fd.writelines(L)
    fd.close()
def execute_command(s):
    os.system(s)


# In[ ]:

#SIZE = 'Half'
SIZE = 'Full'
Projectname = ['SynSCD'+SIZE,'SynM2M'+SIZE,'SynM2O'+SIZE]
#Projectname = ['SynSCDFull','SynM2MFull','SynM2OFull']
Topmodel = ['model_SCD','Multi2Multi','Multi2One']
addfiles = ['SplitIP_SCD_matrix.cpp','SplitIP_Multi2Multi_thred.cpp','SplitIP_Multi2One_thred.cpp']
testbenchs = ['SplitIP_SCD_matrix_TB.cpp', 'SplitIP_Multi2Multi_thred_TB.cpp','SplitIP_Multi2One_thred_TB.cpp']
for n in range(3,4):
    for i in range(idxi):
        if i<4:
            paramn[i] = int(mat[n,i][0][0])
        else:
            paramm[i-4] = mat[n,i][0][0]
    paramn = paramn.astype(int)
    writefile('auto.h', paramn,paramm)
    print('Generate auto.h for {} {}{}{}{}'.format(SIZE, paramn[0], paramn[1], paramn[2], paramn[3]))
    #os.system('matlab -nodisplay -r "cd(\''+cpath+'/matlab/\'); verify_func({},{},{},{});exit"'.format(paramn[0],paramn[1],paramn[2],paramn[3]))
    #sys.exit()
    periods= [3]
    board = 111
    for p in periods:
        for idx in range(3):
            paramp = [Projectname[idx], Topmodel[idx], board, p, paramn[0], paramn[1], paramn[2], paramn[3],addfiles[idx],testbenchs[idx]]
            tempfile = './'+Projectname[idx]+'/solution_{}_{}_{}{}{}{}/impl/ip/xilinx_com_hls_{}_1_0.zip'.format(paramp[2],paramp[3],paramn[0], paramn[1], paramn[2], paramn[3],paramp[1])
            if os.path.exists(tempfile):
                print('Skip solution_{}_{}_{}{}{}{}'.format(paramp[2],paramp[3],paramn[0], paramn[1], paramn[2], paramn[3]))
                #sys.exit()
            else:
                #sys.exit()
                print('Run solution_{}_{}_{}{}{}{}'.format(paramp[2],paramp[3],paramn[0], paramn[1], paramn[2], paramn[3]))
                writetcl('Synthesis.tcl',paramp)
                #sys.exit()
                execute_command('vivado_hls Synthesis.tcl')   # replace with 'vivado_hls run_sim.tcl'
        #fix SCD_Inter.tcl
	        #sys.exit() 
        try:
            os.rename('SCD_Inter.tcl','SCD_Inter2.tcl') 
            f1 = open('SCD_Inter2.tcl','r+')
            f2 = open('SCD_Inter.tcl','w+')
            count = 0
            for ss in f1.readlines():
                if count==50:
                    f2.write(' {}/SynSCD{}/solution_111_3_{}{}{}{}/impl \\'.format(cpath,SIZE,paramn[0], paramn[1], paramn[2], paramn[3]))
                    f2.write('\n')
                elif count==51:
                    f2.write(' {}/SynM2O{}/solution_111_3_{}{}{}{}/impl \\'.format(cpath,SIZE,paramn[0], paramn[1], paramn[2], paramn[3]))
                    f2.write('\n')
                elif count==52:
                    f2.write(' {}/SynM2M{}/solution_111_3_{}{}{}{}/impl \\'.format(cpath,SIZE,paramn[0], paramn[1], paramn[2], paramn[3]))
                    f2.write('\n')
                else:
                    f2.write(ss)
                count=count+1
        finally:
            print("finish to write")   
            f1.close()  
            f2.close() 
            execute_command('rm SCD_Inter2.tcl')           
  
        print("Try to run vivado")    
        #sys.exit()        
        execute_command('make clean')
        execute_command('make all')
        execute_command('mv ./Jupyter/SCD_Inter.bit ./Jupyter/SCD_Inter_{}_{}{}{}{}.bit'.format(SIZE,paramn[0], paramn[1], paramn[2], paramn[3]))
        execute_command('mv ./Jupyter/SCD_Inter.hwh ./Jupyter/SCD_Inter_{}_{}{}{}{}.hwh'.format(SIZE,paramn[0], paramn[1], paramn[2], paramn[3]))
        execute_command('mv ./SCD_Inter/SCD_Inter.runs/impl_1/SCD_Inter_wrapper_utilization_placed.rpt ./Result/Interface_report/{}/{}{}{}{}_utilize.rpt'.format(SIZE,paramn[0], paramn[1], paramn[2], paramn[3]))
        execute_command('mv ./SCD_Inter/SCD_Inter.runs/impl_1/SCD_Inter_wrapper_timing_summary_routed.rpt ./Result/Interface_report/{}/{}{}{}{}_timing.rpt'.format(SIZE,paramn[0], paramn[1], paramn[2], paramn[3]))
        execute_command('mv ./SCD_Inter/SCD_Inter.runs/impl_1/SCD_Inter_wrapper_power_routed.rpt ./Result/Interface_report/{}/{}{}{}{}_power.rpt'.format(SIZE,paramn[0], paramn[1], paramn[2], paramn[3]))

    #sys.exit()


# In[ ]:



# In[ ]:




