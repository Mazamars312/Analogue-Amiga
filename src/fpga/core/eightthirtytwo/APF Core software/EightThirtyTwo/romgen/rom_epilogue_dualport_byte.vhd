		others => (others => x"00")
	);
	signal q1_local : word_t;
	signal q2_local : word_t;

	-- Altera Quartus attributes
	attribute ramstyle: string;
	attribute ramstyle of ram: signal is "no_rw_check";
	
	-- Yosys attribute
	attribute no_rw_check: boolean;
	attribute no_rw_check of ram: signal is true;

begin  -- rtl

	addr_int <= to_integer(unsigned(addr(ADDR_WIDTH-1 downto 0)));
	addr2_int <= to_integer(unsigned(addr2(ADDR_WIDTH-1 downto 0)));

	-- Reorganize the read data from the RAM to match the output
	q(7 downto 0) <= q1_local(3);
	q(15 downto 8) <= q1_local(2);
	q(23 downto 16) <= q1_local(1);
	q(31 downto 24) <= q1_local(0);

	q2(7 downto 0) <= q2_local(3);
	q2(15 downto 8) <= q2_local(2);
	q2(23 downto 16) <= q2_local(1);
	q2(31 downto 24) <= q2_local(0);

	process(clk)
	begin
		if(rising_edge(clk)) then 
			if(we = '1') then
				-- edit this code if using other than four bytes per word
				if (bytesel(3) = '1') then
					ram(addr_int)(3) <= d(7 downto 0);
				end if;
				if (bytesel(2) = '1') then
					ram(addr_int)(2) <= d(15 downto 8);
				end if;
				if (bytesel(1) = '1') then
					ram(addr_int)(1) <= d(23 downto 16);
				end if;
				if (bytesel(0) = '1') then
					ram(addr_int)(0) <= d(31 downto 24);
				end if;
			end if;
			q1_local <= ram(addr_int);
		end if;
	end process;

	process(clk)
	begin
		if(rising_edge(clk)) then 
			if(we2 = '1') then
				-- edit this code if using other than four bytes per word
				if (bytesel2(3) = '1') then
					ram(addr2_int)(3) <= d2(7 downto 0);
				end if;
				if (bytesel(2) = '1') then
					ram(addr2_int)(2) <= d2(15 downto 8);
				end if;
				if (bytesel(1) = '1') then
					ram(addr2_int)(1) <= d2(23 downto 16);
				end if;
				if (bytesel(0) = '1') then
					ram(addr2_int)(0) <= d2(31 downto 24);
				end if;
			end if;
			q2_local <= ram(addr2_int);
		end if;
	end process;
  
end rtl;

