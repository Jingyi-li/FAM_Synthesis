open_project -reset FAM_M1
set_top model
add_files SQNR_check.cpp 
add_files -tb SQNR_check_TB.cpp 
open_solution "solution_111_3_26262626"
set_part {xczu28dr-ffvg1517-2-e} 
create_clock -period 3 -name default
csim_design
