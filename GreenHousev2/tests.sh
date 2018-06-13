#!/bin/bash

host_uri="http://localhost:8080/test -X POST"

AIR_CMD_UNDEF=0
AIR_CMD_IN_PING=1
AIR_CMD_IN_RESET=2
AIR_CMD_IN_GET_EEPROM=3
AIR_CMD_IN_WRITE_EEPROM=4
AIR_CMD_IN_DATA=5
AIR_CMD_IN_UPTIME=6


function send_to() {
    func="$1"
    curl $host_uri -d '
    {
      "func": '$func' ,
      "addr": 0 ,
      "len": 0 ,
      "data": ""
    }
    '
}

send_to $AIR_CMD_IN_PING
send_to $AIR_CMD_IN_RESET
send_to $AIR_CMD_IN_GET_EEPROM
send_to $AIR_CMD_IN_UPTIME

