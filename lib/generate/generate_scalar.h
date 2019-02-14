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

#include "generate_common.h"
#include <array>

namespace GenScalar {

  const std::array<std::array<float, 3>, 31> PMTs
    {{{ 0    , -0.832, 0.55},
      {-0.955,  0    , 0.29},
      {-0.478, -0.827, 0.29},
      { 0.478, -0.827, 0.29},
      { 0.72 , -0.416, 0.55},
      {-0.72 , -0.416, 0.55},
      { 0.955,  0    , 0.29},
      {-0.72 ,  0.416, 0.55},
      { 0.72 ,  0.416, 0.55},
      { 0    ,  0.832, 0.55},
      { 0.478,  0.827, 0.29},
      {-0.478,  0.827, 0.29},
      { 0    ,  0.955, 0.29},
      { 0.416,  0.72 , 0.55},
      { 0    ,  0.527, 0.85},
      { 0.827,  0.478, 0.29},
      {-0.827,  0.478, 0.29},
      {-0.416,  0.72 , 0.55},
      {-0.456,  0.263, 0.85},
      { 0.456,  0.263, 0.85},
      {-0.832,  0    , 0.55},
      { 0.832,  0    , 0.55},
      { 0    ,  0    , 1   },
      { 0.827, -0.478, 0.29},
      { 0    , -0.527, 0.85},
      { 0.456, -0.263, 0.85},
      {-0.456, -0.263, 0.85},
      {-0.827, -0.478, 0.29},
      {-0.416, -0.72 , 0.55},
      { 0.416, -0.72 , 0.55},
      { 0    , -0.955, 0.29}}};

float dot_product(const std::array<float, 3>& left, const std::array<float, 3>& right);

}

std::tuple<storage_t, storage_t> generate_scalar(const long time_start, const long time_end,
                                                 Generators& gens);
