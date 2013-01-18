/*
 * Copyright 2012 Open Source Robotics Foundation
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
#include "Assert.hh"
#include "common/Exception.hh"

namespace boost
{
  void assertion_failed(char const *_expr, char const *_function,
                        char const *_file, long _line)
  {
    throw gazebo::common::AssertionInternalError(_file, _line,
                                                 _expr, _function);
  }


  void assertion_failed_msg(char const *_expr, char const *_msg,
                            char const *_function, char const *_file,
                            long _line)
  {
    throw gazebo::common::AssertionInternalError(_file, _line, _expr,
                                                 _function, _msg);
  }
}
