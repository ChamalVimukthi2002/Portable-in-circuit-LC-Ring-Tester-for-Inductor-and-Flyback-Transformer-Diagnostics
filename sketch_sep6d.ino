#include <Arduino.h>

// ============================================================
// PINS
// ============================================================

#define PULSE_PIN   33    // P3.3 - excitation
#define COUNT_PIN   10    // P1.0 / T2 - comparator output


// ============================================================
// CHARLIEPLEX DISPLAY
// ============================================================

const uint8_t PINS[6] = {
  11, 15, 16, 17, 14, 32
};

const uint8_t SEG_MAP[2][7][2] = {

  // Tens digit
  {
    {3, 1},   // A
    {4, 1},   // B
    {5, 1},   // C
    {0, 2},   // D
    {1, 2},   // E
    {3, 2},   // F
    {4, 2}    // G
  },

  // Ones digit
  {
    {5, 2},   // A
    {0, 3},   // B
    {1, 3},   // C
    {2, 3},   // D
    {4, 3},   // E
    {5, 3},   // F
    {5, 4}    // G
  }
};

const uint8_t DIGIT_PATTERNS[10] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};


// ============================================================
// DISPLAY
// ============================================================

void allDisplayOff()
{
  for (uint8_t i = 0; i < 6; i++) {
    pinMode(PINS[i], INPUT);
  }
}

void lightSegment(uint8_t anodeIdx, uint8_t cathodeIdx)
{
  pinMode(PINS[anodeIdx], OUTPUT);
  digitalWrite(PINS[anodeIdx], HIGH);

  pinMode(PINS[cathodeIdx], OUTPUT);
  digitalWrite(PINS[cathodeIdx], LOW);

  delayMicroseconds(250);

  pinMode(PINS[anodeIdx], INPUT);
  pinMode(PINS[cathodeIdx], INPUT);
}

void displayNumber(uint8_t num, uint16_t duration_ms)
{
  if (num > 99)
    num = 99;

  uint8_t tens = num / 10;
  uint8_t ones = num % 10;

  uint32_t start = millis();

  while ((uint32_t)(millis() - start) < duration_ms) {

    uint8_t pattern;

    // Tens
    pattern = DIGIT_PATTERNS[tens];

    for (uint8_t s = 0; s < 7; s++) {
      if (pattern & (1 << s)) {
        lightSegment(
          SEG_MAP[0][s][0],
          SEG_MAP[0][s][1]
        );
      }
    }

    // Ones
    pattern = DIGIT_PATTERNS[ones];

    for (uint8_t s = 0; s < 7; s++) {
      if (pattern & (1 << s)) {
        lightSegment(
          SEG_MAP[1][s][0],
          SEG_MAP[1][s][1]
        );
      }
    }
  }

  allDisplayOff();
}

void displayDashes(uint16_t duration_ms)
{
  uint32_t start = millis();

  while ((uint32_t)(millis() - start) < duration_ms) {

    lightSegment(
      SEG_MAP[0][6][0],
      SEG_MAP[0][6][1]
    );

    lightSegment(
      SEG_MAP[1][6][0],
      SEG_MAP[1][6][1]
    );
  }

  allDisplayOff();
}


// ============================================================
// TIMER 2 EXTERNAL COUNTER
// ============================================================

void initTimer2Counter()
{
  pinMode(COUNT_PIN, INPUT);

  /*
   * C/T2 = 1
   * Timer 2 operates as an external counter.
   *
   * Falling edges on P1.0/T2 increment the counter.
   */

  T2CON = 0x02;
  T2MOD = 0x00;

  TH2 = 0;
  TL2 = 0;

  RCAP2H = 0;
  RCAP2L = 0;

  TF2 = 0;
  EXF2 = 0;

  TR2 = 0;
}


// ============================================================
// ONE MEASUREMENT
// ============================================================

uint16_t measurePulses()
{
  // Stop Timer 2
  TR2 = 0;

  // Clear counter
  TH2 = 0;
  TL2 = 0;

  // Clear flags
  TF2 = 0;
  EXF2 = 0;

  // Make sure excitation is OFF before starting
  digitalWrite(PULSE_PIN, LOW);

  // Start hardware counter
  TR2 = 1;

  // ----------------------------------------------------------
  // Excitation ON for 100 ms
  // ----------------------------------------------------------

  digitalWrite(PULSE_PIN, HIGH);

  delay(100);

  // ----------------------------------------------------------
  // Excitation OFF
  // ----------------------------------------------------------

  digitalWrite(PULSE_PIN, LOW);

  // Stop counting
  TR2 = 0;

  // Read 16-bit counter
  uint16_t count =
    ((uint16_t)TH2 << 8) |
    TL2;

  return count;
}


// ============================================================
// SETUP
// ============================================================

void setup()
{
  // Display pins high impedance
  allDisplayOff();

  // Excitation output
  pinMode(PULSE_PIN, OUTPUT);
  digitalWrite(PULSE_PIN, LOW);

  // Timer 2 counter
  initTimer2Counter();

  // Startup indication
  displayNumber(88, 500);
}


// ============================================================
// LOOP
// ============================================================

void loop()
{
  // Perform one 100 ms measurement
  uint16_t count = measurePulses();

  // Display result for approximately 3.9 seconds.
  // 0.1 s measurement + 3.9 s display = 4 seconds total.
  if (count == 0) {

    displayDashes(1000);

  } else {

    if (count > 99)
      count = 99;

    displayNumber((uint8_t)count, 1000);
  }
}