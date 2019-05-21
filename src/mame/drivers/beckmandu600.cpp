// license:BSD-3-Clause
// copyright-holders:Joseph Zatarski
/***************************************************************************

    Skeleton driver for the Beckman Instruments DU600 spectrophotometer

    https://hackaday.io/project/6150-beckman-du600-reverse-engineering

    TODO:
    - Just about everything
	- more specifically start with proper 68332 emulation
	- Cirrus Logic CL-GD5429
	- custom AT KB controller (based on NEC uPD8741, needs dumping)
		- normal AT KB controller IC works in it's place, so other devices could suffice for emulation for now
	- add-on ROM and RAM cards
	- floppy controller, I don't have one to document
	- RTC
	- LEDs
	- centronics port
	- 8k EEPROM
	- piezo buzzer
	- Various other completely undocumented motor control and other devices


****************************************************************************/

#include "emu.h"
#include "bus/rs232/rs232.h"
#include "cpu/m68000/m68000.h"
#include "machine/mc68681.h"
#include "machine/clock.h"


class beckmandu600_state : public driver_device
{
public:
	beckmandu600_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_duart0(*this, "duart0")
	{ }

	void beckmandu600(machine_config &config);

private:
	void beckmandu600_mem(address_map &map);

	required_device<cpu_device> m_maincpu;
	required_device<xr68c681_device> m_duart0;
	DECLARE_WRITE_LINE_MEMBER(duart0_irq_handler);
};

void beckmandu600_state::beckmandu600_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x000000, 0x0fffff).rom();
	map(0x400000, 0x4fffff).ram();
	map(0xd00000, 0xd0000f).rw("duart0", FUNC(mc68681_device::read), FUNC(mc68681_device::write));
}


/* Input ports */
static INPUT_PORTS_START( beckmandu600 )
INPUT_PORTS_END

WRITE_LINE_MEMBER(beckmandu600_state::duart0_irq_handler)
{
	m_maincpu->set_input_line_and_vector(5, state, m_duart0->get_irq_vector());
}

void beckmandu600_state::beckmandu600(machine_config &config)
{
	FSCPU32(config, m_maincpu, 16000000); /* this will have to be changed to 68332 eventually */
	m_maincpu->set_addrmap(AS_PROGRAM, &beckmandu600_state::beckmandu600_mem);

	XR68C681(config, m_duart0, 3686400);
	m_duart0->a_tx_cb().set("rs232", FUNC(rs232_port_device::write_txd));
	m_duart0->irq_cb().set(*this, FUNC(beckmandu600_state::duart0_irq_handler));

	rs232_port_device &rs232(RS232_PORT(config, "rs232", default_rs232_devices, "terminal"));
	rs232.rxd_handler().set("duart0", FUNC(mc68681_device::rx_a_w));
}

/* ROM definition */
ROM_START( beckmandu600 )
	ROM_REGION( 0x0100000, "maincpu", 0 )
//	ROM_LOAD( "joemon.bin", 0x0000, 0x2f78, CRC(20a8d0d0) SHA1(544fd8bd8ed017115388c8b0f7a7a59a32253e43) )
	ROM_LOAD16_BYTE( "517243.u36",0x000000, 0x080000, CRC(6cf509f4) SHA1(5f3acb8f51d22a63ebc1e27d50d145acc7742a9b) )
	ROM_LOAD16_BYTE( "517244.u37",0x000001, 0x080000, CRC(5f4a8db2) SHA1(7a292f9f31d383c2cbc8a0c3ec8e748cec1ca773) )
ROM_END

/* Driver */

/*    YEAR  NAME          PARENT  COMPAT  MACHINE        INPUT          CLASS                INIT        COMPANY                FULLNAME         FLAGS */
COMP( ????, beckmandu600, 0,      0,      beckmandu600, beckmandu600, beckmandu600_state, empty_init, "Beckman Instruments", "Beckman DU600", MACHINE_IS_SKELETON | MACHINE_NODEVICE_PRINTER )
