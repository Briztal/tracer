cmake_minimum_required(VERSION 3.2)
project(TRACER)
set(ENV{PATH} "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin")
set(PLATFORMIO_CMD "/usr/local/bin/platformio")

SET(CMAKE_C_COMPILER "/root/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gcc")
SET(CMAKE_CXX_COMPILER "/root/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++")
SET(CMAKE_CXX_FLAGS_DISTRIBUTION "-fno-exceptions -felide-constructors -fno-rtti -std=gnu++14 -Os -Wall -ffunction-sections -fdata-sections -mthumb -mcpu=cortex-m4 -nostdlib -fsingle-precision-constant -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DF_CPU=120000000L -DLAYOUT_US_ENGLISH -DPLATFORMIO=30503 -D__MK64FX512__ -DTEENSY35 -DUSB_SERIAL -DARDUINO=10805 -DTEENSYDUINO=141")
SET(CMAKE_C_FLAGS_DISTRIBUTION "-Os -Wall -ffunction-sections -fdata-sections -mthumb -mcpu=cortex-m4 -nostdlib -fsingle-precision-constant -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DF_CPU=120000000L -DLAYOUT_US_ENGLISH -DPLATFORMIO=30503 -D__MK64FX512__ -DTEENSY35 -DUSB_SERIAL -DARDUINO=10805 -DTEENSYDUINO=141")
set(CMAKE_CXX_STANDARD 11)

add_definitions(-D'F_CPU=120000000L')
add_definitions(-D'LAYOUT_US_ENGLISH')
add_definitions(-D'PLATFORMIO=30503')
add_definitions(-D'__MK64FX512__')
add_definitions(-D'TEENSY35')
add_definitions(-D'USB_SERIAL')
add_definitions(-D'ARDUINO=10805')
add_definitions(-D'TEENSYDUINO=141')

include_directories("$ENV{HOME}/Projects/TRACER/src/hard/chip/kinetis/periph")
include_directories("$ENV{HOME}/Projects/TRACER/src/std")
include_directories("$ENV{HOME}/Projects/TRACER/src")

#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/cores/teensy3")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/ADC")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/AccelStepper/src")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_CC3000")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_CC3000/utility")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_GFX")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_ILI9340")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_ILI9341")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_NeoPixel")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_RA8875")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_SSD1306")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_ST7735")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_STMPE610")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_VS1053")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_nRF8001")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_nRF8001/utility")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/AltSoftSerial")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Artnet")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Audio")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Audio/utility")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Bounce")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Bounce2")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/CapacitiveSensor")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/CryptoAccel/src")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/DS1307RTC")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/DmxSimple")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/DogLcd")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/EEPROM")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/EasyTransfer")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/EasyTransferI2C")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Encoder")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Encoder/utility")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Entropy")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Ethernet")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/FastCRC")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/FastLED")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/FlexCAN")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/FlexiTimer2")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/FreqCount")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/FreqMeasure")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/FreqMeasureMulti")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/FrequencyTimer2")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/ILI9341_t3")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/IRremote")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Keypad/src")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/LedControl/src")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/LedDisplay")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/LiquidCrystal/src")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/LiquidCrystalFast")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/LowPower")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/MFRC522/src")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/MIDI/src")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Metro")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/MsTimer2")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/NXPMotionSense")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/NXPMotionSense/utility")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/OSC")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/OctoWS2811")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/OneWire")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/PS2Keyboard")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/PS2Keyboard/utility")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/PWMServo")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Ping")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/PulsePosition")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/RA8875")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/RadioHead")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/ResponsiveAnalogRead/src")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/SD")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/SD/utility")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/SPI")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/SPIFlash")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/ST7565")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/ST7735_t3")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/SerialFlash")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Servo")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/ShiftPWM")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Snooze")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Snooze/utility")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/SoftPWM")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/SoftwareSerial")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/TFT_ILI9163C")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Talkie")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/TeensyThreads")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Time")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/TimeAlarms")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/TimerOne")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/TimerThree")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/TinyGPS")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Tlc5940")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/TouchScreen")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/USBHost_t36")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/UTFT")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/VirtualWire")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/WS2812Serial")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Wire")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Wire/utility")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/XBee")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/XPT2046_Touchscreen")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/i2c_t3")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/ks0108")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/openGLCD")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/ssd1351")
#include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/x10")
#include_directories("$ENV{HOME}/.platformio/packages/toolchain-gccarmnoneeabi/arm-none-eabi/include")
#include_directories("$ENV{HOME}/.platformio/packages/toolchain-gccarmnoneeabi/arm-none-eabi/include/c++/5.4.1")
#include_directories("$ENV{HOME}/.platformio/packages/toolchain-gccarmnoneeabi/arm-none-eabi/include/c++/5.4.1/arm-none-eabi")
#include_directories("$ENV{HOME}/.platformio/packages/toolchain-gccarmnoneeabi/lib/gcc/arm-none-eabi/5.4.1/include")
#include_directories("$ENV{HOME}/.platformio/packages/toolchain-gccarmnoneeabi/lib/gcc/arm-none-eabi/5.4.1/include-fixed")
#include_directories("$ENV{HOME}/.platformio/packages/tool-unity")

FILE(GLOB_RECURSE SRC_LIST "/root/Projects/TRACER/src/*.*" "/root/Projects/TRACER/lib/*.*" "/root/Projects/TRACER/.piolibdeps/*.*")



add_custom_target(
    PLATFORMIO_BUILD ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_UPLOAD ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run --target upload
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_CLEAN ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run --target clean
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_TEST ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_PROGRAM ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run --target program
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_UPLOADFS ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run --target uploadfs
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_UPDATE_ALL ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion update
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_REBUILD_PROJECT_INDEX ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion init --ide clion
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${PROJECT_NAME} ${SRC_LIST} )