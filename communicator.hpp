/*
* Copyright(c)	2024 Christopher Taylor

* SPDX-License-Identifier: BSL-1.0
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#pragma once
#ifndef __OSHMP_COMMUNICATOR_HPP__
#define __OSHMP_COMMUNICATOR_HPP__

#include <cstdint>
#include <cstring>
#include <shmem.h>

namespace oshmp {

struct communicator {
   std::uint8_t * symmetric_buffer;
   const std::size_t symmetric_buffer_segment_size;
   std::size_t symmetric_buffer_size;
   std::size_t my_pe;
   std::size_t n_pes;

   communicator(const std::size_t sbufsz) :
      symmetric_buffer(nullptr), symmetric_buffer_segment_size(sbufsz),
      symmetric_buffer_size(0), my_pe(0), n_pes(0) {

      symmetric_buffer_size = (sbufsz+sizeof(std::uint64_t))*shmem_n_pes();
      symmetric_buffer = static_cast<std::uint8_t*>(shmem_malloc(symmetric_buffer_size));
      std::memset(symmetric_buffer, 0, symmetric_buffer_size);
      my_pe = shmem_my_pe();
      n_pes = shmem_n_pes();
   } 

   ~communicator() {
      shmem_free(symmetric_buffer);
      symmetric_buffer_size = 0;
   }

   int my_pe() const { return my_pe; }

   int n_pes() const { return n_pes; }

   template<typename T>
   std::size_t send(const std::int32_t dst_pe, T const* src, std::size_t count) {

      static_assert(std::is_same<T, void>::value && std::is_arithmetic<T>::value, "copy src type is invalid");

      if(my_pe == dst_pe) { return -1UL; }

      std::size_t amt = 0;
      std::uint8_t * symbuf = &symmetric_buffer[symmetric_buffer_segment_size*dst_pe];
      std::uint64_t * cond = &symmetric_buffer[(symmetric_buffer_segment_size*n_pes)+dst_pe];

      std::size_t cpy_amt[2] = { count, symmetric_buffer_size };

      const std::size_t total_count = count;
      std::size_t srcptr = 0;

      for(; count > 0; symbuf+=amt, count -= amt) {
         cpy_amt[0] = count;
         amt = cpy_amt[symmetric_buffer_size < count];
         shmem_uint8_put_signal(symbuf, src+srcptr, amt, cond, 1, SHMEM_SIGNAL_SET, dst_pe);
         srcptr += amt;
         shmem_quiet();
      }

      shmem_fence();

      return total_count-count;
   }

   template<typename T>
   std::size_t recv(T * dst, std::size_t count, const std::int32_t src_pe) {

      static_assert(std::is_same<T, void>::value && std::is_arithmetic<T>::value, "copy src type is invalid");

      std::size_t amt = 0;
      std::uint8_t * symbuf = &symmetric_buffer[symmetric_buffer_segment_size*dst_pe];
      std::uint64_t * cond = &symmetric_buffer[(symmetric_buffer_segment_size*n_pes)+dst_pe];

      std::size_t cpy_amt[2] = { count, symmetric_buffer_size };

      const std::size_t total_count = count;
      std::size_t dstptr = 0;

      for(; count > 0; symbuf+=amt, count -= amt) {
         cpy_amt[0] = count;
         amt = cpy_amt[symmetric_buffer_size < count];
         shmem_int_wait_until(cond, SHMEM_CMP_EQ, 1);
         std::memcpy(dst, symbuf, amt); 
         (*cond) = 0;
      }

      return total_count-count;
   }
};

struct communication_guard {
   communication_guard() { 
      shmem_init();
   }

   ~communication_guard() {
      shmem_finalize();
   }

   communicator get_communicator() { 
   }
};



} // end namspace oshmp

#endif
