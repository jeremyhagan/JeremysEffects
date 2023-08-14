/*-----------------------------------------------------------------------------
FastLED effects by Jeremy

Description:
  A library of the effects I have created

History:
  2023-08-11: Created TrainEffect with a single colour
  2023-08-12:
    Created TrainEffect with a rainbow colour
    Created TravellingRainbow effect
  2023-08-13:
    Consolidated TrainEfect functions into a single function.
    Updated TrvellingRainbow to include:
      Mid journey pause with rainbow travelling along solid string
      Fixed disappear effect to include the travelling rainbow while disappearing
-----------------------------------------------------------------------------*/

#include <FastLED.h>
#include <math.h>

#define NUM_LEDS  120
#define LED_PIN   7
#define TAIL      10
#define DELAY     15

CRGB leds[NUM_LEDS] = {0};

void setup() {
  pinMode(LED_PIN, OUTPUT);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.clear(true);
  Serial.begin(115200);
  while (!Serial) { }
  Serial.println("Jeremy's effects starting up");
}

void loop() {
  //TravellingRainbow(50, 50, 100);
  TrainEffect(CRGB::Red, 200, 30, true, true, true, 1);
  //TrainEffect(CRGB::Red, 100, 20, true, true, 1);
  //TrainEffect(CRGB::Blue, 100, 7, true, false, 1);
  //TrainEffect(CRGB::Green, 100, 12, false, false, 1);
  Twinkle(100);
}

#pragma region functions
void Test()
{
  fill_rainbow(leds, 20, 0, 10);
  FastLED.show();
}

void TrainEffect(CRGB colour, uint8_t brightness, int length, bool two_way, bool rainbow, bool comet, int loops)
{
  if (comet) {brightness = 255;}
  FastLED.setBrightness(brightness);
  int delta = floor(160/(length - 1));
  for (int a = 0; a < loops; a++)
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      if(rainbow)
      {
        leds[i] = CHSV(0, 255, brightness);
      }
      else
      {
        leds[i] = colour;
      }
      for (int j = i - 1; j > i - length; j--)
      {
        if (j >= 0)
        {
          if(rainbow)
          {
            // Set each trailing light to a less red colour, leading to blue
            // for example (delta = 16), when i = 1 and j = 0. So the hue is set to 0 + 16 * (1 - 0) = 16
            // when i = 2, j = 1 it is as above, and the next LED, when j = 0 will be 32
            leds[j] = CHSV(0 + delta * (i - j), 255, brightness);
          }
          else
          {
            leds[j] = colour;
          }
          if (comet) {leds[j].fadeToBlackBy((i - j) * 255 / length);}
        }
      }
      if (i - length >= 0)
      {
        leds[i - length] = CRGB::Black;
      }
      delay(DELAY);
      // handle lights getting to the end of the strip
      if (i == NUM_LEDS - 1)
      {
        for (int j = length; j > 0; j--)
        {
          leds[i - j] = CRGB::Black;
          FastLED.show();
          delay(DELAY);
        }
        leds[i] = CRGB::Black;
        FastLED.show();
        delay(DELAY);
      }
      FastLED.show();
    }

    if (two_way)
    {
      // Now do it in reverse
      for (int i = NUM_LEDS - 1; i >= 0; i--)
      {
        if (rainbow)
        {
          leds[i] = CHSV(0, 255, brightness);
        }
        else
        {
          leds[i] = colour;
        }
        for (int j = i + 1; j < i + length; j++)
        {
          if (j < NUM_LEDS)
          {
            if (rainbow)
            {
              leds[j] = CHSV(0 + delta * (j - i), 255, brightness);
            }
            else
            {
              leds[j] = colour;
            }
            if (comet) {leds[j].fadeToBlackBy((j - i) * 255 / length);}
          }
        }
        if (i + length < NUM_LEDS)
        {
          leds[i + length] = CRGB::Black;
        }
        FastLED.show();
        delay(DELAY);
        // handle lights getting to the end of the strip
        if (i == 0)
        {
          for (int j = length; j >= 0; j--)
          {
            leds[i + j] = CRGB::Black;
            FastLED.show();
            delay(DELAY);
          }
        }
      }
    }
  }
}

void TravellingRainbow(int length, uint8_t brightness, int loops)
{
  int delta = floor(160/(length - 1));
  CHSV final_hue;
  // Grow the rrainbow from the beginning to the end
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(0, 255, brightness);
    for (int j = i - 1; j > i - length; j--)
      {
        if (j >= 0)
        {
          fill_rainbow(leds, j, 160 - ((length - i) * delta), -delta);
        }
      }
    FastLED.show();
    delay(DELAY);
  }

  // Continue the rainbox for "loops" cycles
  final_hue = rgb2hsv_approximate(leds[0]);
  uint8_t hue = final_hue.hue - 1;
  for (int a = 0; a < loops; a++)
  {
    fill_rainbow(leds, NUM_LEDS, hue, -delta);
    FastLED.show();
    hue += delta;
    delay(DELAY);
  }

  // Drive the rainbow into the far end of the strip
  final_hue = rgb2hsv_approximate(leds[0]);
  hue = final_hue.hue - 1;
  for (int i = 1; i <= NUM_LEDS; i++)
  {
    fill_rainbow(leds, NUM_LEDS, hue, -delta);
    fill_solid(leds, i, CRGB::Black);
    FastLED.show();
    hue += delta;
    delay(DELAY);
  }
}

double FramesPerSecond(double seconds)
{
  static double framesPerSecond;
  framesPerSecond = (framesPerSecond * 0.9) + (1.0 / seconds * 0.1);
  return framesPerSecond;
}

void Twinkle(int loops)
{
  uint8_t hue;
  uint8_t led;
  uint8_t saturation;
  
  for (int i = 0; i <= loops; i++)
  {
    hue = random8();
    saturation = random8();
    led = random8(NUM_LEDS);
    for (int j = 0; j < NUM_LEDS; j++) {leds[j].fadeToBlackBy(32);}
    leds[led] = CHSV(hue, saturation, 255);
    FastLED.show();
    FastLED.delay(100);
  }
}
#pragma endregion functions
