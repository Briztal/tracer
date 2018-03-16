//
// Created by root on 3/15/18.
//

#ifndef TRACER_COMMUNICATIONPIPIE_H
#define TRACER_COMMUNICATIONPIPIE_H


#include <Interaction/Languages/Language.h>
#include <Interaction/Protocols/Delimiter.h>
#include <HardwareSerial.h>


class CommunicationPipe {

    //--------------------------------------- Initialisation ---------------------------------------

public:

    //Constructor : must take two rvalues in arguments;
    CommunicationPipe(HardwareSerial &serial, Delimiter &&delimiter, Language &&language);

    //Destructor;
    ~CommunicationPipe();


    //--------------------------------------- Processing ---------------------------------------

public:

    //Send a rvalue message over the pipe;
    void send(tstring &message);

    //Read a message comming in the communication interface;
    void readall();


    //--------------------------------------- Data ---------------------------------------

private:

    //The serial that will be used to receive data;
    HardwareSerial serial;

    //The delimiter that will be used to transmit data;
    Delimiter delimiter;

    //The language that will be used to interpret commands;
    Language language;


};


#endif //TRACER_COMMUNICATIONPIPIE_H
