library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library work;
use work.demistify_config_pkg.all;

-- -----------------------------------------------------------------------

entity sockit_top is
	port (
		FPGA_CLK1_50 : in std_logic;
		FPGA_CLK2_50 : in std_logic;
		KEY_RESET_n  : in std_logic;
		KEY          : in std_logic_vector(3 downto 0);
		SW           : in std_logic_vector(3 downto 0);
		LED          : out std_logic_vector(3 downto 0);
		-- SDRAM
		SDRAM_CLK  : out std_logic;
		SDRAM_CKE  : out std_logic;
		SDRAM_A    : out std_logic_vector(12 downto 0);
		SDRAM_BA   : out std_logic_vector(1 downto 0);
		SDRAM_DQ   : inout std_logic_vector(15 downto 0);
		SDRAM_DQMH : out std_logic;
		SDRAM_DQML : out std_logic;
		SDRAM_nCS  : out std_logic;
		SDRAM_nWE  : out std_logic;
		SDRAM_nCAS : out std_logic;
		SDRAM_nRAS : out std_logic;
		-- VGA
		VGA_HS      : out std_logic;
		VGA_VS      : out std_logic;
		VGA_R       : out std_logic_vector(7 downto 0);
		VGA_G       : out std_logic_vector(7 downto 0);
		VGA_B       : out std_logic_vector(7 downto 0);
		VGA_SYNC_n  : out std_logic;
		VGA_BLANK_n : out std_logic;
		VGA_CLK     : out std_logic;
		-- -- AUDIO
		-- SIGMA_R               : OUT STD_LOGIC;
		-- SIGMA_L               : OUT STD_LOGIC;
		-- PS2
		PS2_KEYBOARD_CLK : inout std_logic;
		PS2_KEYBOARD_DAT : inout std_logic;
		PS2_MOUSE_CLK    : inout std_logic;
		PS2_MOUSE_DAT    : inout std_logic;
		-- UART
		UART_RXD : in std_logic;
		UART_TXD : out std_logic;
		-- JOYSTICK
		JOY1_B2_P9 : in std_logic;
		JOY1_B1_P6 : in std_logic;
		JOY1_UP    : in std_logic;
		JOY1_DOWN  : in std_logic;
		JOY1_LEFT  : in std_logic;
		JOY1_RIGHT : in std_logic;
		JOYX_SEL_O : out std_logic := '1';
		-- SD Card
		SD_CS_N_O : out std_logic := '1';
		SD_SCLK_O : out std_logic := '0';
		SD_MOSI_O : out std_logic := '0';
		SD_MISO_I : in std_logic;
		-- AUDIO CODEC   
		AUD_ADCDAT   : in std_logic;
		AUD_ADCLRCK  : inout std_logic;
		AUD_BCLK     : inout std_logic;
		AUD_DACDAT   : out std_logic;
		AUD_DACLRCK  : inout std_logic;
		AUD_I2C_SCLK : out std_logic;
		AUD_I2C_SDAT : inout std_logic;
		AUD_MUTE     : out std_logic;
		AUD_XCK      : out std_logic
	);
end entity;

architecture RTL of sockit_top is

	-- System clocks
	signal locked  : std_logic;
	signal reset_n : std_logic;

	-- SPI signals
	signal sd_clk  : std_logic;
	signal sd_cs   : std_logic;
	signal sd_mosi : std_logic;
	signal sd_miso : std_logic;

	-- internal SPI signals
	signal spi_toguest   : std_logic;
	signal spi_fromguest : std_logic;
	signal spi_ss2       : std_logic;
	signal spi_ss3       : std_logic;
	signal spi_ss4       : std_logic;
	signal conf_data0    : std_logic;
	signal spi_clk_int   : std_logic;

	-- PS/2 Keyboard socket - used for second mouse
	signal ps2_keyboard_clk_in  : std_logic;
	signal ps2_keyboard_dat_in  : std_logic;
	signal ps2_keyboard_clk_out : std_logic;
	signal ps2_keyboard_dat_out : std_logic;

	-- PS/2 Mouse
	signal ps2_mouse_clk_in  : std_logic;
	signal ps2_mouse_dat_in  : std_logic;
	signal ps2_mouse_clk_out : std_logic;
	signal ps2_mouse_dat_out : std_logic;

	signal intercept : std_logic;

	-- Video
	signal vga_red   : std_logic_vector(7 downto 0);
	signal vga_green : std_logic_vector(7 downto 0);
	signal vga_blue  : std_logic_vector(7 downto 0);
	signal vga_hsync : std_logic;
	signal vga_vsync : std_logic;

	signal vga_clk_x : std_logic;
	signal vga_blank : std_logic;

	-- RS232 serial
	signal rs232_rxd : std_logic;
	signal rs232_txd : std_logic;

	-- IO
	signal joya : std_logic_vector(7 downto 0);
	signal joyb : std_logic_vector(7 downto 0);
	signal joyc : std_logic_vector(7 downto 0);
	signal joyd : std_logic_vector(7 downto 0);

	-- DAC AUDIO     
	component I2C_AV_Config
		-- generic (
		--   CLK_Freq : 24000000;
		--   I2C_Freq : 20000;
		--   LUT_SIZE : 11;
		--   Dummy_DATA : 0;
		--   SET_LIN_L : 1;
		--   SET_LIN_R : 2;
		--   SET_HEAD_L : 3;
		--   SET_HEAD_R : 4;
		--   A_PATH_CTRL : 5;
		--   D_PATH_CTRL : 6;
		--   POWER_ON : 7;
		--   SET_FORMAT : 8;
		--   SAMPLE_CTRL : 9;
		--   SET_ACTIVE : 10
		-- );
		  port (
		  iCLK : in std_logic;
		  iRST_N : in std_logic;
		  oI2C_SCLK : out std_logic;
		  oI2C_SDAT : inout std_logic
		);
	  end component;
	  
	component audio_top is
		Port ( 	
			clk_50MHz : in STD_LOGIC; -- system clock (50 MHz)
			dac_MCLK : out STD_LOGIC; -- outputs to PMODI2L DAC
			dac_LRCK : out STD_LOGIC;
			dac_SCLK : out STD_LOGIC;
			dac_SDIN : out STD_LOGIC;
			L_data : 	in std_logic_vector(15 downto 0);  	-- LEFT data (16-bit signed)
			R_data : 	in std_logic_vector(15 downto 0)  	-- RIGHT data (16-bit signed) 
		);
	end component;	

	--signal dac_l: signed(15 downto 0);
	--signal dac_r: signed(15 downto 0);
    --signal dac_midi_l : signed(15 downto 0);
	--signal dac_midi_r : signed(15 downto 0);
	signal dac_l : std_logic_vector(15 downto 0);
	signal dac_r : std_logic_vector(15 downto 0);
	-- signal dac_l_s: std_logic_vector(15 downto 0);
	-- signal dac_r_s: std_logic_vector(15 downto 0);



	-- ADC AUDIO     
	component i2s_decoder is
		port (
			clk       : in std_logic;
			sck       : in std_logic;
			ws        : in std_logic;
			sd        : in std_logic;
			left_out  : out SIGNED(15 downto 0);
			right_out : out SIGNED(15 downto 0)
		);
	end component;

	signal adc_l : SIGNED(15 downto 0);
	signal adc_r : SIGNED(15 downto 0);

	-- EAR
	signal ear : std_logic;

	-- PLL2
	component pll2
		port (
			inclk0 : in std_logic;
			c0     : out std_logic;
			locked : out std_logic
		);
	end component;

	signal act_led : std_logic;

begin


	-- SPI
	SD_CS_N_O <= sd_cs;
	SD_MOSI_O <= sd_mosi;
	sd_miso   <= SD_MISO_I;
	SD_SCLK_O <= sd_clk;

	-- External devices tied to GPIOs
	ps2_mouse_dat_in <= PS2_MOUSE_DAT;
	PS2_MOUSE_DAT    <= '0' when ps2_mouse_dat_out = '0' else 'Z';
	ps2_mouse_clk_in <= PS2_MOUSE_CLK;
	PS2_MOUSE_CLK    <= '0' when ps2_mouse_clk_out = '0' else 'Z';

	ps2_keyboard_dat_in <= PS2_KEYBOARD_DAT;
	PS2_KEYBOARD_DAT    <= '0' when ps2_keyboard_dat_out = '0' else 'Z';
	ps2_keyboard_clk_in <= PS2_KEYBOARD_CLK;
	PS2_KEYBOARD_CLK    <= '0' when ps2_keyboard_clk_out = '0' else 'Z';

	JOYX_SEL_O <= '1';
	joya       <= "11" & JOY1_B2_P9 & JOY1_B1_P6 & JOY1_RIGHT & JOY1_LEFT & JOY1_DOWN & JOY1_UP;
	joyb       <= (others => '1');
	joyc       <= (others => '1');
	joyd       <= (others => '1');

	-- -- PLL2
	-- pll2_inst : pll2
	-- port map (
	-- 	inclk0		=> FPGA_CLK1_50,
	-- 	c0			=> vga_clk_x,		
	-- 	locked		=> open
	-- );

	VGA_R  <= vga_red;
	VGA_G  <= vga_green;
	VGA_B  <= vga_blue;
	VGA_HS <= vga_hsync;
	VGA_VS <= vga_vsync;

	VGA_SYNC_n  <= '0';			-- RGB/Composite sync
	VGA_BLANK_n <= '1'; 		-- not vga_blank;  	-- vga_hsync and vga_vsync; (blank signal usually gives darker blacks)
	VGA_CLK     <= vga_clk_x;	-- use clk_sys from top mist core. Could be used pll2 like UA reloaded
								-- UA reloaded has the same Video DAC ADV7123 

	-- AUDIO CODEC
	AUD_MUTE <= '1'; --SW(0);

	I2C_AV_Config_inst : I2C_AV_Config
	-- generic map (
	--   CLK_Freq => CLK_Freq,
	-- )
	port map (
	  iCLK 		=> FPGA_CLK1_50,
	  iRST_N 	=> reset_n,
	  oI2C_SCLK => AUD_I2C_SCLK,
	  oI2C_SDAT => AUD_I2C_SDAT
	);
  
	audio_i2s: entity work.audio_top
	port map(
		clk_50MHz => FPGA_CLK1_50,
		dac_MCLK  => AUD_XCK,
		dac_LRCK  => AUD_DACLRCK,
		dac_SCLK  => AUD_BCLK,
		dac_SDIN  => AUD_DACDAT,
		L_data    => std_logic_vector(dac_l),
		R_data    => std_logic_vector(dac_r)
	--	L_data    => std_logic_vector(dac_l_s),
	--	R_data    => std_logic_vector(dac_r_s)
	);		

	-- dac_l_s <= ('0' & dac_l(14 downto 0));
	-- dac_r_s <= ('0' & dac_r(14 downto 0));

	-- EAR
	midi_module : i2s_decoder
	port map(
		clk       => FPGA_CLK1_50,
		sck       => AUD_BCLK,
		ws        => AUD_ADCLRCK,
		sd        => AUD_ADCDAT,
		left_out  => adc_l,
		right_out => adc_r
	);

	--Convert adc_l  to  EAR signal 
	--Ramon Martinez Palomares, [7/5/22 23:43]
	--https://github.com/MiSTer-devel/Template_MiSTer/blob/master/sys/ltc2308.sv#L105
	--Puedes hacerte un process buscando la histeresis superior e inferior sobre el sample.
	--Por ejemplo > 20000 que te dé un 1 y < -20000 0

	process (adc_l)
	begin
		if (adc_l > 20000) then
			ear <= '1';
		elsif (adc_l < -20000) then
			ear <= '0';
		end if;
	end process;


	guest : component guest_mist
		port map
		(
			CLOCK_27 => FPGA_CLK1_50,
--         	CLOCK_27 => FPGA_CLK1_50 & FPGA_CLK1_50,
--	        RESET_N => reset_n,
			LED      => act_led,
			--SDRAM
			SDRAM_DQ   => SDRAM_DQ,
			SDRAM_A    => SDRAM_A,
			SDRAM_DQML => SDRAM_DQML,
			SDRAM_DQMH => SDRAM_DQMH,
			SDRAM_nWE  => SDRAM_nWE,
			SDRAM_nCAS => SDRAM_nCAS,
			SDRAM_nRAS => SDRAM_nRAS,
			SDRAM_nCS  => SDRAM_nCS,
			SDRAM_BA   => SDRAM_BA,
			SDRAM_CLK  => SDRAM_CLK,
			SDRAM_CKE  => SDRAM_CKE,
			--UART
--			UART_TX => UART_TXD,
--			UART_RX => UART_RXD,
			UART_TX => open,
			UART_RX => ear,
			--SPI
--			SPI_SD_DI  => sd_miso,
			SPI_DO  => spi_fromguest,
			SPI_DI  => spi_toguest,
			SPI_SCK => spi_clk_int,
			SPI_SS2 => spi_ss2,
			SPI_SS3 => spi_ss3,
--			SPI_SS4 => spi_ss4,
			CONF_DATA0 => conf_data0,
			--VGA
			VGA_HS  => vga_hsync,
			VGA_VS  => vga_vsync,
			VGA_R   => vga_red(7 downto 2),
			VGA_G   => vga_green(7 downto 2),
			VGA_B   => vga_blue(7 downto 2),
				vga_clk   => vga_clk_x,
				vga_blank => vga_blank,
			--AUDIO
			DAC_L   => dac_l,
			DAC_R   => dac_r
				--DAC_MIDI_L=> DAC_MIDI_L,
				--DAC_MIDI_R=> DAC_MIDI_R,
			--AUDIO_L => sigma_l,
			--AUDIO_R => sigma_r,
		);


		-- Pass internal signals to external SPI interface
		sd_clk <= spi_clk_int;

		controller : entity work.substitute_mcu
			generic map(
				sysclk_frequency => 500,
		--		SPI_FASTBIT=>3,
		--		SPI_INTERNALBIT=>2,		--needed if OSD hungs
				debug     => false,
				jtag_uart => false
			)
			port map(
				clk       => FPGA_CLK1_50,
				reset_in  => KEY_RESET_n,		--reset_in  when 0
				reset_out => reset_n,			--reset_out when 0

				-- SPI signals
				spi_miso      => sd_miso,
				spi_mosi      => sd_mosi,
				spi_clk       => spi_clk_int,
				spi_cs        => sd_cs,
				spi_fromguest => spi_fromguest,
				spi_toguest   => spi_toguest,
				spi_ss2       => spi_ss2,
				spi_ss3       => spi_ss3,
				spi_ss4       => spi_ss4,
				conf_data0    => conf_data0,

				-- PS/2 signals
				ps2k_clk_in  => ps2_keyboard_clk_in,
				ps2k_dat_in  => ps2_keyboard_dat_in,
				ps2k_clk_out => ps2_keyboard_clk_out,
				ps2k_dat_out => ps2_keyboard_dat_out,
				ps2m_clk_in  => ps2_mouse_clk_in,
				ps2m_dat_in  => ps2_mouse_dat_in,
				ps2m_clk_out => ps2_mouse_clk_out,
				ps2m_dat_out => ps2_mouse_dat_out,

				-- Buttons
				buttons => (0 => KEY(0), 1 => KEY(1), others => '1'),

				-- Joysticks
				joy1 => joya,
				joy2 => joyb,

				-- UART
				rxd       => rs232_rxd,
				txd       => rs232_txd,
				--
				intercept => intercept
			);

		LED <= (0 => not act_led, others => '1');

	end rtl;
