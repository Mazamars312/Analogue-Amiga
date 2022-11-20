// Composite-like horizontal blending by Kitrinx

module cofi (
    input        clk,
    input        pix_ce,
    input        enable,

    input        hblank,
    input        vblank,
    input        hs,
    input        vs,
    input  [5:0] red,
    input  [5:0] green,
    input  [5:0] blue,

    output reg       hblank_out,
    output reg       vblank_out,
    output reg       hs_out,
    output reg       vs_out,
    output reg [5:0] red_out,
    output reg [5:0] green_out,
    output reg [5:0] blue_out
);

    function bit [5:0] color_blend (
        input [5:0] color_prev,
        input [5:0] color_curr,
        input blank_last
    );
    begin
        color_blend = blank_last ? color_curr : (color_prev >> 1) + (color_curr >> 1);
    end
    endfunction

reg [5:0] red_last;
reg [5:0] green_last;
reg [5:0] blue_last;

wire      ce = enable ? pix_ce : 1'b1;
always @(posedge clk) if (ce) begin
    hblank_out <= hblank;
    vblank_out <= vblank;
    vs_out     <= vs;
    hs_out     <= hs;

    red_last   <= red;
    blue_last  <= blue;
    green_last <= green;

    red_out    <= enable ? color_blend(red_last,   red,   hblank_out) : red;
    blue_out   <= enable ? color_blend(blue_last,  blue,  hblank_out) : blue;
    green_out  <= enable ? color_blend(green_last, green, hblank_out) : green;
end

endmodule
