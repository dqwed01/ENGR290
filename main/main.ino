/* Lift and servo bench test. Open Serial Monitor at 115200 baud. */
#include "src/hovercraft.h"

int main() {
    hovercraft_init();
    while (1) {
        hovercraft_update();
    }
}
