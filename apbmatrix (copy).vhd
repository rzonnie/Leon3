-----------------------------------------------------------------------------
-- Entity:      apbmatrix
-- File:        apbmatrix.vhd
-- Author:      Remi Jonkman
-- Description: 13x13 matrix multiplication unit (without FIFO)
-----------------------------------------------------------------------------

-- include some standard libraries
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

-- include grlib libraries
library grlib;
use grlib.stdlib.all;
use grlib.amba.all;
use grlib.devices.all;

-- for now, this library is not included (uncomment to include)
--library gaisler;
--use gaisler.misc.all;

entity apbmatrix is
  generic(
    pindex      : integer := 0;
    paddr       : integer := 0;
    pmask       : integer := 16#fff#
    );
  port(
    rst         : in std_ulogic;        -- Global asynchronous reset
    clk         : in std_ulogic;        -- Global clock
    apbi        : in apb_slv_in_type;	-- APB slave input
    apbo        : out apb_slv_out_type  -- apb slave output
    );
end;

architecture rtl of apbmatrix is

constant OPERAND_WORD_SIZE : integer := 8;
constant RESULT_WORD_SIZE : integer := 32;
constant ROW_SIZE : integer := 13;
constant COL_SIZE : integer := 16;

type states is (read_data, calc, ready); -- matrix multiplication states
type matrix_row array(0 to COL_SIZE-1) of std_logic_vector(OPERAND_WORD_SIZE-1 downto 0); -- operand register type (13 x 8 bits regsiter)
type matrix is array(0 to ROW_SIZE-1) of matrix_row;

type matrix_result_row is array(0 to (COL_SIZE*4)-1) of std_logic_vector(7 downto 0);
type matrix_result is array(0 to ROW_SIZE-1) of matrix_result_row;

-- type resulttype is array(0 to 168) of std_logic_vector(RESULT_WORD_SIZE-1 downto 0); -- result register (13 x 32 bits regsiter)

type matrix_regs is record
  -- data reg
	state : states;
	read_data : std_ulogic;
	calc : std_ulogic;
	ready : std_ulogic;
	element_index : std_logic_vector(7 downto 0);

  -- operand row (8 bit per element)
  matrix_a : matrix;

  -- operand column (8 bit per element)
  matrix_b : matrix;  

  -- result reg (13 times 32 bit value)
  matrix_c : matrix_result;
end record;

constant REVISION : integer := 0;
constant DEVICE_ID : integer := 0;

-- VENCOR_ID = various contributions
-- DEVICE_ID = 0 (device ID does not matter)
-- VERSION_ID = 1
constant pconfig : apb_config_type := 
	(
		0 => ahb_device_reg (VENDOR_CONTRIB, DEVICE_ID, 0, REVISION, 0),
		1 => apb_iobar(paddr, pmask)
	);

signal r, rin : matrix_regs;

begin
  matrix_op : process(r, rst, apbi)
		variable v  : matrix_regs;
		variable addr_hold : integer;
		variable rdata : std_logic_vector(31 downto 0);
		variable result : integer;
  begin
	  -- initialize some temporary variables
		rdata := (others => '0');
		result := 0;
		
		-- load register contents
    v := r;
    
		-- read address on bus
		addr_hold := to_integer(unsigned(apbi.paddr(7 downto 0)));
		
    -- write registers
    if (apbi.psel(pindex) and apbi.penable and apbi.pwrite) = '1' then
      case addr_hold is
        when 0 =>
				  -- mixed status and control registers
					v.read_data := apbi.pwdata(31);
          v.calc := apbi.pwdata(30);
					v.ready := apbi.pwdata(29);
					v.element_index := apbi.pwdata(7 downto 0);
        when 4 to 172 =>
				  -- matrix A
					v.matrix_a(addr_hold-4) := apbi.pwdata(7 downto 0);
					v.matrix_a(addr_hold-3) := apbi.pwdata(15 downto 8);
					v.matrix_a(addr_hold-2) := apbi.pwdata(23 downto 16);
					v.matrix_a(addr_hold-1) := apbi.pwdata(31 downto 24);
				when 173 to 341 =>
				  -- matrix B
					v.matrix_b(addr_hold-20) := apbi.pwdata(7 downto 0);
					v.matrix_b(addr_hold-19) := apbi.pwdata(15 downto 8);
					v.matrix_b(addr_hold-18) := apbi.pwdata(23 downto 16);
					v.matrix_b(addr_hold-17) := apbi.pwdata(31 downto 24);
        when others =>
          null;
      end case;
    end if;
		
		-- read result registers
    case addr_hold is
			when 0 =>
				-- mixed status and control registers
				rdata(31) := r.read_data;
        rdata(30) := r.calc;
				rdata(29) := r.ready;
				rdata(7 downto 0) := r.element_index(7 downto 0);
			when 4 to 172 =>
				-- row operands
				rdata(7 downto 0) := r.matrix_a(addr_hold-4);
				rdata(15 downto 8) := r.matrix_a(addr_hold-3);
				rdata(23 downto 16) := r.matrix_a(addr_hold-2);
				rdata(31 downto 24) := r.matrix_a(addr_hold-1);
			when 173 to 341 =>
				-- column operands
				rdata(7 downto 0) := r.matrix_b(addr_hold-20);
				rdata(15 downto 8) := r.matrix_b(addr_hold-19);
				rdata(23 downto 16) := r.matrix_b(addr_hold-18);
				rdata(31 downto 24) := r.matrix_b(addr_hold-17);
			when 342 to 1017 =>
				-- result registers
				rdata(7 downto 0) := r.matrix_c(addr_hold-342);
				rdata(15 downto 8) := r.matrix_c(addr_hold-341);
				rdata(23 downto 16) := r.matrix_c(addr_hold-340);
				rdata(31 downto 24) := r.matrix_c(addr_hold-339);
			when others =>
				null;
		end case;

    -- matrix multiplication
    if (v.calc = '1')
		then
			mult : for i in 0 to COL_SIZE-1 loop
				result := result + (to_integer(signed(v.operand_row(i))) * to_integer(signed(v.operand_column(i))));
			end loop mult;
			v.results := std_logic_vector(to_signed(result, v.results'length));
			v.calc := '0';
			v.ready := '1';
    end if;
		
    -- reset operations
    if rst = '0' then
      v.state := ready; 
			v.read_data := '0';
			v.calc := '0';
			v.ready := '0';
			v.element_index := (others => '0');
			v.operand_row := (others => (others => '0'));
			v.operand_column := (others => (others => '0'));
			v.results := (others => '0');
    end if;

    -- update registers
    rin <= v;

    -- drive outputs
    apbo.prdata      <= rdata;
    apbo.pindex      <= pindex;
  end process matrix_op;
	
	-- set configuration
  apbo.pconfig <= pconfig;

	-- update device registers on the rising edge of the clock
  regs : process(clk)
  begin
    if rising_edge(clk) then
      r <= rin;
    end if;
  end process regs;

-- pragma translate_off
    bootmsg : report_version
    generic map ("apbmatrix_" & tost(pindex) & ": APB Matrix Multiplication rev " &
                 tost(REVISION) & ", no irq ");
-- pragma translate_on

end;

