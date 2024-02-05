#include <IRremote.hpp>
#include <LiquidCrystal_I2C.h>

// constants
constexpr byte RED = 0,
               YELLOW = 1,
               GREEN = 2;

struct TrafficLight {

  struct Step {
    unsigned long duration;
    byte actions[3];
  };

  // https://content.instructables.com/F33/RO45/IG6TUEKM/F33RO45IG6TUEKM.png

  static inline constexpr Step cycle[]{
    { .duration = 0, .actions = { LOW, LOW, LOW } },  // unused for now, means that lights are disabled
    { .duration = 6000, .actions = { HIGH, LOW, LOW } },
    { .duration = 2000, .actions = { HIGH, HIGH, LOW } },
    { .duration = 8000, .actions = { LOW, LOW, HIGH } },
    { .duration = 2500, .actions = { LOW, HIGH, LOW } },
  };

  byte currentStep = 0;                            // starting at step 0, this will be overridden on setup
  unsigned long nextStepTime = cycle[0].duration;  // starting at step 0, this will be overridden on setup
  byte signals[3];
  void lighting() const {
    for (byte i = 0; i < 3; ++i)
      digitalWrite(
        signals[i],
        cycle[currentStep].actions[i]);
  }

  // void off() const {
  //   for (byte i = 0; i < 3; ++i)
  //     digitalWrite(
  //       signals[i],
  //       LOW);
  // }

  TrafficLight(byte r, byte g, byte b)
    : signals{ r, g, b } {}

  void gotoStep(byte step) {
    currentStep = step;
    nextStepTime = cycle[currentStep].duration + millis();
  }

  byte followingStep() const {

    return (currentStep % 4) + 1;  // { 0 -> 1 -> 2 -> 3 -> 4 -> 1 ... }

    // currentStep = (currentStep + 1) % 5; // { 0 -> 1 -> 2 -> 3 -> 4 -> 0 ... }
  }

  bool progessCycle() const {

    // Not enough time has passed

    return millis() > nextStepTime;
  }
};

struct Crossing {
  static constexpr byte count = 3;

  TrafficLight tricolors[count]{
    TrafficLight(10, 9, 8),
    TrafficLight(4, 3, 2),
    TrafficLight(5,6,7),
    // TrafficLight(10,9,8),
  };

  // iterator
  TrafficLight *currentLight = tricolors;

  // temporary buffer variable
  TrafficLight *nextLightFromEvent = nullptr;

  void toNextLight() {
    // if (Serial.availableForWrite())
    //   Serial.println(currentLight - tricolors);

    // Cycle

    if (nextLightFromEvent != nullptr) {
      currentLight = nextLightFromEvent;
      nextLightFromEvent = nullptr;
    } else if (currentLight == tricolors + count - 1)
      currentLight = tricolors;
    else
      currentLight++;
  };

  void setCurrentLight(TrafficLight *light) {
    currentLight = light;
  }

  void doCycle() {

    // For the currently active light: is it time to move to the next step?
    if (!currentLight->progessCycle()) {
      return;
    }

    // Enough time has passed
    currentLight->gotoStep(currentLight->followingStep());
    currentLight->lighting();

    // We are back to red, set the active light to the next one in the cyclr
    if (currentLight->currentStep == 1) {
      toNextLight();
    }
  }

} crossing;

struct {
private:
  unsigned int lastUsage = -2000;

  // React to the button event
  static byte nextStepFromEvent(TrafficLight tra, bool isTarget) {
    // try to turn it on only if the light is still red
    if (isTarget) {
      switch (tra.currentStep) {
        case 0:
        case 1:
          return 2;
        default:
          return tra.currentStep;
      }
    } else {
      // turn it off
      switch (tra.currentStep) {
        case 0:
        case 1:
        case 4:
          return tra.currentStep;
        case 2:
          return 3;
        case 3:
          return 4;
        default:
          return tra.currentStep;
      }
    }
  }


  static String commandToKeyName(unsigned int command) {
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
      default: return "unknown";
    }
  }

  // parse the key and return {0,1,2,3,4} with 0 being an invalid key
  static byte keyToTarget(String key) {
    return key.toInt();
  }

public:


  // returns the number ( one indexed ) of the traffic light to turn on if an event did occur and is valid,
  // otherwise returns 0
  byte eventOccured(const Crossing &cro) const {

    // Restrict button usage frequency
    if ((millis() - lastUsage) < 2000)
      return 0;

    // TODO: remember to set the lastUsage when this functions is called with success

    // Ignore
    if (!IrReceiver.decode())
      return 0;

    // 0 to 4 with {1,2,3,4} being a valid choice and 0 anything else
    byte targetTri = keyToTarget(commandToKeyName(IrReceiver.decodedIRData.command));

    // invalid input

    if (targetTri == 0) {
      IrReceiver.resume();  // Enable receiving of the next value
      return 0;
    }

    Serial.println("it is" + String(targetTri, 10));


    // non existant light
    if (targetTri > cro.count) {
      IrReceiver.resume();  // Enable receiving of the next value
      return 0;
    }

    // convert to 0 index
    byte targetTriIndex = targetTri - 1;

    // Do nothing if target not red
    if (cro.tricolors[targetTriIndex].currentStep >= 2) {
      IrReceiver.resume();  // Enable receiving of the next value
      return 0;
    }

    // Do nothing if the currently active light is not green ( step 3 )
    if (cro.currentLight->currentStep != 3) {
      IrReceiver.resume();  // Enable receiving of the next value
      return 0;
    }

    IrReceiver.resume();  // Enable receiving of the next value
    // IrReceiver.printIRResultShort(&Serial);  // Print complete received data in one line
    // IrReceiver.printIRSendUsage(&Serial);    // Print the statement required to send this data

    return targetTri;
  }

} remote;

LiquidCrystal_I2C lcd(0x27, 16, 2);
long long lcdClearTime = -1;

void setup() {

  const byte irLedPin = 11;
  IrReceiver.begin(irLedPin, true);  // Start the receiver
  Serial.begin(9600);

  // setup pins and lighting

  for (TrafficLight &tri : crossing.tricolors) {
    for (byte l : tri.signals) {
      pinMode(l, OUTPUT);
    }

    // All lights red except the starting one
    if (&tri == crossing.currentLight) {
      tri.gotoStep(3);
    } else {
      tri.gotoStep(1);
    }

    // Light up accordingly
    tri.lighting();
  }

  // LCD
  // lcd.begin(16, 2);
  // turn on

  lcd.init();  // initialize the lcd
  // Print a message to the LCD.

  lcd.backlight();
  lcd.clear();
}

void loop() {

  if (byte targetTri = remote.eventOccured(
        crossing);
      targetTri != 0) {
    if (Serial.availableForWrite()) {
      Serial.println(targetTri);
    }

    // Set the next light

    crossing.nextLightFromEvent = crossing.tricolors + (targetTri - 1);

    // Change the time of the next step for the currently active light

    if ((crossing.currentLight->nextStepTime - millis()) > 3000)
      crossing.currentLight->nextStepTime = millis() + 3000;

    // Display LCD message

    lcdClearTime = crossing.currentLight->nextStepTime + 4000;  // display for 4 more seconds

    lcd.clear();
    lcd.home();
    lcd.print("Ambulance on");
    lcd.setCursor(0, 1);
    lcd.print("light #" + String(targetTri, 10));
  }

  // clear the lcd if an event is finished
  if (lcdClearTime != -1 && millis() > lcdClearTime) {
    lcd.clear();
    lcdClearTime = -1;
  }

  // for (TrafficLight tra : tricolors) {

  // for (TrafficLight &t : tricolors) {
  //   t.progessCycle();
  //   t.lighting();
  // }

  crossing.doCycle();
}
