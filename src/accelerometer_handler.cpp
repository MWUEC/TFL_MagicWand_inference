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

#define g 630.0
#define zero 2330.0

const float MAP_IN_MAX = (zero + 3 * g);
const float MAP_IN_MIN = (zero - 3 * g);
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

int skipBeforeMs = 1000 * (1 / sample_every_n);

TfLiteStatus SetupAccelerometer(tflite::ErrorReporter *error_reporter)
{
  return kTfLiteOk;
}

int cnt = 0;
#define accel_data_num 500

class Vector3
{
public:
  float x, y, z;
  Vector3(float _x, float _y, float _z)
  {
    x = _x;
    y = _y;
    z = _z;
  }
  Vector3()
  {
    x = 0.0;
    y = 0.0;
    z = 0.0;
  }
  void Set(float _x, float _y, float _z)
  {
    x = _x;
    y = _y;
    z = _z;
  }
  void Set(Vector3 new_vector)
  {
    x = new_vector.x;
    y = new_vector.y;
    z = new_vector.z;
  }
  void Add(Vector3 new_vector)
  {
    x += new_vector.x;
    y += new_vector.y;
    z += new_vector.z;
  }
  void Sub(Vector3 new_vector)
  {
    x -= new_vector.x;
    y -= new_vector.y;
    z -= new_vector.z;
  }
  void MulScalar(float scalar)
  {
    x *= scalar;
    y *= scalar;
    z *= scalar;
  }
  void Print()
  {
    printf("%f, %f, %f", x, y, z);
  }
  void Println()
  {
    Print();
    printf("\n");
  }
};

Vector3 zero_vector(0, 0, 0);
Vector3 accel_datas[accel_data_num];
Vector3 accel_sum;

// Adapted from https://blog.boochow.com/article/m5stack-tflite-magic-wand.html
static bool AcquireData()
{
  bool new_data = false;

  Vector3 raw(analogRead(PIN_X), analogRead(PIN_Y), analogRead(PIN_Z));
  accel_sum.Sub(accel_datas[cnt]);
  accel_datas[cnt].Set(raw);
  accel_sum.Add(accel_datas[cnt]);

  cnt++;
  if (cnt >= accel_data_num) cnt = 0;

  if ((millis() - lastAcqMillis) < skipBeforeMs /*40*/) return false; // 1.5ms以内での連続した計測をしないようにしている。デフォルト値:40ms
  lastAcqMillis = millis();

  Vector3 accel_avr = accel_sum;
  accel_avr.MulScalar(1.0 / accel_data_num);
  accel_avr.Println();
  const float norm_x = Fmap(accel_avr.x);
  const float norm_y = Fmap(accel_avr.y);
  const float norm_z = Fmap(accel_avr.z);
  printf("%f, %f, %f / %f, %f, %f\n", accel_avr.x, accel_avr.y, accel_avr.z, norm_x, norm_y, norm_z);

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

bool ReadAccelerometer(tflite::ErrorReporter *error_reporter, float *input,
                       int input_length)
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
