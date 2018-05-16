#include <Arduino.h>
#include <manager.h>
#include <ghdisplay.h>

Manager manager;

void setup(void) {
    manager.setup();
    manager.display().showWelcomePage();
}

void loop(void) {
    manager.loop();
}
