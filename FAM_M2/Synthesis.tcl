open_project -reset SynSCDFull
set_top model_SCD
add_files SplitIP_SCD_matrix.cpp 
add_files -tb SplitIP_SCD_matrix_TB.cpp 
open_solution "solution_111_3_24242424"
set_part {xczu28dr-ffvg1517-2-e} 
create_clock -period 3 -name default
csynth_design
export_design -format ip_catalog
