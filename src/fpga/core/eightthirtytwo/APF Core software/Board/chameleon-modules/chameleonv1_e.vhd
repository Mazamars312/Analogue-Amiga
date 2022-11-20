-- -----------------------------------------------------------------------
--
-- Turbo Chameleon
--
-- Toplevel file for Turbo Chameleon 64
--
-- Some signal names changed by AMR to match V2,
-- making it easier to maintain multi-platform cores.

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

