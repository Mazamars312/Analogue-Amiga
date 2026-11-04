// Behavioral simulation model of Altera `dpram` dual-port RAM.
// Used only for local simulation (Quartus uses the real IP core).
//
// Parameters:
//   ADDR_WIDTH - address width (default 8)
//   DATA_WIDTH - data width (default 16)
//
// Ports:
//   clock      - single clock (both ports)
//   address_a  - port A address
//   wren_a     - port A write enable
//   data_a     - port A write data
//   q_a        - port A read data
//   address_b  - port B address
//   wren_b     - port B write enable
//   data_b     - port B write data
//   q_b        - port B read data
//
module dpram #(
    parameter ADDR_WIDTH = 8,
    parameter DATA_WIDTH = 16
) (
    input  wire                    clock,
    input  wire [ADDR_WIDTH-1:0]   address_a,
    input  wire                    wren_a,
    input  wire [DATA_WIDTH-1:0]   data_a,
    output reg  [DATA_WIDTH-1:0]   q_a,
    input  wire [ADDR_WIDTH-1:0]   address_b,
    input  wire                    wren_b,
    input  wire [DATA_WIDTH-1:0]   data_b,
    output reg  [DATA_WIDTH-1:0]   q_b
);

    localparam DEPTH = 1 << ADDR_WIDTH;
    reg [DATA_WIDTH-1:0] mem [0:DEPTH-1];

    // Port A: write-first behavior
    always @(posedge clock) begin
        if (wren_a)
            mem[address_a] <= data_a;
        q_a <= mem[address_a];
    end

    // Port B: write-first behavior
    always @(posedge clock) begin
        if (wren_b)
            mem[address_b] <= data_b;
        q_b <= mem[address_b];
    end

endmodule



