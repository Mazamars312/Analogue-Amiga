-- -----------------------------------------------------------------------
--
-- Chameleon_reconfig.vhd
-- super-simplified version of chameleon_usb which only does reconfiguration.
-- by AMR.
--
-- This source file is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as published
-- by the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This source file is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program. If not, see <http://www.gnu.org/licenses/>.
--
-- -----------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

-- -----------------------------------------------------------------------

entity chameleon_reconfig is
	port (
		clk : in std_logic;
		reconfig : in std_logic := '0';
		reconfig_slot : in unsigned(3 downto 0) := (others => '0');
		flashslot : out unsigned(4 downto 0);

		serial_clk : in std_logic;
		serial_rxd : in std_logic := '1';
		serial_txd : out std_logic;
		serial_cts_n : in std_logic := '0'
	);
end entity;

-- -----------------------------------------------------------------------

architecture rtl of chameleon_reconfig is
	type state_t is (
		STATE_RESET, STATE_IDLE);

	signal state : state_t := STATE_RESET;

	signal recv_trigger : std_logic;
	signal recv_data : unsigned(8 downto 0) := (others => '0');

	signal send_trigger : std_logic := '0';
	signal send_empty : std_logic;
	signal send_data : unsigned(8 downto 0) := (others => '0');
	
	signal flashslot_reg : unsigned(4 downto 0) := (others => '0');
begin

	flashslot <= flashslot_reg;

	myUsart : entity work.gen_usart
		generic map (
			bits => 9
		)
		port map (
			clk => clk,

			d => send_data,
			d_trigger => send_trigger,
			d_empty => send_empty,
			q => recv_data,
			q_trigger => recv_trigger,

			serial_clk => serial_clk,
			serial_rxd => serial_rxd,
			serial_txd => serial_txd,
			serial_cts_n => serial_cts_n
		);

	process(clk)
	begin
		if rising_edge(clk) then
			if reconfig='1' then
				send_data <= "11111" & reconfig_slot;
			else
				send_data <= "100101010"; -- 42, 0x12A
			end if;

			send_trigger <= '0';
			if (send_empty = '1') and (reconfig = '1' or flashslot_reg(4)='0') then
				
				send_trigger <= '1';
			end if;

			if recv_trigger = '1' and recv_data(8)='1' and recv_data(4)='1' then
				flashslot_reg <= recv_data(4 downto 0);
			end if;
		end if;
	end process;

end architecture;



