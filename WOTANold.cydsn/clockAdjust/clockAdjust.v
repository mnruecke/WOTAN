
//`#start header` -- edit after this line, do not edit this line
// ========================================
//
// Copyright YOUR COMPANY, THE YEAR
// All Rights Reserved
// UNPUBLISHED, LICENSED SOFTWARE.
//
// CONFIDENTIAL AND PROPRIETARY INFORMATION
// WHICH IS THE PROPERTY OF your company.
//
// ========================================
`include "cypress.v"
//`#end` -- edit above this line, do not edit this line
// Generated on 01/09/2018 at 16:04
// Component: clockAdjust
module clockAdjust (
	output  clk_out,
	input   clk_inA,
	input   clk_inB,
	input   clk_sw
);
	parameter accu_word = 40;

//`#start body` -- edit after this line, do not edit this line
reg [7:0] cnt;

always @ (posedge clk_inA)
begin
    cnt <= cnt + accu_word;   
end

assign clk_out = (cnt[7] && ~clk_sw) || (clk_inB && clk_sw);


//`#end` -- edit above this line, do not edit this line
endmodule
//`#start footer` -- edit after this line, do not edit this line
//`#end` -- edit above this line, do not edit this line
