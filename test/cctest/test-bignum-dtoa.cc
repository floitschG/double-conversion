// Copyright 2012 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <stdlib.h>

#include "double-conversion/bignum-dtoa.h"

#include "cctest.h"
#include "gay-fixed.h"
#include "gay-precision.h"
#include "gay-shortest.h"
#include "gay-shortest-single.h"
#include "double-conversion/ieee.h"
#include "double-conversion/utils.h"

using namespace double_conversion;

/*
// Removes trailing '0' digits.
// Can return the empty string if all digits are 0.
static void TrimRepresentation(Vector<char> representation) {
  int len = strlen(representation.start());
  int i;
  for (i = len - 1; i >= 0; --i) {
    if (representation[i] != '0') break;
  }
  representation[i + 1] = '\0';
}

*/
static const int kBufferSize = 100;


TEST(BignumDtoaShortestVariousFloats) {
  char buffer_container[kBufferSize];
  Vector<char> buffer(buffer_container, kBufferSize);
  int length;
  int point;

  float min_float = 1e-45f;
  BignumDtoa(min_float, BIGNUM_DTOA_SHORTEST_SINGLE, 0,
             buffer, &length, &point);
  CHECK_EQ("1", buffer.start());
  CHECK_EQ(-44, point);


  float max_float = 3.4028234e38f;
  BignumDtoa(max_float, BIGNUM_DTOA_SHORTEST_SINGLE, 0,
             buffer, &length, &point);
  CHECK_EQ("34028235", buffer.start());
  CHECK_EQ(39, point);

  BignumDtoa(4294967272.0f, BIGNUM_DTOA_SHORTEST_SINGLE, 0,
             buffer, &length, &point);
  CHECK_EQ("42949673", buffer.start());
  CHECK_EQ(10, point);

  BignumDtoa(3.32306998946228968226e+35f, BIGNUM_DTOA_SHORTEST_SINGLE, 0,
             buffer, &length, &point);
  CHECK_EQ("332307", buffer.start());
  CHECK_EQ(36, point);

  BignumDtoa(1.2341e-41f, BIGNUM_DTOA_SHORTEST_SINGLE, 0,
             buffer, &length, &point);
  CHECK_EQ("12341", buffer.start());
  CHECK_EQ(-40, point);

  BignumDtoa(3.3554432e7, BIGNUM_DTOA_SHORTEST_SINGLE, 0,
             buffer, &length, &point);
  CHECK_EQ("33554432", buffer.start());
  CHECK_EQ(8, point);

  BignumDtoa(3.26494756798464e14f, BIGNUM_DTOA_SHORTEST_SINGLE, 0,
             buffer, &length, &point);
  CHECK_EQ("32649476", buffer.start());
  CHECK_EQ(15, point);

  BignumDtoa(3.91132223637771935344e37f, BIGNUM_DTOA_SHORTEST_SINGLE, 0,
             buffer, &length, &point);
  CHECK_EQ("39113222", buffer.start());
  CHECK_EQ(38, point);

  uint32_t smallest_normal32 = 0x00800000;
  float v = Single(smallest_normal32).value();
  BignumDtoa(v, BIGNUM_DTOA_SHORTEST_SINGLE, 0, buffer, &length, &point);
  CHECK_EQ("11754944", buffer.start());
  CHECK_EQ(-37, point);

  uint32_t largest_denormal32 = 0x007FFFFF;
  v = Single(largest_denormal32).value();
  BignumDtoa(v, BIGNUM_DTOA_SHORTEST_SINGLE, 0, buffer, &length, &point);
  CHECK_EQ("11754942", buffer.start());
  CHECK_EQ(-37, point);
}


TEST(BignumDtoaGayShortestSingle) {
  char buffer_container[kBufferSize];
  Vector<char> buffer(buffer_container, kBufferSize);
  int length;
  int point;

  Vector<const PrecomputedShortestSingle> precomputed =
      PrecomputedShortestSingleRepresentations();
  for (int i = 0; i < precomputed.length(); ++i) {
    const PrecomputedShortestSingle current_test = precomputed[i];
    float v = current_test.v;
    BignumDtoa(v, BIGNUM_DTOA_SHORTEST_SINGLE, 0, buffer, &length, &point);
    CHECK_EQ(current_test.decimal_point, point);
    CHECK_EQ(current_test.representation, buffer.start());
  }
}

/*
TEST(BignumDtoaGayFixed) {
  char buffer_container[kBufferSize];
  Vector<char> buffer(buffer_container, kBufferSize);
  int length;
  int point;

  Vector<const PrecomputedFixed> precomputed =
      PrecomputedFixedRepresentations();
  for (int i = 0; i < precomputed.length(); ++i) {
    const PrecomputedFixed current_test = precomputed[i];
    float v = current_test.v;
    int number_digits = current_test.number_digits;
    BignumDtoa(v, BIGNUM_DTOA_FIXED, number_digits, buffer, &length, &point);
    CHECK_EQ(current_test.decimal_point, point);
    CHECK_GE(number_digits, length - point);
    TrimRepresentation(buffer);
    CHECK_EQ(current_test.representation, buffer.start());
  }
}


TEST(BignumDtoaGayPrecision) {
  char buffer_container[kBufferSize];
  Vector<char> buffer(buffer_container, kBufferSize);
  int length;
  int point;

  Vector<const PrecomputedPrecision> precomputed =
      PrecomputedPrecisionRepresentations();
  for (int i = 0; i < precomputed.length(); ++i) {
    const PrecomputedPrecision current_test = precomputed[i];
    float v = current_test.v;
    int number_digits = current_test.number_digits;
    BignumDtoa(v, BIGNUM_DTOA_PRECISION, number_digits,
               buffer, &length, &point);
    CHECK_EQ(current_test.decimal_point, point);
    CHECK_GE(number_digits, length);
    TrimRepresentation(buffer);
    CHECK_EQ(current_test.representation, buffer.start());
  }
}
*/