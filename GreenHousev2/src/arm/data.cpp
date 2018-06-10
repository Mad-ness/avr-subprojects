#include <list>
#include <string.h>
#include <time.h>
#include <ghairdefs.h>
#ifdef DEBUG
#include <stdio.h>
#include <iostream>
#endif
#include <data.h>


DataCollector::~DataCollector() {
    for (auto it = m_requests.begin(); it != m_requests.end(); ++it) {
    }
}
void DataCollector::addRequest( const HttpRequest_t &request ) {
            m_requests.push_back(HttpRequest_t(request));
}
void DataCollector::updateWithResponse(const char *id, AirPacket *pkt) {
            for (auto it = m_requests.begin(); it != m_requests.end(); ++it) {
                //memcpy(&(it->packet), pkt, sizeof(AirPacket));
            }
}
void DataCollector::removeRequest( const char *id ) {

}
void DataCollector::printContent() {
#ifdef DEBUG
            int i = 0;
	    std::cout << "Total requests: " << m_requests.size() << std::endl;
            for (auto it = m_requests.cbegin(); it != m_requests.cend(); ++it) {
                std::cout << ++i << ". " 
                          << "id=" << it->id << " " 
                          << "func=" << (int)it->packet.command << ", "
			  << "addr=" << (int)it->packet.address << ", "
                          << "len=" << (int)it->packet.length << ", "
                          << "data=" << it->packet.data;
            }
            if ( m_requests.cbegin() == m_requests.cend() ) {
                fprintf(stdout, "The queue is empty.\n");
            }
#endif
};

HttpRequest_t *DataCollector::byCommand(const uint8_t cmd) {
    for ( auto it = m_requests.begin(); it != m_requests.end(); it++ ) {
        if ( it->packet.getCommand() == ( cmd & 0x1F ) ) { // ignore the highest, bit 7
            return &(*it); 
        }
    }
    return NULL;
}

