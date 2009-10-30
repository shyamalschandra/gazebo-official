#include <algorithm>
#include "Material.hh"

using namespace gazebo;

std::string Material::ShadeModeStr[SHADE_COUNT] = {"FLAT", "GOURAUD", "PHONG"};

std::string Material::BlendModeStr[BLEND_COUNT] = {"ADD", "MODULATE", "REPLACE"};

////////////////////////////////////////////////////////////////////////////////
/// Constructor
Material::Material()
{
  this->name = "noname";
  this->blendMode = REPLACE;
  this->shadeMode= GOURAUD;
  this->transparency = 0;
  this->shininess = 0;
}

////////////////////////////////////////////////////////////////////////////////
/// Destructor
Material::~Material()
{
}

////////////////////////////////////////////////////////////////////////////////
/// Set the name of the material
void Material::SetName(const std::string &name)
{
  this->name = name;
}

////////////////////////////////////////////////////////////////////////////////
/// Get the name of the material
std::string Material::GetName() const
{
  return this->name;
}

////////////////////////////////////////////////////////////////////////////////
/// Set the ambient color
void Material::SetAmbient(const Color &clr)
{
  this->ambient = clr;
}

////////////////////////////////////////////////////////////////////////////////
/// Get the ambient color
Color Material::GetAmbient() const
{
  return this->ambient;
}

////////////////////////////////////////////////////////////////////////////////
/// Set the diffuse color
void Material::SetDiffuse(const Color &clr)
{
  this->diffuse = clr;
}

////////////////////////////////////////////////////////////////////////////////
/// Get the diffuse color
Color Material::GetDiffuse() const
{
  return this->diffuse;
}

////////////////////////////////////////////////////////////////////////////////
/// Set the specular color
void Material::SetSpecular(const Color &clr)
{
  this->specular = clr;
}

////////////////////////////////////////////////////////////////////////////////
/// Get the specular color
Color Material::GetSpecular() const
{
  return this->specular;
}

////////////////////////////////////////////////////////////////////////////////
/// Set the emissive color
void Material::SetEmissive(const Color &clr)
{
  this->emissive = clr;
}

////////////////////////////////////////////////////////////////////////////////
/// Get the emissive color
Color Material::GetEmissive() const
{
  return this->emissive;
}

////////////////////////////////////////////////////////////////////////////////
/// Set the transparency percentage (0..1)
void Material::SetTransparency(float t)
{
  this->transparency = std::min(t, (float)1.0);
  this->transparency = std::max(this->transparency, (float)0.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Get the transparency percentage (0..1)
float Material::SetTransparency() const
{
  return this->transparency;
}

////////////////////////////////////////////////////////////////////////////////
/// Set the shininess 
void Material::SetShininess(float s)
{
  this->shininess = s;
}

////////////////////////////////////////////////////////////////////////////////
/// Get the shininess 
float Material::GetShininess() const
{
  return this->shininess;
}

////////////////////////////////////////////////////////////////////////////////
/// Get the shininess 
float Material::GetTransparency() const
{
  return this->shininess;
}

////////////////////////////////////////////////////////////////////////////////
/// Set the blending mode
void Material::SetBlendMode(BlendMode b)
{
  this->blendMode = b;
}

////////////////////////////////////////////////////////////////////////////////
/// Get the blending mode
Material::BlendMode Material::GetBlendMode() const
{
  return this->blendMode;
}

////////////////////////////////////////////////////////////////////////////////
/// Set the shading mode
void Material::SetShadeMode(ShadeMode s)
{
  this->shadeMode = s;
}

////////////////////////////////////////////////////////////////////////////////
/// Get the shading mode
Material::ShadeMode Material::GetShadeMode() const
{
  return this->shadeMode;
}
