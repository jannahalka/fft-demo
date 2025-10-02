#include <Arduino.h>
#include <arduinoFFT.h>

#define SAMPLES 128
#define SAMPLING_FREQUENCY 4000

void setColor(bool r, bool g, bool b);

float vReal[SAMPLES];
float vImag[SAMPLES];

int redLED = LEDR; // Nano 33 BLE built-in RGB LED
int greenLED = LEDG;
int blueLED = LEDB;

ArduinoFFT<float> FFT =
    ArduinoFFT<float>(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY);

void setup() {
  Serial.begin(115200);

  // Set LED pins as output
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);

  // Turn all off initially
  digitalWrite(redLED, HIGH); // On Nano 33 BLE, LOW = ON, HIGH = OFF
  digitalWrite(greenLED, HIGH);
  digitalWrite(blueLED, HIGH);
}

void loop() {
  // Example signal (replace with real sensor/mic samples)
  for (int i = 0; i < SAMPLES; i++) {
    vReal[i] = sin(2 * PI * 1000 * i / SAMPLING_FREQUENCY); // test 1000Hz
    vImag[i] = 0;
  }

  // Run FFT
  FFT.windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.complexToMagnitude(vReal, vImag, SAMPLES);

  // Find peak frequency
  double maxMag = 0;
  int index = 0;
  for (int i = 1; i < SAMPLES / 2; i++) {
    if (vReal[i] > maxMag) {
      maxMag = vReal[i];
      index = i;
    }
  }

  double freq = (index * 1.0 * SAMPLING_FREQUENCY) / SAMPLES;
  Serial.print("Dominant frequency: ");
  Serial.println(freq);

  // Map frequency to LED color
  if (freq < 300) {
    setColor(true, false, false); // Red
  } else if (freq < 1000) {
    setColor(false, true, false); // Green
  } else {
    setColor(false, false, true); // Blue
  }

  delay(500);
}

// Helper to set RGB LED color
void setColor(bool r, bool g, bool b) {
  digitalWrite(redLED, r ? LOW : HIGH);
  digitalWrite(greenLED, g ? LOW : HIGH);
  digitalWrite(blueLED, b ? LOW : HIGH);
}
