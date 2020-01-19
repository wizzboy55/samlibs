/*
 * IPTraceMacros.h
 *
 * Created: 2019-08-16 8:15:30 PM
 *  Author: Wizz
 */ 


#ifndef IPTRACEMACROS_H_
#define IPTRACEMACROS_H_

#include "debug_interface.h"

#define iptraceETHERNET_RX_EVENT_LOST() DEBUG_printf( ("IP: Rx Event Lost\n") )
//#define iptraceNETWORK_EVENT_RECEIVED( eEvent ) DEBUG_printf( ("IP: Event Received\n") )
//#define iptraceNETWORK_INTERFACE_RECEIVE() DEBUG_printf( ("IP: Interface Receive\n") )
#define iptraceNETWORK_INTERFACE_TRANSMIT() DEBUG_printf( ("IP: Interface Transmit\n") )

#define iptraceARP_TABLE_ENTRY_CREATED( ulIPAddress, ucMACAddress ) DEBUG_printf( ("IP: ARP Entry created\n") )
#define iptraceCREATING_ARP_REQUEST( ulIPAddress ) DEBUG_printf( ("IP: Creating ARP request\n") )
#define iptracePROCESSING_RECEIVED_ARP_REPLY( ulIPAddress ) DEBUG_printf( ("IP: Processing ARP Reply\n") )
#define iptraceSENDING_ARP_REPLY( ulIPAddress ) DEBUG_printf( ("IP: Sending ARP Reply\n") )
#define iptraceARP_PACKET_RECEIVED() DEBUG_printf( (":IP: ARP Packet Received\n") )

#define iptraceSENDING_PING_REPLY( ulIPAddress ) DEBUG_printf( ("IP: Sending Ping Reply\n") )

#define iptraceSENDING_DHCP_DISCOVER() DEBUG_printf( ("IP: Sending DHCP Discover\n") )
#define iptraceSENDING_DHCP_REQUEST() DEBUG_printf( ("IP: Sending DHCP Request\n") )

#endif /* IPTRACEMACROS_H_ */