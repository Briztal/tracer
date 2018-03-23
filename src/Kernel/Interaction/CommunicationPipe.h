//
// Created by root on 3/15/18.
//

#ifndef TRACER_COMMUNICATIONPIPIE_H
#define TRACER_COMMUNICATIONPIPIE_H


#include <Kernel/Interaction/Language/Language.h>
#include <Kernel/Interaction/Delimiter/Delimiter.h>
#include <HardwareSerial.h>

#include "hardware_language_abstraction.h"



class CommunicationPipe {

    //--------------------------------------- Instantiation ---------------------------------------

public:

    //Constructor : must take two rvalues in arguments;
    CommunicationPipe(usb_serial_class &serial, Delimiter *delimiter, Language *language);

    //Destructor;
    ~CommunicationPipe();


    //--------------------------------------- Initialisation ---------------------------------------

    //Initialise the hardware serial;
    void initialise_hardware();


    //--------------------------------------- Processing ---------------------------------------

public:

    //Send a rvalue message over the pipe;
    void send(tstring &message, uint8_t type);

    //Read a message comming in the communication interface;
    void readall();


    //--------------------------------------- Data ---------------------------------------

private:

    //The serial that will be used to receive data;
    usb_serial_class serial;

    //The delimiter that will be used to transmit data;
    Delimiter *delimiter;

    //The language that will be used to interpret commands;
    Language *language;


};


#endif //TRACER_COMMUNICATIONPIPIE_H
