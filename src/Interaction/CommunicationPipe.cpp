//
// Created by root on 3/15/18.
//

#include "CommunicationPipe.h"


//--------------------------------------- Initialisation ---------------------------------------

/*
 * Constructor : takes two rvalues and moves them in the class;
 */

CommunicationPipe::CommunicationPipe(HardwareSerial &serial, Delimiter &&protocol, Language &&language) :
        language((Language&&)language), delimiter((Delimiter&&)protocol) {}


/*
 * Destructor : no dynamic data to delete;
 */

CommunicationPipe::~CommunicationPipe() = default;


//--------------------------------------- Processing ---------------------------------------

/*
 * send : receives a message from inside the code, and sends it through the pipe;
 */

void CommunicationPipe::send(tstring &message) {

    //First, we must use the language instance to encode the message;
    language.encode(message);

    //Then, we have to add one layer of encoding, using the encoder;
    delimiter.encode(message);

    //Then, we can extract the data;
    const char *data = message.data();

    //Finally, we can send the extracted data;
    serial.print(data);

}


/*
 * read : this function will readall the hardware interface until no char is available anymore;
 */

void CommunicationPipe::readall() {

    //While some data is available :
    while(serial.available()) {

        //Read the data and pass it to the decoder;
        delimiter.decode((char)serial.read());

        //If the decoder has successfully decoded a message :
        if (delimiter.messageDecoded()) {

            //Parse the decoded message;
            language.parse(delimiter.getData());

        }

    }



}
