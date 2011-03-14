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
#ifndef MATERIAL_HH
#define MATERIAL_HH

#include <string>
#include <iostream>
#include "Color.hh"

namespace gazebo
{
	namespace common
{
  class Material
  {
    public: enum ShadeMode{FLAT, GOURAUD, PHONG, SHADE_COUNT};
    public: static std::string ShadeModeStr[SHADE_COUNT];  

    public: enum BlendMode{ADD, MODULATE, REPLACE, BLEND_COUNT};
    public: static std::string BlendModeStr[SHADE_COUNT];  
  
    /// \brief Constructor
    public: Material();

    /// \brief Destructor
    public: virtual ~Material();

    /// \brief Create a material with a default color 
    public: Material(const Color &clr);

    /// \brief Get the name of the material
    public: std::string GetName() const;

    /// \brief Set a texture image
    public: void SetTextureImage(const std::string tex);

    /// \brief Set a texture image
    public: void SetTextureImage(const std::string tex,const std::string resource_path);

    /// \brief Get a thie texture image
    public: std::string GetTextureImage() const;

    /// \brief Set the ambient color
    public: void SetAmbient(const Color &clr);
  
    /// \brief Get the ambient color
    public: Color GetAmbient() const;
  
    /// \brief Set the diffuse color
    public: void SetDiffuse(const Color &clr);
  
    /// \brief Get the diffuse color
    public: Color GetDiffuse() const;
  
    /// \brief Set the specular color
    public: void SetSpecular(const Color &clr);
  
    /// \brief Get the specular color
    public: Color GetSpecular() const;

    /// \brief Set the emissive color
    public: void SetEmissive(const Color &clr);
  
    /// \brief Get the emissive color
    public: Color GetEmissive() const;
 
    /// \brief Set the transparency percentage (0..1)
    public: void SetTransparency(float t);
  
    /// \brief Get the transparency percentage (0..1)
    public: float SetTransparency() const;
  
    /// \brief Set the shininess 
    public: void SetShininess(float t);

    /// \brief Get the shininess 
    public: float GetShininess() const;
  
    /// \brief Get the shininess 
    public: float GetTransparency() const;

    /// \brief Set the blending mode
    public: void SetBlendMode(BlendMode b);

    /// \brief Get the blending mode
    public: BlendMode GetBlendMode() const;

    /// \brief Set the shading mode
    public: void SetShadeMode(ShadeMode b);

    /// \brief Get the shading mode
    public: ShadeMode GetShadeMode() const;

    /// \brief Set the point size
    public: void SetPointSize(double size);

    /// \brief Get the point size
    public: double GetPointSize() const;

    /// \brief Ostream operator
    public: friend std::ostream &operator<<( std::ostream &out, 
                                             const gazebo::Material &m )
            {

              out << "Material:\n"; 
              out << "\tName: " << m.name << "\n";
              out << "\tTexture: " << m.texImage << "\n";
              out << "\tAmbient: " << m.ambient << "\n";
              out << "\tDiffuse: " << m.diffuse << "\n";
              out << "\tSpecular: " << m.specular << "\n";
              out << "\tEmissive: " << m.emissive << "\n";
              out << "\tTransparency: " << m.transparency << "\n";
              out << "\tShininess: " << m.shininess << "\n";
              out << "\tBlendMode: " << BlendModeStr[m.blendMode] << "\n";
              out << "\tShadeMode: " << ShadeModeStr[m.shadeMode] << "\n";
              return out;
            }

    private: void Update();

    private: std::string name;
    private: std::string texImage;
    private: Color ambient;
    private: Color diffuse;
    private: Color specular;
    private: Color emissive;
    private: float transparency;
    private: float shininess;
    private: double pointSize;

    private: BlendMode blendMode;
    private: ShadeMode shadeMode;

    private: static unsigned int counter;
  };
}

}
#endif
