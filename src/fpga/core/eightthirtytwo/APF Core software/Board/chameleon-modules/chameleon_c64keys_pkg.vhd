library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

package chameleon_c64keys_pkg is

	constant c64key_del : integer := 0;
	constant c64key_3 : integer := 1;
	constant c64key_5 : integer := 2;
	constant c64key_7 : integer := 3;
	constant c64key_9 : integer := 4;
	constant c64key_plus : integer := 5;
	constant c64key_pound : integer := 6;
	constant c64key_1 : integer := 7;
	
	constant c64key_return : integer := 8;
	constant c64key_w : integer := 9;
	constant c64key_r : integer := 10;
	constant c64key_y : integer := 11;
	constant c64key_i : integer := 12;
	constant c64key_p : integer := 13;
	constant c64key_asterisk : integer := 14;
	constant c64key_leftarrow : integer := 15;

	constant c64key_cursorleftright : integer := 16;
	constant c64key_a : integer := 17;
	constant c64key_d : integer := 18;
	constant c64key_g : integer := 19;
	constant c64key_j : integer := 20;
	constant c64key_l : integer := 21;
	constant c64key_semicolon : integer := 22;
	constant c64key_control : integer := 23;

	constant c64key_f7 : integer := 24;
	constant c64key_4 : integer := 25;
	constant c64key_6 : integer := 26;
	constant c64key_8 : integer := 27;
	constant c64key_0 : integer := 28;
	constant c64key_minus : integer := 29;
	constant c64key_home : integer := 30;
	constant c64key_2 : integer := 31;
	
	constant c64key_f1 : integer := 32;
	constant c64key_z : integer := 33;
	constant c64key_c : integer := 34;
	constant c64key_b : integer := 35;
	constant c64key_m : integer := 36;
	constant c64key_period : integer := 37;
	constant c64key_rightshift : integer := 38;
	constant c64key_space : integer := 39;

	constant c64key_f3 : integer := 40;
	constant c64key_s : integer := 41;
	constant c64key_f : integer := 42;
	constant c64key_h : integer := 43;
	constant c64key_k : integer := 44;
	constant c64key_colon : integer := 45;
	constant c64key_equals : integer := 46;
	constant c64key_commodore : integer := 47;
	
	constant c64key_f5 : integer := 48;
	constant c64key_e : integer := 49;
	constant c64key_t : integer := 50;
	constant c64key_u : integer := 51;
	constant c64key_o : integer := 52;
	constant c64key_at : integer := 53;
	constant c64key_uparrow : integer := 54;
	constant c64key_q : integer := 55;
	
	constant c64key_cursorupdown : integer := 56;
	constant c64key_leftshift : integer := 57;
	constant c64key_x : integer := 58;
	constant c64key_v : integer := 59;
	constant c64key_n : integer := 60;
	constant c64key_comma : integer := 61;
	constant c64key_slash : integer := 62;
	constant c64key_stop : integer := 63;

end package;

