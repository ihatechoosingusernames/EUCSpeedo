#ifndef UIELEMENT_H_
#define UIELEMENT_H_

#include <functional>
#include <vector>

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "processdata.h"
#include "constants.h"
namespace euc {

using ColourProvider = std::function<uint16_t(ProcessData*)>;

#define UI_SETUP(element, code, ...)                                    \
  public:                                                               \
    static UiElement* Builder(uint8_t data[], size_t data_len) {        \
      return (UiElement*) new element(data, data_len);                  \
    };                                                                  \
    ~element() { onDestroy(); };                                        \
    std::vector<ArgType> ArgList() override { return {__VA_ARGS__}; }   \
    String Name() { return #element; }                                  \
    element(uint8_t data[], size_t data_len) {                          \
      assert(data[0] == code);                                          \
      getArgsFromData(data, data_len);                                  \
      onCreate();                                                       \
    };                                                                  \
  private: static bool registered;

#define UI_ARGS(arg, ...)                                               \
  public: std::vector<String> ArgNames() override { return {arg, __VA_ARGS__}; }

#define UI_REGISTER(element, code)                                      \
  bool element::registered = euc::UiElement::RegisterElement<element, code>();

class UiElement {
  public:
    static UiElement* Factory(uint8_t data[], size_t data_len);

    size_t DataSize();  // Returns the number of bytes the element has used from the arg list
    std::vector<DataType> DataTypeArgs(); // Returns all data types used by the element to help with config generation

    virtual ~UiElement() {}; // Declaring virtual destructor to allow safe "delete" calls on base classes

    virtual void onCreate() {};  // To allow setup and cleanup of UI elements
    virtual void onDestroy() {};
    
    virtual void Draw(ProcessData* data, TFT_eSprite* sprite) = 0;   // Draws the element
    
    virtual std::vector<String> ArgNames();   // Returns optional names of the args to help with config generation
    virtual std::vector<ArgType> ArgList() = 0; // Returns the number and type of args to help with config generation
    virtual String Name() = 0; // Returns the element name to help with config generation
  
  protected:
    // Registers each UI element and stores it in the lookup array
    template <class T, uint8_t code>
    static bool RegisterElement() {
      ui_element_lookup[code] = T::Builder;
      ui_element_codes[code] = code;
      return true;
    }

    void getArgsFromData(uint8_t data[], size_t data_len);

    size_t data_size = 1;

    std::vector<DataType> kDataType_args;
    std::vector<ColourProvider> kColour_args;
    std::vector<uint8_t> kConstant_args;
    std::vector<String> kText_args;
  
  private:
    // Functions to decode argument data and assign the arguments 
    static ColourProvider getColourProvider(uint8_t data[], size_t data_len, size_t* bytes_used);
    static DataType getDataType(uint8_t data[], size_t data_len, size_t* bytes_used);
    static uint8_t getConstant(uint8_t data[], size_t data_len, size_t* bytes_used);

    // A function pointer array, points to an initialisation function for each registered UI element
    static UiElement* (*ui_element_lookup[kMaxUiElementCode])(uint8_t data[], size_t data_len);
    // Stores which UI element codes have been registered
    static uint8_t ui_element_codes[kMaxUiElementCode];
};

}

#endif