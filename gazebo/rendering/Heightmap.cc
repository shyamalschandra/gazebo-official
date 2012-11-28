/*
 * Copyright 2011 Nate Koenig
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
/* Desc: Heightmap geometry
 * Author: Nate Koenig
 * Date: 12 May 2009
 */

#include <string.h>
#include <math.h>

#include "gazebo/common/Common.hh"
#include "gazebo/common/Exception.hh"

#include "gazebo/math/Helpers.hh"

#include "gazebo/rendering/RTShaderSystem.hh"
#include "gazebo/rendering/Scene.hh"
#include "gazebo/rendering/Light.hh"
#include "gazebo/rendering/Conversions.hh"
#include "gazebo/rendering/Heightmap.hh"

using namespace gazebo;
using namespace rendering;

//////////////////////////////////////////////////
Heightmap::Heightmap(ScenePtr _scene)
{
  this->scene = _scene;
}

//////////////////////////////////////////////////
Heightmap::~Heightmap()
{
  this->scene.reset();
}

//////////////////////////////////////////////////
void Heightmap::LoadFromMsg(ConstVisualPtr &_msg)
{
  msgs::Set(this->heightImage, _msg->geometry().heightmap().image());
  this->terrainSize = msgs::Convert(_msg->geometry().heightmap().size());
  this->terrainOrigin = msgs::Convert(_msg->geometry().heightmap().origin());

  for (int i = 0; i < _msg->geometry().heightmap().texture_size(); ++i)
  {
    this->diffuseTextures.push_back(common::find_file(
        _msg->geometry().heightmap().texture(i).diffuse()));
    this->normalTextures.push_back(common::find_file(
        _msg->geometry().heightmap().texture(i).normal()));
    this->worldSizes.push_back(
        _msg->geometry().heightmap().texture(i).size());
  }

  for (int i = 0; i < _msg->geometry().heightmap().blend_size(); ++i)
  {
    this->blendHeight.push_back(
        _msg->geometry().heightmap().blend(i).min_height());
    this->blendFade.push_back(
        _msg->geometry().heightmap().blend(i).fade_dist());
  }

  this->Load();
}

//////////////////////////////////////////////////
void Heightmap::Load()
{
  this->terrainGlobals = new Ogre::TerrainGlobalOptions();

  if (this->heightImage.GetWidth() != this->heightImage.GetHeight() ||
      !math::isPowerOfTwo(this->heightImage.GetWidth() - 1))
  {
    gzthrow("Heightmap image size must be square, with a size of 2^n+1\n");
  }

  this->imageSize = this->heightImage.GetWidth();
  this->maxPixel = this->heightImage.GetMaxColor().r;

  if (math::equal(this->maxPixel, 0.0))
    this->maxPixel = 1.0;

  // Create terrain group, which holds all the individual terrain instances.
  // Param 1: Pointer to the scene manager
  // Param 2: Alignment plane
  // Param 3: Number of vertices along one edge of the terrain (2^n+1).
  //          Terrains must be square, with each side a power of 2 in size
  // Param 4: World size of each terrain instance, in meters.
  this->terrainGroup = new Ogre::TerrainGroup(
      this->scene->GetManager(), Ogre::Terrain::ALIGN_X_Y,
      this->imageSize, this->terrainSize.x);

  this->terrainGroup->setFilenameConvention(
      Ogre::String("gazebo_terrain"), Ogre::String("dat"));

  this->terrainGroup->setOrigin(Conversions::Convert(this->terrainOrigin));

  this->ConfigureTerrainDefaults();

  this->SetupShadows(true);

  for (int x = 0; x <= 0; ++x)
    for (int y = 0; y <= 0; ++y)
      this->DefineTerrain(x, y);

  // sync load since we want everything in place when we start
  this->terrainGroup->loadAllTerrains(true);

  // Calculate blend maps
  if (this->terrainsImported)
  {
    Ogre::TerrainGroup::TerrainIterator ti =
      this->terrainGroup->getTerrainIterator();
    while (ti.hasMoreElements())
    {
      Ogre::Terrain *t = ti.getNext()->instance;
      this->InitBlendMaps(t);
    }
  }

  this->terrainGroup->freeTemporaryResources();
}

///////////////////////////////////////////////////
void Heightmap::ConfigureTerrainDefaults()
{
  // Configure global

  // MaxPixelError: Decides how precise our terrain is going to be.
  // A lower number will mean a more accurate terrain, at the cost of
  // performance (because of more vertices)
  this->terrainGlobals->setMaxPixelError(5);

  // CompositeMapDistance: decides how far the Ogre terrain will render
  // the lightmapped terrain.
  this->terrainGlobals->setCompositeMapDistance(1000);

  // Get the first directional light
  LightPtr directionalLight;
  for (unsigned int i = 0; i < this->scene->GetLightCount(); ++i)
  {
    LightPtr light = this->scene->GetLight(i);
    if (light->GetType() == "directional")
    {
      directionalLight = light;
      break;
    }
  }

  this->terrainGlobals->setCompositeMapAmbient(
      this->scene->GetManager()->getAmbientLight());

  // Important to set these so that the terrain knows what to use for
  // derived (non-realtime) data
  if (directionalLight)
  {
    this->terrainGlobals->setLightMapDirection(
        Conversions::Convert(directionalLight->GetDirection()));
    this->terrainGlobals->setCompositeMapDiffuse(
        Conversions::Convert(directionalLight->GetDiffuseColor()));
  }
  else
  {
    this->terrainGlobals->setLightMapDirection(Ogre::Vector3(0, 0, -1));
    this->terrainGlobals->setCompositeMapDiffuse(
        Ogre::ColourValue(.6, .6, .6, 1));
  }

  // Configure default import settings for if we use imported image
  Ogre::Terrain::ImportData &defaultimp =
    this->terrainGroup->getDefaultImportSettings();

  defaultimp.terrainSize = this->imageSize;
  defaultimp.worldSize = this->terrainSize.x;

  defaultimp.inputScale = this->terrainSize.z / this->maxPixel;

  defaultimp.minBatchSize = 33;
  defaultimp.maxBatchSize = 65;

  // textures. The default material generator takes two materials per layer.
  //    1. diffuse_specular - diffuse texture with a specular map in the
  //    alpha channel
  //    2. normal_height - normal map with a height map in the alpha channel
  {
    // number of texture layers
    defaultimp.layerList.resize(this->diffuseTextures.size());

    // The worldSize decides how big each splat of textures will be.
    // A smaller value will increase the resolution
    for (unsigned int i = 0; i < this->diffuseTextures.size(); ++i)
    {
      defaultimp.layerList[i].worldSize = this->worldSizes[i];
      defaultimp.layerList[i].textureNames.push_back(this->diffuseTextures[i]);
      defaultimp.layerList[i].textureNames.push_back(this->normalTextures[i]);
    }
  }
}

/////////////////////////////////////////////////
void Heightmap::DefineTerrain(int x, int y)
{
  Ogre::String filename = this->terrainGroup->generateFilename(x, y);

  if (Ogre::ResourceGroupManager::getSingleton().resourceExists(
        this->terrainGroup->getResourceGroup(), filename))
  {
    this->terrainGroup->defineTerrain(x, y);
  }
  else
  {
    Ogre::Image img;
    bool flipX = x % 2 != 0;
    bool flipY = y % 2 != 0;

    unsigned char *data = NULL;
    unsigned int count = 0;
    this->heightImage.GetData(&data, count);

    if (this->heightImage.GetPixelFormat() == common::Image::L_INT8)
    {
      img.loadDynamicImage(data, this->heightImage.GetWidth(),
          this->heightImage.GetHeight(), Ogre::PF_L8);
    }
    else if (this->heightImage.GetPixelFormat() == common::Image::RGBA_INT8)
    {
      img.loadDynamicImage(data, this->heightImage.GetWidth(),
          this->heightImage.GetHeight(), Ogre::PF_R8G8B8A8);
    }
    else if (this->heightImage.GetPixelFormat() == common::Image::RGB_INT8)
    {
      img.loadDynamicImage(data, this->heightImage.GetWidth(),
          this->heightImage.GetHeight(), Ogre::PF_R8G8B8);
    }
    else
    {
      gzerr << "Unable to handle image format["
            << this->heightImage.GetPixelFormat() << "]\n";
    }

    if (flipX)
      img.flipAroundY();
    if (flipY)
      img.flipAroundX();

    this->terrainGroup->defineTerrain(x, y, &img);
    this->terrainsImported = true;

    // delete [] data;
  }
}

/////////////////////////////////////////////////
bool Heightmap::InitBlendMaps(Ogre::Terrain *_terrain)
{
  if (!_terrain)
  {
    std::cerr << "Invalid  terrain\n";
    return false;
  }

  Ogre::Real val, height;
  unsigned int i = 0;

  std::vector<Ogre::TerrainLayerBlendMap *> blendMaps;
  std::vector<float*> pBlend;

  // Create the blend maps
  for (i = 0; i < this->blendHeight.size(); ++i)
  {
    blendMaps.push_back(_terrain->getLayerBlendMap(i+1));
    pBlend.push_back(blendMaps[i]->getBlendPointer());
  }

  // Set the blend values based on the height of the terrain
  for (Ogre::uint16 y = 0; y < _terrain->getLayerBlendMapSize(); ++y)
  {
    for (Ogre::uint16 x = 0; x < _terrain->getLayerBlendMapSize(); ++x)
    {
      Ogre::Real tx, ty;

      blendMaps[0]->convertImageToTerrainSpace(x, y, &tx, &ty);
      height = _terrain->getHeightAtTerrainPosition(tx, ty);

      for (i = 0; i < this->blendHeight.size(); ++i)
      {
        val = (height - this->blendHeight[i]) / this->blendFade[i];
        val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
        *pBlend[i]++ = val;
      }
    }
  }

  // Make sure the blend maps are properly updated
  for (i = 0; i < blendMaps.size(); ++i)
  {
    blendMaps[i]->dirty();
    blendMaps[i]->update();
  }

  return true;
}

/////////////////////////////////////////////////
double Heightmap::GetHeight(double x, double y)
{
  return this->terrainGroup->getHeightAtWorldPosition(x, y, 600);
}

/////////////////////////////////////////////////
void Heightmap::SetupShadows(bool _enableShadows)
{
  // Assume we get a shader model 2 material profile
  Ogre::TerrainMaterialGeneratorA::SM2Profile *matProfile;

  // RTSS PSSM shadows compatible terrain material
  Ogre::TerrainMaterialGenerator *matGen =
    new GzTerrainMatGen();

  Ogre::TerrainMaterialGeneratorPtr ptr = Ogre::TerrainMaterialGeneratorPtr();
  ptr.bind(matGen);

  this->terrainGlobals->setDefaultMaterialGenerator(ptr);
  matProfile = static_cast<GzTerrainMatGen::SM2Profile*>(
      matGen->getActiveProfile());
  if (!matProfile)
    gzerr << "Invalid mat profile\n";

  if (_enableShadows)
  {
    // Make sure PSSM is already setup
    matProfile->setReceiveDynamicShadowsEnabled(true);
    matProfile->setReceiveDynamicShadowsPSSM(
        RTShaderSystem::Instance()->GetPSSMShadowCameraSetup());
    matProfile->setReceiveDynamicShadowsDepth(true);
    matProfile->setReceiveDynamicShadowsLowLod(false);
  }
  else
  {
    matProfile->setReceiveDynamicShadowsPSSM(NULL);
  }
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
// GzTerrainMatGen
/////////////////////////////////////////////////
/////////////////////////////////////////////////


/////////////////////////////////////////////////
GzTerrainMatGen::GzTerrainMatGen()
: TerrainMaterialGeneratorA()
{
  /// \TODO - This will have to be changed if TerrainMaterialGeneratorA
  /// ever supports more profiles than only CG

  // Add custom SM2Profile SPAM
  this->mProfiles.clear();

  this->mProfiles.push_back(OGRE_NEW SM2Profile(this, "SM2",
        "Profile for rendering on Shader Model 2 capable cards "
        "(RTSS depth shadows compatible)"));

  /// \TODO - check hardware capabilities & use fallbacks if required
  /// (more profiles needed)
  this->setActiveProfile(this->mProfiles[0]);
}

/////////////////////////////////////////////////
GzTerrainMatGen::~GzTerrainMatGen()
{
}

/////////////////////////////////////////////////
GzTerrainMatGen::SM2Profile::SM2Profile(
    Ogre::TerrainMaterialGenerator *_parent, const Ogre::String &_name,
    const Ogre::String &_desc)
: TerrainMaterialGeneratorA::SM2Profile::SM2Profile(_parent, _name, _desc)
{
}

/////////////////////////////////////////////////
GzTerrainMatGen::SM2Profile::~SM2Profile()
{
  // Because the base SM2Profile has no virtual destructor:
  OGRE_DELETE this->mShaderGen;
}

/////////////////////////////////////////////////
void GzTerrainMatGen::SM2Profile::addTechnique(
    const Ogre::MaterialPtr &_mat, const Ogre::Terrain *_terrain,
    TechniqueType _tt)
{
  // Initiate specialized mShaderGen
  Ogre::GpuProgramManager &gmgr = Ogre::GpuProgramManager::getSingleton();

  Ogre::HighLevelGpuProgramManager &hmgr =
    Ogre::HighLevelGpuProgramManager::getSingleton();

  if (!this->mShaderGen)
  {
    bool check2x = this->mLayerNormalMappingEnabled ||
                   this->mLayerParallaxMappingEnabled;

    if (hmgr.isLanguageSupported("cg"))
    {
      this->mShaderGen =
        OGRE_NEW GzTerrainMatGen::SM2Profile::ShaderHelperCg();
    }
    else if (hmgr.isLanguageSupported("hlsl") &&
             ((check2x && gmgr.isSyntaxSupported("ps_4_0")) ||
              (check2x && gmgr.isSyntaxSupported("ps_2_x")) ||
              (!check2x && gmgr.isSyntaxSupported("ps_2_0"))))
    {
      this->mShaderGen = OGRE_NEW ShaderHelperHLSL();
    }
    else if (hmgr.isLanguageSupported("glsl"))
    {
      this->mShaderGen = OGRE_NEW ShaderHelperGLSL();
    }
    else if (hmgr.isLanguageSupported("glsles"))
    {
      this->mShaderGen = OGRE_NEW ShaderHelperGLSLES();
    }
    else
    {
      gzthrow("No supported shader languages");
    }

    // check SM3 features
    this->mSM3Available =
      Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("ps_3_0");
    this->mSM4Available =
      Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("ps_4_0");
  }

  // Unfortunately this doesn't work
  // Default implementation
  //TerrainMaterialGeneratorA::SM2Profile::addTechnique(mat, terrain, tt);

  // So we have to replicate the entire method:
  Ogre::Technique *tech = _mat->createTechnique();

  // Only supporting one pass
  Ogre::Pass *pass = tech->createPass();

  // Doesn't delegate to the proper method otherwise
  Ogre::HighLevelGpuProgramPtr vprog =
    ((GzTerrainMatGen::SM2Profile::ShaderHelperCg*)this->mShaderGen)->
    generateVertexProgram(this, _terrain, _tt);

  Ogre::HighLevelGpuProgramPtr fprog =
    this->mShaderGen->generateFragmentProgram(this, _terrain, _tt);

  pass->setVertexProgram(vprog->getName());
  pass->setFragmentProgram(fprog->getName());

  if (_tt == HIGH_LOD || _tt == RENDER_COMPOSITE_MAP)
  {
    // global normal map
    Ogre::TextureUnitState* tu = pass->createTextureUnitState();
    tu->setTextureName(_terrain->getTerrainNormalMap()->getName());
    tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

    // global colour map
    if (_terrain->getGlobalColourMapEnabled() &&
        this->isGlobalColourMapEnabled())
    {
      tu = pass->createTextureUnitState(
          _terrain->getGlobalColourMap()->getName());
      tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
    }

    // light map
    if (this->isLightmapEnabled())
    {
      tu = pass->createTextureUnitState(_terrain->getLightmap()->getName());
      tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
    }

    // blend maps
    unsigned int maxLayers = this->getMaxLayers(_terrain);

    unsigned int numBlendTextures = std::min(
        _terrain->getBlendTextureCount(maxLayers),
        _terrain->getBlendTextureCount());

    unsigned int numLayers = std::min(
        maxLayers, static_cast<unsigned int>(_terrain->getLayerCount()));

    for (unsigned int i = 0; i < numBlendTextures; ++i)
    {
      tu = pass->createTextureUnitState(_terrain->getBlendTextureName(i));
      tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
    }

    // layer textures
    for (unsigned int i = 0; i < numLayers; ++i)
    {
      // diffuse / specular
      pass->createTextureUnitState(_terrain->getLayerTextureName(i, 0));

      // normal / height
      pass->createTextureUnitState(_terrain->getLayerTextureName(i, 1));
    }
  }
  else
  {
    // LOW_LOD textures
    // composite map
    Ogre::TextureUnitState *tu = pass->createTextureUnitState();
    tu->setTextureName(_terrain->getCompositeMap()->getName());
    tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
  }

  // Add shadow textures (always at the end)
  if (this->isShadowingEnabled(_tt, _terrain))
  {
    unsigned int numTextures = 1;

    if (this->getReceiveDynamicShadowsPSSM())
    {
      numTextures = this->getReceiveDynamicShadowsPSSM()->getSplitCount();
    }
    for (unsigned int i = 0; i < numTextures; ++i)
    {
      Ogre::TextureUnitState *tu = pass->createTextureUnitState();
      tu->setContentType(Ogre::TextureUnitState::CONTENT_SHADOW);
      tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
      tu->setTextureBorderColour(Ogre::ColourValue::White);
    }
  }
}

/////////////////////////////////////////////////
// generate() and generateForCompositeMap() are identical to
// TerrainMaterialGeneratorA implementation, the only reason for repeating
// them is that, unfortunately, addTechnique() is not declared virtual.
Ogre::MaterialPtr GzTerrainMatGen::SM2Profile::generate(
    const Ogre::Terrain *_terrain)
{
  // re-use old material if exists
  Ogre::MaterialPtr mat = _terrain->_getMaterial();

  if (mat.isNull())
  {
    Ogre::MaterialManager &matMgr = Ogre::MaterialManager::getSingleton();

    // it's important that the names are deterministic for a given terrain, so
    // use the terrain pointer as an ID
    const Ogre::String &matName = _terrain->getMaterialName();
    mat = matMgr.getByName(matName);

    if (mat.isNull())
    {
      mat = matMgr.create(matName,
          Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    }
  }

  // clear everything
  mat->removeAllTechniques();

  // Automatically disable normal & parallax mapping if card cannot handle it
  // We do this rather than having a specific technique for it since it's
  // simpler.
  Ogre::GpuProgramManager &gmgr = Ogre::GpuProgramManager::getSingleton();

  if (!gmgr.isSyntaxSupported("ps_4_0") &&
      !gmgr.isSyntaxSupported("ps_3_0") &&
      !gmgr.isSyntaxSupported("ps_2_x") &&
      !gmgr.isSyntaxSupported("fp40") &&
      !gmgr.isSyntaxSupported("arbfp1"))
  {
    this->setLayerNormalMappingEnabled(false);
    this->setLayerParallaxMappingEnabled(false);
  }

  this->addTechnique(mat, _terrain, HIGH_LOD);

  // LOD
  if(this->mCompositeMapEnabled)
  {
    this->addTechnique(mat, _terrain, LOW_LOD);
    Ogre::Material::LodValueList lodValues;
    lodValues.push_back(
        Ogre::TerrainGlobalOptions::getSingleton().getCompositeMapDistance());

    mat->setLodLevels(lodValues);
    Ogre::Technique *lowLodTechnique = mat->getTechnique(1);
    lowLodTechnique->setLodIndex(1);
  }

  this->updateParams(mat, _terrain);

  return mat;
}

Ogre::MaterialPtr GzTerrainMatGen::SM2Profile::generateForCompositeMap(
    const Ogre::Terrain *_terrain)
{
  // re-use old material if exists
  Ogre::MaterialPtr mat = _terrain->_getCompositeMapMaterial();

  if (mat.isNull())
  {
    Ogre::MaterialManager &matMgr = Ogre::MaterialManager::getSingleton();

    // it's important that the names are deterministic for a given terrain, so
    // use the terrain pointer as an ID
    const Ogre::String &matName = _terrain->getMaterialName() + "/comp";

    mat = matMgr.getByName(matName);

    if (mat.isNull())
    {
      mat = matMgr.create(matName,
          Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    }
  }

  // clear everything
  mat->removeAllTechniques();

  this->addTechnique(mat, _terrain, RENDER_COMPOSITE_MAP);

  this->updateParamsForCompositeMap(mat, _terrain);

  return mat;
}

/////////////////////////////////////////////////
void GzTerrainMatGen::SM2Profile::ShaderHelperCg::defaultVpParams(
    const SM2Profile *_prof, const Ogre::Terrain *_terrain,
    TechniqueType _tt, const Ogre::HighLevelGpuProgramPtr &_prog)
{

  Ogre::GpuProgramParametersSharedPtr params = _prog->getDefaultParameters();
  params->setIgnoreMissingParams(true);

  params->setNamedAutoConstant("worldMatrix",
      Ogre::GpuProgramParameters::ACT_WORLD_MATRIX);

  params->setNamedAutoConstant("viewProjMatrix",
      Ogre::GpuProgramParameters::ACT_VIEWPROJ_MATRIX);

  params->setNamedAutoConstant("lodMorph",
      Ogre::GpuProgramParameters::ACT_CUSTOM,
      Ogre::Terrain::LOD_MORPH_CUSTOM_PARAM);

  params->setNamedAutoConstant("fogParams",
      Ogre::GpuProgramParameters::ACT_FOG_PARAMS);

  if (_prof->isShadowingEnabled(_tt, _terrain))
  {
    unsigned int numTextures = 1;
    if (_prof->getReceiveDynamicShadowsPSSM())
    {
      numTextures = _prof->getReceiveDynamicShadowsPSSM()->getSplitCount();
    }
    for (unsigned int i = 0; i < numTextures; ++i)
    {
      params->setNamedAutoConstant("texViewProjMatrix" +
          Ogre::StringConverter::toString(i),
          Ogre::GpuProgramParameters::ACT_TEXTURE_VIEWPROJ_MATRIX, i);

      // Don't add depth range params
      //if (prof->getReceiveDynamicShadowsDepth())
      //{
      //  params->setNamedAutoConstant("depthRange" +
      //      Ogre::StringConverter::toString(i),
      //      Ogre::GpuProgramParameters::ACT_SHADOW_SCENE_DEPTH_RANGE, i);
      //}
    }
  }

  if (_terrain->_getUseVertexCompression() && _tt != RENDER_COMPOSITE_MAP)
  {
    Ogre::Matrix4 posIndexToObjectSpace;
    _terrain->getPointTransform(&posIndexToObjectSpace);
    params->setNamedConstant("posIndexToObjectSpace", posIndexToObjectSpace);
  }
}

/////////////////////////////////////////////////
void
GzTerrainMatGen::SM2Profile::ShaderHelperCg::generateVpDynamicShadows(
    const SM2Profile *_prof, const Ogre::Terrain * /*_terrain*/,
    TechniqueType /*_tt*/, Ogre::StringUtil::StrStreamType &_outStream)
{
  unsigned int numTextures = 1;

  if (_prof->getReceiveDynamicShadowsPSSM())
  {
    numTextures = _prof->getReceiveDynamicShadowsPSSM()->getSplitCount();
  }

  // Calculate the position of vertex in light space
  for (unsigned int i = 0; i < numTextures; ++i)
  {
    _outStream << "   oLightSpacePos" << i << " = mul(texViewProjMatrix"
               << i << ", worldPos); \n";

    // Don't linearize depth range: RTSS PSSM implementation uses
    // view-space depth
    //if (prof->getReceiveDynamicShadowsDepth())
    //{
    //  // make linear
    //  outStream << "oLightSpacePos" << i << ".z = (oLightSpacePos" << i
    //            << ".z - depthRange" << i << ".x) * depthRange" << i
    //            << ".w;\n";
    //}
  }

  if (_prof->getReceiveDynamicShadowsPSSM())
  {
    _outStream << "   // pass cam depth\n   oUVMisc.z = oPos.z;\n";
  }
}

/////////////////////////////////////////////////
unsigned int GzTerrainMatGen::SM2Profile::ShaderHelperCg::
generateVpDynamicShadowsParams(unsigned int _texCoord, const SM2Profile *_prof,
    const Ogre::Terrain * /*_terrain*/, TechniqueType /*_tt*/,
    Ogre::StringUtil::StrStreamType &_outStream)
{

  // out semantics & params
  unsigned int numTextures = 1;

  if (_prof->getReceiveDynamicShadowsPSSM())
  {
    numTextures = _prof->getReceiveDynamicShadowsPSSM()->getSplitCount();
  }

  for (unsigned int i = 0; i < numTextures; ++i)
  {
    _outStream << ", out float4 oLightSpacePos" << i
               << " : TEXCOORD" << _texCoord++ << " \n"
               << ", uniform float4x4 texViewProjMatrix" << i << " \n";

    // Don't add depth range params
    // if (prof->getReceiveDynamicShadowsDepth())
    // {
    //   _outStream << ", uniform float4 depthRange" << i
    //             << " // x = min, y = max, z = range, w = 1/range \n";
    // }
  }

  return _texCoord;
}

/////////////////////////////////////////////////
// This method is identical to
// TerrainMaterialGeneratorA::SM2Profile::ShaderHelperCg::generateVpHeader()
// but is needed because generateVpDynamicShadowsParams() is not declared
// virtual.
void GzTerrainMatGen::SM2Profile::ShaderHelperCg::generateVpHeader(
    const SM2Profile *_prof, const Ogre::Terrain *_terrain,
    TechniqueType _tt, Ogre::StringUtil::StrStreamType &_outStream)
{
  _outStream << "void main_vp(\n";

  bool compression = _terrain->_getUseVertexCompression() &&
                     _tt != RENDER_COMPOSITE_MAP;

  if (compression)
  {
    _outStream << "float2 posIndex : POSITION,\nfloat height  : TEXCOORD0,\n";
  }
  else
  {
    _outStream << "float4 pos : POSITION,\nfloat2 uv  : TEXCOORD0,\n";
  }

  if (_tt != RENDER_COMPOSITE_MAP)
    _outStream << "float2 delta  : TEXCOORD1,\n";

  _outStream <<
    "uniform float4x4 worldMatrix,\n"
    "uniform float4x4 viewProjMatrix,\n"
    "uniform float2   lodMorph,\n";

  if (compression)
  {
    _outStream <<
      "uniform float4x4   posIndexToObjectSpace,\n"
      "uniform float    baseUVScale,\n";
  }

  // uv multipliers
  unsigned int maxLayers = _prof->getMaxLayers(_terrain);
  unsigned int numLayers = std::min(maxLayers,
      static_cast<unsigned int>(_terrain->getLayerCount()));

  unsigned int numUVMultipliers = (numLayers / 4);

  if (numLayers % 4)
    ++numUVMultipliers;

  for (unsigned int i = 0; i < numUVMultipliers; ++i)
    _outStream << "uniform float4 uvMul_" << i << ", \n";

  _outStream <<
    "out float4 oPos : POSITION,\n"
    "out float4 oPosObj : TEXCOORD0 \n";

  unsigned int texCoordSet = 1;
  _outStream << ", out float4 oUVMisc : TEXCOORD" << texCoordSet++
            << " // xy = uv, z = camDepth\n";

  // layer UV's premultiplied, packed as xy/zw
  unsigned int numUVSets = numLayers / 2;

  if (numLayers % 2)
    ++numUVSets;

  if (_tt != LOW_LOD)
  {
    for (unsigned int i = 0; i < numUVSets; ++i)
    {
      _outStream << ", out float4 oUV" << i
                << " : TEXCOORD" << texCoordSet++ << "\n";
    }
  }

  if (_prof->getParent()->getDebugLevel() && _tt != RENDER_COMPOSITE_MAP)
  {
    _outStream << ", out float2 lodInfo : TEXCOORD" << texCoordSet++ << "\n";
  }

  bool fog = _terrain->getSceneManager()->getFogMode() != Ogre::FOG_NONE &&
             _tt != RENDER_COMPOSITE_MAP;

  if (fog)
  {
    _outStream <<
      ", uniform float4 fogParams\n"
      ", out float fogVal : COLOR\n";
  }

  if (_prof->isShadowingEnabled(_tt, _terrain))
  {
    texCoordSet = generateVpDynamicShadowsParams(texCoordSet, _prof,
        _terrain, _tt, _outStream);
  }

  // check we haven't exceeded texture coordinates
  if (texCoordSet > 8)
  {
    OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
        "Requested options require too many texture coordinate sets! "
        "Try reducing the number of layers.",
        __FUNCTION__);
  }

  _outStream <<
    ")\n"
    "{\n";

  if (compression)
  {
    _outStream << "   float4 pos;\n"
      << "   pos = mul(posIndexToObjectSpace, float4(posIndex, height, 1));\n"
      << "   float2 uv = float2(posIndex.x * baseUVScale, 1.0 - "
      << "(posIndex.y * baseUVScale));\n";
  }

  _outStream <<
    "   float4 worldPos = mul(worldMatrix, pos);\n"
    "   oPosObj = pos;\n";

  if (_tt != RENDER_COMPOSITE_MAP)
  {
    // determine whether to apply the LOD morph to this vertex
    // we store the deltas against all vertices so we only want to apply
    // the morph to the ones which would disappear. The target LOD which is
    // being morphed to is stored in lodMorph.y, and the LOD at which
    // the vertex should be morphed is stored in uv.w. If we subtract
    // the former from the latter, and arrange to only morph if the
    // result is negative (it will only be -1 in fact, since after that
    // the vertex will never be indexed), we will achieve our aim.
    // sign(vertexLOD - targetLOD) == -1 is to morph
    _outStream <<
      "   float toMorph = -min(0, sign(delta.y - lodMorph.y));\n";

    // this will either be 1 (morph) or 0 (don't morph)
    if (_prof->getParent()->getDebugLevel())
    {
      // x == LOD level (-1 since value is target level, we want to
      // display actual)
      _outStream << "lodInfo.x = (lodMorph.y - 1) / "
                 << _terrain->getNumLodLevels() << ";\n";

      // y == LOD morph
      _outStream << "lodInfo.y = toMorph * lodMorph.x;\n";
    }

    // morph
    switch (_terrain->getAlignment())
    {
      case Ogre::Terrain::ALIGN_X_Y:
        _outStream << "   worldPos.z += delta.x * toMorph * lodMorph.x;\n";
        break;
      case Ogre::Terrain::ALIGN_X_Z:
        _outStream << "   worldPos.y += delta.x * toMorph * lodMorph.x;\n";
        break;
      case Ogre::Terrain::ALIGN_Y_Z:
        _outStream << "   worldPos.x += delta.x * toMorph * lodMorph.x;\n";
        break;
      default:
        gzerr << "Invalid alignment\n";
    };
  }

  // generate UVs
  if (_tt != LOW_LOD)
  {
    for (unsigned int i = 0; i < numUVSets; ++i)
    {
      unsigned int layer  =  i * 2;
      unsigned int uvMulIdx = layer / 4;

      _outStream << "   oUV" << i << ".xy = " << " uv.xy * uvMul_"
                 << uvMulIdx << "." << getChannel(layer) << ";\n";
      _outStream << "   oUV" << i << ".zw = " << " uv.xy * uvMul_"
                 << uvMulIdx << "." << getChannel(layer+1) << ";\n";
    }
  }
}

/////////////////////////////////////////////////
// This method is identical to
// TerrainMaterialGeneratorA::SM2Profile::ShaderHelperCg::generateVpFooter()
// but is needed because generateVpDynamicShadows() is not declared virtual.
void GzTerrainMatGen::SM2Profile::ShaderHelperCg::generateVpFooter(
    const SM2Profile *_prof, const Ogre::Terrain *_terrain,
    TechniqueType _tt, Ogre::StringUtil::StrStreamType &_outStream)
{
  _outStream << "   oPos = mul(viewProjMatrix, worldPos);\n"
             << "   oUVMisc.xy = uv.xy;\n";

  bool fog = _terrain->getSceneManager()->getFogMode() != Ogre::FOG_NONE &&
             _tt != RENDER_COMPOSITE_MAP;
  if (fog)
  {
    if (_terrain->getSceneManager()->getFogMode() == Ogre::FOG_LINEAR)
    {
      _outStream <<
        "   fogVal = saturate((oPos.z - fogParams.y) * fogParams.w);\n";
    }
    else
    {
      _outStream <<
        "   fogVal = 1 - saturate(1 / (exp(oPos.z * fogParams.x)));\n";
    }
  }

  if (_prof->isShadowingEnabled(_tt, _terrain))
    this->generateVpDynamicShadows(_prof, _terrain, _tt, _outStream);

  _outStream << "}\n";
}

/////////////////////////////////////////////////
void GzTerrainMatGen::SM2Profile::ShaderHelperCg::
generateVertexProgramSource(const SM2Profile *_prof,
    const Ogre::Terrain* _terrain, TechniqueType _tt,
    Ogre::StringUtil::StrStreamType &_outStream)
{
  this->generateVpHeader(_prof, _terrain, _tt, _outStream);

  if (_tt != LOW_LOD)
  {
    unsigned int maxLayers = _prof->getMaxLayers(_terrain);
    unsigned int numLayers = std::min(maxLayers,
        static_cast<unsigned int>(_terrain->getLayerCount()));

    for (unsigned int i = 0; i < numLayers; ++i)
      this->generateVpLayer(_prof, _terrain, _tt, i, _outStream);
  }

  this->generateVpFooter(_prof, _terrain, _tt, _outStream);
}

/////////////////////////////////////////////////
Ogre::HighLevelGpuProgramPtr
GzTerrainMatGen::SM2Profile::ShaderHelperCg::generateVertexProgram(
    const SM2Profile *_prof, const Ogre::Terrain *_terrain,
    TechniqueType _tt)
{
  Ogre::HighLevelGpuProgramPtr ret =
    this->createVertexProgram(_prof, _terrain, _tt);

  Ogre::StringUtil::StrStreamType sourceStr;
  this->generateVertexProgramSource(_prof, _terrain, _tt, sourceStr);

  ret->setSource(sourceStr.str());
  ret->load();
  this->defaultVpParams(_prof, _terrain, _tt, ret);

#if OGRE_DEBUG_MODE
  Ogre::LogManager::getSingleton().stream(LML_TRIVIAL)
    << "*** Terrain Vertex Program: "
    << ret->getName() << " ***\n" << ret->getSource() << "\n***   ***";
#endif

  return ret;
}
