/*
Adapted by Andri Yadi.
Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "output_handler.h"
#include "constants.h"
#include "Arduino.h"

#define PIN_SPEAKER 10
#define DO_FRQ 100
#define CHECK_TONE_TIME 100

#define PIN_LED 8

void HandleOutput(tflite::ErrorReporter *error_reporter, int kind)
{
  // The first time this method runs, set up our LED
  static bool is_initialized = false;
  if (!is_initialized)
  {
    is_initialized = true;
  }

  // Print some ASCII art for each gesture
  if (kind == 0)
  {
#if DEBUG_INF_RES
    // error_reporter->Report("\n\r█ Wingardium Leviosa █\n\r");
    error_reporter->Report(
        "\n\r          *\n\r       *     *\n\r     *         *\n\r "
        "   *           *\n\r     *         *\n\r       *     *\n\r      "
        "    *\n\r");

    error_reporter->Report("\n\r");
    error_reporter->Report("╔══════════════════════╗");
    error_reporter->Report("║  Circle Clock Wise   ║");
    error_reporter->Report("╚══════════════════════╝\n\r");
#endif
  }
  else if (kind == 1)
  {
#if DEBUG_INF_RES
    // error_reporter->Report("\n\r█ Obliviate █\n\r");
    error_reporter->Report(
        "\n\r          *\n\r       *     *\n\r     *         *\n\r "
        "   *           *\n\r     *         *\n\r       *     *\n\r      "
        "    *\n\r");

    error_reporter->Report("\n\r");
    error_reporter->Report("╔════════════════════════╗");
    error_reporter->Report("║ Ciecle Anti Clock Wise ║");
    error_reporter->Report("╚════════════════════════╝\n\r");

// LightUpRGB(kind);
#endif
  }
  else if (kind == 2)
  {
    // tone(PIN_SPEAKER, DO_FRQ, CHECK_TONE_TIME);
    //digitalWrite(PIN_LED, HIGH);
    //delay(300);
    //digitalWrite(PIN_LED, LOW);
    //delay(100);
    //digitalWrite(PIN_LED, HIGH);
    //delay(100);
    //digitalWrite(PIN_LED, LOW);
#if DEBUG_INF_RES
    // error_reporter->Report("\n\r█ Obliviate █\n\r");
    error_reporter->Report(
        "\n\r"
        "               *  \n\r"
        "             *    \n\r"
        "   *       *      \n\r"
        "     *   *        \n\r"
        "       *          \n\r");

    error_reporter->Report("\n\r");
    error_reporter->Report("╔════════════════════════╗");
    error_reporter->Report("║         Check          ║");
    error_reporter->Report("╚════════════════════════╝\n\r");

// LightUpRGB(kind);
#endif
  }
  else
  {
    // tone(PIN_SPEAKER, 100, 100);
    //  ESPectro32.LedMatrix().clear();
  }
}
