//noisemunger.v
//this takes a series of independently distributed noisy channels such that P(C(n) == 1) = 1/2^n
//plus a series of data channels, the distribution of which is unknown.
//the output is to depend on the strength of each channel such that if a channel with strength 1 is on then
//the probability of the output is to be twice that when the channel is off, and a channel of strength 2 is
//exactly equivalent to two channels of strength 1 which are exactly correlated.

module noisemunger(clk, nois, in, out);
  input [0:0] clk;
  input [3:0] nois;
  input [7:0] in;
  output [0:0] out;
  always @(posedge clk) begin
    out <= in[0] | nois[0];
  end
endmodule

