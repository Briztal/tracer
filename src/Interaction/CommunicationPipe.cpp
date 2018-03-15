//
// Created by root on 3/15/18.
//

#include "CommunicationPipe.h"


//--------------------------------------- Initialisation ---------------------------------------

/*
 * Constructor : takes two rvalues and moves them in the class;
 */

CommunicationPipe::CommunicationPipe(Language &&language, Protocol &&protocol) :
        language((Language&&)language), protocol((Protocol&&)protocol) {}


/*
 * Destructor : no dynamic data to delete;
 */

CommunicationPipe::~CommunicationPipe() {}


//--------------------------------------- Processing ---------------------------------------

/*
 * send : receives a message from inside the code, and sends it through the pipe;
 */

void CommunicationPipe::send(tstring &&message) {

    //First, we must use the language instance to encode the message;
    language.encode((tstring&)message);

    //Then, we have to add one layer of encoding, using the encoder;
    protocol.encode_data(message);

    //Finally, we can extract the data and send it.
    //TODO

}

void CommunicationPipe::read() {

}
