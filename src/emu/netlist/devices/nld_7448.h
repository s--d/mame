// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_7448.h
 *
 *  DM7448: BCD to 7-Segment decoders/drivers
 *
 *           +--------------+
 *         B |1     ++    16| VCC
 *         C |2           15| f
 * LAMP TEST |3           14| g
 *    BI/RBQ |4    7448   13| a
 *       RBI |5           12| b
 *         D |6           11| c
 *         A |7           10| d
 *       GND |8            9| e
 *           +--------------+
 *
 *
 *  Naming conventions follow National Semiconductor datasheet
 *
 */

#ifndef NLD_7448_H_
#define NLD_7448_H_

#include "../nl_base.h"

#define TTL_7448(_name, _A0, _A1, _A2, _A3, _LTQ, _BIQ, _RBIQ)                      \
		NET_REGISTER_DEV(7448, _name)                                               \
		NET_CONNECT(_name, A, _A0)                                                  \
		NET_CONNECT(_name, B, _A1)                                                  \
		NET_CONNECT(_name, C, _A2)                                                  \
		NET_CONNECT(_name, D, _A3)                                                  \
		NET_CONNECT(_name, LTQ, _LTQ)                                               \
		NET_CONNECT(_name, BIQ, _BIQ)                                               \
		NET_CONNECT(_name, RBIQ, _RBIQ)

#define TTL_7448_DIP(_name)                                                         \
		NET_REGISTER_DEV(7448_dip, _name)

/*
 * FIXME: Big FIXME
 * The truthtable will ignore BIQ for certain states.
 * This causes a severe issue in pongf when USE_DEACTIVATE is used.
 * For now, just use the old implementation.
 *
 */
#if (0 && USE_TRUTHTABLE)
#include "nld_truthtable.h"

NETLIB_TRUTHTABLE(7448, 7, 7, 0);
#else

NETLIB_SUBDEVICE(7448_sub,
	ATTR_HOT void update_outputs(UINT8 v);
	static const UINT8 tab7448[16][7];

	netlist_logic_input_t m_A;
	netlist_logic_input_t m_B;
	netlist_logic_input_t m_C;
	netlist_logic_input_t m_D;
	netlist_logic_input_t m_RBIQ;

	UINT8 m_state;
	int m_active;

	netlist_logic_output_t m_Q[7];  /* a .. g */

	// FIXME: doesn't work
#if 0
	ATTR_HOT void inc_active();
	ATTR_HOT void dec_active();
#endif
);

NETLIB_DEVICE(7448,
public:
	NETLIB_NAME(7448_sub) sub;

	netlist_logic_input_t m_LTQ;
	netlist_logic_input_t m_BIQ;
);
#endif
NETLIB_DEVICE_DERIVED(7448_dip, 7448,
);

#endif /* NLD_7448_H_ */
