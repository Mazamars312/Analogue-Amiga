library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

package icesugarpro_pmod_pkg is
	-- Pin numberings for SD card PMOD
	constant PMOD_SD_CS : integer := 0;
	constant PMOD_SD_MOSI : integer := 1;
	constant PMOD_SD_MISO : integer := 2;
	constant PMOD_SD_CLK : integer := 3;
	constant PMOD_SD_DAT1 : integer := 4;
	constant PMOD_SD_DAT2 : integer := 5;
	constant PMOD_SD_CD : integer := 6;
	constant PMOD_SD_WP : integer := 7;
	
	-- Pin numberings for PS/2 PMOD
	constant PMOD_PS2_KDAT : integer := 0; 
	constant PMOD_PS2_MDAT : integer := 1;
	constant PMOD_PS2_KCLK : integer := 2;
	constant PMOD_PS2_MCLK : integer := 3;
	
	-- Pin numberings for I2S PMOD
	constant PMOD_I2S_DA_MCLK : integer := 0;
	constant PMOD_I2S_DA_LRCK : integer := 1;
	constant PMOD_I2S_DA_SCLK : integer := 2;
	constant PMOD_I2S_DA_SDIN : integer := 3;
	constant PMOD_I2S_AD_MCLK : integer := 4;
	constant PMOD_I2S_AD_LRCK : integer := 5;
	constant PMOD_I2S_AD_SCLK : integer := 6;
	constant PMOD_I2S_AD_SDOUT : integer := 7;
	
end package;

