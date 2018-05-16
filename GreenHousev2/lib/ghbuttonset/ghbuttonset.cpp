#include <ghbuttonset.h>

GHButtonSet::GHButtonSet(const int pin)
: m_bset(ButtonSet::createWithValues(pin, m_resistor_values, sizeof(m_resistor_values) / sizeof(int)))
{
//    m_bset = ButtonSet::createWithValues(pin, m_resistor_values, sizeof(m_resistor_values) / sizeof(int));
}

ButtonSet &GHButtonSet::buttons() {
    return this->m_bset;
}

ButtonSet::~ButtonSet() {};

//ButtonSet buttonSet2= ButtonSet::createWithValues(A0, values, sizeof(values) / sizeof(int));
