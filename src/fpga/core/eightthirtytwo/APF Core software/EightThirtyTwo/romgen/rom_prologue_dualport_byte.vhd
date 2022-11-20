
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


entity eightthirtytwo_rom is
generic
	(
		ADDR_WIDTH : integer := 15 -- Specify your actual ROM size to save LEs and unnecessary block RAM usage.
	);
port (
	clk : in std_logic;
	reset_n : in std_logic := '1';
	addr : in std_logic_vector(ADDR_WIDTH-1 downto 0);
	q : out std_logic_vector(31 downto 0);
	-- Allow writes - defaults supplied to simplify projects that don't need to write.
	d : in std_logic_vector(31 downto 0) := X"00000000";
	we : in std_logic := '0';
	bytesel : in std_logic_vector(3 downto 0) := "1111";
	addr2 : in std_logic_vector(ADDR_WIDTH-1 downto 0);
	q2 : out std_logic_vector(31 downto 0);
	-- Allow writes - defaults supplied to simplify projects that don't need to write.
	d2 : in std_logic_vector(31 downto 0) := X"00000000";
	we2 : in std_logic := '0';
	bytesel2 : in std_logic_vector(3 downto 0) := "1111"
);
end entity;

architecture rtl of eightthirtytwo_rom is

	signal addr_int : integer range 0 to 2**ADDR_WIDTH-1;
	signal addr2_int : integer range 0 to 2**ADDR_WIDTH-1;

	--  build up 2D array to hold the memory
	type word_t is array (0 to 3) of std_logic_vector(7 downto 0);
	type ram_t is array (0 to 2 ** ADDR_WIDTH - 1) of word_t;

	signal ram : ram_t:=
	(

