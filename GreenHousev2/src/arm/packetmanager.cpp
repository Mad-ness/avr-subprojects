#include <algorithm>
#include <time.h>
#include "packetmanager.h"

void UserPacket::init() {
    client_id = "";
    packet_id = 0;
    when.request_received = 0;
    when.request_senttoboard = 0;
    when.board_responded = 0;
    used_attempts = 0;
}

UserPacket::UserPacket(const string c_id, const AirPacket &pkt) {
    init();
    client_id = c_id;
    rf24packet = pkt;
    memcpy(&rf24packet, &pkt, sizeof(pkt));
    when.request_received = time(NULL);
}


uint32_t UserPacket::packetId() {
	return packet_id;
}

void UserPacket::attemptedToSend(bool with_success) {
    when.request_senttoboard = time(NULL);
    used_attempts++;
    if ( with_success ) {
        has_sent = true;
    }
}

AirPacket &UserPacket::airpacket() {
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
    s += has_sent == true ? "yes" : "no";
    s += ", UsedAttempts: ";
    s += to_string(used_attempts);
    s += ", AirPacket: { cmd: ";
    s += to_string(rf24packet.getCommand());
    s += ", isResponse: ";
    s += rf24packet.isResponse() == true ? "yes" : "no";
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

void UserPacket::replacePacket(const AirPacket &pkt) {
    memcpy(&rf24packet, &pkt, sizeof(pkt));
    when.board_responded = time(NULL);
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

bool UserPacket::hasShipped() {
    return has_sent;
}

packet_time_t UserPacket::getSentTime() {
    return when.request_senttoboard;
}

string UserPacket::clientId() {
    return client_id;
}

void UserPacket::setClientId(const string id) {
    client_id = id;
}


/*****************************************************************************/

void PacketManager::incrementPacketId(UserPacket &pkt) {
	pkt.setPacketId(++curr_packet_id);
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

bool matchAirPacketResponse(AirPacket &origin, AirPacket &compare_to) {
    return (
             ( ! origin.isResponse() ) &&
             ( origin.getCommand() == compare_to.getCommand() ) &&
             ( origin.address == compare_to.address )     
           );
}

/**
 * Fills the initial packet with the answer from the remote board.
 * It returns True if the origin has been found.
 * It also iterates over all queue and fills the packets, which have the
 * same set of attributes, with the answer making them not needed to be 
 * requested from the remote board. Preventing sending of duplicate
 * requests.
 */
bool PacketManager::updateWithResponse(AirPacket &pkt) {


    bool result = false;
    for ( auto it = m_packets.begin(); it != m_packets.end(); it++ ) {

        if ( matchAirPacketResponse(it->airpacket(), pkt) ) {
            if ( it->getSentTime() != 0 ) {

                it->replacePacket(pkt);
                packet_time_t when_sent = it->getSentTime();
                result = true;

                // iterate over other packets which have the same attributes
                // and its headers saving personal information
                for ( auto it2 = m_packets.begin(); it2 != m_packets.end(); it2++ ) {
                    if ( matchAirPacketResponse(it->airpacket(), pkt) && it2->getSentTime() == 0 ) {
                        const uint32_t packet_id = it2->packetId();
                        const string client_id = it2->clientId();
                        it2 = it;
                        it2->setPacketId(packet_id);
                        it2->setClientId(client_id);
                    }
                }
                break;

            } 
            // not break the loop since there maybe other packets 
            // with the same set of attributes, so update them as well
#if defined(DEBUG)
            std::cout << "Updated with response: " << it->str() << std::endl;
#endif
        }
    }
    return result;
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
		// *result = true;
		// pkt = &(*m_packets.begin());
        for ( auto it = m_packets.begin(); it != m_packets.end(); it++ ) {
            if ( ! it->airpacket().isResponse() && ! it->hasShipped() ) {
                
                if ( pkt == NULL ) {
                    pkt = &(*it);
                    // Choose the packet with 
                    // less number of performed attempts
                } else if ( it->timeAddInQueue() < pkt->timeAddInQueue() &&
        			 it->usedAttempts() >= pkt->usedAttempts() &&
                     it->timeOfLastAttempt() <= pkt->timeOfLastAttempt() )
                {
                    pkt = &(*it);
                }
                *result = true;
            }
        }
	}
	return *pkt;
}

