open_project -reset SynM2OFull
set_top Multi2One
add_files SplitIP_Multi2One_thred.cpp 
add_files -tb SplitIP_Multi2One_thred_TB.cpp 
open_solution "solution_111_3_16161616"
set_part {xczu28dr-ffvg1517-2-e} 
create_clock -period 3 -name default
csynth_design
export_design -format ip_catalog
