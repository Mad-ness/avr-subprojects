#include <time.h>
#include "packetmanager.h"

void UserPacket::init() {
    client_id = "";
    packet_id = 0;
    when.request_received = 0;
    when.request_senttoboard = 0;
    when.board_responded = 0;
    used_attempts = 0;
	std::cout << "Constructor1 >>> " << str() << std::endl;
}

UserPacket::UserPacket(const string c_id, const AirPacket &pkt) {
    init();
    std::cout << "Constructor2 >>> " << str() << std::endl;
    client_id = c_id;
    rf24packet = pkt;
    std::cout << "Constructor3 >>> " << str() << std::endl;
    memcpy(&rf24packet, &pkt, sizeof(pkt));
    std::cout << "Constructor4 >>> " << str() << std::endl;
    when.request_received = time(NULL);
	std::cout << "Constructor5 >>> " << str() << std::endl;
}


uint32_t UserPacket::packetId() {
	return packet_id;
}

void UserPacket::markAsSentOut(void) {
    has_sent = true;
}

AirPacket &UserPacket::radiopacket() {
    return rf24packet;
}

bool UserPacket::operator==(const UserPacket &pkt) {
    return (
        ( packet_id == pkt.packet_id ) &&
        ( client_id == pkt.client_id )
    );
}

/**
 * Returns a printable string about the packet itself.
 */ 
string UserPacket::str(void) {
    string s;
    s += "ClientId: "; 
    s += client_id;
    s += ", PacketId: ";
    s += to_string(packet_id);
    s += ", PacketSent: ";
    s += has_sent ? "yes" : "no";
    s += ", UsedAttempts: ";
    s += to_string(used_attempts);
    s += ", AirPacket: { cmd: ";
    s += to_string(rf24packet.command);
    s += ", addr: ";
    s += to_string(rf24packet.address);
    s += ", len: ";
    s += to_string(rf24packet.length);
    s += " }";
    s += ", UserRequested: ";
    s += to_string(when.request_received);
    s += ", SentToBoard: ";
    s += to_string(when.request_senttoboard);
    s += ", BoardResponded: ";
    s += to_string(when.board_responded);
    return s;
}

bool UserPacket::isResponse() {
    return rf24packet.isResponse();
}

void UserPacket::replacePacket(const AirPacket &pkt) {
    memcpy(&rf24packet, &pkt, sizeof(pkt));
}

uint32_t UserPacket::usedAttempts() {
    return used_attempts;
}

packet_time_t &UserPacket::timeOfLastAttempt() {
	return when.request_senttoboard;
}
packet_time_t &UserPacket::timeAddInQueue() {
	return when.request_received;
}

void UserPacket::setPacketId(const uint32_t &id) {
	packet_id = id;
	std::cout << "Increased packet id: " << packet_id << std::endl;
}



/*****************************************************************************/

void PacketManager::incrementPacketId(UserPacket &pkt) {
	pkt.setPacketId(++curr_packet_id);
	std::cout << "Incremented packet index to " << curr_packet_id << std::endl;
}

void PacketManager::addRequest(const UserPacket &packet) {
    m_packets.push_back(UserPacket(packet));
	incrementPacketId(m_packets.back());
}

void PacketManager::print() {
    int i = 0;
    for ( auto it = m_packets.begin(); it != m_packets.end(); it++ ) {
        std::cout << ++i << ". " << it->str() << std::endl;
    }
}

bool PacketManager::updateWithResponse(AirPacket *pkt) {
    for ( auto it = m_packets.begin(); it != m_packets.end(); it++ ) {
        if ( ! it->radiopacket().isResponse() &&
             it->radiopacket().getCommand() == pkt->getCommand() &&
             it->radiopacket().address == pkt->address ) {
            it->replacePacket(*pkt);
            return true;
        }
    }
    return false;
}

/**
 * Selects the packet for sending to the remote board.
 * If the result == True, a proper packet has been selected
 * and the function returns a pointer at that packet.
 */
UserPacket &PacketManager::nextPacket(bool *result) {
    UserPacket *pkt = NULL;	
	*result = false;
	if ( m_packets.size() > 0 ) { 
		*result = true;
		pkt = &(*m_packets.begin());
		auto it = m_packets.begin();
		it++;
        for ( it; it != m_packets.end(); it++ ) {
            if ( ! it->isResponse() ) {
                    // Choose the packet with 
                    // less number of performed attempts
                if ( it->timeAddInQueue() < pkt->timeAddInQueue() &&
        			 it->usedAttempts() >= pkt->usedAttempts() &&
                     it->timeOfLastAttempt() <= pkt->timeOfLastAttempt() )
                {
                    pkt = &(*it);
                }
            }
        }
	}
	return *pkt;
}

