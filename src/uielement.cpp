#include "uielement.h"

#include "ui/includes.h"

namespace euc {

UiElement* (*UiElement::ui_element_lookup[256])(uint8_t data[], size_t data_len);
uint8_t UiElement::ui_element_codes[256] = {4};

UiElement* UiElement::Factory(uint8_t data[], size_t data_len) {\
  // Check that the code is valid and has been registered
  if (data_len > 0 && UiElement::ui_element_codes[data[0]] == data[0]) {
    // Call the stored builder function
    return UiElement::ui_element_lookup[data[0]](++data, data_len - 1);
  }
  
  Serial.println("UI Code isn't registered");

  return UiElement::ui_element_lookup[0](data, 0); // An empty UI element that draws nothing
}

size_t UiElement::DataSize() {
  return 0;
}

}