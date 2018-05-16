#ifndef __GHBUTTONSET_H__
#define __GHBUTTONSET_H__

#include <Arduino.h>
#include <ButtonSet.h>
#include <ghobject.h>

#define DEFAULT_BOUNCE_TIME 75


class GHButtonSet: public GHObject {
private:
    const int m_resistor_values[4] = { 238, 385, 485, 557 };
    ButtonSet m_bset;
public:
    GHButtonSet(const int pin);
    ButtonSet &buttons();
};


#endif //__GHBUTTONSET_H__
