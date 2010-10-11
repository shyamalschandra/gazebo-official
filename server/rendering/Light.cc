/*
 *  Gazebo - Outdoor Multi-Robot Simulator
 *  Copyright (C) 2003
 *     Nate Koenig & Andrew Howard
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* Desc: A Light
 * Author: Nate Koenig
 * Date: 15 July 2003
 */

#include <Ogre.h>
#include <boost/bind.hpp>

#include "Events.hh"
#include "Scene.hh"
#include "RTShaderSystem.hh"
#include "World.hh"
#include "Model.hh"
#include "OgreDynamicLines.hh"
#include "OgreVisual.hh"
#include "OgreCreator.hh"
#include "OgreAdaptor.hh"
#include "XMLConfig.hh"
#include "GazeboError.hh"
#include "Global.hh"
#include "GazeboMessage.hh"
#include "Light.hh"

using namespace gazebo;

unsigned int Light::lightCounter = 0;

////////////////////////////////////////////////////////////////////////////////
/// Constructor
Light::Light(Entity *parent, unsigned int sceneIndex)
  : Entity(parent)
{
  this->type.push_back("light");
  this->scene = OgreAdaptor::Instance()->GetScene(sceneIndex);

  std::ostringstream stream;

  if (parent)
    stream << parent->GetName() << "_";
  stream << "LIGHT" << this->lightCounter;
  this->SetName(stream.str());

  this->lightCounter++;

  Param::Begin(&this->parameters);
  this->lightTypeP = new ParamT<std::string>("type", std::string("point"), 1);
  this->lightTypeP->Callback(&Light::SetLightType, this);

  this->diffuseP  = new ParamT<Color>("diffuseColor", Color(.5, .5, .5, 1), 0);
  this->diffuseP->Callback(&Light::SetDiffuseColor, this);

  this->specularP = new ParamT<Color>("specularColor", Color(.1, .1, .1), 0);
  this->specularP->Callback(&Light::SetSpecularColor, this);

  this->directionP  = new ParamT<Vector3>("direction", Vector3(0, 0, -1), 0);
  this->directionP->Callback(&Light::SetDirection, this);

  this->attenuationP  = new ParamT<Vector3>("attenuation", Vector3(.1, 0.01, .001), 1);
  this->attenuationP->Callback(&Light::SetAttenuation, this);

  this->spotInnerAngleP = new ParamT<double>("innerAngle", 10, 0);
  this->spotInnerAngleP->Callback(&Light::SetSpotInnerAngle, this);

  this->spotOutterAngleP = new ParamT<double>("outerAngle", 20, 0);
  this->spotOutterAngleP->Callback(&Light::SetSpotOutterAngle, this);

  this->spotFalloffP = new ParamT<double>("falloff", 1, 0);
  this->spotFalloffP->Callback(&Light::SetSpotFalloff, this);

  this->rangeP  = new ParamT<double>("range",10,1);
  this->rangeP->Callback(&Light::SetRange, this);

  this->castShadowsP = new ParamT<bool>("castShadows",true,0);
  this->castShadowsP->Callback(&Light::SetCastShadows, this);
  Param::End();

  Events::ConnectShowLightsSignal(boost::bind(&Light::ToggleShowVisual, this));

  try
  {
    this->light = this->scene->GetManager()->createLight(this->GetName());
  }
  catch (Ogre::Exception e)
  {

    gzthrow("Ogre Error:" << e.getFullDescription() << "\n" << \
        "Unable to create a light");
  }

  RTShaderSystem::Instance()->UpdateShaders();
}

////////////////////////////////////////////////////////////////////////////////
/// Destructor
Light::~Light()
{
  if (this->light)
  {
    this->scene->GetManager()->destroyLight(this->GetName());
  }

  delete this->lightTypeP;
  delete this->diffuseP;
  delete this->specularP;
  delete this->directionP;
  delete this->attenuationP;
  delete this->rangeP;
  delete this->castShadowsP;
  delete this->spotInnerAngleP;
  delete this->spotOutterAngleP;
  delete this->spotFalloffP;

}

////////////////////////////////////////////////////////////////////////////////
/// Load the light
void Light::Load(XMLConfigNode *node)
{
  Vector3 vec;
  double range,constant,linear,quad;

  this->lightTypeP->Load(node);
  this->diffuseP->Load(node);
  this->specularP->Load(node);
  this->directionP->Load(node);
  this->attenuationP->Load(node);
  this->rangeP->Load(node);
  this->castShadowsP->Load(node);
  this->spotInnerAngleP->Load(node);
  this->spotOutterAngleP->Load(node);
  this->spotFalloffP->Load(node);

  this->SetLightType( **this->lightTypeP );
  this->SetDiffuseColor(**this->diffuseP);
  this->SetSpecularColor(**this->specularP);
  this->SetDirection(**this->directionP);
  this->SetAttenuation(**this->attenuationP);
  this->SetRange(**this->rangeP);
  this->SetCastShadows(**this->castShadowsP);
  this->SetSpotInnerAngle(**this->spotInnerAngleP);
  this->SetSpotOutterAngle(**this->spotOutterAngleP);
  this->SetSpotFalloff(**this->spotFalloffP);

  //this->light->setSpotlightRange(Ogre::Radian(20), Ogre::Radian(40),32.0), 
  //this->light->setSpotlightInnerAngle( Ogre::Radian(Ogre::Degree(20)) );
  //this->light->setSpotlightOuterAngle( Ogre::Radian(Ogre::Degree(40)) ); 

  // TODO: More options for Spot lights, etc.
  //  options for spotlights
  /*if ((**this->lightTypeP) == "spot")
  {
    vec = node->GetVector3("spotCone", Vector3(30.0, 65.0, 1.0));
    this->light->setSpotlightRange(Ogre::Degree(vec.x), 
                                   Ogre::Degree(vec.y), vec.z);
  }*/

  this->visualNode->AttachObject(light);

  this->CreateVisual();
  this->SetupShadows();
}

////////////////////////////////////////////////////////////////////////////////
// Save a light
void Light::Save(const std::string &prefix, std::ostream &stream)
{
  stream << prefix << "<light>\n";
  stream << prefix << "  " << *(this->lightTypeP) << "\n";
  stream << prefix << "  " << *(this->directionP) << "\n";
  stream << prefix << "  " << *(this->diffuseP) << "\n";
  stream << prefix << "  " << *(this->specularP) << "\n";
  stream << prefix << "  " << *(this->rangeP) << "\n";
  stream << prefix << "  " << *(this->attenuationP) << "\n";
  stream << prefix << "  " << *(this->spotInnerAngleP) << "\n";
  stream << prefix << "  " << *(this->spotOutterAngleP) << "\n";
  stream << prefix << "  " << *(this->spotFalloffP) << "\n";
  stream << prefix << "  " << *(this->castShadowsP) << "\n";
  stream << prefix << "</light>\n";
}


////////////////////////////////////////////////////////////////////////////////
// Helper node to create a visual representation of the light
void Light::CreateVisual()
{
  if (this->light->getType() == Ogre::Light::LT_DIRECTIONAL)
    return;

  // The lines draw a visualization of the camera
  this->line = this->visualNode->AddDynamicLine(
      OgreDynamicRenderable::OT_LINE_LIST);

  if ( **this->lightTypeP == "point" )
  {
    float s=0.1;
    this->line->AddPoint(Vector3(-s,-s,0));
    this->line->AddPoint(Vector3(-s,s,0));

    this->line->AddPoint(Vector3(-s,s,0));
    this->line->AddPoint(Vector3(s,s,0));

    this->line->AddPoint(Vector3(s,s,0));
    this->line->AddPoint(Vector3(s,-s,0));

    this->line->AddPoint(Vector3(s,-s,0));
    this->line->AddPoint(Vector3(-s,-s,0));


    this->line->AddPoint(Vector3(-s,-s,0));
    this->line->AddPoint(Vector3(0,0,s));

    this->line->AddPoint(Vector3(-s,s,0));
    this->line->AddPoint(Vector3(0,0,s));

    this->line->AddPoint(Vector3(s,s,0));
    this->line->AddPoint(Vector3(0,0,s));

    this->line->AddPoint(Vector3(s,-s,0));
    this->line->AddPoint(Vector3(0,0,s));



    this->line->AddPoint(Vector3(-s,-s,0));
    this->line->AddPoint(Vector3(0,0,-s));

    this->line->AddPoint(Vector3(-s,s,0));
    this->line->AddPoint(Vector3(0,0,-s));

    this->line->AddPoint(Vector3(s,s,0));
    this->line->AddPoint(Vector3(0,0,-s));

    this->line->AddPoint(Vector3(s,-s,0));
    this->line->AddPoint(Vector3(0,0,-s));

  }
  else if ( this->light->getType() == Ogre::Light::LT_SPOTLIGHT )
  {
    double innerAngle = this->light->getSpotlightInnerAngle().valueRadians();
    double outerAngle = this->light->getSpotlightOuterAngle().valueRadians();

    double angles[2];
    double range = 0.2;
    angles[0] = range * tan(outerAngle);
    angles[1] = range * tan(innerAngle);
    for (unsigned int i=0; i < 2; i++)
    {
      this->line->AddPoint(Vector3(0,0,0));
      this->line->AddPoint(Vector3(angles[i],angles[i], -range));

      this->line->AddPoint(Vector3(0,0,0));
      this->line->AddPoint(Vector3(-angles[i],-angles[i], -range));

      this->line->AddPoint(Vector3(0,0,0));
      this->line->AddPoint(Vector3(angles[i],-angles[i], -range));

      this->line->AddPoint(Vector3(0,0,0));
      this->line->AddPoint(Vector3(-angles[i],angles[i], -range));

      this->line->AddPoint(Vector3(angles[i],angles[i], -range));
      this->line->AddPoint(Vector3(-angles[i],angles[i], -range));

      this->line->AddPoint(Vector3(-angles[i],angles[i], -range));
      this->line->AddPoint(Vector3(-angles[i],-angles[i], -range));

      this->line->AddPoint(Vector3(-angles[i],-angles[i], -range));
      this->line->AddPoint(Vector3(angles[i],-angles[i], -range));

      this->line->AddPoint(Vector3(angles[i],-angles[i], -range));
      this->line->AddPoint(Vector3(angles[i],angles[i], -range));
    }
    
  }

  this->line->setMaterial("Gazebo/WhiteGlow");
  this->line->setVisibilityFlags(GZ_LASER_CAMERA);

  // turn off light source box visuals by default
  this->visualNode->SetVisible(true);
}

////////////////////////////////////////////////////////////////////////////////
/// Set whether this entity has been selected by the user through the gui
bool Light::SetSelected( bool s )
{
  Entity::SetSelected(s);

  if (this->light->getType() != Ogre::Light::LT_DIRECTIONAL)
  {
    if (s)
      this->line->setMaterial("Gazebo/PurpleGlow");
    else
      this->line->setMaterial("Gazebo/WhiteGlow");
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Toggle light visual visibility
void Light::ToggleShowVisual()
{
  this->visualNode->ToggleVisible();
}

////////////////////////////////////////////////////////////////////////////////
// Set whether to show the visual
void Light::ShowVisual(bool s)
{
  this->visualNode->SetVisible(s);
}

////////////////////////////////////////////////////////////////////////////////
/// Set the light type
void Light::SetLightType(const std::string &type)
{
  // Set the light type
  if (type == "point")
    this->light->setType(Ogre::Light::LT_POINT);
  else if (type == "directional")
  {
    this->light->setType(Ogre::Light::LT_DIRECTIONAL);
    if (this->parent && this->parent->HasType("model"))
      this->parent->GetParentModel()->SetStatic(true);
  }
  else if (type == "spot")
    this->light->setType(Ogre::Light::LT_SPOTLIGHT);

  if (**this->lightTypeP != type)
    this->lightTypeP->SetValue( type );
}

////////////////////////////////////////////////////////////////////////////////
/// Set the diffuse
void Light::SetDiffuseColor(const Color &color)
{
  if (**this->diffuseP != color)
    this->diffuseP->SetValue( color );

  this->light->setDiffuseColour(color.R(), color.G(), color.B());
}

////////////////////////////////////////////////////////////////////////////////
/// Set the specular color
void Light::SetSpecularColor(const Color &color)
{
  if (**this->specularP != color)
    this->specularP->SetValue( color );

  this->light->setSpecularColour(color.R(), color.G(), color.B());
}

////////////////////////////////////////////////////////////////////////////////
/// Set the direction
void Light::SetDirection(const Vector3 &dir)
{
  // Set the direction which the light points
  Vector3 vec = dir;
  vec.Normalize();

  if (**this->directionP != vec)
    this->directionP->SetValue( vec );

  this->light->setDirection(vec.x, vec.y, vec.z);
}

////////////////////////////////////////////////////////////////////////////////
/// Set the attenuation
void Light::SetAttenuation(const Vector3 &att)
{
  Vector3 vec = att;

  // Constant factor. 1.0 means never attenuate, 0.0 is complete attenuation
  if (vec.x < 0)
    vec.x = 0;
  else if (vec.x > 1.0)
    vec.x = 1.0;

  // Linear factor. 1 means attenuate evenly over the distance
  if (vec.y < 0)
    vec.y = 0;
  else if (vec.y > 1.0)
    vec.y = 1.0;

  if (**this->attenuationP != vec)
    this->attenuationP->SetValue( vec );

  // Set attenuation
  this->light->setAttenuation((**this->rangeP), vec.x, vec.y, vec.z);
}


////////////////////////////////////////////////////////////////////////////////
/// Set the range
void Light::SetRange(const double &range)
{
  if (**this->rangeP != range)
    this->rangeP->SetValue( range );

  this->light->setAttenuation(range, (**this->attenuationP).x, 
      (**this->attenuationP).y, (**this->attenuationP).z);
}

////////////////////////////////////////////////////////////////////////////////
/// Set cast shadowsj
void Light::SetCastShadows(const bool &cast)
{
  if (this->light->getType() == Ogre::Light::LT_POINT)
    this->light->setCastShadows(false);
  else
  {
    if (**this->castShadowsP != cast)
      this->castShadowsP->SetValue( cast );

    this->light->setCastShadows(cast);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Set the spot light inner angle
void Light::SetSpotInnerAngle(const double &angle)
{
  if (**this->spotInnerAngleP != angle)
    this->spotInnerAngleP->SetValue( angle );

  if (this->light->getType() == Ogre::Light::LT_SPOTLIGHT)
    this->light->setSpotlightRange(
        Ogre::Degree(**this->spotInnerAngleP), 
        Ogre::Degree(**this->spotOutterAngleP), 
        **this->spotFalloffP);
}

////////////////////////////////////////////////////////////////////////////////
/// Set the spot light outter angle
void Light::SetSpotOutterAngle(const double &angle)
{
  if (**this->spotOutterAngleP != angle)
    this->spotOutterAngleP->SetValue( angle );

  if (this->light->getType() == Ogre::Light::LT_SPOTLIGHT)
    this->light->setSpotlightRange(
        Ogre::Degree(**this->spotInnerAngleP), 
        Ogre::Degree(**this->spotOutterAngleP), 
        **this->spotFalloffP);
}

////////////////////////////////////////////////////////////////////////////////
/// Set the spot light falloff
void Light::SetSpotFalloff(const double &angle)
{
  if (**this->spotFalloffP != angle)
    this->spotFalloffP->SetValue( angle );

  if (this->light->getType() == Ogre::Light::LT_SPOTLIGHT)
    this->light->setSpotlightRange(
        Ogre::Degree(**this->spotInnerAngleP), 
        Ogre::Degree(**this->spotOutterAngleP), 
        **this->spotFalloffP);

}

////////////////////////////////////////////////////////////////////////////////
// Setup the shadow camera for the light
void Light::SetupShadows()
{
  if (this->light->getType() == Ogre::Light::LT_DIRECTIONAL)
  {
    unsigned int numShadowTextures = 3;

    // shadow camera setup
    Ogre::PSSMShadowCameraSetup* pssmSetup = new Ogre::PSSMShadowCameraSetup();

    Ogre::PSSMShadowCameraSetup::SplitPointList splitPointList = pssmSetup->getSplitPoints();

    // These were hand tuned by me (Nate)...hopefully they work for all cases.
    splitPointList[0] = 0.1;
    splitPointList[1] = 10.5;
    splitPointList[2] = 20.0;

    pssmSetup->setSplitPoints(splitPointList);
    pssmSetup->setSplitPadding(5.2);
    pssmSetup->setUseSimpleOptimalAdjust(true);

    // set the LISPM adjustment factor (see API documentation for these)
    /*pssmSetup->setOptimalAdjustFactor(0, 5.0);
    pssmSetup->setOptimalAdjustFactor(1, 3.0);
    pssmSetup->setOptimalAdjustFactor(2, 1.0);
    */

    this->light->setCustomShadowCameraSetup(Ogre::ShadowCameraSetupPtr(pssmSetup));
    //this->manager->setShadowCameraSetup(Ogre::ShadowCameraSetupPtr(pssmSetup));

    Ogre::Vector4 splitPoints;
    for (int i = 0; i < numShadowTextures; ++i)
      splitPoints[i] = splitPointList[i];

    Ogre::MaterialManager::ResourceMapIterator iter = Ogre::MaterialManager::getSingleton().getResourceIterator();

    // Iterate over all the materials, and set the pssm split points
    while(iter.hasMoreElements())
    {
      Ogre::MaterialPtr mat = iter.getNext();
      for(int i = 0; i < mat->getNumTechniques(); i++) 
      {
        Ogre::Technique *tech = mat->getTechnique(i);
        for(int j = 0; j < tech->getNumPasses(); j++) 
        {
          Ogre::Pass *pass = tech->getPass(j);
          if (pass->hasFragmentProgram())
          {
            Ogre::GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();
            if (params->_findNamedConstantDefinition("pssm_split_points"))
              params->setNamedConstant("pssm_split_points", splitPoints);
          }
        }
      }
    }
  }
  else if (this->light->getType() == Ogre::Light::LT_SPOTLIGHT)
  {
    this->light->setCustomShadowCameraSetup(Ogre::ShadowCameraSetupPtr(new Ogre::DefaultShadowCameraSetup()));
  }
}
