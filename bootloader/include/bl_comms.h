#ifndef _BL_COMMS_H_
#define _BL_COMMS_H_

#include "comms.h"

namespace motor_driver {

extern UARTEndpoint comms_endpoint;

extern Server comms_server;

extern ProtocolFSM comms_protocol_fsm;

void startComms();

void runComms();

} // namespace motor_driver

#endif /* _BL_COMMS_H_ */
