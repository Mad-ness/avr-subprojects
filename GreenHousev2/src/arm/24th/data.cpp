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
void DataCollector::addRequest( const HttpRequest_t *request ) {
            m_requests.push_back(HttpRequest_t(*request));
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
                          << "func=" << it->packet.command << std::endl;
                //fprintf(stdout, "%03d. id=%s, func=0x%02x, addr=0x%02x, len=0x%02x\n", ++i, it->packet.command, it->packet.address, it->packet.length);
            }
            if ( m_requests.cbegin() == m_requests.cend() ) {
                fprintf(stdout, "The queue is empty.\n");
            }
            fflush(stdout);
#endif
};

