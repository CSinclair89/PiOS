#include "timer.h"

#define COUNT_REG 0x3f003004

// System timer count registration
unsigned long getTimerCount() {
        unsigned long *timerCountReg = (unsigned long *)COUNT_REG;
        return *timerCountReg;
}

// Method to wait for a few seconds
void waitFor(unsigned long toc) {
        unsigned long tic = getTimerCount();
        toc = tic + toc;

        while (getTimerCount() < toc) {}
}
