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

#ifdef HAVE_CUDA
#include <thrust/host_vector.h>
#include <thrust/system/cuda/experimental/pinned_allocator.h>
#else
#include <vector>
#endif

#include "aligned_allocator.h"

namespace storage {
   const int n_dom = 115, n_mod = 18;
   extern int n_per_mod;
   extern int n_hits;
}

#ifdef HAVE_CUDA
using storage_t = thrust::host_vector<long, thrust::cuda::experimental::pinned_allocator<int>>;
#else
using storage_t = std::vector<long, aligned_allocator<long>>;
#endif

using queue_t = std::vector<std::tuple<storage_t, storage_t>>;
