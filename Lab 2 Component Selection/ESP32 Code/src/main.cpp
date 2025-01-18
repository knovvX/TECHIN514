#include <Arduino.h>


#define ANALOG_PIN_RED A0
#define ANALOG_PIN_BLUE A1

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // Initialize serial communication at 115200 baud
  analogReadResolution(12); // Set ADC resolution to 12-bit (values from 0 to 4095)

}

void loop() {
  // put your main code here, to run repeatedly:
   // Read the raw analog value from the defined pin
  int adcValueRed = analogRead(ANALOG_PIN_RED);
  int adcValueBlue = analogRead(ANALOG_PIN_BLUE);

  // Convert the ADC value to a voltage (0-3.3V)
  float voltageRed = (adcValueRed / 4095.0) * 3.3;
  float voltageBlue = (adcValueBlue / 4095.0) * 3.3;

  // Print the raw ADC value and the corresponding voltage to the Serial Monitor
  Serial.print("ADC Value for Red LED: ");
  Serial.print(adcValueRed);
  Serial.print("\tVoltage for Red LED: ");
  Serial.print(voltageRed);
  Serial.println(" V");


  delay(1000); // Wait for 1 second before taking the next reading
  Serial.print("ADC Value for Blue LED: ");
  Serial.print(adcValueBlue);
  Serial.print("\tVoltage for Blue LED: ");
  Serial.print(voltageBlue);
  Serial.println(" V");
}

