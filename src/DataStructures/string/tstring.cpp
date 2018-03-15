//
// Created by root on 3/14/18.
//

#include "tstring.h"


//----------------------------------- Constructors -----------------------------------

/*
 * Constructor : initialises the container, to contain at most 10 strings;
 */

tstring::tstring() : PointerContainer(255), string_generated(false), size(1), final_data(nullptr) {}


/*
 * Constructor : initialises the container, to contain at most 10 strings;
 */

tstring::tstring(uint8_t max_strings) : PointerContainer(max_strings), string_generated(false), size(1),
                                        final_data(nullptr) {}


/*
 * Constructor : takes a string;
 */

tstring::tstring(const char *buffer) : tstring() {

    add(new string(buffer));

}


/*
 * Constructor with string lvalue : copies the string and adds it;
 */

tstring::tstring(const string &s) : tstring() {

    add(new string(s));

}


/*
 * Constructor with string rvalue : moves data to a new one and adds it;
 */

tstring::tstring(const string &&s) : tstring() {

    add(new string((string &&) s));
}


/*
 * Destructor : all strings will be deleted automatically by the PointerContainer's destructor;
 */

tstring::~tstring() {

    delete[] final_data;

}


/*
 * clear : resets all data to initial state;
 */

void tstring::clear() {

    //Clear the content;
    PointerContainer::clear();

    //Delete the data;
    deleteData();

}


//------------------------------------------- Copy Constructors -------------------------------------------


/*
 * Copy constructor : this function will duplicate the tstring;
 */

tstring::tstring(const tstring &src) : PointerContainer(src), string_generated(src.string_generated), size(src.size),
                                       final_data(src.final_data) {}


/*
 * Move constructor : this function will move dynamic data to us;
 */

tstring::tstring(tstring &&src) : PointerContainer((tstring &&) src), string_generated(src.string_generated),
                                  size(src.size), final_data(src.final_data) {

    //Reset src's data;
    src.final_data = nullptr;
    src.string_generated = false;
    src.size = 0;

}


//------------------------------------------- Assignment operators -------------------------------------------


/*
 * copy assignment operator : will duplicate the tstring;
 */

tstring &tstring::operator=(const tstring &src) {

    //If the container is us, do nothing;
    if (&src == this) {
        return *this;
    }

    //Create a copy;
    tstring copy = tstring(src);

    //Swap, to that our data is deleted at the end of copy's scope;
    swap(*this, copy);

    //Return a reference to us;
    return *this;

}


/*
 * move assignment operator : will move dynamic data to us;
 */

tstring &tstring::operator=(tstring &&src) {

    //Move the parent container;
    PointerContainer::operator=((tstring &&) src);

    //Swap, to that our data is deleted at the end of container's scope;
    swap(*this, src);

    //Return a reference to us;
    return *this;
}


/*
 * Swap : this function will swap content of the two containers;
 */

void tstring::swap(tstring &a, tstring &b) {

    //Swap max sizes;
    bool tb = a.string_generated;
    a.string_generated = b.string_generated;
    b.string_generated = tb;

    //Swap sizes;
    uint16_t ts = a.size;
    a.size = b.size;
    b.size = ts;

    //Swap contents;
    char *ptr = a.final_data;
    a.final_data = b.final_data;
    b.final_data = ptr;

}



//----------------------------------- Concatenation operators -----------------------------------


/*
 * Concatenation : this operator takes an lvalue, whose content must be copied and added to the vector;
 */

tstring &tstring::operator+=(const string &src) {

    //First, if data has been generated, delete it;
    deleteData();

    //Increment the size of the string's length;
    size += src.length();

    //Create a heap-copy of the string;
    string *s = new string(src);

    //Increment the size of the string's length;
    size += src.length();

    //Add the string to the container;
    PointerContainer::add(s);
    
    //Return a reference to us;
    return *this;

}


/*
 * Concatenation : this operator takes an rvalue, whose content can be swapped to a string added to the vector;
 */

tstring &tstring::operator+=(string &&src) {

    //First, if data has been generated, delete it;
    deleteData();

    //Move constructor;
    string *s = new string(src);

    //Add the moved string to the container;
    PointerContainer::add(s);
    
    //Return a reference to us;
    return *this;

}


/*
 * Concatenation operator for tstring rvalue;
 */

tstring &tstring::operator+=(tstring &&src) {

    //Concatenate the tree on the right;
    rightConcatenation((tstring &&) src);

    //Return a reference to us;
    return *this;

}


/*
 * rightConcatenation : this function will concatenate the tstring on our right;
 */

void tstring::rightConcatenation(tstring &&src) {

    //First, delete all temp data;
    deleteData();
    src.deleteData();

    //Concatenate data; if it succeeds :
    if (PointerContainer::rightConcatenation((tstring &&) src)) {

        //Update our size;
        size += src.size - (uint16_t) 1;

    }


}


/*
 * rightConcatenation : this function will concatenate the tstring on our left;
 */

void tstring::leftConcatenation(tstring &&src) {

    //First, delete all temp data;
    deleteData();
    src.deleteData();

    //Concatenate data; if it succeeds :
    if (PointerContainer::leftConcatenation((tstring &&) src)) {

        //Update our size;
        size += src.size - (uint16_t) 1;

    }

}




//------------------------------------------- Data Getter -------------------------------------------

/*
 * data : this function will eventually generate the string data, and return a pointer to it;
 */

const char *tstring::data() {

    //If the string data has not been generated :
    if (!string_generated) {

        //Generate the string data;
        generateData();

    }

    //Return the generated data;
    return final_data;
}


//------------------------------------------- string Data -------------------------------------------

/*
 * deleteData : this function will delete the generated string, if it has been generated;
 */

void tstring::deleteData() {

    //If the string has been generated;
    if (string_generated) {

        //Delete the string;
        delete[] final_data;

        //Mark the string as not generated;
        string_generated = false;
    }

}


/*
 * generateData : this function will generate the final string;
 */

void tstring::generateData() {

    //Cache the size;
    const uint16_t size = this->size;

    //Create an array that will contain the final string;
    final_data = new char[size];

    //Cache the data pointer;
    char *dptr = final_data;

    //For each string :
    for (uint8_t element_index = 0; element_index < size; element_index++) {

        //Cache the data pointer of the string;
        const char *string_pointer = getElement(element_index)->data();

        //The current char;
        char c;

        //While the current char is not '\0' :
        while ((c = *string_pointer)) {

            //Copy the current char at the current case and increment the data pointer;
            *(dptr++) = c;

        }

    }

    //Null terminate the string;
    *dptr = 0;

}


//----------------------------------- External operators -----------------------------------


/*
 * rvalue add, with lvalue string;
 */

tstring &&operator+(tstring &&s0, const string &s) {

    //Add s to s0 (lvalue, so copy will be made);
    s0 += s;

    //return a pointer to the tstring;
    return (tstring &&) s0;

}


/*
 * rvalue add, with rvalue string;
 */

tstring &&operator+(tstring &&s0, string &&s) {

    //Add s to s0 (rvalue, no copy will be made);
    //return a pointer to the tstring;
    return (tstring &&) (s0 += s);

}


/*
 * rvalue add, with rvalue string;
 */

tstring &&operator+(string &&i, tstring &&t) {

    return (tstring &&) (t += i);

}
