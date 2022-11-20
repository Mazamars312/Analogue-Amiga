
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

-- -----------------------------------------------------------------------

entity chameleon64v2_top is
	port (
-- Clocks
		clk50m : in std_logic;
		phi2_n : in std_logic;
		dotclk_n : in std_logic;

-- Buttons
		usart_cts : in std_logic;  -- Left button
		freeze_btn : in std_logic; -- Middle button
		reset_btn : in std_logic;  -- Right

-- PS/2, IEC, LEDs
		iec_present : in std_logic;

		ps2iec_sel : out std_logic;
		ps2iec : in unsigned(3 downto 0);

		ser_out_clk : out std_logic;
		ser_out_dat : out std_logic;
		ser_out_rclk : out std_logic;

		iec_clk_out : out std_logic;
		iec_srq_out : out std_logic;
		iec_atn_out : out std_logic;
		iec_dat_out : out std_logic;

-- SPI, Flash and SD-Card
		flash_cs : out std_logic;
		rtc_cs : out std_logic;
		mmc_cs : out std_logic;
		mmc_cd : in std_logic;
		mmc_wp : in std_logic;
		spi_clk : out std_logic;
		spi_miso : in std_logic;
		spi_mosi : out std_logic;

-- Clock port
		clock_ior : out std_logic;
		clock_iow : out std_logic;

-- C64 bus
		reset_in : in std_logic;

		ioef : in std_logic;
		romlh : in std_logic;

		dma_out : out std_logic;
		game_out : out std_logic;
		exrom_out : out std_logic;

		irq_in : in std_logic;
		irq_out : out std_logic;
		nmi_in : in std_logic;
		nmi_out : out std_logic;
		ba_in : in std_logic;
		rw_in : in std_logic;
		rw_out : out std_logic;

		sa_dir : out std_logic;
		sa_oe : out std_logic;
		sa15_out : out std_logic;
		low_a : inout unsigned(15 downto 0);

		sd_dir : out std_logic;
		sd_oe : out std_logic;
		low_d : inout unsigned(7 downto 0);

-- SDRAM
		ram_clk : out std_logic;
		ram_ldqm : out std_logic;
		ram_udqm : out std_logic;
		ram_ras : out std_logic;
		ram_cas : out std_logic;
		ram_we : out std_logic;
		ram_ba : out std_logic_vector(1 downto 0);
		ram_a : out std_logic_vector(12 downto 0);
		ram_d : inout std_logic_vector(15 downto 0);

-- IR eye
		ir_data : in std_logic;

-- USB micro
		usart_clk : in std_logic;
		usart_rts : in std_logic;
		usart_rx : out std_logic;
		usart_tx : in std_logic;

-- Video output
		red : out unsigned(4 downto 0);
		grn : out unsigned(4 downto 0);
		blu : out unsigned(4 downto 0);
		hsync_n : out std_logic;
		vsync_n : out std_logic;

-- Audio output
		sigma_l : out std_logic;
		sigma_r : out std_logic
	);
end entity;



architecture rtl of chameleon64v2_top is
   constant reset_cycles : integer := 131071;
	
-- System clocks

	signal slowclk : std_logic;
	signal fastclk : std_logic;
	signal pll_locked : std_logic;
	signal ena_1mhz : std_logic;
	signal ena_1khz : std_logic;
	signal phi2 : std_logic;
	
-- Global signals
	signal reset : std_logic;
	signal n_reset : std_logic;

-- LEDs
	signal led_green : std_logic;
	signal led_red : std_logic;

-- Docking station
	signal no_clock : std_logic;
	signal docking_station : std_logic;
	signal phi_cnt : unsigned(7 downto 0);
	signal phi_end_1 : std_logic;
	
-- PS/2 Keyboard socket - used for second mouse
	signal ps2_keyboard_clk_in : std_logic;
	signal ps2_keyboard_dat_in : std_logic;
	signal ps2_keyboard_clk_out : std_logic;
	signal ps2_keyboard_dat_out : std_logic;

-- PS/2 Mouse
	signal ps2_mouse_clk_in: std_logic;
	signal ps2_mouse_dat_in: std_logic;
	signal ps2_mouse_clk_out: std_logic;
	signal ps2_mouse_dat_out: std_logic;
	
	signal sdram_req : std_logic := '0';
	signal sdram_ack : std_logic;
	signal sdram_we : std_logic := '0';
	signal sdram_a : unsigned(24 downto 0) := (others => '0');
	signal sdram_d : unsigned(7 downto 0);
	signal sdram_q : unsigned(7 downto 0);

	signal hsync : std_logic;
	signal vsync : std_logic;	
	signal wred : unsigned(7 downto 0);
	signal wgrn : unsigned(7 downto 0);
	signal wblu : unsigned(7 downto 0);

	-- Video
	signal vga_r: std_logic_vector(7 downto 0);
	signal vga_g: std_logic_vector(7 downto 0);
	signal vga_b: std_logic_vector(7 downto 0);
	signal vga_window : std_logic;
	signal vga_hsync : std_logic;
	signal vga_vsync : std_logic;
	
	
-- RS232 serial
	signal rs232_rxd : std_logic;
	signal rs232_txd : std_logic;

-- Sound
	signal audio_l : std_logic_vector(15 downto 0);
	signal audio_r : std_logic_vector(15 downto 0);

-- IO
	signal button_reset_n : std_logic;

	signal c64_keys : unsigned(63 downto 0);
	signal c64_restore_key_n : std_logic;
	signal c64_nmi_n : std_logic;
	signal c64_joy1 : unsigned(6 downto 0);
	signal c64_joy2 : unsigned(6 downto 0);
	signal joystick3 : unsigned(6 downto 0);
	signal joystick4 : unsigned(6 downto 0);
	signal gp1_run : std_logic;
	signal gp1_select : std_logic;
	signal joy1 : unsigned(7 downto 0);
	signal joy2 : unsigned(7 downto 0);
	signal joy3 : unsigned(7 downto 0);
	signal joy4 : unsigned(7 downto 0);
	signal ir : std_logic;
	signal ir_d : std_logic;

	-- Sigma Delta audio
	COMPONENT hybrid_pwm_sd
	generic ( depop : integer := 1 );
	PORT
	(
		clk	:	IN STD_LOGIC;
		terminate : in STD_LOGIC := '0';
		d_l	:	IN STD_LOGIC_VECTOR(15 DOWNTO 0);
		q_l	:	OUT STD_LOGIC;
		d_r	:	IN STD_LOGIC_VECTOR(15 DOWNTO 0);
		q_r	:	OUT STD_LOGIC
	);
	END COMPONENT;

	COMPONENT video_vga_dither
	GENERIC ( outbits : INTEGER := 4 );
	PORT
	(
		clk	:	IN STD_LOGIC;
		hsync	:	IN STD_LOGIC;
		vsync	:	IN STD_LOGIC;
		vid_ena	:	IN STD_LOGIC;
		iRed	:	IN UNSIGNED(7 DOWNTO 0);
		iGreen	:	IN UNSIGNED(7 DOWNTO 0);
		iBlue	:	IN UNSIGNED(7 DOWNTO 0);
		oRed	:	OUT UNSIGNED(outbits-1 DOWNTO 0);
		oGreen	:	OUT UNSIGNED(outbits-1 DOWNTO 0);
		oBlue	:	OUT UNSIGNED(outbits-1 DOWNTO 0)
	);
	END COMPONENT;

begin
-- -----------------------------------------------------------------------
-- Unused pins
-- -----------------------------------------------------------------------
	iec_clk_out <= '0';
	iec_atn_out <= '0';
	iec_dat_out <= '0';
	iec_srq_out <= '0';
	irq_out <= '0';
	nmi_out <= '0';
	usart_rx<='1';

	-- put these here?
	flash_cs <= '1';
	rtc_cs <= '0';
	

-- -----------------------------------------------------------------------
-- Reset
-- -----------------------------------------------------------------------
	myReset : entity work.gen_reset
		generic map (
			resetCycles => reset_cycles
		)
		port map (
			clk => slowclk,
			enable => '1',

			button => not reset_btn,
			reset => reset,
			nreset => n_reset
		);

-- -----------------------------------------------------------------------
-- 1 Mhz and 1 Khz clocks
-- -----------------------------------------------------------------------
	my1Mhz : entity work.chameleon_1mhz
		generic map (
			clk_ticks_per_usec => 100
		)
		port map (
			clk => fastclk,
			ena_1mhz => ena_1mhz,
			ena_1mhz_2 => open
		);

	my1Khz : entity work.chameleon_1khz
		port map (
			clk => fastclk,
			ena_1mhz => ena_1mhz,
			ena_1khz => ena_1khz
		);
	
-- -----------------------------------------------------------------------
-- PS2IEC multiplexer
-- -----------------------------------------------------------------------
	io_ps2iec_inst : entity work.chameleon2_io_ps2iec
		port map (
			clk => slowclk,

			ps2iec_sel => ps2iec_sel,
			ps2iec => ps2iec,

			ps2_mouse_clk => ps2_mouse_clk_in,
			ps2_mouse_dat => ps2_mouse_dat_in,
			ps2_keyboard_clk => ps2_keyboard_clk_in,
			ps2_keyboard_dat => ps2_keyboard_dat_in,

			iec_clk => open, -- iec_clk_in,
			iec_srq => open, -- iec_srq_in,
			iec_atn => open, -- iec_atn_in,
			iec_dat => open  -- iec_dat_in
		);

-- -----------------------------------------------------------------------
-- LED, PS2 and reset shiftregister
-- -----------------------------------------------------------------------
	io_shiftreg_inst : entity work.chameleon2_io_shiftreg
		port map (
			clk => slowclk,

			ser_out_clk => ser_out_clk,
			ser_out_dat => ser_out_dat,
			ser_out_rclk => ser_out_rclk,

			reset_c64 => reset,
			reset_iec => reset,
			ps2_mouse_clk => ps2_mouse_clk_out,
			ps2_mouse_dat => ps2_mouse_dat_out,
			ps2_keyboard_clk => ps2_keyboard_clk_out,
			ps2_keyboard_dat => ps2_keyboard_dat_out,
			led_green => led_green,
			led_red => led_red
		);

-- -----------------------------------------------------------------------
-- Chameleon IO, docking station and cartridge port
-- -----------------------------------------------------------------------
	chameleon2_io_blk : block
	begin
		chameleon2_io_inst : entity work.chameleon2_io
			generic map (
				enable_docking_station => true,
				enable_cdtv_remote => true,
				enable_c64_joykeyb => true,
				enable_c64_4player => true
			)
			port map (
				clk => fastclk,
				ena_1mhz => ena_1mhz,
				phi2_n => phi2_n,
				dotclock_n => dotclk_n,

				reset => reset,

				ir_data => ir,
				ioef => ioef,
				romlh => romlh,

				dma_out => dma_out,
				game_out => game_out,
				exrom_out => exrom_out,

				ba_in => ba_in,
--				rw_in => rw_in,
				rw_out => rw_out,

				sa_dir => sa_dir,
				sa_oe => sa_oe,
				sa15_out => sa15_out,
				low_a => low_a,

				sd_dir => sd_dir,
				sd_oe => sd_oe,
				low_d => low_d,

				no_clock => no_clock,
				docking_station => docking_station,

				phi_cnt => phi_cnt,
				phi_end_1 => phi_end_1,

				joystick1 => c64_joy1,
				joystick2 => c64_joy2,
				joystick3 => joystick3,
				joystick4 => joystick4,
				keys => c64_keys,
--				restore_key_n => restore_n
				restore_key_n => open
			);
	end block;

-- Synchronise IR signal
process (slowclk)
begin
	if rising_edge(slowclk) then
		ir_d<=ir_data;
		ir<=ir_d;
	end if;
end process;

		
--joy1<=not gp1_run & not gp1_select & (c64_joy1 and cdtv_joy1);
gp1_run<=c64_keys(11) and c64_keys(56) when c64_joy1="1111111" else '1';
gp1_select<=c64_keys(60) when c64_joy1="1111111" else '1';
joy1<=gp1_run & (gp1_select and c64_joy1(6)) & c64_joy1(5 downto 0);
joy2<="1" & c64_joy2;
joy3<="1" & joystick3;
joy4<="1" & joystick4;
	
vga_window<='1';


end architecture;

