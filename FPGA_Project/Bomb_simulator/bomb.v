module bomb(CLOCK_50, HEX0, SW, LEDR, LEDG);
input [0:3]SW;
input CLOCK_50;
output [0:6]HEX0;
output [0:15]LEDR;
output [0:3]LEDG;
wire slow_clock;
assign LEDG=SW;
reg[3:0] second;
reg [25:0]cntr;
wire check=  SW[1];
reg [0:15]light= 16'b0000000000000000;
assign LEDR [0:15] = light;

initial 
	begin
	second= 9;
	cntr=0;
	end 
always @(posedge CLOCK_50 & (~(check)) & (second != 0))
begin 
	if(~SW[0]) //sw0 for reset and sw1 to stop clock 
	begin
		cntr=cntr+1;
		if(cntr>=50000000)
		begin
			second=second-1;
			cntr = 0;
		end
	end
	
	else 
	begin
		second=9;
		cntr=0;
	end
end	

hex_display H0(second, HEX0);
always @(posedge CLOCK_50)
begin
if(second == 0)
begin
	
	 light= 16'b1111111111111111;	
end
end



endmodule

module hex_display(sec, Dis);
input [3:0]sec;
output reg [0:6]Dis;
always begin
		case(sec)
		0:Dis=7'b0000001;
		1:Dis=7'b1001111;
		2:Dis=7'b0010010;
		3:Dis=7'b0000110;
		4:Dis=7'b1001100;
		5:Dis=7'b0100100;
		6:Dis=7'b0100000;
		7:Dis=7'b0001111;
		8:Dis=7'b0000000;
		9:Dis=7'b0001100;
		endcase
	end
endmodule

//module slow(clk, slow_clk);
//input clk;
//output slow_clk;
//reg cntr[25:0];
//initial  cntr=0;
// always @ (posedge clk)
// begin
// 
 