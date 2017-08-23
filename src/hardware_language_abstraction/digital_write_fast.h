


#define  digital_write_fast(uint8_t pin, uint8_t val)\
        }
    } else {
        if (val) {\
            *portSetRegister(pin) = digitalPinToBitMask(pin);\
        } else {
            *portClearRegister(pin) = digitalPinToBitMask(pin);\
        }
    }
}
