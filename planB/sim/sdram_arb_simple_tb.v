`timescale 1ns/1ps

// Simplified SDRAM arbitration behavioral model for Plan B baseline.
//
// Models the arbitration priority in sdram_ctrl.v state 0:
//   if(~chipDMA | ~chipRW) slot = CHIP      (chipset priority)
//   else if(write_req)     slot = CPU_WRITE
//   else if(cache_req)     slot = CPU_READ
//
// The SDRAM state machine runs on clk_114 (~113.5 MHz), advancing one state
// per c_7m (~7 MHz) pulse. 16 states per full SDRAM cycle.
//
// Measures: how many clk cycles the CPU waits for ramready under chip DMA.

module sdram_arb_simple_tb;

    // Clock: 113.5 MHz
    reg clk = 0;
    always #4.4 clk = ~clk;

    // c_7m: ~7 MHz enable (divide clk by 16)
    reg [3:0] c7_div = 0;
    wire c_7m_pulse = (c7_div == 15);
    always @(posedge clk) begin
        if (c7_div == 15) c7_div <= 0;
        else c7_div <= c7_div + 1;
    end

    // SDRAM state machine (16 states, 0-15)
    reg [3:0] sdram_state = 0;
    always @(posedge clk) begin
        if (c_7m_pulse) begin
            if (sdram_state == 15) sdram_state <= 0;
            else sdram_state <= sdram_state + 1;
        end
    end

    // ---- Inputs ----
    reg chipRW = 1;
    reg cpuCS = 0;
    reg [1:0] cpustate = 1;
    reg cpuU = 0, cpuL = 0;
    reg cache_req = 0;
    reg write_req = 0;

    // Chipset DMA model: pulses active every DMA_PERIOD clk cycles
    // simulates display/audio/disk DMA requesting SDRAM slots
    reg chipDMA = 0;
    reg [7:0] dma_counter = 0;
    parameter DMA_PERIOD = 8;   // chip DMA request every 8 clk cycles

    always @(posedge clk) begin
        if (dma_counter == DMA_PERIOD - 1) begin
            dma_counter <= 0;
            chipDMA <= 1;
        end else begin
            dma_counter <= dma_counter + 1;
            chipDMA <= 0;
        end
    end

    // Allow manual override of chipDMA for burst tests
    reg chip_dma_manual = 0;
    wire chipDMA_effective = chipDMA | chip_dma_manual;

    // ---- Arbitration (state 0 only) ----
    // From sdram_ctrl.v: if(~chipDMA | ~chipRW) slot=CHIP
    wire chip_priority = (~chipDMA_effective | ~chipRW);
    wire cpu_write_prio = write_req & ~chip_priority;
    wire cpu_read_prio  = cache_req & ~chip_priority & ~cpu_write_prio;

    wire slot_chip      = chip_priority & (sdram_state == 0);
    wire slot_cpu_write = cpu_write_prio & (sdram_state == 0);
    wire slot_cpu_read  = cpu_read_prio & (sdram_state == 0);

    // CPU ramready: high when CPU gets a slot
    wire ramready = slot_cpu_read | slot_cpu_write;

    // ---- Counters ----
    integer cpu_accesses = 0;
    integer cpu_wait_cycles = 0;
    integer chip_accesses = 0;
    integer total_cycles = 0;
    integer cpu_stall_max = 0;
    integer cpu_stall_cur = 0;
    integer t1_wait, t1_acc, t1_max;

    always @(posedge clk) total_cycles <= total_cycles + 1;

    // ---- Task: CPU read — waits for ramready ----
    task cpu_read;
        begin
            reg got_ack;
            @(posedge clk);
            cpuCS <= 1;
            cpustate <= 2;       // data read
            cache_req <= 1;
            cpu_accesses <= cpu_accesses + 1;
            cpu_stall_cur <= 0;
            got_ack = 0;

            repeat (1000) begin
                @(posedge clk);
                if (ramready && !got_ack) begin
                    got_ack = 1;
                    @(posedge clk);
                    cpuCS <= 0;
                    cpustate <= 1;
                    cache_req <= 0;
                    if (cpu_stall_cur > cpu_stall_max) cpu_stall_max <= cpu_stall_cur;
                end
                if (!got_ack) begin
                    cpu_stall_cur <= cpu_stall_cur + 1;
                    cpu_wait_cycles <= cpu_wait_cycles + 1;
                end
            end
            if (!got_ack) begin
                $display("ERROR: CPU read timeout");
                $finish;
            end
        end
    endtask

    // ---- Task: manual chip DMA pulse ----
    task chip_dma_pulse;
        input        write;
    begin
        @(posedge clk);
        chip_dma_manual <= 1;
        chipRW <= write ? 0 : 1;
        chip_accesses <= chip_accesses + 1;
        @(posedge clk);
        chip_dma_manual <= 0;
        chipRW <= 1;
    end
    endtask

    initial begin
        $display("=== Plan B SDRAM Arbitration (Behavioral Model) ===");
        $display("Arbitration: if(~chipDMA|~chipRW) slot=CHIP else if(write_req) CPU-W else if(cache_req) CPU-R");
        $display("SDRAM: 16 states, c_7m gate (~7 MHz), chip DMA every %0d clk cycles", DMA_PERIOD);

        // ---- Test 1: CPU reads, no manual chip DMA (only periodic model DMA) ----
        $display("\n--- Test 1: CPU reads, periodic chip DMA only ---");
        chip_dma_manual <= 0;
        chipRW <= 1;
        write_req <= 0;
        cache_req <= 0;
        for (integer i = 0; i < 32; i = i + 1) begin
            cpu_read;
        end
        $display("CPU accesses: %0d, wait cycles: %0d, max stall: %0d clk",
                 cpu_accesses, cpu_wait_cycles, cpu_stall_max);

        // ---- Test 2: CPU reads with chip DMA every cycle (continuous) ----
        $display("\n--- Test 2: CPU reads with continuous chip DMA (every cycle) ---");
        t1_wait = cpu_wait_cycles;
        t1_acc = cpu_accesses;
        t1_max = cpu_stall_max;

        for (integer i = 0; i < 32; i = i + 1) begin
            chip_dma_pulse(0);   // chip read every cycle
            cpu_read;            // CPU read — competes for slot
        end
        $display("CPU accesses: %0d, wait cycles: %0d, max stall: %0d clk",
                 cpu_accesses - t1_acc,
                 cpu_wait_cycles - t1_wait,
                 cpu_stall_max > t1_max ? cpu_stall_max : t1_max);

        // ---- Test 3: Burst chip DMA then CPU read ----
        $display("\n--- Test 3: Burst chip DMA (16 pulses) then single CPU read ---");
        t1_wait = cpu_wait_cycles;
        t1_acc = cpu_accesses;

        for (integer i = 0; i < 16; i = i + 1) begin
            chip_dma_pulse(0);
        end
        cpu_read;
        $display("CPU wait cycles after burst DMA: %0d clk", cpu_wait_cycles - t1_wait);

        // ---- Test 4: Continuous chip DMA, many CPU reads ----
        $display("\n--- Test 4: Continuous chip DMA, 100 CPU reads ---");
        t1_wait = cpu_wait_cycles;
        t1_acc = cpu_accesses;

        for (integer i = 0; i < 100; i = i + 1) begin
            chip_dma_pulse(0);
            cpu_read;
        end
        $display("CPU accesses: %0d, wait cycles: %0d, max stall: %0d clk",
                 cpu_accesses - t1_acc,
                 cpu_wait_cycles - t1_wait,
                 cpu_stall_max > t1_max ? cpu_stall_max : t1_max);

        // ---- Test 5: Continuous chip WRITE DMA (worst case - chip always wins) ----
        $display("\n--- Test 5: Continuous chip WRITE DMA (chip always wins state 0) ---");
        t1_wait = cpu_wait_cycles;
        t1_acc = cpu_accesses;

        for (integer i = 0; i < 32; i = i + 1) begin
            chip_dma_pulse(1);   // chip write every cycle
            cpu_read;            // CPU read — starved
        end
        $display("CPU accesses: %0d, wait cycles: %0d, max stall: %0d clk",
                 cpu_accesses - t1_acc,
                 cpu_wait_cycles - t1_wait,
                 cpu_stall_max > t1_max ? cpu_stall_max : t1_max);

        $display("\n=== RESULTS ===");
        $display("Total chip DMA pulses: %0d", chip_accesses);
        $display("Total CPU accesses:    %0d", cpu_accesses);
        $display("Total CPU wait cycles: %0d clk", cpu_wait_cycles);
        $display("Max CPU stall:         %0d clk cycles", cpu_stall_max);
        $display("Avg CPU wait/access:   %0.2f clk cycles",
                 cpu_accesses > 0 ? (1.0 * cpu_wait_cycles / cpu_accesses) : 0.0);
        $display("Total cycles:          %0d clk cycles", total_cycles);

        $finish;
    end

endmodule
