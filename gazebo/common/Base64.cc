/*
 * Copyright (C) 2012-2014 Open Source Robotics Foundation
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
 *
*/
/*
   base64.cpp and base64.h

   Copyright (C) 2004-2008 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
   claim that you wrote the original source code. If you use this source code
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch
*/
#include "gazebo/common/Base64.hh"

static const std::string base64Chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

/////////////////////////////////////////////////
static inline bool IsBase64(unsigned char _c)
{
  return (isalnum(_c) || (_c == '+') || (_c == '/'));
}

/////////////////////////////////////////////////
void Base64Encode(const char *_bytesToEncode, unsigned int _inLen,
    std::string &_result)
{
  int i = 0;
  int j = 0;
  unsigned char charArray3[3];
  unsigned char charArray4[4];

  while (_inLen--)
  {
    charArray3[i++] = *(_bytesToEncode++);
    if (i == 3)
    {
      charArray4[0] = (charArray3[0] & 0xfc) >> 2;
      charArray4[1] = ((charArray3[0] & 0x03) << 4) +
        ((charArray3[1] & 0xf0) >> 4);
      charArray4[2] = ((charArray3[1] & 0x0f) << 2) +
        ((charArray3[2] & 0xc0) >> 6);
      charArray4[3] = charArray3[2] & 0x3f;

      for (i = 0; i < 4; ++i)
        _result += base64Chars[charArray4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for (j = i; j < 3; ++j)
      charArray3[j] = '\0';

    charArray4[0] = (charArray3[0] & 0xfc) >> 2;
    charArray4[1] = ((charArray3[0] & 0x03) << 4) +
      ((charArray3[1] & 0xf0) >> 4);
    charArray4[2] = ((charArray3[1] & 0x0f) << 2) +
      ((charArray3[2] & 0xc0) >> 6);
    charArray4[3] = charArray3[2] & 0x3f;

    for (j = 0; (j < i + 1); ++j)
      _result += base64Chars[charArray4[j]];

    while ((i++ < 3))
      _result += '=';
  }
}

/////////////////////////////////////////////////
std::string Base64Decode(const std::string &_encodedString)
{
  int inLen = _encodedString.size();
  int i = 0;
  int j = 0;
  int in = 0;
  unsigned char charArray4[4], charArray3[3];
  std::string ret;

  while (inLen-- && (_encodedString[in] != '=') &&
      IsBase64(_encodedString[in]))
  {
    charArray4[i++] = _encodedString[in];
    in++;

    if (i == 4)
    {
      for (i = 0; i < 4; ++i)
        charArray4[i] = base64Chars.find(charArray4[i]);

      charArray3[0] = (charArray4[0] << 2) +
        ((charArray4[1] & 0x30) >> 4);
      charArray3[1] = ((charArray4[1] & 0xf) << 4) +
        ((charArray4[2] & 0x3c) >> 2);
      charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];

      for (i = 0; (i < 3); ++i)
        ret += charArray3[i];
      i = 0;
    }
  }

  if (i)
  {
    for (j = i; j <4; ++j)
      charArray4[j] = 0;

    for (j = 0; j <4; ++j)
      charArray4[j] = base64Chars.find(charArray4[j]);

    charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
    charArray3[1] = ((charArray4[1] & 0xf) << 4) +
      ((charArray4[2] & 0x3c) >> 2);
    charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];

    for (j = 0; (j < i - 1); ++j)
      ret += charArray3[j];
  }

  return ret;
}
