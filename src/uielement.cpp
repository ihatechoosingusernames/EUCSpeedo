#include "uielement.h"

#include "ui/includes.h"

namespace euc {

UiElement* (*UiElement::ui_element_lookup[kMaxUiElementCode])(uint8_t data[], size_t data_len);
uint8_t UiElement::ui_element_codes[kMaxUiElementCode] = {4};

UiElement* UiElement::Factory(uint8_t data[], size_t data_len) {\
  // Check that the code is valid and has been registered
  if (data_len > 0 && UiElement::ui_element_codes[data[0]] == data[0]) {
    // Call the stored builder function
    Serial.printf("Factory recognised code %X\n", data[0]);
    return UiElement::ui_element_lookup[data[0]](data, data_len);
  }

  return UiElement::ui_element_lookup[0](data, 0); // An empty UI element that draws nothing
}

size_t UiElement::DataSize() {
  return data_size;
}

std::vector<String> UiElement::ArgNames() {
  return {};
}

ColourProvider UiElement::getColourProvider(uint8_t data[], size_t data_len, size_t* bytes_used) {
  if (!data_len || *bytes_used >= data_len) {  // No Data provided, default is to return #000000 (black)
    *bytes_used += 0;
    return [](ProcessData* data){ return 0; };
  }

  // Make sure that the value of bytes_used is subtracted from the data we have
  data_len -= *bytes_used;
  data += *bytes_used;

  switch ((ColourType)data[0]) {

    // A basic, constant colour
    case ColourType::kConstant:
      if (data_len >= 4) {
        // The following 3 bytes should be the RGB values
        *bytes_used += 4;
        uint16_t colour = (uint16_t)((data[3] << 11) | (data[2] << 5) | data[1]); // Screen uses BGR format
        printf("kConstant Colour provider providing %x\n", colour);
        return [=](ProcessData* data){ return colour; };
      }
      break;

    // Colour changes based on min and max value
    case ColourType::kDynamicBetweenValues:
      if (data_len >= 10) {
        // The first byte should be a data type
        DataType type = (DataType)data[1];
        // Next byte is a low value for the data type
        int8_t min_val = static_cast<int8_t>(data[2]);
        // Next byte is an high value for the data type
        int8_t max_val = static_cast<int8_t>(data[3]);
        // The next three bytes are the low colour
        uint8_t min_colour[3] = {data[4], data[5], data[6]};
        // The next three bytes are the high colour
        uint8_t max_colour[3] = {data[7], data[8], data[9]};

        *bytes_used += 10;

        return [=](ProcessData* data){
          // Limit the multiple to the predefined range and translate it to a decimal
          double multiple = std::min(std::max(data->getDoubleData(type), (double)min_val), (double)max_val) / (max_val - min_val);
          // Multiply each of R, G and B by that decimal, then add back the min value
          uint16_t red = static_cast<uint16_t>(((max_colour[0] - min_colour[0]) * multiple) + min_colour[0]);
          uint16_t green = static_cast<uint16_t>(((max_colour[1] - min_colour[1]) * multiple) + min_colour[1]);
          uint16_t blue = static_cast<uint16_t>(((max_colour[2] - min_colour[2]) * multiple) + min_colour[2]);

          return (blue << 11) | (green << 5) | red; // Combine to form BGR format used by the tft screen
        };
      }
  }

  // No valid Data provided, default is to return #000000 (black)
  *bytes_used += 1;
  return [](ProcessData* data){ return 0; };
}

}