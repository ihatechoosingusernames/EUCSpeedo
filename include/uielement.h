#ifndef UIELEMENT_H_
#define UIELEMENT_H_

#include <functional>
#include <list>

#include <Arduino.h>

#include "processdata.h"
namespace euc {

using ColourProvider = std::function<uint32_t(ProcessData*)>;

#define UI_SETUP(element, ...)                                          \
  public: static UiElement* Builder(uint8_t data[], size_t data_len) {  \
      return (UiElement*) new element(data, data_len);                  \
    };                                                                  \
    std::list<ArgType> ArgList() { return {__VA_ARGS__}; }              \
    char* Name() { return #element; }                                   \
  private: static bool registered;

#define UI_REGISTER(element, code)  \
  bool element::registered = euc::UiElement::RegisterElement<element, code>();

class UiElement {
  public:
    static UiElement* Factory(uint8_t data[], size_t data_len);
    static size_t getElementSize();

    size_t DataSize();  // Returns the number of bytes the element has used from the arg list

    virtual void Draw(ProcessData* data);   // Draws the element
    virtual std::list<ArgType> ArgList();   // Returns the number and type of args to help with config generation
    virtual char* Name();                   // Returns the name to help with config generation
  
  protected:
    // Registers each UI element and stores it in the lookup array
    template <class T, uint8_t code>
    static bool RegisterElement() {
      ui_element_lookup[code] = T::Builder;
      ui_element_codes[code] = code;
      return true;
    }

    static ColourProvider getColourProvider(uint8_t data[], size_t data_len, size_t* bytes_used);

    size_t data_size = 1;
  
  private:
    // A function pointer array, points to an initialisation function for each registered UI element
    static UiElement* (*ui_element_lookup[kMaxUiElementCode])(uint8_t data[], size_t data_len);
    // Stores which UI element codes have been registered
    static uint8_t ui_element_codes[kMaxUiElementCode];
};

}

#endif