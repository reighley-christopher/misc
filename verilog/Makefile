obj_dir/Vnoisemunger : obj_dir/Vnoisemunger.cpp nm_wrapper.cpp 
	cd obj_dir ; make -f Vnoisemunger.mk

obj_dir/Vnoisemunger.mk obj_dir/Vnoisemunger.cpp: noisemunger.v nm_wrapper.cpp
	verilator --cc noisemunger.v --clk clk --exe nm_wrapper.cpp

clean : 
	rm -r obj_dir
