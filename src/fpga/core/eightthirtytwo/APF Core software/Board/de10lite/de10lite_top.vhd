library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

-- -----------------------------------------------------------------------

entity de10lite_top is
	port
	(
		ADC_CLK_10		:	 IN STD_LOGIC;
		MAX10_CLK1_50		:	 IN STD_LOGIC;
		MAX10_CLK2_50		:	 IN STD_LOGIC;
		KEY		:	 IN STD_LOGIC_VECTOR(1 DOWNTO 0);
		SW		:	 IN STD_LOGIC_VECTOR(9 DOWNTO 0);
		LEDR		:	 OUT STD_LOGIC_VECTOR(9 DOWNTO 0);
		HEX0		:	 OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		HEX1		:	 OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		HEX2		:	 OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		HEX3		:	 OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		HEX4		:	 OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		HEX5		:	 OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
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
		VGA_R		:	 OUT UNSIGNED(3 DOWNTO 0);
		VGA_G		:	 OUT UNSIGNED(3 DOWNTO 0);
		VGA_B		:	 OUT UNSIGNED(3 DOWNTO 0);
		CLK_I2C_SCL		:	 OUT STD_LOGIC;
		CLK_I2C_SDA		:	 INOUT STD_LOGIC;
		GSENSOR_SCLK		:	 OUT STD_LOGIC;
		GSENSOR_SDO		:	 INOUT STD_LOGIC;
		GSENSOR_SDI		:	 INOUT STD_LOGIC;
		GSENSOR_INT		:	 IN STD_LOGIC_VECTOR(2 DOWNTO 1);
		GSENSOR_CS_N		:	 OUT STD_LOGIC;
		GPIO		:	 INOUT STD_LOGIC_VECTOR(35 DOWNTO 0);
		ARDUINO_IO		:	 INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
		ARDUINO_RESET_N		:	 INOUT STD_LOGIC
	);
END entity;

architecture RTL of de10lite_top is
   constant reset_cycles : integer := 131071;
	
-- System clocks

	signal slowclk : std_logic;
	signal fastclk : std_logic;
	signal pll_locked : std_logic;

-- SPI signals

	signal spi_clk : std_logic;
	signal spi_cs : std_logic;
	signal spi_mosi : std_logic;
	signal spi_miso : std_logic;
	
-- Global signals
	signal n_reset : std_logic;

-- PS/2 Keyboard socket - used for second mouse
	alias ps2_keyboard_clk : std_logic is GPIO(10);
	alias ps2_keyboard_dat : std_logic is GPIO(12);

	signal ps2_keyboard_clk_in : std_logic;
	signal ps2_keyboard_dat_in : std_logic;
	signal ps2_keyboard_clk_out : std_logic;
	signal ps2_keyboard_dat_out : std_logic;

-- PS/2 Mouse
	alias ps2_mouse_clk : std_logic is GPIO(14);
	alias ps2_mouse_dat : std_logic is GPIO(16);

	signal ps2_mouse_clk_in: std_logic;
	signal ps2_mouse_dat_in: std_logic;
	signal ps2_mouse_clk_out: std_logic;
	signal ps2_mouse_dat_out: std_logic;

	
-- Video
	signal vga_red: std_logic_vector(7 downto 0);
	signal vga_green: std_logic_vector(7 downto 0);
	signal vga_blue: std_logic_vector(7 downto 0);
	signal vga_window : std_logic;
	signal vga_hsync : std_logic;
	signal vga_vsync : std_logic;
	
	
-- RS232 serial
	signal rs232_rxd : std_logic;
	signal rs232_txd : std_logic;

-- ESP8266 serial
	signal esp_rxd : std_logic;
	signal esp_txd : std_logic;

-- Sound
	signal audio_l : std_logic_vector(15 downto 0);
	signal audio_r : std_logic_vector(15 downto 0);

	alias sigma_l : std_logic is GPIO(18);
	alias sigma_r : std_logic is GPIO(20);

	
-- IO


-- Sigma Delta audio
	COMPONENT hybrid_pwm_sd
	generic ( depop : integer := 1 );
	PORT
	(
		clk	:	IN STD_LOGIC;
		terminate : in std_logic;
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

-- SPI

ARDUINO_IO(10)<=spi_cs;
ARDUINO_IO(11)<=spi_mosi;
ARDUINO_IO(12)<='Z';
spi_miso<=ARDUINO_IO(12);
ARDUINO_IO(13)<=spi_clk;


U00 : entity work.pll
	port map(
		inclk0 => MAX10_CLK1_50,       -- 50 MHz external
		c0     => DRAM_CLK,        -- Fast clock - external
		c1     => fastclk,         -- Fast clock - internal
		c2     => slowclk,         -- Slow clock - internal
		locked => pll_locked
	);

vga_window<='1';

n_reset<=KEY(0);


-- External devices tied to GPIOs

ps2_mouse_dat_in<=ps2_mouse_dat;
ps2_mouse_dat <= '0' when ps2_mouse_dat_out='0' else 'Z';
ps2_mouse_clk_in<=ps2_mouse_clk;
ps2_mouse_clk <= '0' when ps2_mouse_clk_out='0' else 'Z';

ps2_keyboard_dat_in<=ps2_keyboard_dat;
ps2_keyboard_dat <= '0' when ps2_keyboard_dat_out='0' else 'Z';
ps2_keyboard_clk_in<=ps2_keyboard_clk;
ps2_keyboard_clk <= '0' when ps2_keyboard_clk_out='0' else 'Z';



virtualtoplevel : entity work.VirtualToplevel
	generic map(
		sdram_rows => 13,
		sdram_cols => 10,
		sysclk_frequency => 1000 -- Sysclk frequency * 10
	)
	port map(
		clk => fastclk,
		slowclk => slowclk,
		reset_in => n_reset,

		-- VGA
		unsigned(vga_red) => vga_red,
		unsigned(vga_green) => vga_green,
		unsigned(vga_blue) => vga_blue,
		vga_hsync => vga_hsync,
		vga_vsync => vga_vsync,
		vga_window => open,

		-- SDRAM
		sdr_data => DRAM_DQ,
		sdr_addr	=> DRAM_ADDR,
		sdr_dqm(1) => DRAM_UDQM,
		sdr_dqm(0) => DRAM_LDQM,
		sdr_we => DRAM_WE_N,
		sdr_cas => DRAM_CAS_N,
		sdr_ras => DRAM_RAS_N,
		sdr_ba => DRAM_BA,
		sdr_cs => DRAM_CS_N,
		sdr_cke => DRAM_CKE,

		
    -- PS/2 keyboard ports
	 ps2k_clk_out => ps2_keyboard_clk_out,
	 ps2k_dat_out => ps2_keyboard_dat_out,
	 ps2k_clk_in => ps2_keyboard_clk_in,
	 ps2k_dat_in => ps2_keyboard_dat_in,

	 ps2m_clk_out => ps2_mouse_clk_out,
	 ps2m_dat_out => ps2_mouse_dat_out,
	 ps2m_clk_in => ps2_mouse_clk_in,
	 ps2m_dat_in => ps2_mouse_dat_in,
 
    -- SD/MMC slot ports
	spi_clk => spi_clk,
	spi_mosi => spi_mosi,
	spi_cs => spi_cs,
	spi_miso => spi_miso,

	signed(audio_l) => audio_l,
	signed(audio_r) => audio_r,
	 
	rxd => rs232_rxd,
	txd => rs232_txd,
	rxd2 => esp_rxd,
	txd2 => esp_txd
);

	
-- Dither the video down to 5 bits per gun.
	vga_window<='1';
	VGA_HS<= not vga_hsync;
	VGA_VS<= not vga_vsync;	

	mydither : component video_vga_dither
		generic map(
			outbits => 4
		)
		port map(
			clk=>fastclk,
			hsync=>vga_hsync,
			vsync=>vga_vsync,
			vid_ena=>vga_window,
			iRed => unsigned(vga_red),
			iGreen => unsigned(vga_green),
			iBlue => unsigned(vga_blue),
			oRed => VGA_R,
			oGreen => VGA_G,
			oBlue => VGA_B
		);
	
audio_sd: component hybrid_pwm_sd
	port map
	(
		clk => fastclk,
		terminate => '0',
		d_l(15) => not audio_l(15),
		d_l(14 downto 0) => std_logic_vector(audio_l(14 downto 0)),
		q_l => sigma_l,
		d_r(15) => not audio_r(15),
		d_r(14 downto 0) => std_logic_vector(audio_r(14 downto 0)),
		q_r => sigma_r
	);
	

GPIO(0)<=rs232_txd;
GPIO(1) <= 'Z';
rs232_rxd<=GPIO(1);
ARDUINO_IO(1) <= esp_txd;
ARDUINO_IO(0) <= 'Z';
esp_rxd <= ARDUINO_IO(0);

end architecture;
