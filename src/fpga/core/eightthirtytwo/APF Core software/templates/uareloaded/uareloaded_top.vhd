library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

-- -----------------------------------------------------------------------

entity uareloaded_top is
	port
	(
		CLOCK_50	:	 IN STD_LOGIC;
		LED         :   OUT STD_LOGIC;
		DRAM_CLK		:	 OUT STD_LOGIC;
		DRAM_CKE		:	 OUT STD_LOGIC;
		DRAM_ADDR		:	 OUT STD_LOGIC_VECTOR(12 DOWNTO 0);
		DRAM_BA		:	 OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
		DRAM_DQ		:	 INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
		DRAM_LDQM		:	 OUT STD_LOGIC;
		DRAM_UDQM		:	 OUT STD_LOGIC;
		DRAM_CS_N		:	 OUT STD_LOGIC;
		DRAM_WE_N		:	 OUT STD_LOGIC;
		DRAM_CAS_N		:	 OUT STD_LOGIC;
		DRAM_RAS_N		:	 OUT STD_LOGIC;
		VGA_HS		:	 OUT STD_LOGIC;
		VGA_VS		:	 OUT STD_LOGIC;
		VGA_R		:	 OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		VGA_G		:	 OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		VGA_B		:	 OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		VGA_BLANK:   OUT STD_LOGIC;
		VGA_CLOCK:   OUT STD_LOGIC;
		-- AUDIO
		SIGMA_R                     : OUT STD_LOGIC;
		SIGMA_L                     : OUT STD_LOGIC;
		-- PS2
		PS2_KEYBOARD_CLK            :    INOUT STD_LOGIC;
		PS2_KEYBOARD_DAT            :    INOUT STD_LOGIC;
		PS2_MOUSE_CLK               :    INOUT STD_LOGIC;
		PS2_MOUSE_DAT               :    INOUT STD_LOGIC;
		-- UART
		AUDIO_IN                    : IN STD_LOGIC;
		--STM32
        STM_RST                     : out std_logic     := 'Z'; -- '0' to hold the microcontroller reset line, to free the SD card
        -- I2S
		SCLK                        : out std_logic;
		SDIN                        : out std_logic;
		MCLK                        : out std_logic := 'Z';
		LRCLK                       : out std_logic;
		-- Joystick
		JOYSTICK1                   : in std_logic_vector (5 downto 0);
		JOYSTICK2                   : in std_logic_vector (5 downto 0);
		JOY_SELECT                  : out std_logic :='1';
		-- SD Card
		SD_CS                       : out   std_logic := '1';
		SD_SCK                      : out   std_logic := '0';
		SD_MOSI                     : out   std_logic := '0';
		SD_MISO                     : in    std_logic
	
	);
END entity;

architecture RTL of uareloaded_top is
	
-- System clocks
	signal locked : std_logic;
	signal reset_n : std_logic;

-- SPI signals
--	signal sd_clk : std_logic;
--	signal sd_cs : std_logic;
--	signal sd_mosi : std_logic;
--	signal sd_miso : std_logic;
	
-- internal SPI signals
	signal spi_toguest : std_logic;
	signal spi_fromguest : std_logic;
	signal spi_ss2 : std_logic;
	signal spi_ss3 : std_logic;
	signal spi_ss4 : std_logic;
	signal conf_data0 : std_logic;
	signal spi_clk_int : std_logic;

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

-- Video
	signal vga_red: std_logic_vector(7 downto 0);
	signal vga_green: std_logic_vector(7 downto 0);
	signal vga_blue: std_logic_vector(7 downto 0);
	signal vga_hsync : std_logic;
	signal vga_vsync : std_logic;

-- RS232 serial
	signal rs232_rxd : std_logic;
	signal rs232_txd : std_logic;
	
-- IO
	signal joya : std_logic_vector(7 downto 0);
	signal joyb : std_logic_vector(7 downto 0);
	signal joyc : std_logic_vector(7 downto 0);
	signal joyd : std_logic_vector(7 downto 0);
	
-- DAC AUDIO
   signal dac_l: signed(15 downto 0);
   signal dac_r: signed(15 downto 0);
   --signal dac_l_s: signed(15 downto 0);
   --signal dac_r_s: signed(15 downto 0);
	
-- DAC VGA
signal vga_clk_o :  std_logic;
signal vga_blank_o  :  std_logic;

begin


-- SPI

--sd_cs_n_o<=sd_cs;
--sd_mosi_o<=sd_mosi;
--sd_miso<=sd_miso_i;
--sd_sclk_o<=sd_clk;


-- External devices tied to GPIOs
ps2_mouse_dat_in<=ps2_mouse_dat;
ps2_mouse_dat <= '0' when ps2_mouse_dat_out='0' else 'Z';
ps2_mouse_clk_in<=ps2_mouse_clk;
ps2_mouse_clk <= '0' when ps2_mouse_clk_out='0' else 'Z';

ps2_keyboard_dat_in <=ps2_keyboard_dat;
ps2_keyboard_dat <= '0' when ps2_keyboard_dat_out='0' else 'Z';
ps2_keyboard_clk_in<=ps2_keyboard_clk;
ps2_keyboard_clk <= '0' when ps2_keyboard_clk_out='0' else 'Z';
	
JOY_SELECT<= '1';

	
joya<="11" & JOYSTICK1(5) & JOYSTICK1(4) & JOYSTICK1(0) & JOYSTICK1(1) & JOYSTICK1(2) &JOYSTICK1(3);
joyB<="11" & JOYSTICK2(5) & JOYSTICK2(4) & JOYSTICK2(0) & JOYSTICK2(1) & JOYSTICK2(2) &JOYSTICK2(3);


joyc<=(others=>'1');
joyd<=(others=>'1');

STM_RST <= '0';


pll_vga: entity work.pll_vga
port map (
     inclk0  => CLOCK_50,
     c0      => VGA_CLOCK,     
     locked  => open
);


VGA_R  <=vga_red;
VGA_G  <=vga_green;
VGA_B  <=vga_blue;
VGA_HS <=vga_hsync;
VGA_VS <=vga_vsync;
VGA_BLANK<='1';    -- not vga_blank_o
--VGA_CLOCK<=CLOCK_50;


---- I2S out

audio_i2s : entity work.audio_top
port map(
     clk_50MHz => clock_50,
	  dac_MCLK  => MCLK,
	  dac_SCLK  => SCLK,
	  dac_SDIN  => SDIN,
	  dac_LRCK  => LRCLK,
	  L_data    => std_logic_vector (dac_l),
	  R_data    => std_logic_vector (dac_r)
);

--dac_l_s <= (dac_l & dac_l(9 downto 4));
--dac_r_s <= (dac_r & dac_r(9 downto 4));


guest: COMPONENT  guest_mist
  port map (			
--	CLOCK_27 => CLOCK_50,
    CLOCK_27 => CLOCK_50&CLOCK_50,
--	RESET_N => reset_n,
    LED => LED,
	--SPI
	SPI_DO => spi_fromguest,
	SPI_DI => spi_toguest,
	SPI_SCK => spi_clk_int,
	SPI_SS2 => spi_ss2,
	SPI_SS3 => spi_ss3,
--	SPI_SS4 => spi_ss4,
	CONF_DATA0 => conf_data0,
	--UART
--	UART_TX  => open,
--	UART_RX  => AUDIO_IN,
	--SDRAM
	SDRAM_DQ => DRAM_DQ,
	SDRAM_A => DRAM_ADDR,
	SDRAM_DQML => DRAM_LDQM,
	SDRAM_DQMH => DRAM_UDQM,
	SDRAM_nWE => DRAM_WE_N,
	SDRAM_nCAS => DRAM_CAS_N,
	SDRAM_nRAS => DRAM_RAS_N,
	SDRAM_nCS => DRAM_CS_N,
	SDRAM_BA => DRAM_BA,
	SDRAM_CLK => DRAM_CLK,
	SDRAM_CKE => DRAM_CKE,
	--AUDIO
    AUDIO_L => SIGMA_L,
    AUDIO_R => SIGMA_R,
		DAC_L   => dac_l,
		DAC_R   => dac_r,
	--VGA
	VGA_HS => vga_hsync,
	VGA_VS => vga_vsync,
	VGA_R => vga_red(7 downto 2),
	VGA_G => vga_green(7 downto 2),
	VGA_B => vga_blue(7 downto 2),
	  VGA_BLANK => vga_blank_o,
	  VGA_CLK => vga_clk_o
  );




-- Pass internal signals to external SPI interface
sd_sck <= spi_clk_int;

controller : entity work.substitute_mcu
	generic map (
		sysclk_frequency => 500,
		--		SPI_FASTBIT=>3,
		--		SPI_INTERNALBIT=>2,		--needed if OSD hungs
		debug => false,
		jtag_uart => false
	)
	port map (
		clk => CLOCK_50,
		reset_in =>  '1',		--reset_in when 0
		reset_out => reset_n,	--reset_out when 0

		-- SPI signals
		spi_miso => sd_miso,
		spi_mosi	=> sd_mosi,
		spi_clk => spi_clk_int,
		spi_cs => sd_cs,
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

		-- Buttons
		buttons => (others=>'1'),
		
        -- joy
		joy1 => joya,
		joy2 => joyb,
		
		-- UART
		rxd => rs232_rxd,
		txd => rs232_txd,
		intercept => intercept
);

end rtl;

