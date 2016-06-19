/*
 * dhcp.h
 *
 *  Created on: 2016/06/18
 *      Author: t9knowledge
 */

#ifndef DHCP_H_
#define DHCP_H_
struct DHCPHeader {
    /// Message op code / message type. 1 = BOOTREQUEST, 2 = BOOTREPLY
    byte op;

    /// Hardware address type, see ARP section in "Assigned Numbers" RFC; e.g., '1' = 10mb ethernet.
    byte htype;

    /// Hardware address length (e.g.  '6' for 10mb ethernet).
    byte hlen;

    /// Client sets to zero, optionally used by relay agents when booting via a relay agent.
    byte hops;

    /// Transaction ID, a random number chosen by the client, used by the client and server to associate messages and responses between a client and a server.
    int xid;

    /// Filled in by client, seconds elapsed since client began address acquisition or renewal process.
    short secs;

    /// Flags. (Only the BROADCAST flag is defined.)
    short flags;

    /// Client IP address; only filled in if client is in BOUND, RENEW or REBINDING state and can respond to ARP requests.
    int ciaddr;

    /// 'your' (client) IP address.
    int yiaddr;

    /// IP address of next server to use in bootstrap; returned in DHCPOFFER, DHCPACK by server.
    int siaddr;

    /// Relay agent IP address, used in booting via a relay agent.
    int giaddr;

    /// Client hardware address.
    byte chaddr[16];

    /// Optional server host name, null terminated string.
    char sname[64];

    /// Boot file name, null terminated string; "generic" name or null in DHCPDISCOVER, fully qualified directory-path name in DHCPOFFER.
    char file[128];
};

struct DHCPOption {
    byte type;
    byte data[1];
};

struct DHCPMessage {
    struct DHCPHeader header;
    struct DHCPOption options[1];
};

enum DHCPMessageType {
    discover = 1, offer, request, decline, ack, nak, release, inform
};

enum NETBIOSNodeType {
    bNode = 1, pNode, mMode, hNode
};

enum DHCPOptionType
{
	subnetMask = 1,
	timeOffset = 2,
	router = 3,
	timeServer = 4,
	nameServer = 5,
	domainNameServer = 6,
	domainName = 15,
	broadcastServerOption = 28,
	netbiosNodeType = 46,
	leaseTime = 51,
	dhcpMessageType = 53,
	serverIdentifier = 54,
	parameterRequestList = 55,
	renewalTime = 58,
	rebindingTime = 59,
	vendorClassIdentifier = 60,
	tftpServerName = 66,
	bootfileName = 67,
};

#endif /* DHCP_H_ */
