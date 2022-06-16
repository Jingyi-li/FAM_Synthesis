overlay_name := SCD_Inter
design_name := SCD_Inter

all: block bitstream
	@echo
	@tput setaf 2 ; echo "Built $(overlay_name) successfully!"; tput sgr0;
	@echo

block:
	vivado -mode batch -source $(overlay_name).tcl -notrace
	vivado -mode batch -source create_design.tcl -notrace

bitstream:
	vivado -mode batch -source build_bitstream.tcl -notrace

clean:
	rm -rf $(overlay_name) *.jou *.log NA
