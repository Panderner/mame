// license:BSD-3-Clause
// copyright-holders:hap
/***************************************************************************

  Taito Capriccio Z80 crane hardware (let's call it 1st generation)

  These are presumed to be on similar hardware:
  - Capriccio         1991
  - New Capriccio     1992
  - Caprina           1993
  - New Capriccio 2   1993
  - Capriccio Spin    1994
  - Capriccio Spin 2  1996

  The next released game of this series is Capriccio Cyclone, see caprcyc.c
  More games were released after this.

TODO:
- get cspin2 working a bit:
  * unknown reads and writes
  * should have a rombank somewhere
  * what causes the nmi?
  * what kind of device lives at C008-C009 and C00C-C00D? looks like a
    Mitsubishi M66300 Parallel-In Serial-Out Data Buffer with FIFO
    (each is initialized with 80 to control port, then operated by writing
    0A, 08, 00 to control port and transferring five bytes from memory to
    the data port, finishing by writing 01 to the control port)
  * 2 players, 1 7seg led on each cpanel, 3 7seg leds on cranes
- get more dumps, find out technical differences between games and document them
- the rest can come later

***************************************************************************/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "machine/te7750.h"
#include "sound/2203intf.h"
#include "sound/okim6295.h"
#include "speaker.h"


class capr1_state : public driver_device
{
public:
	capr1_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu")
	{ }

	DECLARE_WRITE8_MEMBER(output_w);

	required_device<cpu_device> m_maincpu;
	void cspin2(machine_config &config);
	void cspin2_map(address_map &map);
};

WRITE8_MEMBER(capr1_state::output_w)
{
	// bit 7 = watchdog?
}


/***************************************************************************

  I/O

***************************************************************************/

void capr1_state::cspin2_map(address_map &map)
{
	map(0x0000, 0x7fff).rom();
	map(0x8000, 0x9fff).ram();
	map(0xa000, 0xa00f).rw("te7750", FUNC(te7750_device::read), FUNC(te7750_device::write));
	map(0xc000, 0xc001).rw("ym", FUNC(ym2203_device::read), FUNC(ym2203_device::write));
	map(0xc004, 0xc004).rw("oki", FUNC(okim6295_device::read), FUNC(okim6295_device::write));
//  AM_RANGE(0xc008, 0xc009) AM_DEVWRITE("fifo1", m66300_device, write)
//  AM_RANGE(0xc00c, 0xc00d) AM_DEVWRITE("fifo2", m66300_device, write)
//  AM_RANGE(0xe000, 0xe001) AM_WRITENOP
//  AM_RANGE(0xe002, 0xe004) AM_WRITENOP
}



/***************************************************************************

  Inputs

***************************************************************************/

static INPUT_PORTS_START( cspin2 )
	// just some test stuff
	PORT_START("INA")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON4 )

	PORT_START("INB")
	PORT_DIPUNKNOWN_DIPLOC( 0x01, 0x01, "SW1:1" )
	PORT_DIPUNKNOWN_DIPLOC( 0x02, 0x02, "SW1:2" )
	PORT_DIPUNKNOWN_DIPLOC( 0x04, 0x04, "SW1:3" )
	PORT_DIPUNKNOWN_DIPLOC( 0x08, 0x08, "SW1:4" )
	PORT_DIPUNKNOWN_DIPLOC( 0x10, 0x10, "SW1:5" )
	PORT_DIPUNKNOWN_DIPLOC( 0x20, 0x20, "SW1:6" )
	PORT_DIPUNKNOWN_DIPLOC( 0x40, 0x40, "SW1:7" )
	PORT_DIPUNKNOWN_DIPLOC( 0x80, 0x80, "SW1:8" )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN5")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN6")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN7")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN8")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END



/***************************************************************************

  Machine Config

***************************************************************************/

MACHINE_CONFIG_START(capr1_state::cspin2)

	/* basic machine hardware */
	MCFG_DEVICE_ADD("maincpu", Z80, 4000000) // clock frequency unknown
	MCFG_DEVICE_PROGRAM_MAP(cspin2_map)
	//MCFG_DEVICE_PERIODIC_INT_DRIVER(capr1_state, nmi_line_pulse, 20)

	MCFG_DEVICE_ADD("te7750", TE7750, 0) // guess
	MCFG_TE7750_IOS_CB(CONSTANT(7))
	MCFG_TE7750_IN_PORT1_CB(IOPORT("IN1"))
	MCFG_TE7750_IN_PORT2_CB(IOPORT("IN2"))
	MCFG_TE7750_IN_PORT3_CB(IOPORT("IN3"))
	MCFG_TE7750_IN_PORT4_CB(IOPORT("IN4"))
	MCFG_TE7750_IN_PORT5_CB(IOPORT("IN5"))
	MCFG_TE7750_IN_PORT6_CB(IOPORT("IN6"))
	MCFG_TE7750_IN_PORT7_CB(IOPORT("IN7"))
	MCFG_TE7750_IN_PORT8_CB(IOPORT("IN8"))
	MCFG_TE7750_OUT_PORT9_CB(WRITE8(*this, capr1_state, output_w))

	/* no video! */

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	MCFG_DEVICE_ADD("ym", YM2203, 4000000) // clock frequency unknown
	MCFG_YM2203_IRQ_HANDLER(INPUTLINE("maincpu", 0))
	MCFG_AY8910_PORT_A_READ_CB(IOPORT("INA"))
	MCFG_AY8910_PORT_B_READ_CB(IOPORT("INB"))
	MCFG_SOUND_ROUTE(0, "mono", 0.15)
	MCFG_SOUND_ROUTE(1, "mono", 0.15)
	MCFG_SOUND_ROUTE(2, "mono", 0.15)
	MCFG_SOUND_ROUTE(3, "mono", 0.40)

	MCFG_DEVICE_ADD("oki", OKIM6295, 1056000, okim6295_device::PIN7_HIGH) // clock frequency & pin 7 not verified
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_CONFIG_END



/***************************************************************************

  Game drivers

***************************************************************************/

/*

CAPRICCIO SPIN 2
(c)1996 TAITO

CPU   : Z80
SOUND : YM2203 MSM6295

E30-01-1.BIN ; MAIN PRG
E30-02.BIN   ; ADPCM
*/

ROM_START( cspin2 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "e30-01-1.bin",  0x000000, 0x010000, CRC(30bc0620) SHA1(965d43cbddbd809ebbfdd78ebeb0b87e441d9849) )

	ROM_REGION( 0x40000, "oki", 0 )
	ROM_LOAD( "e30-02.bin",    0x000000, 0x040000, CRC(519e5474) SHA1(04b344b34d780f2f83207bf6eee2573cc0ce421e) )
ROM_END


GAME (1996, cspin2, 0, cspin2, cspin2, capr1_state, 0, ROT0, "Taito", "Capriccio Spin 2", MACHINE_IS_SKELETON_MECHANICAL )
