-- -----------------------------------------------------------------------
--
-- Toplevel file for Turbo Chameleon 64 V1
--

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.ALL;

library work;
use work.demistify_config_pkg.all;

-- -----------------------------------------------------------------------

entity chameleon64_top is
generic (
	resetCycles: integer := 131071
);
port (
-- Clocks
	clk8 : in std_logic;
	phi2_n : in std_logic;
	dotclock_n : in std_logic;

-- Bus
	romlh_n : in std_logic;
	ioef_n : in std_logic;

-- Buttons
	freeze_n : in std_logic;

-- MMC/SPI
	spi_miso : in std_logic;
	mmc_cd_n : in std_logic;
	mmc_wp : in std_logic;

-- MUX CPLD
	mux_clk : out std_logic;
	mux : out unsigned(3 downto 0);
	mux_d : out unsigned(3 downto 0);
	mux_q : in unsigned(3 downto 0);

-- USART
	usart_tx : in std_logic;
	usart_clk : in std_logic;
	usart_rts : in std_logic;
	usart_cts : in std_logic;

-- SDRam
	ram_clk : out std_logic;
	ram_data : inout std_logic_vector(15 downto 0);
	ram_addr : out std_logic_vector(12 downto 0);
	ram_we_n : out std_logic;
	ram_ras_n : out std_logic;
	ram_cas_n : out std_logic;
	ram_ba_0 : out std_logic;
	ram_ba_1 : out std_logic;
	ram_ldqm : out std_logic;
	ram_udqm : out std_logic;

-- Video
	red : out unsigned(4 downto 0);
	grn : out unsigned(4 downto 0);
	blu : out unsigned(4 downto 0);
	hsync_n : out std_logic;
	vsync_n : out std_logic;

-- Audio
	sigma_l : out std_logic;
	sigma_r : out std_logic
);
end entity;

-- -----------------------------------------------------------------------

architecture rtl of chameleon64_top is
   constant reset_cycles : integer := 131071;
	
-- System clocks
	signal clk_100 : std_logic;
	signal clk_50 : std_logic;
	signal pll_locked : std_logic;
	
-- Global signals
	signal reset_8 : std_logic;
	signal reset_100 : std_logic;
	signal reset : std_logic;
	signal reset_n : std_logic;
	signal reset_core : std_logic;
	
-- MUX
	signal mux_clk_reg : std_logic := '0';
	signal mux_reg : unsigned(3 downto 0) := (others => '1');
	signal mux_d_reg : unsigned(3 downto 0) := (others => '1');
	signal mux_d_regd : unsigned(3 downto 0) := (others => '1');
	signal mux_regd : unsigned(3 downto 0) := (others => '1');

-- LEDs
	signal led_green : std_logic;
	signal led_red : std_logic;
	signal socleds : std_logic_vector(7 downto 0);

-- PS/2 Keyboard
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

-- SD card
	signal spi_mosi : std_logic;
	signal mmc_cs : std_logic;
	signal spi_clk : std_logic;
	signal spi_ack : std_logic;
	signal spi_req : std_logic;

-- internal SPI signals
	
	signal spi_toguest : std_logic;
	signal spi_fromguest : std_logic;
	signal spi_ss2 : std_logic;
	signal spi_ss3 : std_logic;
	signal spi_ss4 : std_logic;
	signal conf_data0 : std_logic;
	signal spi_clk_int : std_logic;
	
-- RTC

	signal rtc_cs : std_logic;
	
-- RS232 serial
	signal rs232_rxd : std_logic;
	signal rs232_txd : std_logic;
	signal midi_rxd : std_logic;
	signal midi_txd : std_logic;

-- IO
	signal ena_1mhz : std_logic;
	signal button_reset_n : std_logic;

	signal no_clock : std_logic;
	signal docking_station : std_logic;
	signal c64_keys : unsigned(64 downto 0);
	signal c64_restore_key_n : std_logic;
	signal c64_nmi_n : std_logic;
	signal c64_joy1 : unsigned(6 downto 0);
	signal c64_joy2 : unsigned(6 downto 0);
	signal c64_joy3 : unsigned(6 downto 0);
	signal c64_joy4 : unsigned(6 downto 0);
	signal joy1 : unsigned(7 downto 0);
	signal joy2 : unsigned(7 downto 0);
	signal joy3 : unsigned(7 downto 0);
	signal joy4 : unsigned(7 downto 0);
	signal usart_rx : std_logic:='1'; -- Safe default
	signal ir_data : std_logic;

-- Debounced buttons
	signal menu_button_n : std_logic;
	signal freeze_button_n : std_logic;

-- Amiga keyboard on docking station
	signal amiga_reset_n : std_logic;
	signal amiga_key : unsigned(7 downto 0);
	signal amiga_key_stb : std_logic;

	signal vga_csync : std_logic;
	signal vga_hsync : std_logic;
	signal vga_vsync : std_logic;
	signal vga_red : std_logic_vector(7 downto 0);
	signal vga_green : std_logic_vector(7 downto 0);
	signal vga_blue : std_logic_vector(7 downto 0);
	
	signal red_dithered :unsigned(7 downto 0);
	signal grn_dithered :unsigned(7 downto 0);
	signal blu_dithered :unsigned(7 downto 0);
	signal hsync_n_dithered : std_logic;
	signal vsync_n_dithered : std_logic;
	
	signal iec_atn_in : std_logic;
	signal iec_dat_in : std_logic;
	signal iec_clk_in : std_logic;
	signal iec_atn_out : std_logic;
	signal iec_dat_out : std_logic;
	signal iec_clk_out : std_logic;
	signal iec_atn_out_r : std_logic;
	signal iec_dat_out_r : std_logic;
	signal iec_clk_out_r : std_logic;
begin

rtc_cs<='0';

-- -----------------------------------------------------------------------
-- Clocks and PLL
-- -----------------------------------------------------------------------

	clocks: entity work.hostclocks
	port map
	(
		inclk0 => clk8,
		c0 => clk_100,
		c1 => clk_50,
		locked => pll_locked	
	);

	my1mhz : entity work.chameleon_1mhz
	generic map (
		-- Timer calibration. Clock speed in Mhz.
		clk_ticks_per_usec => 100
	)
	port map(
		clk => clk_100,
		ena_1mhz => ena_1mhz
	);

	-- Reset handling

	myReset : entity work.gen_reset
	generic map (
		resetCycles => reset_cycles
	)
	port map (
		clk => clk8,
		enable => '1',
		button => not (pll_locked and button_reset_n),
		reset => reset_8
	);
		
	process(clk_100,reset_8)
	begin
		if rising_edge(clk_100) then
			reset_100<=reset_8;
			reset<=reset_100;
		end if;
	end process;
	
	-- Synchronize outgoing IEC signals to clk_100
	process(clk_100)
	begin
		if rising_edge(clk_100) then
			iec_clk_out_r<=iec_clk_out;
			iec_dat_out_r<=iec_dat_out;
			iec_atn_out_r<=iec_atn_out;
		end if;
	end process;

	-- IO handling
	
	myIO : entity work.chameleon_io
	generic map (
		enable_docking_station => true,
		enable_docking_irq => true,
		enable_c64_joykeyb => true,
		enable_c64_4player => true,
		enable_raw_spi => true,
		enable_iec_access =>true
	)
	port map (
	-- Clocks
		clk => clk_100,
		clk_mux => clk_100,
		ena_1mhz => ena_1mhz,
		reset => reset,
		
		no_clock => no_clock,
		docking_station => docking_station,
		
		-- Chameleon FPGA pins
		-- C64 Clocks
		phi2_n => phi2_n,
		dotclock_n => dotclock_n, 
		-- C64 cartridge control lines
		io_ef_n => ioef_n,
		rom_lh_n => romlh_n,
		-- SPI bus
		spi_miso => spi_miso,
		-- CPLD multiplexer
		mux_clk => mux_clk,
		mux => mux,
		mux_d => mux_d,
		mux_q => mux_q,
		
		to_usb_rx => usart_rx,

		-- SPI raw signals (enable_raw_spi must be set to true)
		rtc_cs => rtc_cs,
		mmc_cs_n => mmc_cs,
		spi_raw_clk => spi_clk,
		spi_raw_mosi => spi_mosi,
		spi_raw_req => spi_req,
		spi_raw_ack => spi_ack,

		-- LEDs
		led_green => led_green,
		led_red => led_red,
		ir => ir_data,
	
		-- PS/2 Keyboard
		ps2_keyboard_clk_out => ps2_keyboard_clk_out,
		ps2_keyboard_dat_out => ps2_keyboard_dat_out,
		ps2_keyboard_clk_in => ps2_keyboard_clk_in,
		ps2_keyboard_dat_in => ps2_keyboard_dat_in,

		-- PS/2 Mouse
		ps2_mouse_clk_out => ps2_mouse_clk_out,
		ps2_mouse_dat_out => ps2_mouse_dat_out,
		ps2_mouse_clk_in => ps2_mouse_clk_in,
		ps2_mouse_dat_in => ps2_mouse_dat_in,

		-- Buttons
		button_reset_n => button_reset_n,

		-- Joysticks
		joystick1 => c64_joy1,
		joystick2 => c64_joy2,
		joystick3 => c64_joy3,
		joystick4 => c64_joy4,

		-- Keyboards
		keys => c64_keys(63 downto 0),
		restore_key_n => c64_restore_key_n,
		c64_nmi_n => c64_nmi_n,

		amiga_reset_n => amiga_reset_n,
		amiga_trigger => amiga_key_stb,
		amiga_scancode => amiga_key,

		midi_txd => midi_txd,
		midi_rxd => midi_rxd,

		iec_atn_out => iec_atn_out_r,
		iec_dat_out => iec_dat_out_r,
		iec_clk_out => iec_clk_out_r,
--			iec_srq_out : in std_logic := '1';
		iec_atn_in => iec_atn_in,
		iec_dat_in => iec_dat_in,
		iec_clk_in => iec_clk_in
--			iec_srq_in => iec_srq_in	
	);

	rs232_rxd <= '1';

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
		button_freeze_n => freeze_n,
		button_reset_n => button_reset_n,

		c64_joy1 => c64_joy1,
		c64_joy2 => c64_joy2,
		c64_joy3 => c64_joy3,
		c64_joy4 => c64_joy4,
		c64_keys => c64_keys(63 downto 0),
		c64_joykey_ena => '1', -- Set to 1 if you want the C64 keys to open the menu and emulate joypad buttons

		c64_restore_in => c64_nmi_n and c64_restore_key_n,
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

	guest: COMPONENT guest_top
	PORT map
	(
		CLOCK_27 => clk8&clk8, -- Comment out one of these lines to match the guest core.
		CLOCK_27 => clk8,
--		RESET_N => reset_n,
		-- clocks
		LED => led_red,
	
		-- SDRAM
		SDRAM_DQ => ram_data,
		SDRAM_A => ram_addr,
		SDRAM_DQML => ram_ldqm,
		SDRAM_DQMH => ram_udqm,
		SDRAM_nWE => ram_we_n,
		SDRAM_nCAS => ram_cas_n,
		SDRAM_nRAS => ram_ras_n,
--		SDRAM_nCS => ram_cs_n,	-- Hardwired on TC64
		SDRAM_BA(0) => ram_ba_0,
		SDRAM_BA(1) => ram_ba_1,
		SDRAM_CLK => ram_clk,
--		SDRAM_CKE => ram_cke, -- Hardwired on TC64

		-- SPI interface to control module
		SPI_DO_IN => spi_miso,
		SPI_DO => spi_fromguest,
		SPI_DI => spi_toguest,
		SPI_SCK => spi_clk_int,
		SPI_SS2	=> spi_ss2,
		SPI_SS3 => spi_ss3,
		SPI_SS4	=> spi_ss4,			
		CONF_DATA0 => conf_data0,
		-- Video output
		VGA_HS => vga_hsync,
		VGA_VS => vga_vsync,
		VGA_R => vga_red(7 downto 2),
		VGA_G => vga_green(7 downto 2),
		VGA_B => vga_blue(7 downto 2),
		-- Keyboard
--		PS2_CLK_IN => ps2_keyboard_clk_in or intercept,
--		PS2_DAT_IN => ps2_keyboard_dat_in or intercept,
--		C64_KEYS => std_logic_vector(c64_keys),
		-- Auxiliary signals
--		TAPE_BUTTON_N => freeze_button_n and joy1(7) and joy2(7),
--		IEC_ATN_I => iec_atn_in,
--		IEC_DATA_I => iec_dat_in,
--		IEC_CLK_I => iec_clk_in,
--		IEC_ATN_O => iec_atn_out,
--		IEC_DATA_O => iec_dat_out,
--		IEC_CLK_O => iec_clk_out,
		-- Audio output
		AUDIO_L => sigma_l,
		AUDIO_R => sigma_r
	);
	
	-- Safe defaults if we haven't connected the IEC signals.
	iec_atn_out<='1';
	iec_dat_out<='1';
	iec_clk_out<='1';
	
	-- invert sync for Chameleon V1 hardware
	
	red<=unsigned(vga_red(7 downto 3));
	grn<=unsigned(vga_green(7 downto 3));
	blu<=unsigned(vga_blue(7 downto 3));
	hsync_n<=not vga_hsync;
	vsync_n<=not vga_vsync;


	-- Pass internal signals to external SPI interface
	spi_clk <= spi_clk_int;

	controller : entity work.substitute_mcu
	generic map (
		sysclk_frequency => 500,
		debug => false,
		jtag_uart => false,
		-- SPI_FASTBIT => 2, -- Reducing this will make SPI comms faster, for cores which are clocked fast enough.
		-- SPI_INTERNALBIT => 0, -- This will make SPI comms faster, for cores which are clocked fast enough.
		SPI_EXTERNALCLK => true -- V1 hardware has external limitations on SD card speed.
	)
	port map (
		clk => clk_50,
		reset_in => not reset,
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
		spi_req => spi_req,
		spi_ack => spi_ack,
		
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

led_green<=not spi_ss4;

end architecture;
