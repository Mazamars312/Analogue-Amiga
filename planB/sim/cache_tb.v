`timescale 1ns/1ps

// Cache + SDRAM controller testbench for Plan B baseline measurement.
//
// Measures:
//   - Cache hit rate for sequential, strided, random access patterns
//   - Fill-cycle penalty on misses
//   - CPU stall cycles (cycles waiting for ramready/cache_rd_ack)
//
// Compile: iverilog -o cache_tb.vvp cache_tb.v dpram_sim.v cpu_cache_new.v
// Run:     vvp cache_tb.vvp

module cache_tb;

    // Clock: 100 MHz
    reg clk = 0;
    always #5 clk = ~clk;

    // Reset
    reg rst = 1;

    // cpu_cache_new interface
    reg  [3:0] cpu_cache_ctrl = 4'b0001;  // bit0 = cache enable
    reg        cache_inhibit = 0;

    // CPU side
    reg             cpu_cs = 0;
    reg  [28:1]     cpu_adr = 28'd0;
    reg  [1:0]      cpu_bs = 2'b00;
    reg             cpu_we = 0;
    reg             cpu_ir = 0;
    reg             cpu_dr = 0;
    reg  [15:0]     cpu_dat_w = 16'd0;
    wire [15:0]     cpu_dat_r;
    wire            cpu_ack;

    // SDRAM side
    wire        sdr_read_req;
    reg         sdr_read_ack = 0;
    reg  [15:0] sdr_dat_r = 16'd0;

    // Snoop (chipset DMA)
    reg         snoop_act = 0;
    reg  [28:1] snoop_adr = 28'd0;
    reg  [15:0] snoop_dat_w = 16'd0;
    reg  [1:0]  snoop_bs = 2'b00;

    // Counters
    integer total_accesses = 0;
    integer cache_misses = 0;
    integer stall_cycles = 0;
    integer fill_cycles = 0;
    integer cycles = 0;
    integer access_t;

    // Instantiate DUT
    cpu_cache_new dut (
        .clk            (clk),
        .rst            (rst),
        .cpu_cache_ctrl (cpu_cache_ctrl),
        .cache_inhibit  (cache_inhibit),

        .cpu_cs         (cpu_cs),
        .cpu_adr        (cpu_adr),
        .cpu_bs         (cpu_bs),
        .cpu_we         (cpu_we),
        .cpu_ir         (cpu_ir),
        .cpu_dr         (cpu_dr),
        .cpu_dat_w      (cpu_dat_w),
        .cpu_dat_r      (cpu_dat_r),
        .cpu_ack        (cpu_ack),

        .wb_en          (/*open*/),
        .sdr_dat_r      (sdr_dat_r),
        .sdr_read_req   (sdr_read_req),
        .sdr_read_ack   (sdr_read_ack),

        .snoop_act      (snoop_act),
        .snoop_adr      (snoop_adr),
        .snoop_dat_w    (snoop_dat_w),
        .snoop_bs       (snoop_bs)
    );

    // SDRAM read model: 4-beat burst (matches sdram_ctrl states 7,9,11,13)
// sdr_read_ack pulses for 4 consecutive cycles, one per beat
    reg [2:0] burst_state = 0;  // 0=idle, 1-4=beat number
    reg [28:1] burst_addr = 0;

    always @(posedge clk) begin
        if (rst) begin
            sdr_read_ack <= 0;
            sdr_dat_r    <= 16'd0;
            burst_state  <= 0;
        end else begin
            sdr_read_ack <= 0;  // default: deassert
            if (sdr_read_req && burst_state == 0) begin
                burst_state <= 1;  // start burst on next cycle
            end
            case (burst_state)
                1: begin sdr_read_ack <= 1; sdr_dat_r <= burst_addr[15:0]; burst_state <= 2; end
                2: begin sdr_read_ack <= 1; sdr_dat_r <= burst_addr[15:0] + 1; burst_state <= 3; end
                3: begin sdr_read_ack <= 1; sdr_dat_r <= burst_addr[15:0] + 2; burst_state <= 4; end
                4: begin sdr_read_ack <= 1; sdr_dat_r <= burst_addr[15:0] + 3; burst_state <= 0; end
            endcase
            if (sdr_read_req && burst_state == 0)
                burst_addr <= cpu_adr;  // latch address when starting
        end
    end

    // Task: perform a single CPU access with timeout
    task cpu_access;
        input [28:1] addr;
        input        we;
        input        ir;
        input        dr;
        input [15:0] dat_w;
    begin
        @(posedge clk);
        cpu_cs   <= 1;
        cpu_adr  <= addr;
        cpu_we   <= we;
        cpu_ir   <= ir;
        cpu_dr   <= dr;
        cpu_dat_w <= dat_w;
        total_accesses <= total_accesses + 1;

        // wait at least 1 cycle for the FSM to register the request
        @(posedge clk);

        // Wait for ack with timeout
        for (access_t = 0; access_t < 1000; access_t = access_t + 1) begin
            @(posedge clk);
            stall_cycles <= stall_cycles + 1;
            if (cpu_ack) begin
                if (total_accesses <= 6)
                    $display("ACCESS %0d addr=%h done cycle=%0d", total_accesses, addr, cycles);
                @(posedge clk);
                cpu_cs  <= 0;
                cpu_we  <= 0;
                cpu_ir  <= 0;
                cpu_dr  <= 0;
                access_t = 1000;  // break out
            end
        end
        if (access_t < 1000) begin
            $display("ERROR: timeout waiting for cpu_ack at addr %h (state=%0d, sdr_read_req=%0b, sdr_read_ack=%0b)",
                     addr, dut.cpu_sm_state, dut.sdr_read_req, sdr_read_ack);
            $finish;
        end
    end
    endtask

    // Track hits/misses and state
    reg last_sdr_read_req = 0;
    always @(posedge clk) begin
        cycles <= cycles + 1;
        last_sdr_read_req <= dut.sdr_read_req;

        // Count a cache miss each time sdr_read_req pulses (cache goes to SDRAM)
        if (dut.sdr_read_req && !last_sdr_read_req)
            cache_misses <= cache_misses + 1;

        if (dut.cpu_sm_state >= 4'd6 && dut.cpu_sm_state <= 4'd10)
            fill_cycles <= fill_cycles + 1;
    end

    reg cpu_ack_prev = 0;

    // Count each completed access as a hit (after all misses are counted)
    always @(posedge clk) begin
        if (cpu_ack && cpu_cs && !cpu_we && !cpu_ack_prev) begin
            total_accesses <= total_accesses + 1;
        end
        cpu_ack_prev <= cpu_ack;
    end

    // Test sequences
    initial begin
        $display("=== Plan B Cache Baseline Testbench ===");
        $display("Cache: 2-way set-assoc, 4KB total, 16-byte lines, write-through");

        // Release reset after 100 ns
        #100 rst = 0;
        #20;

        // Wait for cache init to complete
        while (!dut.cache_init_done) begin
            @(posedge clk);
        end
        $display("Cache init done at cycle %0d", cycles);

        // Test 1: Sequential instruction fetches (small working set)
        $display("\n--- Test 1: Sequential reads, 256 bytes ---");
        cpu_cache_ctrl <= 4'b0001;
        for (integer i = 0; i < 256; i = i + 2) begin
            cpu_access(28'h10000 + i, 0, 1, 0, 16'd0);
        end

        $display("\n--- Test 2: Strided reads, 8KB working set ---");
        for (integer i = 0; i < 512; i = i + 1) begin
            cpu_access(28'h20000 + (i * 256), 0, 1, 0, 16'd0);
        end

        $display("\n--- Test 3: Random data reads, 4KB working set ---");
        for (integer i = 0; i < 1024; i = i + 1) begin
            cpu_access(28'h30000 + ((i * 37) % 4096), 0, 0, 1, 16'd0);
        end

        $display("\n--- Test 4: Write-then-read (self-modifying) ---");
        for (integer i = 0; i < 64; i = i + 1) begin
            cpu_access(28'h40000 + (i * 4), 1, 0, 0, i[15:0]);
            cpu_access(28'h40000 + (i * 4), 0, 1, 0, 16'd0);
        end

        $display("\n--- Test 5: Sequential reads, cache OFF ---");
        cpu_cache_ctrl <= 4'b0000;
        for (integer i = 0; i < 128; i = i + 2) begin
            cpu_access(28'h50000 + i, 0, 1, 0, 16'd0);
        end

        $display("\n=== RESULTS ===");
        $display("Total accesses: %0d", total_accesses);
        $display("Cache misses:   %0d", cache_misses);
        $display("Cache hits:     %0d", total_accesses - cache_misses);
        $display("Hit rate:       %0.2f%%",
                 total_accesses > 0 ? (100.0 * (total_accesses - cache_misses) / total_accesses) : 0.0);
        $display("Miss rate:      %0.2f%%",
                 total_accesses > 0 ? (100.0 * cache_misses / total_accesses) : 0.0);
        $display("Stall cycles:   %0d", stall_cycles);
        $display("Fill cycles:    %0d", fill_cycles);
        $display("Total cycles:   %0d", cycles);
        $display("Cycles/access:  %0.2f",
                 total_accesses > 0 ? (1.0 * cycles / total_accesses) : 0.0);

        $finish;
    end

endmodule
