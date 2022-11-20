library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library std;
use std.textio.all;

entity cpu_tb is
end cpu_tb;

architecture behaviour of cpu_tb
is
	constant clk_period : time := 10 ns;
	signal clk : std_logic;

	signal reset_n : std_logic;

	signal uart_read : std_logic_vector(31 downto 0);
	signal uart_count : unsigned(1 downto 0) :="00" ;

	signal ram_addr : std_logic_vector(31 downto 2);
	signal from_ram : std_logic_vector(31 downto 0);
	signal from_rom : std_logic_vector(31 downto 0);
	signal to_ram : std_logic_vector(31 downto 0);
	signal ram_wr : std_logic;
	signal ram_bytesel : std_logic_vector(3 downto 0);
	signal ram_req : std_logic;
	signal ram_ack : std_logic;

	signal rom_wr :std_logic;
	signal ramwait : unsigned(3 downto 0) := "0000";

	signal interrupt : std_logic;
	signal intcounter : unsigned(5 downto 0) := "000000";

	type tbstates is (RESET,INIT,MAIN);
	signal tbstate : tbstates:=RESET;

begin

	rom : entity work._rom
	generic map
	(
		ADDR_WIDTH=> 14
	)
	port map(
		clk => clk,
		addr => ram_addr(15 downto 2),
		d => to_ram,
		q => from_rom,
		we => rom_wr,
		bytesel => ram_bytesel
	);


	uart_read<= X"00000000" when uart_count/="0000" else X"00000300";
	from_ram<=from_rom when ram_addr(31)='0' else uart_read;

	rom_wr<=(ram_wr and ram_req) when ram_addr(31)='0' else '0';

	cpu : entity work.eightthirtytwo_cpu
	generic map
	(
		littleendian => false,
		dualthread => false
	)
	port map
	(
		clk => clk,
		reset_n => reset_n,

		interrupt => '0', -- interrupt,

		-- cpu fetch interface

		addr => ram_addr,
		d => from_ram,
		q => to_ram,
		bytesel => ram_bytesel,
		wr => ram_wr,
		req => ram_req,
		ack => ram_ack
	);

interrupt<='1' when intcounter(5 downto 3)="111" else '0';

  -- Clock process definition
  clk_process: process
  begin
    clk <= '0';
    wait for clk_period/2;
    clk <= '1';
    wait for clk_period/2;
  end process;

	process(clk)
		variable textline : line;
	begin

		if rising_edge(clk) then

			intcounter<=intcounter+1;

			reset_n<='1';
			ram_ack<='0';


			if ram_req='1' and ramwait="0000" then
				if ram_addr(31)='1' then
					if ram_wr='1' then
--					report "out: " & integer'image(to_integer(unsigned(to_ram(15 downto 0))));
						write(textline,character'val(to_integer(unsigned(to_ram(7 downto 0)))));
						if to_ram(7 downto 0)=X"0a" then
							writeline(output,textline);
						end if;
						uart_count<="10";
					elsif uart_count/="00" then
						uart_count<=uart_count-1;
					end if;
				end if;
--				ram_ack<='1';
				ramwait<="0000";
			end if;

			if ramwait="0000" then
				ram_ack<=ram_req;
			else
				ramwait<=ramwait-1;
			end if;

			case tbstate is
				when RESET =>
					reset_n<='0';
					tbstate<=INIT;
				when INIT =>
					tbstate<=MAIN;
				when MAIN =>
					tbstate<=MAIN;

			end case;

		end if;

	end process;

end behaviour;

