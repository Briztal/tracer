//
// Created by root on 3/15/18.
//

#ifndef TRACER_COMMUNICATIONPIPIE_H
#define TRACER_COMMUNICATIONPIPIE_H


#include <Interaction/Languages/Language.h>
#include <Interaction/Protocols/Protocol.h>


class CommunicationPipe {

    //--------------------------------------- Initialisation ---------------------------------------

    //Constructor : must take two rvalues in arguments;
    CommunicationPipe(Language &&language, Protocol &&protocol);

    //Destructor;
    ~CommunicationPipe();


    //--------------------------------------- Processing ---------------------------------------

    //Send a rvalue message over the pipe;
    void send(tstring &&message);

    //Read a message comming in the communication interface;
    void read();


    //--------------------------------------- Data ---------------------------------------

    //The language that will be used to interpret commands;
    Language language;

    //The protocol that will be used to transmit data;
    Protocol protocol;


};


#endif //TRACER_COMMUNICATIONPIPIE_H
