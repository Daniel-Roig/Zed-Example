------------------------------------------------------------------------
--	video_merge.vhd  -- Merge Logo with Background
------------------------------------------------------------------------
-- Author: He Cong
--	Copyright 2011 Digilent, Inc.
------------------------------------------------------------------------
-- Module description
--   The module merges Logo data with background
--
--  Inputs:
--		PIX_CLK_I - 25MHz Pixel Clock Input
--		PIX_CNT_I - Counter of pixels in a line
--		LINE_CNT_I - Counter of lines in a frame
--		LOGO_DATA_I - Input of Logo Data (R:G:B 3:3:2)
--
--  Outputs:
--		LOGO_ADDR_O - Address for Logo RAM
--		VIDEO_DATA_O - Video Data Output (R:G:B 3:3:2)
--
------------------------------------------------------------------------
-- Revision History:
--
--	13Apr2011(CongH): created
--	13Apr2011(SteveW): coding sytle
--
------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity video_merge is
	Port( 
		PIX_CLK_I		: in   std_logic;
		PIX_CNT_I		: in   std_logic_vector (9 downto 0);
		LINE_CNT_I		: in   std_logic_vector (9 downto 0);
		LOGO_DATA_I		: in   std_logic_vector (7 downto 0);
		LOGO_ADDR_O		: out  std_logic_vector (13 downto 0);
		RED_O				: out std_logic_vector(3 downto 0);
		GREEN_O			: out std_logic_vector(3 downto 0);
		BLUE_O			: out std_logic_vector(3 downto 0)
	);
end video_merge;


architecture Behavioral of video_merge is

   --Box sinals
	signal baseaddr	  : STD_LOGIC_VECTOR (14 downto 0);
	
	signal deltax       : STD_LOGIC;
	signal deltay       : STD_LOGIC;

	signal rgb_logo     : STD_LOGIC_VECTOR (7 downto 0) := "00000000"; -- logo
	signal rgb_bg       : STD_LOGIC_VECTOR (7 downto 0) := "00000000"; -- background
	signal rgb          : STD_LOGIC_VECTOR (7 downto 0) := "00000000"; -- to rgbOut
	signal address      : STD_LOGIC_VECTOR (13 downto 0):= "00000000000000"; -- addr
	signal pixel_x      : STD_LOGIC_VECTOR (9 DOWNTO 0) := "0000000000";  -- cordinate x
	signal pixel_y      : STD_LOGIC_VECTOR (9 DOWNTO 0) := "0000000000"; -- cordinate y
	
	--display constants
	constant displeftX   : STD_LOGIC_VECTOR (9 DOWNTO 0) := "0010010000";   -- 144 px 
	constant disprightX  : STD_LOGIC_VECTOR (9 DOWNTO 0) := "1100010000"; -- 784 px wide
	constant dispupY     : STD_LOGIC_VECTOR (9 DOWNTO 0) := "0000100011";     -- 35
	constant dispdownY   : STD_LOGIC_VECTOR (9 DOWNTO 0) := "1000000011"; -- 515
	constant displogoLX  : STD_LOGIC_VECTOR (9 DOWNTO 0) := "0101101100";  -- 364
	constant displogoRX  : STD_LOGIC_VECTOR (9 DOWNTO 0) := "1000110100"; -- 564
   constant displogoUY  : STD_LOGIC_VECTOR (9 DOWNTO 0) := "0011111111";   -- 255
   constant displogoDY  : STD_LOGIC_VECTOR (9 DOWNTO 0) := "0100101100";    -- 300
	
	-- cordinate
	constant line_num    : STD_LOGIC_VECTOR (6 DOWNTO 0) := "1011010";
	constant pixel_num   : STD_LOGIC_VECTOR (8 DOWNTO 0) := "110010000";
	
begin
	BASE_ADDR: process(PIX_CLK_I)
	begin
		if PIX_CLK_I'Event and PIX_CLK_I = '1' then
			if PIX_CNT_I = "0000000001" and LINE_CNT_I = "0000000000" then
				if (baseaddr >= "110001111") then
					baseaddr <= (others => '0');
				else
					baseaddr <= baseaddr + 1;
				end if;
			end if;
		end if;
	end process BASE_ADDR;
	
	RGB_PROC:process(PIX_CLK_I)
	begin
		--Draw the box within its bounds
		if PIX_CLK_I'Event and PIX_CLK_I = '1' then 
			if (PIX_CNT_I >= displeftX) and (PIX_CNT_I < disprightX)
				and (LINE_CNT_I >= dispupY) and (LINE_CNT_I < dispdownY) then
				if (PIX_CNT_I >= pixel_x) and (PIX_CNT_I < (pixel_x+displogoRX-displogoLX))
					and (LINE_CNT_I >= pixel_y) and (LINE_CNT_I < pixel_y+displogoDY-displogoUY) then
					rgb <= LOGO_DATA_I;
				else rgb <= PIX_CNT_I(7 downto 0);
				end if;
			else
				rgb <= (others => '0');
			end if;
		end if;
	end process RGB_PROC;
		
	addr_PROC : process(PIX_CLK_I)
	begin
		--Draw the box within its bounds
		if PIX_CLK_I'Event and PIX_CLK_I = '1' then 			
			if (PIX_CNT_I >= pixel_x) and (PIX_CNT_I < (pixel_x+displogoRX-displogoLX))
					and (LINE_CNT_I >= pixel_y) and (LINE_CNT_I < pixel_y+displogoDY-displogoUY) then
				address <= address + 1;
			elsif (PIX_CNT_I = "0000000001") and (LINE_CNT_I="0000000000") then
				address <= baseaddr(14 downto 1);
			end if;		 
		end if;
	end process addr_PROC;
	
	delt_x: process(PIX_CLK_I)
	begin
		if PIX_CLK_I'Event and PIX_CLK_I = '1' then
			if PIX_CNT_I = "0000000001" and LINE_CNT_I = "0000000000" then
				if (pixel_x > disprightX-displogoRX+displogoLX) or (pixel_x <displeftX) then
					pixel_x <= displeftX;
					deltax <= '1';
				elsif (pixel_x = (disprightX-displogoRX+displogoLX)-1)  then 
					deltax <= '0';
				elsif (pixel_x = displeftX+1) then 
					deltax <= '1';
				end if;
				
				if (deltax = '1') then
						pixel_x <= pixel_x + 1;	
				else
						pixel_x <= pixel_x - 1;
				end if;
			end if;
		end if;
	end process delt_x;
	
	delt_y: process(PIX_CLK_I)
	begin
		if PIX_CLK_I'Event and PIX_CLK_I = '1' then
			if PIX_CNT_I = "0000000001" and LINE_CNT_I = "0000000000" then
				if (pixel_y > dispdownY-displogoDY+displogoUY) or (pixel_y <dispupY) then
					pixel_y <= dispupY;
					deltay <= '1';
				elsif (pixel_y = (dispdownY-displogoDY+displogoUY)-1)  then 
					deltay <= '0';
				elsif (pixel_y = dispupY+1) then 
					deltay <= '1';
				end if;
				
				if (deltay = '1') then
						pixel_y <= pixel_y + 1;	
				else
						pixel_y <= pixel_y - 1;
				end if;
			end if;
		end if;
	end process delt_y;
	
	LOGO_ADDR_O  <= address;
	RED_O <= rgb(7 downto 5) & "0";
	GREEN_O <= rgb(4 downto 2) & "0";
	BLUE_O <= rgb(1 downto 0) & "00";
	
end Behavioral;

