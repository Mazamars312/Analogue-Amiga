`timescale 1ns/1ps

// SDRAM arbitration testbench for Plan B baseline measurement.
//
// Measures CPU wait cycles under chipset DMA load in sdram_ctrl.v.
//
// Arbitration (sdram_ctrl state 0):
//   1. Chipset DMA (~chipDMA | ~chipRW) — highest priority
//   2. CPU write buffer (write_req)
//   3. CPU read cache (cache_req)
//   4. Refresh (&rcnt)
//
// Chipset gets first access to every SDRAM cycle; CPU only gets the slot
// if chipset is idle. Under heavy chipset DMA (display/audio/disk), CPU
// waits.
//
// Compile: iverilog -o sdram_arb_tb.vvp sdram_arb_tb.v dpram_sim.v sdram_ctrl.v cpu_cache_new.v
// Run:     vvp sdram_arb_tb.vvp

module sdram_arb_tb;

    // Clock: 113.5 MHz (clk_114, drives SDRAM sysclk)
    reg clk = 0;
    always #4.4 clk = ~clk;

    // 7 MHz clock enable (c1 from amiga_clk, gates SDRAM state machine)
    reg [3:0] c7_div = 0;
    wire c_7m = (c7_div == 0);
    always @(posedge clk) c7_div <= c7_div + 1;

    // Reset
    reg reset_n = 0;

    // SDRAM controller interface
    reg         sd_we = 1;
    reg         sd_ras = 1;
    reg         sd_cas = 1;
    reg  [1:0]  sd_dqm = 3;
    reg         sd_clk = 0;
    wire [12:0] sd_addr;
    wire  [1:0] sd_ba;
    wire        sd_cs;
    wire [15:0] sd_data;
    wire        sd_cke;

    // SDRAM data inout model
    reg [15:0] sd_data_drive = 16'hzzzz;
    reg        sd_data_oe = 0;
    assign sd_data = sd_data_oe ? sd_data_drive : 16'hzzzz;

    // Chip interface
    reg  [22:1] chipAddr = 0;
    reg         chipL = 0;
    reg         chipU = 0;
    reg         chipRW = 1;
    reg         chipDMA = 0;
    reg  [15:0] chipWR = 0;
    wire [15:0] chipRD;
    wire [47:0] chip48;

    // CPU interface
    reg  [25:1] cpuAddr = 0;
    reg         cpuCS = 0;
    reg  [1:0]  cpustate = 1;
    reg         cpuL = 0;
    reg         cpuU = 0;
    reg  [15:0] cpuWR = 0;
    wire [15:0] cpuRD;
    wire        ramready;

    // CPU cache control
    reg  [3:0]  cpu_cache_ctrl = 4'b0001;

    // SDRAM memory model
    reg [15:0] sdram_mem [0:65535];
    reg [12:0] last_rd_addr = 0;

    // Instantiate SDRAM controller (includes cache internally)
    sdram_ctrl dut (
        .sysclk        (clk),
        .c_7m          (c_7m),
        .reset_n       (reset_n),
        .cache_rst     (1'b0),
        .cache_inhibit (1'b0),
        .cpu_cache_ctrl(cpu_cache_ctrl),

        .sd_data       (sd_data),
        .sd_addr       (sd_addr),
        .sd_dqm        (sd_dqm),
        .sd_ba         (sd_ba),
        .sd_we         (sd_we),
        .sd_ras        (sd_ras),
        .sd_cas        (sd_cas),
        .sd_cke        (sd_cke),
        .sd_clk        (sd_clk),

        .chipWR        (chipWR),
        .chipAddr      (chipAddr),
        .chipU         (chipU),
        .chipL         (chipL),
        .chipRW        (chipRW),
        .chipDMA       (chipDMA),
        .chipRD        (chipRD),
        .chip48        (chip48),

        .cpuWR         (cpuWR),
        .cpuAddr       ({4'b0, cpuAddr}),
        .cpuU          (cpuU),
        .cpuL          (cpuL),
        .cpustate      (cpustate),
        .cpuCS         (cpuCS),
        .cpuRD         (cpuRD),
        .ramready      (ramready)
    );

    // SDRAM read data model
    always @(posedge clk) begin
        // DUT drives sd_data during write CAS cycle (state 2)
        // We drive during read CAS cycle
        sd_data_oe <= 0;
        if (!sd_we && !sd_cas && !sd_ras && sd_clk) begin
            // CAS read cycle — provide data
            sd_data_drive <= sdram_mem[sd_addr[12:1]];
            sd_data_oe <= 1;
            last_rd_addr <= sd_addr[12:1];
        end
    end

    // Memory write model (DUT drives sd_data, we capture it)
    always @(posedge clk) begin
        if (!sd_ras && !sd_cas && sd_we && sd_clk && sd_addr[12:11] != 2'b00) begin
            // Write cycle with DQM from sd_addr[12:11]
            if (sd_addr[11] == 0) sdram_mem[sd_addr[12:1]][7:0]   <= sd_data;
            if (sd_addr[12] == 0) sdram_mem[sd_addr[12:1]][15:8] <= sd_data;
        end
    end

    // Counters
    integer cpu_accesses = 0;
    integer cpu_wait_cycles = 0;
    integer chip_accesses = 0;
    integer total_cycles = 0;
    integer cpu_stall_max = 0;
    integer cpu_stall_cur = 0;
    integer got_ack;
    integer t1_wait, t1_acc, t1_max;

    // Cycle counter
    always @(posedge clk) total_cycles <= total_cycles + 1;

    // Task: perform a CPU read access
    task cpu_read;
        input [25:1] addr;
    begin
        @(posedge clk);
        cpuCS   <= 1;
        cpuAddr <= addr;
        cpuL    <= 0;
        cpuU    <= 0;
        cpustate <= 2;  // data read
        cpu_accesses <= cpu_accesses + 1;
        cpu_stall_cur <= 0;

        // Wait for ramready (with timeout)
        got_ack = 0;
        repeat (2000) begin
            @(posedge clk);
            if (ramready && !got_ack) begin
                got_ack = 1;
                @(posedge clk);
                cpuCS   <= 0;
                cpustate <= 1;
                if (cpu_stall_cur > cpu_stall_max) cpu_stall_max <= cpu_stall_cur;
            end
            if (!got_ack) begin
                cpu_stall_cur <= cpu_stall_cur + 1;
                cpu_wait_cycles <= cpu_wait_cycles + 1;
            end
        end
        if (!got_ack) begin
            $display("ERROR: CPU read timeout at addr %h", addr);
            $finish;
        end
    end
    endtask

    // Task: perform a chipset DMA access (held for 1 clk cycle, picked up on c_7m)
    task chip_dma;
        input [22:1] addr;
        input        write;
    begin
        @(posedge clk);
        chipDMA <= 1;
        chipAddr <= addr;
        chipRW   <= write ? 0 : 1;
        chipL    <= 0;
        chipU    <= 0;
        chipWR   <= 16'h55aa;
        chip_accesses <= chip_accesses + 1;

        @(posedge clk);
        chipDMA <= 0;
        chipRW   <= 1;
    end
    endtask

    // Test sequence
    initial begin
        $display("=== Plan B SDRAM Arbitration Baseline Testbench ===");
        $display("SDRAM on clk_114 (~113.5 MHz), state machine gated by c_7m (~7 MHz)");
        $display("Arbitration: chipset DMA > CPU write > CPU read > refresh");

        // Reset
        reset_n = 0;
        #200;
        reset_n = 1;
        #40;

        // Wait for SDRAM init (init_done -> ramready works)
        repeat (1000) @(posedge clk);
        $display("SDRAM init complete at cycle %0d", total_cycles);

        // Test 1: CPU reads with NO chipset DMA (baseline)
        $display("\n--- Test 1: CPU reads, no chipset DMA ---");
        chipDMA <= 0;
        for (integer i = 0; i < 32; i = i + 1) begin
            cpu_read(25'h10000 + i);
        end

        $display("CPU accesses: %0d, wait cycles: %0d, max stall: %0d",
                 cpu_accesses, cpu_wait_cycles, cpu_stall_max);

        // Test 2: CPU reads with HEAVY chipset DMA (every cycle)
        $display("\n--- Test 2: CPU reads with continuous chipset DMA ---");
        t1_wait = cpu_wait_cycles;
        t1_acc = cpu_accesses;
        t1_max = cpu_stall_max;

        for (integer i = 0; i < 32; i = i + 1) begin
            chip_dma(22'h20000 + i, 0);
            cpu_read(25'h30000 + i);
        end

        $display("CPU accesses: %0d, wait cycles: %0d, max stall: %0d",
                 cpu_accesses - t1_acc,
                 cpu_wait_cycles - t1_wait,
                 cpu_stall_max > t1_max ? cpu_stall_max : t1_max);

        // Test 3: Burst chipset DMA then single CPU read
        $display("\n--- Test 3: Burst chipset DMA (16 beats) then CPU read ---");
        t1_wait = cpu_wait_cycles;
        t1_acc = cpu_accesses;

        for (integer i = 0; i < 16; i = i + 1) begin
            chip_dma(22'h40000 + i, 0);
        end
        cpu_read(25'h50000);

        $display("CPU wait cycles after burst DMA: %0d",
                 cpu_wait_cycles - t1_wait);

        // Report
        $display("\n=== RESULTS ===");
        $display("Total chip accesses: %0d", chip_accesses);
        $display("Total CPU accesses:  %0d", cpu_accesses);
        $display("Total CPU wait cycles: %0d", cpu_wait_cycles);
        $display("Max CPU stall:         %0d cycles", cpu_stall_max);
        $display("Avg CPU wait/access:   %0.2f cycles",
                 cpu_accesses > 0 ? (1.0 * cpu_wait_cycles / cpu_accesses) : 0.0);
        $display("Total cycles:          %0d", total_cycles);

        $finish;
    end

endmodule
