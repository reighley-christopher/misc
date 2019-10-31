obj_dir/Vi2c : obj_dir/Vi2c.cpp i2c_wrapper.cpp 
	cd obj_dir ; make -f Vi2c.mk

obj_dir/Vnotgate__ALL.a : obj_dir/Vnotgate.mk obj_dir/Vnotgate.cpp
	cd obj_dir ; make -f Vnotgate.mk

obj_dir/Vi2c.mk obj_dir/Vi2c.cpp: i2c.v i2c_wrapper.cpp
	verilator --cc i2c.v --clk scl --exe i2c.cpp 

obj_dir/Vnoisemunger : obj_dir/Vnoisemunger.cpp nm_wrapper.cpp 
	cd obj_dir ; make -f Vnoisemunger.mk

obj_dir/Vnoisemunger.mk obj_dir/Vnoisemunger.cpp: noisemunger.v nm_wrapper.cpp
	verilator --cc noisemunger.v --clk clk --exe nm_wrapper.cpp

obj_dir/Vnotgate.mk obj_dir/Vnotgate.cpp obj_dir/Vnotgate.h: notgate/source/notgate.v
	verilator -cc notgate/source/notgate.v 


clean : 
	rm -r obj_dir
	rm notgate.bin

notgate.bin : obj_dir/verilated.o obj_dir/notgate.o obj_dir/Vnotgate__ALL.a
	g++ obj_dir/verilated.o obj_dir/notgate.o obj_dir/Vnotgate__ALL.a -lguile-2.0 -lbfd -onotgate.bin

obj_dir/verilated.o : 
	g++ -c /usr/local/share/verilator/include/verilated.cpp -oobj_dir/verilated.o

obj_dir/notgate.o : guile_wrappers/notgate_wrapper.cpp obj_dir/Vnotgate.h
	g++ -c guile_wrappers/notgate_wrapper.cpp -I/usr/local/include/guile/2.0 -I/usr/local/include/verilator -Iobj_dir -oobj_dir/notgate.o


