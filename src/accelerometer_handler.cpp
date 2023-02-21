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

#include "accelerometer_handler.h"

#include <Arduino.h>
#include "constants.h"

#define PIN_X A0
#define PIN_Y A1
#define PIN_Z A2

#define g 630
#define zero 2330

const float MAP_IN_MAX = (float)(zero + 3 * g);
const float MAP_IN_MIN = (float)(zero - 3 * g);
const float MAP_OUT_MAX = 3000.0;
const float MAP_OUT_MIN = -MAP_OUT_MAX;

const float ratio = (MAP_OUT_MAX - MAP_OUT_MIN) / (MAP_IN_MAX - MAP_IN_MIN);
float Fmap(float input)
{
  return (input - MAP_IN_MIN) * ratio + MAP_OUT_MIN;
}
#define RING_BUFFER_SIZE_1CH 100
#define RING_BUFFER_SIZE (RING_BUFFER_SIZE_1CH * 3) /*channel*/ // 200 * 3

// A buffer holding the last 200 sets of 3-channel values
float save_data[RING_BUFFER_SIZE] = {0.0};
// Most recent position in the save_data buffer
int begin_index = 0;
// True if there is not yet enough data to run inference
bool pending_initial_data = true;
// How often we should save a measurement during downsampling
int sample_every_n = kTargetHz;
// The number of measurements since we last saved one
int sample_skip_counter = 1;

long lastAcqMillis = 0;

int skipBeforeMs = 1000 * (1.0 / sample_every_n);

TfLiteStatus SetupAccelerometer(tflite::ErrorReporter *error_reporter)
{
  return kTfLiteOk;
}

// Adapted from https://blog.boochow.com/article/m5stack-tflite-magic-wand.html
static bool AcquireData()
{
  bool new_data = false;
  // 1.5ms以内での連続した計測をしないようにしている。デフォルト値:40ms
  if ((millis() - lastAcqMillis) < skipBeforeMs /*40*/) return false;
  lastAcqMillis = millis();

  int accel_raw_data_x = analogRead(PIN_X);
  int accel_raw_data_y = analogRead(PIN_Y);
  int accel_raw_data_z = analogRead(PIN_Z);

  const float norm_x = Fmap(accel_raw_data_x);
  const float norm_y = Fmap(accel_raw_data_y);
  const float norm_z = Fmap(accel_raw_data_z);

  save_data[begin_index++] = norm_x /* * 1000*/;
  save_data[begin_index++] = norm_y /* * 1000*/;
  save_data[begin_index++] = norm_z /* * 1000*/;

  if (begin_index >= RING_BUFFER_SIZE)
  {
    begin_index = 0;
  }
  new_data = true;

  return new_data;
}

bool ReadAccelerometer(tflite::ErrorReporter *error_reporter, 
                       float *input, int input_length)
{

  // Skip this round if data is not ready yet
  if (!AcquireData())
  {
    return false;
  }

  // Check if we are ready for prediction or still pending more initial data
  if (pending_initial_data && begin_index >= RING_BUFFER_SIZE_1CH) // pending : 未決定で 宙ぶらりんで pending_initial_data == true で計測中
  {                                                                // buf が埋まるまで待機っぽい?
    pending_initial_data = false;
    // Could be a sign to be ready
  }

  // Return if we don't have enough data
  if (pending_initial_data)
  {
    return false;
  }

  // Copy the requested number of bytes to the provided input tensor
  for (int i = 0; i < input_length; ++i)
  {
    int ring_array_index = begin_index + i - input_length;
    if (ring_array_index < 0)
    {
      ring_array_index += RING_BUFFER_SIZE;
    }
    input[i] = save_data[ring_array_index];
  }

  return true;
}
