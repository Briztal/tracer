//
// Created by root on 3/15/18.
//

#include "CommunicationPipe.h"

#include "Kernel/Scheduler/TaskScheduler/TaskScheduler.h"
#include "Interaction.h"

//--------------------------------------- Initialisation ---------------------------------------

/*
 * Constructor : takes two rvalues and moves them in the class;
 */

CommunicationPipe::CommunicationPipe(usb_serial_class &serial, Delimiter *protocol, Language *language) :
       serial(serial), delimiter(protocol),  language(language) {}


/*
 * Destructor : delete the language and the delimiter;
 */

CommunicationPipe::~CommunicationPipe() {
    delete language;
    delete delimiter;
}

//--------------------------------------- Initialisation ---------------------------------------

/*
 * hardware init : initialises the serial;
 */
void CommunicationPipe::initialise_hardware() {

    serial.begin(115200);

    serial.println("SERIAL ECHO");

    std_out("INITIALISED");


}

//--------------------------------------- Processing ---------------------------------------

/*
 * send : receives a message from inside the code, and sends it through the pipe;
 */

void CommunicationPipe::send(tstring &message, uint8_t type) {

    //First, we must use the language instance to encode the message;
    language->encode(message, type);

    //Then, we have to add one layer of encoding, using the encoder;
    delimiter->encode(message);

    //Then, we can extract the data;
    const char *data = message.data();

    //Finally, we can send the extracted data;
    Serial.print(data);

}


/*
 * read : this function will readall the hardware if until no char is available anymore;
 */

void CommunicationPipe::readall() {

    //While some data is available :
    while(serial.available()) {

        //Read the data and pass it to the decoder;
        delimiter->process((char)serial.read());


        //If the decoder has successfully decoded a message :
        if (delimiter->isMessageDecoded()) {

            //Parse the decoded message;
            language->decode(delimiter->getData());

            //Reset the delimiter buffer;
            delimiter->clear();

        }

    }

}
