------------------------------------------------------------------------
--	vga_flyinglogo/vgaflyinglogo.vhd
------------------------------------------------------------------------
-- Author: He Cong
--	Copyright 2011 Digilent, Inc.
------------------------------------------------------------------------
-- Module description
--   The module displays a moving Digilent Logo on VGA Port
--
--  Inputs:
--		PIX_CLK_I - 25MHz Pixel Clock Input
--
--  Outputs:
--	   RED_O - 4-bit red channel
--	   GREEN_O - 4-bit green channel
--	   BLUE_O - 4-bit blue channel
--	   HSYNC_O - horizontal synchronize signal
--	   VSYNC_O - vertical synchronize signal
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

entity vga_flyinglogo is
   Port(
		PIX_CLK_I 	: in  std_logic;
      VSYNC_O		: out std_logic;
		HSYNC_O		: out std_logic;
		RED_O			: out std_logic_vector(3 downto 0);
		GREEN_O		: out std_logic_vector(3 downto 0);
		BLUE_O		: out std_logic_vector(3 downto 0)
	);
end vga_flyinglogo;

architecture Behavioral of vga_flyinglogo is

	------------------------------------------------------------------------
	-- Component Declarations
	------------------------------------------------------------------------
	
	-- Sychronize Signals Generator
	component sync_gen
      Port( 
			PIX_CLK_I		: in  std_logic;
			HSYNC_O			: out std_logic;
			VSYNC_O			: out std_logic;
			PIX_CNT_O		: out std_logic_vector (9 downto 0);
			LINE_CNT_O		: out std_logic_vector (9 downto 0)			 
		);
   end component;

	-- Merge Logo and Background
	component video_merge
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
   end component;
	
	-- Logo BRAM
	component logo_bram
		port (
			clka				: in std_logic;
			wea				: in std_logic_vector(0 downto 0);
			addra				: in std_logic_vector(13 downto 0);
			dina				: in std_logic_vector(7 downto 0);
			douta				: out std_logic_vector(7 downto 0)
		);
	end component;


	------------------------------------------------------------------------
	-- Signals Declarations
	------------------------------------------------------------------------

	signal line_cnt  : STD_LOGIC_VECTOR (9 DOWNTO 0);-- V line
	signal pix_cnt : STD_LOGIC_VECTOR (9 DOWNTO 0);-- H pixel

	signal logo_ram_addr : STD_LOGIC_VECTOR (13 downto 0); -- address of bram
	signal logo_ram_data :STD_LOGIC_VECTOR (7 downto 0);   -- data of bram
	
	------------------------------------------------------------------------
	-- Data path signals
	------------------------------------------------------------------------

------------------------------------------------------------------------
-- Implementation
------------------------------------------------------------------------
begin

   SYNC_GEN_INST : sync_gen
      port map (
			PIX_CLK_I  => PIX_CLK_I,
			HSYNC_O      => HSYNC_O,
			VSYNC_O  => VSYNC_O,
			PIX_CNT_O => pix_cnt,
			LINE_CNT_O      => line_cnt
			);
			
	VIDEO_MERGE_INST : video_merge
      port map (
			PIX_CLK_I  => PIX_CLK_I,
			PIX_CNT_I => pix_cnt,
			LINE_CNT_I  => line_cnt,
			LOGO_DATA_I    => logo_ram_data,
			LOGO_ADDR_O   => logo_ram_addr,
			RED_O  => RED_O,
			GREEN_O => GREEN_O,
			BLUE_O => BLUE_O
			);

	LOGO_BRAM_INST : logo_bram
		port map (
			clka => PIX_CLK_I,
			wea => "0",
			addra => logo_ram_addr,
			dina => x"00",
			douta => logo_ram_data
			);

end Behavioral;

