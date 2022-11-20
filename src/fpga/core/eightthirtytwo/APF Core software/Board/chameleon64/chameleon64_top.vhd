-- -----------------------------------------------------------------------
--
-- Turbo Chameleon
--
-- Toplevel file for Turbo Chameleon 64
--

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.ALL;

library work;
use work.Toplevel_Config.ALL;

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

architecture rtl of chameleon64_top is
	
-- System clocks
	signal fastclk : std_logic;
	signal slowclk : std_logic;

	signal reset_button_n : std_logic;
	signal pll_locked : std_logic;
	
-- Global signals
	signal n_reset : std_logic;
	
-- MUX
	signal mux_clk_reg : std_logic := '0';
	signal mux_reg : unsigned(3 downto 0) := (others => '1');
	signal mux_d_reg : unsigned(3 downto 0) := (others => '1');
	signal mux_d_regd : unsigned(3 downto 0) := (others => '1');
	signal mux_regd : unsigned(3 downto 0) := (others => '1');

-- LEDs
	signal led_green : std_logic;
	signal led_red : std_logic;

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

-- Video
	signal vga_r: unsigned(7 downto 0);
	signal vga_g: unsigned(7 downto 0);
	signal vga_b: unsigned(7 downto 0);
	signal vga_window : std_logic;
	signal vga_hsync : std_logic;
	signal vga_vsync : std_logic;

-- SD card
	signal spi_mosi : std_logic;
	signal spi_cs : std_logic;
	signal spi_clk : std_logic;
	
-- RS232 serial
	signal rs232_rxd : std_logic;
	signal rs232_txd : std_logic;

-- Sound
	signal audio_l : signed(15 downto 0);
	signal audio_r : signed(15 downto 0);

-- IO
	signal ena_1mhz : std_logic;
	signal button_reset_n : std_logic;

	signal no_clock : std_logic;
	signal docking_station : std_logic;
	signal c64_keys : unsigned(63 downto 0);
	signal c64_restore_key_n : std_logic;
	signal c64_nmi_n : std_logic;
	signal c64_joy1 : unsigned(6 downto 0);
	signal c64_joy2 : unsigned(6 downto 0);
	signal joystick3 : unsigned(6 downto 0);
	signal joystick4 : unsigned(6 downto 0);
	signal usart_rx : std_logic:='1'; -- Safe default
	signal ir : std_logic;

	-- Sigma Delta audio
	COMPONENT hybrid_pwm_sd
	generic ( depop : integer := 1 );
	PORT
	(
		clk	:	IN STD_LOGIC;
		terminate : in std_logic := '0';
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
	
--	sd_addr(12)<='0'; -- FIXME - genericise the SDRAM size
	
-- -----------------------------------------------------------------------
-- Clocks and PLL
-- -----------------------------------------------------------------------
	mypll : entity work.pll
		port map (
			inclk0 => clk8,
			c0 => ram_clk,
			c1 => fastclk,
			c2 => slowclk,
			locked => pll_locked
		);


my1mhz : entity work.chameleon_1mhz
	generic map (
		-- Timer calibration. Clock speed in Mhz.
		clk_ticks_per_usec => 100
	)
	port map(
		clk => fastclk,
		ena_1mhz => ena_1mhz
	);

myReset : entity work.gen_reset
	generic map (
		resetCycles => 131071
	)
	port map (
		clk => fastclk,
		enable => '1',
		button => not freeze_n,
		nreset => n_reset
	);
	
	myIO : entity work.chameleon_io
		generic map (
			enable_docking_station => true,
			enable_c64_joykeyb => true,
			enable_c64_4player => true,
			enable_raw_spi => true,
			enable_iec_access =>true
		)
		port map (
		-- Clocks
			clk => fastclk,
			clk_mux => fastclk,
			ena_1mhz => ena_1mhz,
			reset => not n_reset,
			
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
			mmc_cs_n => spi_cs,
			spi_raw_clk => spi_clk,
			spi_raw_mosi => spi_mosi,
--			spi_raw_ack => spi_raw_ack,

		-- LEDs
			led_green => '1',
			led_red => '1',
			ir => ir,
		
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
			joystick3 => joystick3, 
			joystick4 => joystick4,

		-- Keyboards
			keys => c64_keys,
			restore_key_n => c64_restore_key_n,
			c64_nmi_n => c64_nmi_n,

--
--			iec_clk_out : in std_logic := '1';
--			iec_dat_out : in std_logic := '1';
			iec_atn_out => rs232_txd,
--			iec_srq_out : in std_logic := '1';
			iec_clk_in => rs232_rxd
--			iec_dat_in : out std_logic;
--			iec_atn_in : out std_logic;
--			iec_srq_in : out std_logic
	
		);


	myproject : entity work.VirtualToplevel
		generic map(
			sdram_rows => 13,
			sdram_cols => 9,
			sysclk_frequency => 1333
		)
		port map(
			clk => fastclk,
			slowclk => slowclk,
			reset_in => freeze_n and pll_locked,
			
			-- SDRAM
			sdr_addr => ram_addr(12 downto 0),
			sdr_data(15 downto 0) => ram_data,
			sdr_ba(1) => ram_ba_1,
			sdr_ba(0) => ram_ba_0,
			sdr_cke => open, -- sd_cke,
			sdr_dqm(1) => ram_udqm,
			sdr_dqm(0) => ram_ldqm,
			sdr_cs => open,
			sdr_we => ram_we_n,
			sdr_cas => ram_cas_n,
			sdr_ras => ram_ras_n,
			
			-- VGA
			vga_red => vga_r,
			vga_green => vga_g,
			vga_blue => vga_b,
			
			vga_hsync => vga_hsync,
			vga_vsync => vga_vsync,
			
			vga_window => vga_window,

			-- UART
			rxd => rs232_rxd, -- rs232_rxd,
			txd => rs232_txd, -- rs232_txd,
			
			-- PS/2
			ps2k_clk_in => ps2_keyboard_clk_in,
			ps2k_dat_in => ps2_keyboard_dat_in,
			ps2k_clk_out => ps2_keyboard_clk_out,
			ps2k_dat_out => ps2_keyboard_dat_out,
			ps2m_clk_in => ps2_mouse_clk_in,
			ps2m_dat_in => ps2_mouse_dat_in,
			ps2m_clk_out => ps2_mouse_clk_out,
			ps2m_dat_out => ps2_mouse_dat_out,
			
			-- SD Card interface
			spi_cs => spi_cs,
			spi_miso => spi_miso,
			spi_mosi => spi_mosi,
			spi_clk => spi_clk,
			
			-- Audio - FIXME abstract this out, too.
			audio_l => audio_l,
			audio_r => audio_r
	);

	
dither1: if Toplevel_UseVGA=true generate
-- Dither the video down to 5 bits per gun.

	mydither : component video_vga_dither
		generic map(
			outbits => 5
		)
		port map(
			clk=>fastclk,
			hsync=>vga_hsync,
			vsync=>vga_vsync,
			vid_ena=>vga_window,
			iRed => vga_r,
			iGreen => vga_g,
			iBlue => vga_b,
			oRed => red,
			oGreen => grn,
			oBlue => blu
		);
end generate;

vsync_n <= vga_vsync;
hsync_n <= vga_hsync;

	
-- Do we have audio?  If so, instantiate two DAC channels.

audio2: if Toplevel_UseAudio = true generate
audiosd: component hybrid_pwm_sd
	port map
	(
		clk => fastclk,
		d_l(15) => not audio_l(15),
		d_l(14 downto 0) => std_logic_vector(audio_l(14 downto 0)),
		q_l => sigma_l,
		d_r(15) => not audio_r(15),
		d_r(14 downto 0) => std_logic_vector(audio_r(14 downto 0)),
		q_r => sigma_r
	);
end generate;


-- No audio?  Make the audio pins high Z.

audio3: if Toplevel_UseAudio = false generate
	sigma_l<='Z';
	sigma_r<='Z';
end generate;

end architecture;
