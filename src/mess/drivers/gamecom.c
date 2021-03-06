// license:BSD-3-Clause
// copyright-holders:Wilbert Pol, Robbbert
/***************************************************************************

Driver file to handle emulation of the Tiger Game.com by
  Wilbert Pol

Todo:
- Fix cpu and system problems that prevent the games from working.

Game Status:
- The DAC sound partially works, sound from ports 1,2,3 not done
- Inbuilt ROM and PDA functions all work
- On the screen where the cart goes into the slot there is video flicker
- Due to an irritating message, the NVRAM is commented out in the machine config
- Most of the cart games have severe video issues such as flickering and nonsense gfx
- Lights Out works
- Centipede works
- Wheel of Fortune 1 & 2 are working.
- Frogger works, but it is difficult to capture the female frog or the insect.
- Quiz Wiz works, but the end-of-round score doesn't show
- Jeopardy, playable with bad gfx
- Tiger Web Link & Internet, they look ok, obviously aren't going to connect to anything
- Williams Arcade Classics, Robotron works, the rest are no use.
- Monopoly is starting to show promise. It's playable but the video is terrible.
- The remaining carts are not functional to any useful degree.

***************************************************************************/

#include "includes/gamecom.h"
#include "gamecom.lh"

static ADDRESS_MAP_START(gamecom_mem_map, AS_PROGRAM, 8, gamecom_state)
	AM_RANGE( 0x0000, 0x0013 )  AM_RAM AM_REGION("maincpu", 0x00)
	AM_RANGE( 0x0014, 0x0017 )  AM_READWRITE( gamecom_pio_r, gamecom_pio_w )        // buttons
	AM_RANGE( 0x0018, 0x001F )  AM_RAM AM_REGION("maincpu", 0x18)
	AM_RANGE( 0x0020, 0x007F )  AM_READWRITE( gamecom_internal_r, gamecom_internal_w )/* CPU internal register file */
	AM_RANGE( 0x0080, 0x03FF )  AM_RAM AM_REGION("maincpu", 0x80)                     /* RAM */
	AM_RANGE( 0x0400, 0x0FFF )  AM_NOP                                                /* Nothing */
	AM_RANGE( 0x1000, 0x1FFF )  AM_ROM                                                /* Internal ROM (initially), or External ROM/Flash. Controlled by MMU0 (never swapped out in game.com) */
	AM_RANGE( 0x2000, 0x3FFF )  AM_ROMBANK("bank1")                                   /* External ROM/Flash. Controlled by MMU1 */
	AM_RANGE( 0x4000, 0x5FFF )  AM_ROMBANK("bank2")                                   /* External ROM/Flash. Controlled by MMU2 */
	AM_RANGE( 0x6000, 0x7FFF )  AM_ROMBANK("bank3")                                   /* External ROM/Flash. Controlled by MMU3 */
	AM_RANGE( 0x8000, 0x9FFF )  AM_ROMBANK("bank4")                                   /* External ROM/Flash. Controlled by MMU4 */
	AM_RANGE( 0xA000, 0xDFFF )  AM_RAM AM_SHARE("videoram")             /* VRAM */
	AM_RANGE( 0xE000, 0xFFFF )  AM_RAM AM_SHARE("nvram")           /* Extended I/O, Extended RAM */
ADDRESS_MAP_END

static INPUT_PORTS_START( gamecom )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_NAME( "Up" )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_NAME( "Down" )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_NAME( "Left" )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_NAME( "Right" )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME( "Menu" ) PORT_CODE( KEYCODE_M )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME( DEF_STR(Pause) ) PORT_CODE( KEYCODE_V )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME( "Sound" ) PORT_CODE( KEYCODE_S )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME( "Button A" ) PORT_CODE( KEYCODE_A ) PORT_CODE( KEYCODE_LCONTROL )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME( "Button B" ) PORT_CODE( KEYCODE_B ) PORT_CODE( KEYCODE_LALT )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME( "Button C" ) PORT_CODE( KEYCODE_C ) PORT_CODE( KEYCODE_SPACE )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME( "Reset" ) PORT_CODE( KEYCODE_N )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME( "Button D" ) PORT_CODE( KEYCODE_D ) PORT_CODE( KEYCODE_LSHIFT )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME( "Stylus press" ) PORT_CODE( KEYCODE_Z ) PORT_CODE( MOUSECODE_BUTTON1 )

	PORT_START("GRID.0")
	PORT_BIT( 0x001, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x002, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x004, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x008, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x010, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x020, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x040, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x080, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x100, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x200, IP_ACTIVE_HIGH, IPT_OTHER)

	PORT_START("GRID.1")
	PORT_BIT( 0x001, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x002, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x004, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x008, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x010, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x020, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x040, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x080, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x100, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x200, IP_ACTIVE_HIGH, IPT_OTHER)

	PORT_START("GRID.2")
	PORT_BIT( 0x001, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x002, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x004, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x008, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x010, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x020, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x040, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x080, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x100, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x200, IP_ACTIVE_HIGH, IPT_OTHER)

	PORT_START("GRID.3")
	PORT_BIT( 0x001, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x002, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x004, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x008, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x010, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x020, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x040, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x080, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x100, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x200, IP_ACTIVE_HIGH, IPT_OTHER)

	PORT_START("GRID.4")
	PORT_BIT( 0x001, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x002, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x004, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x008, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x010, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x020, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x040, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x080, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x100, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x200, IP_ACTIVE_HIGH, IPT_OTHER)

	PORT_START("GRID.5")
	PORT_BIT( 0x001, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x002, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x004, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x008, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x010, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x020, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x040, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x080, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x100, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x200, IP_ACTIVE_HIGH, IPT_OTHER)

	PORT_START("GRID.6")
	PORT_BIT( 0x001, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x002, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x004, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x008, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x010, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x020, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x040, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x080, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x100, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x200, IP_ACTIVE_HIGH, IPT_OTHER)

	PORT_START("GRID.7")
	PORT_BIT( 0x001, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x002, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x004, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x008, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x010, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x020, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x040, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x080, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x100, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x200, IP_ACTIVE_HIGH, IPT_OTHER)

	PORT_START("GRID.8")
	PORT_BIT( 0x001, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x002, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x004, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x008, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x010, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x020, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x040, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x080, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x100, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x200, IP_ACTIVE_HIGH, IPT_OTHER)

	PORT_START("GRID.9")
	PORT_BIT( 0x001, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x002, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x004, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x008, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x010, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x020, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x040, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x080, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x100, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x200, IP_ACTIVE_HIGH, IPT_OTHER)

	PORT_START("GRID.10")
	PORT_BIT( 0x001, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x002, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x004, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x008, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x010, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x020, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x040, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x080, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x100, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x200, IP_ACTIVE_HIGH, IPT_OTHER)

	PORT_START("GRID.11")
	PORT_BIT( 0x001, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x002, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x004, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x008, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x010, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x020, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x040, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x080, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x100, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x200, IP_ACTIVE_HIGH, IPT_OTHER)

	PORT_START("GRID.12")
	PORT_BIT( 0x001, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x002, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x004, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x008, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x010, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x020, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x040, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x080, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x100, IP_ACTIVE_HIGH, IPT_OTHER)
	PORT_BIT( 0x200, IP_ACTIVE_HIGH, IPT_OTHER)
	INPUT_PORTS_END

PALETTE_INIT_MEMBER(gamecom_state, gamecom)
{
	palette.set_pen_color(0, 0x00, 0x00, 0x00 ); // Black
	palette.set_pen_color(1, 0x0F, 0x4F, 0x2F ); // Gray 1
	palette.set_pen_color(2, 0x6F, 0x8F, 0x4F ); // Gray 2
	palette.set_pen_color(3, 0x8F, 0xCF, 0x8F ); // Grey 3
	palette.set_pen_color(4, 0xDF, 0xFF, 0x8F ); // White
}

UINT32 gamecom_state::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	copybitmap(bitmap, m_bitmap, 0, 0, 0, 0, cliprect);
	return 0;
}

INTERRUPT_GEN_MEMBER(gamecom_state::gamecom_interrupt)
{
	m_maincpu->set_input_line(sm8500_cpu_device::LCDC_INT, ASSERT_LINE );
}

static MACHINE_CONFIG_START( gamecom, gamecom_state )
	/* basic machine hardware */
	MCFG_CPU_ADD( "maincpu", SM8500, XTAL_11_0592MHz/2 )   /* actually it's an sm8521 microcontroller containing an sm8500 cpu */
	MCFG_CPU_PROGRAM_MAP( gamecom_mem_map)
	MCFG_SM8500_DMA_CB( WRITE8( gamecom_state, gamecom_handle_dma ) )
	MCFG_SM8500_TIMER_CB( WRITE8( gamecom_state, gamecom_update_timers ) )
	MCFG_CPU_VBLANK_INT_DRIVER("screen", gamecom_state,  gamecom_interrupt)

	MCFG_QUANTUM_TIME(attotime::from_hz(60))

	//MCFG_NVRAM_ADD_0FILL("nvram")

	/* video hardware */
	MCFG_SCREEN_ADD("screen", LCD)
	MCFG_SCREEN_REFRESH_RATE( 59.732155 )
	MCFG_SCREEN_VBLANK_TIME(500)
	MCFG_SCREEN_UPDATE_DRIVER(gamecom_state, screen_update)
	MCFG_SCREEN_SIZE( 208, 160 )
	MCFG_SCREEN_VISIBLE_AREA( 0, 207, 0, 159 )
	MCFG_SCREEN_PALETTE("palette")

	MCFG_DEFAULT_LAYOUT(layout_gamecom)
	MCFG_PALETTE_ADD("palette", 5)
	MCFG_PALETTE_INIT_OWNER(gamecom_state, gamecom)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_STEREO( "lspeaker", "rspeaker" )
	/* TODO: much more complex than this */
	MCFG_SOUND_ADD("dac", DAC, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "lspeaker", 1.00)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "rspeaker", 1.00)

	/* cartridge */
	MCFG_GENERIC_CARTSLOT_ADD("cartslot1", generic_linear_slot, "gamecom_cart")
	MCFG_GENERIC_EXTENSIONS("bin,tgc")
	MCFG_GENERIC_LOAD(gamecom_state, gamecom_cart1)

	MCFG_GENERIC_CARTSLOT_ADD("cartslot2", generic_linear_slot, "gamecom_cart")
	MCFG_GENERIC_EXTENSIONS("bin,tgc")
	MCFG_GENERIC_LOAD(gamecom_state, gamecom_cart2)

	MCFG_SOFTWARE_LIST_ADD("cart_list","gamecom")
MACHINE_CONFIG_END

ROM_START( gamecom )
	ROM_REGION( 0x2000, "maincpu", 0 )
	ROM_LOAD( "internal.bin", 0x1000,  0x1000, CRC(a0cec361) SHA1(03368237e8fed4a8724f3b4a1596cf4b17c96d33) )

	ROM_REGION( 0x40000, "kernel", 0 )
	ROM_LOAD( "external.bin", 0x00000, 0x40000, CRC(e235a589) SHA1(97f782e72d738f4d7b861363266bf46b438d9b50) )
ROM_END

/*    YEAR  NAME     PARENT COMPAT MACHINE  INPUT    CLASS            INIT    COMPANY  FULLNAME */
CONS( 1997, gamecom, 0,     0,     gamecom, gamecom, gamecom_state, gamecom, "Tiger", "Game.com", GAME_NOT_WORKING | GAME_IMPERFECT_SOUND)
