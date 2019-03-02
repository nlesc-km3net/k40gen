/*
 * Copyright 2018-2019 NWO-I
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <Vc/Vc>
#include <array>
#include "generate_common.h"

namespace GenAVX2 {

  const std::array<Vc::SimdArray<float, 3>, 31> PMTs
    {{{ 0.000f, -0.832f,  0.555f},
      {-0.955f,  0.000f,  0.295f},
      {-0.478f, -0.827f,  0.295f},
      { 0.478f, -0.827f,  0.295f},
      { 0.720f, -0.416f,  0.555f},
      {-0.720f, -0.416f,  0.555f},
      { 0.955f,  0.000f,  0.295f},
      {-0.720f,  0.416f,  0.555f},
      { 0.720f,  0.416f,  0.555f},
      { 0.000f,  0.832f,  0.555f},
      { 0.478f,  0.827f,  0.295f},
      {-0.478f,  0.827f,  0.295f},
      { 0.000f,  0.955f, -0.295f},
      { 0.416f,  0.720f, -0.555f},
      { 0.000f,  0.527f, -0.850f},
      { 0.827f,  0.478f, -0.295f},
      {-0.827f,  0.478f, -0.295f},
      {-0.416f,  0.720f, -0.555f},
      {-0.456f,  0.263f, -0.850f},
      { 0.456f,  0.263f, -0.850f},
      {-0.832f,  0.000f, -0.555f},
      { 0.832f,  0.000f, -0.555f},
      { 0.000f,  0.000f, -1.000f},
      { 0.827f, -0.478f, -0.295f},
      { 0.000f, -0.527f, -0.850f},
      { 0.456f, -0.263f, -0.850f},
      {-0.456f, -0.263f, -0.850f},
      {-0.827f, -0.478f, -0.295f},
      {-0.416f, -0.720f, -0.555f},
      { 0.416f, -0.720f, -0.555f},
      { 0.000f, -0.955f, -0.295f}}};


float dot_product(const Vc::SimdArray<float, 3>& left, const Vc::SimdArray<float, 3>& right);

}

std::tuple<storage_t, storage_t> generate_avx2(const long time_start, const long time_end,
                                               Generators& gens);
