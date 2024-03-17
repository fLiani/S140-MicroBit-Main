#include "MicroBit.h"

MicroBit uBit;

int main()
{
    uBit.init();
    while(1)
    {
        uBit.sleep(1000);
        uBit.serial.printf("hello");
    }
}
