#ifndef UIELEMENT_H_
#define UIELEMENT_H_

#include <Arduino.h>

namespace euc {

#define UI_SETUP(element) \
  public: static UiElement* Builder(uint8_t data[], size_t data_len) {  \
      return (UiElement*) new element(data, data_len);  \
    };  \
  private: static bool registered;

#define UI_REGISTER(element, code)  \
  bool element::registered = euc::UiElement::RegisterElement<element, code>();

class UiElement {
  public:
    static UiElement* Factory(uint8_t data[], size_t data_len);

    virtual void Draw();  // Draws the element
    virtual size_t DataSize();  // Returns the number of bytes the element has used from the arg list
  
  protected:
    // Registers each UI element and stores it in the lookup array
    template <class T, uint8_t code>
    static bool RegisterElement() {
      ui_element_lookup[code] = T::Builder;
      ui_element_codes[code] = code;
      return true;
    }
  
  private:
    // A function pointer array, points to an initialisation function for each UI element
    static UiElement* (*ui_element_lookup[256])(uint8_t data[], size_t data_len);
    // Stores which UI element codes have been registered
    static uint8_t ui_element_codes[256];
};

}

#endif