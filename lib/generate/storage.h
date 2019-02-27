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

#include <vector>

#ifdef USE_AVX2
#include <Vc/Allocator>
using storage_t = std::vector<long, Vc::Allocator<long>>;
using pmts_t = std::vector<int, Vc::Allocator<int>>;
#else
using storage_t = std::vector<long>;
using pmts_t = std::vector<int>;
#endif

using queue_t = std::vector<std::tuple<storage_t, storage_t>>;
