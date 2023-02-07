/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

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

#ifndef TENSORFLOW_LITE_MICRO_EXAMPLES_MAGIC_WAND_CONSTANTS_H_
#define TENSORFLOW_LITE_MICRO_EXAMPLES_MAGIC_WAND_CONSTANTS_H_

// Show debug infomation
#define DEBUG_INF_RES 1

// The expected accelerometer data sample frequency
constexpr float kTargetHz = 25.0; /*25Hz*/

// What gestures are supported.
constexpr int kGestureCount = 4;
constexpr int kCircleCWGesture = 0;
constexpr int kCircleACWGesture = 1;
constexpr int kCheckGesure = 2;
constexpr int kNoGesture = 3;

// The number of expected consecutive inferences for each gesture type
extern const int kConsecutiveInferenceThresholds[3 /*kGestureCount - 1*/];

// These control the sensitivity of the detection algorithm. If you're seeing
// too many false positives or not enough true positives, you can try tweaking
// these thresholds. Often, increasing the size of the training set will give
// more robust results though, so consider retraining if you are seeing poor
// predictions.
constexpr float kDetectionThreshold = 0.80f;                   // 0.8f;
constexpr int kPredictionHistoryLength = 10;                   // 10
constexpr int kPredictionSuppressionDuration = (int)kTargetHz; // 25
/*stop recognition after recognition some gesture kPreddicitionSuppressionDuration/kTargetHz second.*/

#endif // TENSORFLOW_LITE_MICRO_EXAMPLES_MAGIC_WAND_CONSTANTS_H_
