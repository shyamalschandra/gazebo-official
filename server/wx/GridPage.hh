/*
 * Copyright 2011 Nate Koenig & Andrew Howard
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
#ifndef GRIDPAGE_HH
#define GRIDPAGE_HH

#include <wx/textctrl.h>
#include <wx/clrpicker.h>

#include "ParamPage.hh"

namespace gazebo
{
  class GridPage : public ParamPage
  {
    public: GridPage(wxWindow *parent);
    public: virtual ~GridPage();

    public: virtual void Apply();

    /// \brief On colour changed
    private: void OnColorChange(wxColourPickerEvent &event);

    private: wxTextCtrl *gridSizeCtrl;
    private: wxTextCtrl *spacingCtrl;
    private: wxColourPickerCtrl *colorCtrl;
  };
}

#endif
