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

#include <cstdint>
#include <vector>
#include <iostream>

template <typename T, std::size_t N = alignof(T)>
class aligned_allocator {
    //  helper type with the alignment we want
    using aligned_type = std::aligned_storage_t<sizeof(T), N>;

public:
   //  type to allocate
   typedef T * pointer;
   typedef const T * const_pointer;
   typedef T& reference;
   typedef const T& const_reference;
   typedef T value_type;
   typedef std::size_t size_type;
   typedef ptrdiff_t difference_type;

    //  ensure basic special class methods are enabled
    aligned_allocator()                                         = default;
    aligned_allocator(const aligned_allocator&)                 = default;
    aligned_allocator& operator=(const aligned_allocator&)      = default;
    aligned_allocator(aligned_allocator&&) noexcept             = default;
    aligned_allocator& operator=(aligned_allocator&&) noexcept  = default;

    //  allow construction from an allocator for another type
    //  (with the same alignment)
    template <typename U>
    aligned_allocator(const aligned_allocator<U>&) {}

    //  implementations may derive from the allocator polymorphically
    virtual ~aligned_allocator() noexcept = default;

    //  rebinding the allocator for another type should respect the alignment
    //  of that other type
    template <typename U>
    struct rebind {
        using other = aligned_allocator<U>;
    };

    //  allocate a type that we know will have the correct alignment
    //  then reinterpret the pointer
    T* allocate(std::size_t n) {
        return reinterpret_cast<T*>(new aligned_type[n]);
    }

    //  deallocate previously allocated memory
    void deallocate(T* p, std::size_t) {  //  don't care about size here
        delete[] reinterpret_cast<aligned_type*>(p);
    }
};

template <typename T, typename U, std::size_t N>
constexpr bool operator==(const aligned_allocator<T, N>&,
                          const aligned_allocator<U, N>&) {
    return true;
}

template <typename T, typename U, std::size_t N>
constexpr bool operator!=(const aligned_allocator<T, N>&,
                          const aligned_allocator<U, N>&) {
    return false;
}
