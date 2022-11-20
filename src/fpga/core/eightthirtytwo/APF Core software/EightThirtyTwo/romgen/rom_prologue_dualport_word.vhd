library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity eightthirtytwo_rom is
generic
	(
		ADDR_WIDTH : integer := 15; -- Specify your actual ROM size to save LEs and unnecessary block RAM usage.
        COL_WIDTH  : integer := 8;  -- Column width (8bit -> byte)
        NB_COL     : integer := 4  -- Number of columns in memory
	);
port (
	clk : in std_logic;
	reset_n : in std_logic := '1';
	addr : in std_logic_vector(ADDR_WIDTH-2 downto 0);
	q : out std_logic_vector(31 downto 0);
	-- Allow writes - defaults supplied to simplify projects that don't need to write.
	d : in std_logic_vector(31 downto 0) := X"00000000";
	we : in std_logic := '0';
	bytesel : in std_logic_vector(3 downto 0) := "1111";
	-- Second port
	addr2 : in std_logic_vector(ADDR_WIDTH-2 downto 0) := (others=>'0');
	q2 : out std_logic_vector(31 downto 0);
	d2 : in std_logic_vector(31 downto 0) := X"00000000";
	we2 : in std_logic := '0';
	bytesel2 : in std_logic_vector(3 downto 0) := "1111"	
);
end entity;

architecture arch of eightthirtytwo_rom is

-- type word_t is std_logic_vector(31 downto 0);
type ram_type is array (0 to 2 ** (ADDR_WIDTH-1) - 1) of std_logic_vector(NB_COL * COL_WIDTH - 1 downto 0);

   -- type rom_type is array (0 to 2**ADDR_WIDTH-1)
   --     of std_logic_vector(DATA_WIDTH-1 downto 0);

signal ram : ram_type :=
(

