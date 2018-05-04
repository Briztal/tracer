#include <Kernel/DriverManager.h>

#include "teensy35_arch.h"

#include "UART.h"

/*
 * initialise_peripheral_manager : will declare every supported peripheral in the resource manager;
 */

void teensy35::initialise_peripheral_manager() {

    PeripheralManager::reset();

#ifdef SUPPORT_UART0
    PeripheralManager::addPeripheral(UART0, UART0Driver);
#endif

#ifdef SUPPORT_UART1
    PeripheralManager::addPeripheral(UART1, UART1Driver);
#endif

#ifdef SUPPORT_UART2
    PeripheralManager::addPeripheral(UART2, UART2Driver);
#endif

#ifdef SUPPORT_UART3
    PeripheralManager::addPeripheral(UART3, UART3Driver);
#endif

#ifdef SUPPORT_UART4
    PeripheralManager::addPeripheral(UART4, UART4Driver);
#endif

#ifdef SUPPORT_UART5
    PeripheralManager::addPeripheral(UART5, UART5Driver);
#endif


}