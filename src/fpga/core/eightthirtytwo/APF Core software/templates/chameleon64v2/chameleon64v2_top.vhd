-- -----------------------------------------------------------------------
--
-- Toplevel file for Turbo Chameleon 64 V2
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library work;
use work.demistify_config_pkg.all;

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
	signal clk_100 : std_logic;
	signal clk_50 : std_logic;
	signal pll_locked : std_logic;

	signal ena_1mhz : std_logic;
	signal phi2 : std_logic;
	
-- Global signals
	signal reset_n : std_logic;

-- LEDs
	signal led_green : std_logic;
	signal led_red : std_logic;

-- Docking station
	signal no_clock : std_logic;
	signal docking_station : std_logic;
	signal docking_irq : std_logic;
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
	
	signal intercept : std_logic;
	
	signal sdram_req : std_logic := '0';
	signal sdram_ack : std_logic;
	signal sdram_we : std_logic := '0';
	signal sdram_a : unsigned(24 downto 0) := (others => '0');
	signal sdram_d : unsigned(7 downto 0);
	signal sdram_q : unsigned(7 downto 0);

	-- Video
	signal vga_red: std_logic_vector(7 downto 0);
	signal vga_green: std_logic_vector(7 downto 0);
	signal vga_blue: std_logic_vector(7 downto 0);
	signal vga_pixel : std_logic;
	signal vga_window : std_logic;
	signal vga_hsync : std_logic;
	signal vga_vsync : std_logic;
	signal vga_csync : std_logic;
	signal vga_selcsync : std_logic;
	
	signal red_dithered :unsigned(7 downto 0);
	signal grn_dithered :unsigned(7 downto 0);
	signal blu_dithered :unsigned(7 downto 0);
	signal hsync_n_dithered : std_logic;
	signal vsync_n_dithered : std_logic;
	
-- RS232 serial
	signal rs232_rxd : std_logic:='1';
	signal rs232_txd : std_logic;
	signal midi_rxd : std_logic;
	signal midi_txd : std_logic;

-- Sound
	signal audio_l : std_logic_vector(15 downto 0);
	signal audio_r : std_logic_vector(15 downto 0);

-- IO	
	signal c64_keys : unsigned(64 downto 0);
	signal c64_restore_key_n : std_logic;
	signal c64_joy1 : unsigned(6 downto 0);
	signal c64_joy2 : unsigned(6 downto 0);
	signal c64_joy3 : unsigned(6 downto 0);
	signal c64_joy4 : unsigned(6 downto 0);
	signal joy1 : unsigned(7 downto 0);
	signal joy2 : unsigned(7 downto 0);
	signal joy3 : unsigned(7 downto 0);
	signal joy4 : unsigned(7 downto 0);

-- Debounced buttons
	signal menu_button_n : std_logic;
	signal freeze_button_n : std_logic;

-- Amiga keyboard on docking station
	signal amiga_reset_n : std_logic;
	signal amiga_key : unsigned(7 downto 0);
	signal amiga_key_stb : std_logic;

-- internal SPI signals
	
	signal spi_toguest : std_logic;
	signal spi_fromguest : std_logic;
	signal spi_ss2 : std_logic;
	signal spi_ss3 : std_logic;
	signal spi_ss4 : std_logic;
	signal conf_data0 : std_logic;
	signal spi_clk_int : std_logic;

	signal act_led : std_logic;
	
	signal iec_clk_in : std_logic;
	signal iec_srq_in : std_logic;
	signal iec_atn_in : std_logic;
	signal iec_dat_in : std_logic;

	-- Internal copies of the IEC signals since they need inverting.
	signal iec_srq_s : std_logic;
	signal iec_clk_s : std_logic;
	signal iec_atn_s : std_logic;
	signal iec_dat_s : std_logic;

begin

-- -----------------------------------------------------------------------
-- Unused pins
-- -----------------------------------------------------------------------
	nmi_out <= '0';
--	usart_rx<='1';

	-- put these here?
	flash_cs <= '1';
	rtc_cs <= '0';
	
	clock_ior <='1';
	clock_iow <='1';
	irq_out <= not docking_irq;

	
	clocks: entity work.hostclocks
	port map
	(
		inclk0 => clk50m,
		c0 => clk_100,
		c1 => clk_50,
		locked => pll_locked	
	);

-- -----------------------------------------------------------------------
-- 1 Mhz and 1 Khz clocks
-- -----------------------------------------------------------------------
	my1Mhz : entity work.chameleon_1mhz
	generic map (
		clk_ticks_per_usec => 100
	)
	port map (
		clk => clk_100,
		ena_1mhz => ena_1mhz,
		ena_1mhz_2 => open
	);
	
-- -----------------------------------------------------------------------
-- PS2IEC multiplexer
-- -----------------------------------------------------------------------
	io_ps2iec_inst : entity work.chameleon2_io_ps2iec
		port map (
			clk => clk_100,

			ps2iec_sel => ps2iec_sel,
			ps2iec => ps2iec,

			ps2_mouse_clk => ps2_mouse_clk_in,
			ps2_mouse_dat => ps2_mouse_dat_in,
			ps2_keyboard_clk => ps2_keyboard_clk_in,
			ps2_keyboard_dat => ps2_keyboard_dat_in,

			iec_clk => iec_clk_in,
			iec_srq => iec_srq_in,
			iec_atn => iec_atn_in,
			iec_dat => iec_dat_in
		);

-- -----------------------------------------------------------------------
-- LED, PS2 and reset shiftregister
-- -----------------------------------------------------------------------
	io_shiftreg_inst : entity work.chameleon2_io_shiftreg
		port map (
			clk => clk_100,

			ser_out_clk => ser_out_clk,
			ser_out_dat => ser_out_dat,
			ser_out_rclk => ser_out_rclk,

			reset_c64 => not reset_n,
			reset_iec => not reset_n,
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
			enable_cdtv_remote => false,
			enable_c64_joykeyb => true,
			enable_c64_4player => true
		)
		port map (
			clk => clk_100,
			ena_1mhz => ena_1mhz,
			phi2_n => phi2_n,
			dotclock_n => dotclk_n,

			reset => not reset_n,

			ir_data => '1',
			ioef => ioef,
			romlh => romlh,

			dma_out => dma_out,
			game_out => game_out,
			exrom_out => exrom_out,

			ba_in => ba_in,
--			rw_in => rw_in,
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
			docking_irq => docking_irq,

			phi_cnt => phi_cnt,
			phi_end_1 => phi_end_1,

			joystick1 => c64_joy1,
			joystick2 => c64_joy2,
			joystick3 => c64_joy3,
			joystick4 => c64_joy4,
			keys => c64_keys(63 downto 0),
			restore_key_n => c64_restore_key_n,
			amiga_power_led => led_green,
			amiga_drive_led => led_red,
			amiga_reset_n => amiga_reset_n,
			amiga_trigger => amiga_key_stb,
			amiga_scancode => amiga_key,
			midi_rxd => midi_rxd,
			midi_txd => midi_txd
		);
	end block;

	-- Input mapping

	mergeinputs : entity work.chameleon_mergeinputs
	generic map (
		button1=>demistify_button1,
		button2=>demistify_button2,
		button3=>demistify_button3,
		button4=>demistify_button4
	)
	port map (
		clk => clk_100,
		reset_n => reset_n,
		ena_1mhz => ena_1mhz,
		ir_data => ir_data,
		button_menu_n => usart_cts,
		button_freeze_n => freeze_btn,
		button_reset_n => reset_btn,
		c64_joy1 => c64_joy1,
		c64_joy2 => c64_joy2,
		c64_joy3 => c64_joy3,
		c64_joy4 => c64_joy4,
		c64_keys => c64_keys(63 downto 0),
		c64_joykey_ena => '1', -- Set to 1 if you want the C64 keys to open the menu and emulate joypad buttons

		c64_restore_in => c64_restore_key_n and nmi_in,
		c64_restore_out => c64_keys(64),

		joy1_out => joy1,
		joy2_out => joy2,
		joy3_out => joy3,
		joy4_out => joy4,
		menu_out_n => menu_button_n,
		freeze_out_n => freeze_button_n,

		usart_cts => usart_rts,
		usart_rxd => usart_tx,
		usart_txd => usart_rx,
		usart_clk => usart_clk
	);

	-- Guest core
	
	midi_txd<='1';

	hsync_n <= vga_hsync;
	vsync_n <= vga_vsync;
	red <= unsigned(vga_red(7 downto 3));
	grn <= unsigned(vga_green(7 downto 3));
	blu <= unsigned(vga_blue(7 downto 3));


	guest: COMPONENT guest_top
	PORT map
	(
		CLOCK_27 => clk50m&clk50m, -- Comment out one of these lines to match the guest core.
		CLOCK_27 => clk50m,
--		RESET_N => reset_n,
		LED => led_red,
		-- clocks
		SDRAM_DQ => ram_d,
		SDRAM_A => ram_a,
		SDRAM_DQML => ram_ldqm,
		SDRAM_DQMH => ram_udqm,
		SDRAM_nWE => ram_we,
		SDRAM_nCAS => ram_cas,
		SDRAM_nRAS => ram_ras,
--		SDRAM_nCS => ram_cs_n,
		SDRAM_BA => ram_ba,
		SDRAM_CLK => ram_clk,
--		SDRAM_CKE => ram_cke,
		
		SPI_DO_IN => spi_miso,
		SPI_DO => spi_fromguest,
		SPI_DI => spi_toguest,
		SPI_SCK => spi_clk_int,
		SPI_SS2	=> spi_ss2,
		SPI_SS3 => spi_ss3,
		SPI_SS4	=> spi_ss4,
		CONF_DATA0 => conf_data0,

		VGA_HS => vga_hsync,
		VGA_VS => vga_vsync,
		VGA_R => vga_red(7 downto 2),
		VGA_G => vga_green(7 downto 2),
		VGA_B => vga_blue(7 downto 2),
--		C64_KEYS => std_logic_vector(c64_keys),
--		PS2_CLK_IN => ps2_keyboard_clk_in,
--		PS2_DAT_IN => ps2_keyboard_dat_in,
--		TAPE_BUTTON_N => freeze_button_n and joy1(7) and joy2(7),
--		IEC_ATN_I => iec_atn_in,
--		IEC_DATA_I => iec_dat_in,
--		IEC_CLK_I => iec_clk_in,
--		IEC_ATN_O => iec_atn_out,
--		IEC_DATA_O => iec_dat_out,
--		IEC_CLK_O => iec_clk_out,
		AUDIO_L => sigma_l,
		AUDIO_R => sigma_r
	);

	-- Safe defaults if we haven't connected the IEC port...
	iec_atn_s<='1';
	iec_dat_s<='1';
	iec_clk_s<='1';
	iec_srq_s<='1';

	iec_atn_out <= not iec_atn_s;
	iec_dat_out <= not iec_dat_s;
	iec_clk_out <= not iec_clk_s;
	iec_srq_out <= not iec_srq_s;

	-- Pass internal signals to external SPI interface
	spi_clk <= spi_clk_int;

	controller : entity work.substitute_mcu
	generic map (
		sysclk_frequency => 500,
		debug => false,
		-- SPI_FASTBIT => 2, -- Reducing this will make SPI comms faster, for cores which are clocked fast enough.
		-- SPI_INTERNALBIT => 0, -- This will make SPI comms faster, for cores which are clocked fast enough.
		jtag_uart => false
	)
	port map (
		clk => clk_50,
		reset_in => reset_btn,
		reset_out => reset_n,

		-- SPI signals
		spi_miso => spi_miso,
		spi_mosi	=> spi_mosi,
		spi_clk => spi_clk_int,
		spi_cs => mmc_cs,
		spi_fromguest => spi_fromguest,
		spi_toguest => spi_toguest,
		spi_ss2 => spi_ss2,
		spi_ss3 => spi_ss3,
		spi_ss4 => spi_ss4,
		conf_data0 => conf_data0,
		
		-- PS/2 signals
		ps2k_clk_in => ps2_keyboard_clk_in,
		ps2k_dat_in => ps2_keyboard_dat_in,
		ps2k_clk_out => ps2_keyboard_clk_out,
		ps2k_dat_out => ps2_keyboard_dat_out,
		ps2m_clk_in => ps2_mouse_clk_in,
		ps2m_dat_in => ps2_mouse_dat_in,
		ps2m_clk_out => ps2_mouse_clk_out,
		ps2m_dat_out => ps2_mouse_dat_out,

		-- Joysticks

		joy1 => std_logic_vector(joy1),
		joy2 => std_logic_vector(joy2),
		joy3 => std_logic_vector(joy3),
		joy4 => std_logic_vector(joy4),

		buttons => (0=>menu_button_n,others=>'0'),

		-- UART
		rxd => rs232_rxd,
		txd => rs232_txd,
		
		intercept => intercept
	);

led_green<=(not act_led) and not spi_ss4;
	
end architecture;

