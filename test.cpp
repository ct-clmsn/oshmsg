/*
* Copyright(c)	2024 Christopher Taylor

* SPDX-License-Identifier: BSL-1.0
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#include "communicator.hpp"

using namespace oshmp;

int main(int argc, char * argv[]) {

   communication_guard commguard{};

   communicator c = commguard.get_communicator();

   std::vector<int> values();
   values.reserve(100);

   std::iota(values.begin(), values.end(), 100);
   
   std::uint8_t * data[100];
   std::memset(data, 0, sbufsz+1);

   if(c.my_pe() == 0) {
      c.copy(data, values.data(), 100);
   }
   else {
      c.copy(data, nullptr, 100);
   }

   return 0;
}
