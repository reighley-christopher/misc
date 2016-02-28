//i2c module. first hardware exercise is to build an i2c module and connect it to an arduino board.
//first milestone, respond to start bit (sca active low) by raising ready.
module i2c(scl, sca, address, register, ready);
  input [0:0] scl;
  input [0:0] sca;
  output [7:0] register;
  output [0:0] ready;
  always @(negedge scl) begin
    
  end
endmodule

