// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria
/***************************************************************************

Shanghai

driver by Nicola Salmoria

TODO:
- games are currently too fast (especially noticeable with kothello screen transitions), maybe unemulated HD63484 wait state penalties?
- minor glitch with gfx copy on shanghai stage info panel (garbage on right);
- irq ack, shanghai and shangha2 uses it, kothello auto acks, maybe latter really runs on NMI instead
  (vector 2 matches same pattern as shanghai games);
- shanghai: IC37 returns bad in service mode;

* kothello

Notes: If you use the key labeled as 'Service Coin' you can start the game
with a single 'coin' no matter the Coinage Setting, but the credit is not
displayed.

***************************************************************************/

#include "emu.h"
#include "cpu/nec/nec.h"
#include "video/hd63484.h"
#include "audio/seibu.h"
#include "sound/2203intf.h"
#include "screen.h"
#include "speaker.h"


class shanghai_state : public driver_device
{
public:
	shanghai_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_screen(*this,"screen")
		{ }

	DECLARE_WRITE8_MEMBER(shanghai_coin_w);
	DECLARE_PALETTE_INIT(shanghai);
	INTERRUPT_GEN_MEMBER(half_vblank_irq);

	void shanghai(machine_config &config);
	void shangha2(machine_config &config);
	void kothello(machine_config &config);
	void hd63484_map(address_map &map);
	void kothello_map(address_map &map);
	void kothello_sound_map(address_map &map);
	void shangha2_map(address_map &map);
	void shangha2_portmap(address_map &map);
	void shanghai_map(address_map &map);
	void shanghai_portmap(address_map &map);
private:
	required_device<cpu_device> m_maincpu;
	required_device<screen_device> m_screen;
};


PALETTE_INIT_MEMBER(shanghai_state,shanghai)
{
	int i;

	for (i = 0;i < palette.entries();i++)
	{
		int bit0,bit1,bit2,r,g,b;

		/* red component */
		bit0 = (i >> 2) & 0x01;
		bit1 = (i >> 3) & 0x01;
		bit2 = (i >> 4) & 0x01;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* green component */
		bit0 = (i >> 5) & 0x01;
		bit1 = (i >> 6) & 0x01;
		bit2 = (i >> 7) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* blue component */
		bit0 = 0;
		bit1 = (i >> 0) & 0x01;
		bit2 = (i >> 1) & 0x01;
		b = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		palette.set_pen_color(i,rgb_t(r,g,b));
	}
}

INTERRUPT_GEN_MEMBER(shanghai_state::half_vblank_irq)
{
	// definitely running at vblank / 2 (hd63484 irq mask not used)
	if(m_screen->frame_number() & 1)
		device.execute().set_input_line_and_vector(0,HOLD_LINE,0x80);
}

WRITE8_MEMBER(shanghai_state::shanghai_coin_w)
{
	machine().bookkeeping().coin_counter_w(0,data & 1);
	machine().bookkeeping().coin_counter_w(1,data & 2);
}

void shanghai_state::shanghai_map(address_map &map)
{
	map(0x00000, 0x03fff).ram();
	map(0x80000, 0xfffff).rom();
}


void shanghai_state::shangha2_map(address_map &map)
{
	map(0x00000, 0x03fff).ram();
	map(0x04000, 0x041ff).w("palette", FUNC(palette_device::write16)).share("palette");
	map(0x80000, 0xfffff).rom();
}


void shanghai_state::shanghai_portmap(address_map &map)
{
	map(0x00, 0x01).rw("hd63484", FUNC(hd63484_device::status16_r), FUNC(hd63484_device::address16_w));
	map(0x02, 0x03).rw("hd63484", FUNC(hd63484_device::data16_r), FUNC(hd63484_device::data16_w));
	map(0x20, 0x23).rw("ymsnd", FUNC(ym2203_device::read), FUNC(ym2203_device::write)).umask16(0x00ff);
	map(0x40, 0x41).portr("P1");
	map(0x44, 0x45).portr("P2");
	map(0x48, 0x49).portr("SYSTEM");
	map(0x4c, 0x4c).w(this, FUNC(shanghai_state::shanghai_coin_w));
}


void shanghai_state::shangha2_portmap(address_map &map)
{
	map(0x00, 0x01).portr("P1");
	map(0x10, 0x11).portr("P2");
	map(0x20, 0x21).portr("SYSTEM");
	map(0x30, 0x31).rw("hd63484", FUNC(hd63484_device::status16_r), FUNC(hd63484_device::address16_w));
	map(0x32, 0x33).rw("hd63484", FUNC(hd63484_device::data16_r), FUNC(hd63484_device::data16_w));
	map(0x40, 0x43).rw("ymsnd", FUNC(ym2203_device::read), FUNC(ym2203_device::write)).umask16(0x00ff);
	map(0x50, 0x50).w(this, FUNC(shanghai_state::shanghai_coin_w));
}

void shanghai_state::kothello_map(address_map &map)
{
	map(0x00000, 0x07fff).ram();
	map(0x08010, 0x08011).rw("hd63484", FUNC(hd63484_device::status16_r), FUNC(hd63484_device::address16_w));
	map(0x08012, 0x08013).rw("hd63484", FUNC(hd63484_device::data16_r), FUNC(hd63484_device::data16_w));
	map(0x09010, 0x09011).portr("P1");
	map(0x09012, 0x09013).portr("P2");
	map(0x09014, 0x09015).portr("SYSTEM");
	map(0x09016, 0x0901f).nopw(); // 0x9016 is set to 0 at the boot
	map(0x0a000, 0x0a1ff).w("palette", FUNC(palette_device::write16)).share("palette");
	map(0x0b010, 0x0b01f).rw("seibu_sound", FUNC(seibu_sound_device::main_r), FUNC(seibu_sound_device::main_w)).umask16(0x00ff);
	map(0x80000, 0xfffff).rom();
}

void shanghai_state::kothello_sound_map(address_map &map)
{
	map(0x0000, 0x1fff).rom();
	map(0x2000, 0x27ff).ram();
	map(0x4000, 0x4000).w("seibu_sound", FUNC(seibu_sound_device::pending_w));
	map(0x4001, 0x4001).w("seibu_sound", FUNC(seibu_sound_device::irq_clear_w));
	map(0x4002, 0x4002).w("seibu_sound", FUNC(seibu_sound_device::rst10_ack_w));
	map(0x4003, 0x4003).w("seibu_sound", FUNC(seibu_sound_device::rst18_ack_w));
	map(0x4005, 0x4006).w("adpcm", FUNC(seibu_adpcm_device::adr_w));
	map(0x4007, 0x4007).w("seibu_sound", FUNC(seibu_sound_device::bank_w));
	map(0x4008, 0x4009).rw("seibu_sound", FUNC(seibu_sound_device::ym_r), FUNC(seibu_sound_device::ym_w));
	map(0x4010, 0x4011).r("seibu_sound", FUNC(seibu_sound_device::soundlatch_r));
	map(0x4012, 0x4012).r("seibu_sound", FUNC(seibu_sound_device::main_data_pending_r));
	map(0x4013, 0x4013).portr("COIN");
	map(0x4018, 0x4019).w("seibu_sound", FUNC(seibu_sound_device::main_data_w));
	map(0x401a, 0x401a).w("adpcm", FUNC(seibu_adpcm_device::ctl_w));
	map(0x401b, 0x401b).w("seibu_sound", FUNC(seibu_sound_device::coin_w));
	map(0x8000, 0xffff).bankr("seibu_bank1");
}

static INPUT_PORTS_START( kothello )
	SEIBU_COIN_INPUTS   /* coin inputs read through sound cpu */

	PORT_START("P1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("P2")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("SYSTEM")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW1:1,2,3")
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0x38, 0x38, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SW1:4,5,6")
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x38, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x28, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unused ) ) PORT_DIPLOCATION("SW1:7")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unused ) ) PORT_DIPLOCATION("SW1:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x03, 0x01, "Move Timer (Versus)" ) PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(    0x03, "20 Seconds" )
	PORT_DIPSETTING(    0x01, "25 Seconds" )
	PORT_DIPSETTING(    0x02, "30 Seconds" )
	PORT_DIPSETTING(    0x00, "35 Seconds" )
	PORT_DIPNAME( 0x0c, 0x04, "Move Timer (Puzzle)" ) PORT_DIPLOCATION("SW2:3,4")
	PORT_DIPSETTING(    0x0c, "30 Seconds" )
	PORT_DIPSETTING(    0x04, "35 Seconds" )
	PORT_DIPSETTING(    0x08, "40 Seconds" )
	PORT_DIPSETTING(    0x00, "45 Seconds" )
	PORT_DIPNAME( 0x30, 0x10, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(    0x30, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Medium_Easy ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Medium_Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hard ) )
	PORT_DIPNAME( 0xc0, 0x40, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SW2:7,8") // alleged to be number of losses to end tsume mode
	PORT_DIPSETTING(    0xc0, "1" )
	PORT_DIPSETTING(    0x40, "2" )
	PORT_DIPSETTING(    0x80, "3" )
	PORT_DIPSETTING(    0x00, "4" )
INPUT_PORTS_END

static INPUT_PORTS_START( shanghai_common )
	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1) PORT_NAME("P1 Select Button")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1) PORT_NAME("P1 Cancel Button")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1) PORT_NAME("P1 Help Button")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2) PORT_NAME("P2 Select Button")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2) PORT_NAME("P2 Cancel Button")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2) PORT_NAME("P2 Help Button")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

static INPUT_PORTS_START( shanghai )
	PORT_INCLUDE( shanghai_common )

	PORT_START("DSW1")
	PORT_SERVICE_DIPLOC( 0x01, IP_ACTIVE_LOW, "SW1:8" )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Allow_Continue ) )   PORT_DIPLOCATION("SW1:7")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x1c, 0x1c, DEF_STR( Coin_B ) )       PORT_DIPLOCATION("SW1:6,5,4")
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x1c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x14, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0xe0, 0xe0, DEF_STR( Coin_A ) )       PORT_DIPLOCATION("SW1:3,2,1")
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_4C ) )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x01, 0x01, "Confirmation" )      PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(    0x01, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x02, 0x02, "Help" )          PORT_DIPLOCATION("SW2:7")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x0c, 0x08, "2 Players Move Time" )   PORT_DIPLOCATION("SW2:6,5")
	PORT_DIPSETTING(    0x0c, "8" )
	PORT_DIPSETTING(    0x08, "10" )
	PORT_DIPSETTING(    0x04, "12" )
	PORT_DIPSETTING(    0x00, "14" )
	PORT_DIPNAME( 0x30, 0x20, "Bonus Time for Making Pair" )    PORT_DIPLOCATION("SW2:4,3")
	PORT_DIPSETTING(    0x30, "3" )
	PORT_DIPSETTING(    0x20, "4" )
	PORT_DIPSETTING(    0x10, "5" )
	PORT_DIPSETTING(    0x00, "6" )
	PORT_DIPNAME( 0xc0, 0x40, "Start Time" )        PORT_DIPLOCATION("SW2:2,1")
	PORT_DIPSETTING(    0xc0, "30" )
	PORT_DIPSETTING(    0x80, "60" )
	PORT_DIPSETTING(    0x40, "90" )
	PORT_DIPSETTING(    0x00, "120" )
INPUT_PORTS_END

static INPUT_PORTS_START( shangha2 )
	PORT_INCLUDE( shanghai_common )

	PORT_START("DSW1")
	PORT_SERVICE_DIPLOC( 0x01, IP_ACTIVE_LOW, "SW2:8" )
	PORT_DIPNAME( 0x06, 0x06, DEF_STR( Difficulty ) )   PORT_DIPLOCATION("SW2:7,6")
	PORT_DIPSETTING(    0x06, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x08, 0x00, "2 Players Move Time" )   PORT_DIPLOCATION("SW2:5")
	PORT_DIPSETTING(    0x08, "8" )
	PORT_DIPSETTING(    0x00, "10" )
	PORT_DIPNAME( 0x30, 0x20, "Bonus Time for Making Pair" )    PORT_DIPLOCATION("SW2:4,3")
	PORT_DIPSETTING(    0x30, "3" )
	PORT_DIPSETTING(    0x20, "4" )
	PORT_DIPSETTING(    0x10, "5" )
	PORT_DIPSETTING(    0x00, "6" )
	PORT_DIPNAME( 0xc0, 0x40, "Start Time" )        PORT_DIPLOCATION("SW2:2,1")
	PORT_DIPSETTING(    0xc0, "30" )
	PORT_DIPSETTING(    0x80, "60" )
	PORT_DIPSETTING(    0x40, "90" )
	PORT_DIPSETTING(    0x00, "120" )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x03, 0x03, "Mystery Tiles" )     PORT_DIPLOCATION("SW1:8,7")
	PORT_DIPSETTING(    0x03, "0" )
	PORT_DIPSETTING(    0x02, "4" )
	PORT_DIPSETTING(    0x01, "6" )
	PORT_DIPSETTING(    0x00, "8" )
	PORT_DIPNAME( 0x1c, 0x1c, DEF_STR( Coin_B ) )       PORT_DIPLOCATION("SW1:6,5,4")
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x1c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x14, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0xe0, 0xe0, DEF_STR( Coin_A ) )       PORT_DIPLOCATION("SW1:3,2,1")
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_4C ) )
INPUT_PORTS_END

void shanghai_state::hd63484_map(address_map &map)
{
	map(0x00000, 0x3ffff).ram();
}

MACHINE_CONFIG_START(shanghai_state::shanghai)

	/* basic machine hardware */
	MCFG_DEVICE_ADD("maincpu", V30, XTAL(16'000'000)/2) /* NEC D70116C-8 */
	MCFG_DEVICE_PROGRAM_MAP(shanghai_map)
	MCFG_DEVICE_IO_MAP(shanghai_portmap)
	MCFG_DEVICE_VBLANK_INT_DRIVER("screen", shanghai_state, half_vblank_irq)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(57)
	//MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500))
	MCFG_SCREEN_SIZE(384, 280)
	MCFG_SCREEN_VISIBLE_AREA(0, 384-1, 0, 280-1)
	MCFG_SCREEN_UPDATE_DEVICE("hd63484", hd63484_device, update_screen)
	MCFG_SCREEN_PALETTE("palette")

	MCFG_PALETTE_ADD("palette", 256)
	MCFG_PALETTE_FORMAT(xxxxBBBBGGGGRRRR)
	MCFG_PALETTE_INIT_OWNER(shanghai_state,shanghai)

	MCFG_HD63484_ADD("hd63484", 0, hd63484_map)

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	MCFG_DEVICE_ADD("ymsnd", YM2203, XTAL(16'000'000)/4)
	MCFG_AY8910_PORT_A_READ_CB(IOPORT("DSW1"))
	MCFG_AY8910_PORT_B_READ_CB(IOPORT("DSW2"))
	MCFG_SOUND_ROUTE(0, "mono", 0.15)
	MCFG_SOUND_ROUTE(1, "mono", 0.15)
	MCFG_SOUND_ROUTE(2, "mono", 0.15)
	MCFG_SOUND_ROUTE(3, "mono", 0.80)
MACHINE_CONFIG_END


MACHINE_CONFIG_START(shanghai_state::shangha2)

	/* basic machine hardware */
	MCFG_DEVICE_ADD("maincpu", V30, XTAL(16'000'000)/2) /* ? */
	MCFG_DEVICE_PROGRAM_MAP(shangha2_map)
	MCFG_DEVICE_IO_MAP(shangha2_portmap)
	MCFG_DEVICE_VBLANK_INT_DRIVER("screen", shanghai_state, half_vblank_irq)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(57)
	//MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500))
	MCFG_SCREEN_SIZE(384, 280)
	MCFG_SCREEN_VISIBLE_AREA(0, 384-1, 0, 280-1)
	MCFG_SCREEN_UPDATE_DEVICE("hd63484", hd63484_device, update_screen)
	MCFG_SCREEN_PALETTE("palette")

	MCFG_PALETTE_ADD("palette", 256)
	MCFG_PALETTE_FORMAT(xxxxBBBBGGGGRRRR)

	MCFG_HD63484_ADD("hd63484", 0, hd63484_map)

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	MCFG_DEVICE_ADD("ymsnd", YM2203, XTAL(16'000'000)/4)
	MCFG_AY8910_PORT_A_READ_CB(IOPORT("DSW1"))
	MCFG_AY8910_PORT_B_READ_CB(IOPORT("DSW2"))
	MCFG_SOUND_ROUTE(0, "mono", 0.15)
	MCFG_SOUND_ROUTE(1, "mono", 0.15)
	MCFG_SOUND_ROUTE(2, "mono", 0.15)
	MCFG_SOUND_ROUTE(3, "mono", 0.80)
MACHINE_CONFIG_END


MACHINE_CONFIG_START(shanghai_state::kothello)

	/* basic machine hardware */
	MCFG_DEVICE_ADD("maincpu", V30, XTAL(16'000'000))
	MCFG_DEVICE_PROGRAM_MAP(kothello_map)
	MCFG_DEVICE_VBLANK_INT_DRIVER("screen", shanghai_state, half_vblank_irq)

	MCFG_DEVICE_ADD("audiocpu", Z80, XTAL(16'000'000)/4)
	MCFG_DEVICE_PROGRAM_MAP(kothello_sound_map)

	MCFG_QUANTUM_TIME(attotime::from_hz(12000))

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(57)
	//MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500))
	MCFG_SCREEN_SIZE(384, 280)
	MCFG_SCREEN_VISIBLE_AREA(0, 384-1, 0, 280-1)
	MCFG_SCREEN_UPDATE_DEVICE("hd63484", hd63484_device, update_screen)
	MCFG_SCREEN_PALETTE("palette")

	MCFG_PALETTE_ADD("palette", 256)
	MCFG_PALETTE_FORMAT(xxxxBBBBGGGGRRRR)

	MCFG_HD63484_ADD("hd63484", 0, hd63484_map)
	MCFG_HD63484_EXTERNAL_SKEW(2)

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	/* same as standard seibu ym2203, but also reads "DSW" */
	MCFG_DEVICE_ADD("ymsnd", YM2203, XTAL(16'000'000)/4)
	MCFG_YM2203_IRQ_HANDLER(WRITELINE("seibu_sound", seibu_sound_device, fm_irqhandler))
	MCFG_AY8910_PORT_A_READ_CB(IOPORT("DSW1"))
	MCFG_AY8910_PORT_B_READ_CB(IOPORT("DSW2"))
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.15)

	MCFG_DEVICE_ADD("seibu_sound", SEIBU_SOUND, 0)
	MCFG_SEIBU_SOUND_CPU("audiocpu")
	MCFG_SEIBU_SOUND_ROMBANK("seibu_bank1")
	MCFG_SEIBU_SOUND_YM_READ_CB(READ8("ymsnd", ym2203_device, read))
	MCFG_SEIBU_SOUND_YM_WRITE_CB(WRITE8("ymsnd", ym2203_device, write))

	MCFG_DEVICE_ADD("adpcm", SEIBU_ADPCM, 8000) // actually MSM5205
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)
MACHINE_CONFIG_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

/*

Shanghai
Sunsoft, (c) 1988
original copyright (c) 1988 Activision, Inc.
Arcade system designed by Sun Electronics (c) 1988

PCB Layout

SHG-01-B
+------------------------------------------+
| DSW2 DSW1       16MHz                    |
|  YM2203C      D70116C-8       D4364 D4364|
|                               IC12* IC13*|
|                               IC21  IC22 |
|       YM3014B                 IC27  IC28 |
|J                              IC36  IC37 |
|A                                         |
|M                       HD63484P8         |
|M  18MHz       MB81464             MB81464|
|A              MB81464             MB81464|
|         PAL   MB81464             MB81464|
|               MB81464             MB81464|
|               MB81464             MB81464|
|               MB81464             MB81464|
|               MB81464             MB81464|
|               MB81464             MB81464|
+------------------------------------------+

  CPU: NEC D70116C-8 V30
Sound: YM2203C + YM3014B DAC
Video: HD63484P8

Ram:
Fujitsu MB81464-12 64K x 4bit DRAM
NEC D4364C-15L 8K x 8bit SRAM

IC12 & IC13 unpopulated

*/

ROM_START( shanghai )
	ROM_REGION( 0x100000, "maincpu", 0 )
	ROM_LOAD16_BYTE( "shg-22a.ic22", 0xa0001, 0x10000, CRC(e0a085be) SHA1(e281043f97c4cd34a33eb1ec7154abbe67a9aa03) )
	ROM_LOAD16_BYTE( "shg-21a.ic21", 0xa0000, 0x10000, CRC(4ab06d32) SHA1(02667d1270b101386b947d5b9bfe64052e498041) )
	ROM_LOAD16_BYTE( "shg-28a.ic28", 0xc0001, 0x10000, CRC(983ec112) SHA1(110e120e35815d055d6108a7603e83d2d990c666) )
	ROM_LOAD16_BYTE( "shg-27a.ic27", 0xc0000, 0x10000, CRC(41af0945) SHA1(dfc4638a17f716ccc8e59f275571d6dc1093a745) )
	ROM_LOAD16_BYTE( "shg-37b.ic37", 0xe0001, 0x10000, BAD_DUMP CRC(ead3d66c) SHA1(f9be9a4773ea6c9ba931f7aa8c79121caacc231c) ) /* Single byte difference from IC37 below  0xD58C == 0x01 */
	ROM_LOAD16_BYTE( "shg-36b.ic36", 0xe0000, 0x10000, CRC(a1d6af96) SHA1(01c4c22bf03b3d260fffcbc6dfc5f2dd2bcba14a) )
ROM_END

ROM_START( shanghaij )
	ROM_REGION( 0x100000, "maincpu", 0 )
	ROM_LOAD16_BYTE( "shg-22a.ic22", 0xa0001, 0x10000, CRC(e0a085be) SHA1(e281043f97c4cd34a33eb1ec7154abbe67a9aa03) )
	ROM_LOAD16_BYTE( "shg-21a.ic21", 0xa0000, 0x10000, CRC(4ab06d32) SHA1(02667d1270b101386b947d5b9bfe64052e498041) )
	ROM_LOAD16_BYTE( "shg-28a.ic28", 0xc0001, 0x10000, CRC(983ec112) SHA1(110e120e35815d055d6108a7603e83d2d990c666) )
	ROM_LOAD16_BYTE( "shg-27a.ic27", 0xc0000, 0x10000, CRC(41af0945) SHA1(dfc4638a17f716ccc8e59f275571d6dc1093a745) )
	ROM_LOAD16_BYTE( "shg-37b.ic37", 0xe0001, 0x10000, CRC(3f192da0) SHA1(e70d5da5d702e9bf9ac6b77df62bcf51894aadcf) ) /*  0xD58C == 0x00 */
	ROM_LOAD16_BYTE( "shg-36b.ic36", 0xe0000, 0x10000, CRC(a1d6af96) SHA1(01c4c22bf03b3d260fffcbc6dfc5f2dd2bcba14a) )
ROM_END

ROM_START( shangha2 )
	ROM_REGION( 0x100000, "maincpu", 0 )
	ROM_LOAD16_BYTE( "sht-27j", 0x80001, 0x20000, CRC(969cbf00) SHA1(350025f4e39c7d89cb72e46b52fb467e3e9056f4) )
	ROM_LOAD16_BYTE( "sht-26j", 0x80000, 0x20000, CRC(4bf01ab4) SHA1(6928374db080212a371991ee98cd563e158907f0) )
	ROM_LOAD16_BYTE( "sht-31j", 0xc0001, 0x20000, CRC(312e3b9d) SHA1(f15f76a087d4972aa72145eced8d1fb15329b359) )
	ROM_LOAD16_BYTE( "sht-30j", 0xc0000, 0x20000, CRC(2861a894) SHA1(6da99d15f41e900735f8943f2710487817f98579) )
ROM_END

ROM_START( shangha2a ) // content is the same, just different ROM sizes
	ROM_REGION( 0x100000, "maincpu", 0 )
	ROM_LOAD16_BYTE( "3.bin", 0x80001, 0x10000, CRC(93aacccb) SHA1(8b29b9b24cf268a4376b7f653c19d6f46d698552) )
	ROM_LOAD16_BYTE( "1.bin", 0x80000, 0x10000, CRC(0fb2d8ee) SHA1(fee8074d8116f551c634f088b8121d48a9b4a008) )
	ROM_LOAD16_BYTE( "7.bin", 0xa0001, 0x10000, CRC(f9e06880) SHA1(7840b6672cc02fd70f478a5c9f11cfc26ddfca52) )
	ROM_LOAD16_BYTE( "5.bin", 0xa0000, 0x10000, CRC(06ada73c) SHA1(13ee91b94489096f03afc05fdd3d4c65a87a6628) )
	ROM_LOAD16_BYTE( "4.bin", 0xc0001, 0x10000, CRC(b4d82724) SHA1(84496b7ad43817c307227bdab4f58a19484519bb) )
	ROM_LOAD16_BYTE( "2.bin", 0xc0000, 0x10000, CRC(97a25fdb) SHA1(43f065b737e5c4bd44c02ab1d0d6fa34aea8d139) )

	ROM_LOAD16_BYTE( "8.bin", 0xf0001, 0x08000, CRC(21c41557) SHA1(967c97a6b35407a5b32938c88bf7e719a1489b6b) )
	ROM_LOAD16_BYTE( "6.bin", 0xf0000, 0x08000, CRC(14250057) SHA1(15af554099c977e3c753d758080805581a9e4c50) )
ROM_END


/*

Kyuukyoku no Othello
Success Corp. 1990

PCB Layout

SSS8906
|------------------------------------------|
| YM3014 DSW2  DSW1           4464    4464 |
|                             4464    4464 |
|              YM2203         4464    4464 |
|J      M5205  Z80            4464    4464 |
|A             6116           4464    4464 |
|M             PAL            4464    4464 |
|M                            4464    4464 |
|A  ROM6                      4464    4464 |
|              ROM5           HD63484      |
| YM3931                             898-3 |
|                  S1S6091                 |
|                                          |
|                                          |
| PAL                                      |
| PAL              ROM3  ROM4              |
|                                    PAL   |
|                  ROM2  ROM1              |
| CXQ70116                                 |
| D71011           6264  6264              |
|16MHz     PAL     6264  6264              |
|------------------------------------------|
Notes:
      Z80 clock     : 4.000MHz
      CXQ70116 clock: 16.000MHz
      YM3931 clock  : 4.000MHz
      YM2203 clock  : 4.000MHz
      M5205 clock   : 444598Hz; sample rate = M5205 clock / 48
      HD63484 clock : pin50- 4.000MHz, pin52- 2.000MHz
      VSync         : 57Hz

      HD63484  : CRT Controller
      CXQ70116 : Compatible with NEC V30 (DIP40)
      D71011   : ? (DIP18) 710xx is series of common NEC ICs; timers, counters, parallel interface, interrupt controllers etc
      898-3    : BI 898-3-R 22  8920  (?, DIP16, tied to hd63484)
      YM3931   : Also printed 'SEI0100BU' (SDIP64)
      S1S6091  : Custom QFP80 (Graphics controller?)
      4464     : 64K x4 DRAM
      6264       8K x8 SRAM
      6116     : 2K x8 SRAM

*/


ROM_START( kothello )
	ROM_REGION( 0x100000, "maincpu", 0 )
	ROM_LOAD16_BYTE( "rom1.3e", 0x80001, 0x20000, CRC(8601dcfa) SHA1(e7ffc6da0bfb5cec5a543a2a5223b235c3428eb3) )
	ROM_LOAD16_BYTE( "rom2.5e", 0x80000, 0x20000, CRC(68f6b7a3) SHA1(9f7e217e07bc79b1e95551cd0fe107294bf5889f) )
	ROM_LOAD16_BYTE( "rom3.3f", 0xc0001, 0x20000, CRC(2f3dacd1) SHA1(35bfdc1f377b87a80c3abbb48f9f0b52108fbfc0) )
	ROM_LOAD16_BYTE( "rom4.5f", 0xc0000, 0x20000, CRC(ee8bbea7) SHA1(35dfa7aa89cecba6482b18a5233511bacc4bf331) )

	ROM_REGION( 0x20000, "audiocpu", 0 )
	ROM_LOAD( "rom5.5l",   0x00000, 0x02000, CRC(7eb6e697) SHA1(4476e13f9a9e04472581f2c069760f53b33d5672))
	ROM_CONTINUE(          0x10000, 0x0e000 )

	ROM_REGION( 0x10000, "adpcm", 0 )
	ROM_LOAD( "rom6.7m",   0x00000, 0x10000, CRC(4ab1335d) SHA1(3a803e8a7e9b0c2a26ee23e7ac9c89c70cf2504b))
ROM_END



GAME( 1988, shanghai,  0,        shanghai, shanghai, shanghai_state, 0, ROT0, "Sunsoft", "Shanghai (World)",           MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE )
GAME( 1988, shanghaij, shanghai, shanghai, shanghai, shanghai_state, 0, ROT0, "Sunsoft", "Shanghai (Japan)",           MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE )
GAME( 1989, shangha2,  0,        shangha2, shangha2, shanghai_state, 0, ROT0, "Sunsoft", "Shanghai II (Japan, set 1)", MACHINE_SUPPORTS_SAVE )
GAME( 1989, shangha2a, shangha2, shangha2, shangha2, shanghai_state, 0, ROT0, "Sunsoft", "Shanghai II (Japan, set 2)", MACHINE_SUPPORTS_SAVE )
GAME( 1990, kothello,  0,        kothello, kothello, shanghai_state, 0, ROT0, "Success", "Kyuukyoku no Othello",       MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE )
