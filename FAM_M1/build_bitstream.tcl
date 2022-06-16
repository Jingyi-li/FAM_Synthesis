set overlay_name "SCD_Inter"
set design_name "SCD_Inter"


# open project and block design
open_project -quiet ./${overlay_name}/${overlay_name}.xpr
open_bd_design ./${overlay_name}/${overlay_name}.srcs/sources_1/bd/${design_name}/${design_name}.bd

# synthesis
launch_runs synth_1 -jobs 8
wait_on_run synth_1


# implementation
launch_runs impl_1 -to_step write_bitstream -jobs 8
wait_on_run impl_1

# copy project bit file
file copy -force ./${overlay_name}/${overlay_name}.runs/impl_1/${design_name}_wrapper.bit ./Jupyter/${overlay_name}.bit

# copy project hwh file
file copy -force ./${overlay_name}/${overlay_name}.srcs/sources_1/bd/${design_name}/hw_handoff/${design_name}.hwh ./Jupyter/${overlay_name}.hwh
