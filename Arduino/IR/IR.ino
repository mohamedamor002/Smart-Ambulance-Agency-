#include <IRremote.hpp>

void setup() {
  const byte irReceiverPin = 8;
  IrReceiver.begin(irReceiverPin, ENABLE_LED_FEEDBACK);  // Start the receiver,
  Serial.begin(9600);
}

String commandToKeyName(unsigned int command) {
  switch (command) {
    case 0x19: return "0";
    case 0x45: return "1";
    case 0x46: return "2";
    case 0x47: return "3";
    case 0x44: return "4";
    case 0x40: return "5";
    case 0x43: return "6";
    case 0x7: return "7";
    case 0x15: return "8";
    case 0x9: return "9";
    case 0x16: return "*";
    case 0xD: return "#";
    case 0x18: return "^";
    case 0x5A: return ">";
    case 0x52: return "v";
    case 0x8: return "<";
    case 0x1C: return "OK";
    case 0x0: return "NONE";
    default: return "UNKNOWN";
  }
}

void loop() {
  if (IrReceiver.decode()) {
    if (Serial.availableForWrite()) {
      const String buttonName = commandToKeyName(IrReceiver.decodedIRData.command);
      Serial.println(buttonName);
    }
    // IrReceiver.printIRResultShort(&Serial);  // Print complete received data in one line
    // IrReceiver.printIRSendUsage(&Serial);    // Print the statement required to send this data

    IrReceiver.resume();  // Enable receiving of the next value
  }
}
