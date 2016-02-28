obj_dir/Vi2c : obj_dir/Vi2c.cpp i2c_wrapper.cpp 
	cd obj_dir ; make -f Vi2c.mk

obj_dir/Vi2c.mk obj_dir/Vi2c.cpp: i2c.v i2c_wrapper.cpp
	verilator --cc i2c.v --clk scl --exe i2c.cpp


obj_dir/Vnoisemunger : obj_dir/Vnoisemunger.cpp nm_wrapper.cpp 
	cd obj_dir ; make -f Vnoisemunger.mk

obj_dir/Vnoisemunger.mk obj_dir/Vnoisemunger.cpp: noisemunger.v nm_wrapper.cpp
	verilator --cc noisemunger.v --clk clk --exe nm_wrapper.cpp

clean : 
	rm -r obj_dir
