library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity i2s_dac is
generic (
	sysclk_frequency : integer := 100;
	mclk_to_lrclk : integer := 256; -- Must be an integer multiple of width
	samplerate : integer := 44100;
	width : integer := 16
);
port (
	sysclk : in std_logic;
	reset_n : in std_logic;
	--
	left_in : std_logic_vector(width-1 downto 0);
	right_in : std_logic_vector(width-1 downto 0);
	--
	mclk : out std_logic;
	lrclk : out std_logic;
	sclk : out std_logic;
	sdata : out std_logic
);
end entity;

architecture rtl of i2s_dac is
	signal mclk_tick : std_logic := '0';
	signal sclk_tick : std_logic := '0';
	signal lrclk_out : std_logic := '0';
	signal sdata_out : std_logic := '0';
begin

	mclock : block
		constant mclk_ticks : integer := (sysclk_frequency*500000)/(samplerate*mclk_to_lrclk)-1;
		signal mclk_count : unsigned(7 downto 0) := to_unsigned(mclk_ticks,8);	
		signal mclk_out : std_logic := '0';
	begin
		mclk <= mclk_out;

		process(sysclk, reset_n) begin
			if reset_n='0' then
				mclk_count<=to_unsigned(mclk_ticks,mclk_count'high+1);
			elsif rising_edge(sysclk) then
				mclk_tick<='0';
				if mclk_count=0 then
					mclk_tick<= '1';	-- Momentary tick for the benefit of the rest of the logic;
					mclk_out <= not mclk_out;
					mclk_count<=to_unsigned(mclk_ticks,mclk_count'high+1);
				else
					mclk_count<=mclk_count-1;
				end if;
			end if;
		end process;

	end block;
	
	sclock : block
		constant sclk_ticks : integer := (mclk_to_lrclk / width) - 1;
		signal sclk_count : unsigned(7 downto 0) := to_unsigned(sclk_ticks,8);
		signal sclk_out : std_logic := '0';
	begin
		sclk <= sclk_out;

		process(sysclk,reset_n) begin
			if reset_n='0' then
				sclk_count<=to_unsigned(sclk_ticks,sclk_count'high+1);
			elsif rising_edge(sysclk) then
				sclk_tick<='0';
				if mclk_tick='1' then
					if sclk_count = 0 then
						sclk_tick <= sclk_out;	-- Momentary pulse on the falling edge of sclk
						sclk_out <= not sclk_out;
						sclk_count <= to_unsigned(sclk_ticks,sclk_count'high+1);
					else
						sclk_count <= sclk_count-1;
					end if;
				end if;
			end if;
		end process;	
	end block;

	lrclock : block
		signal lrclk_count : unsigned(4 downto 0) := to_unsigned(width-1,5);
		signal shift : std_logic_vector(width-1 downto 0);
	begin
		lrclk <= lrclk_out;
		sdata <= sdata_out;

		process(sysclk,reset_n) begin
			if reset_n='0' then
				lrclk_count<=to_unsigned(width-1,lrclk_count'high+1);
				lrclk_out <= '0';
			elsif rising_edge(sysclk) then

				if sclk_tick='1' then
					sdata_out<=shift(shift'high);
					shift<=shift(shift'high-1 downto 0)&'0';
					if lrclk_count=0 then
						lrclk_count<=to_unsigned(width-1,lrclk_count'high+1);
						lrclk_out <= not lrclk_out;
						if lrclk_out='1' then
							shift <= left_in;
						else
							shift <= right_in;
						end if;
					else
						lrclk_count<=lrclk_count-1;
					end if;
				end if;
			end if;
		end process;

	end block;
	
end architecture;

