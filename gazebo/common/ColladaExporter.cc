/*
 * Copyright (C) 2012-2014 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <tinyxml.h>

#include "gazebo/common/Material.hh"
#include "gazebo/common/Mesh.hh"
#include "gazebo/common/ColladaExporter.hh"

using namespace gazebo;
using namespace common;

//////////////////////////////////////////////////
ColladaExporter::ColladaExporter()
: MeshExporter()
{
}

//////////////////////////////////////////////////
ColladaExporter::~ColladaExporter()
{
}

//////////////////////////////////////////////////
void ColladaExporter::Export(const Mesh *_mesh)
{
  this->mesh = _mesh;
  this->materialCount = this->mesh->GetMaterialCount();

  // Mesh name
  std::string meshName = _mesh->GetName();
  meshName = meshName.substr(0, meshName.find(".dae"));

  // Collada file
  TiXmlDocument xmlDoc;

  // XML declaration
  TiXmlDeclaration *declarationXml = new TiXmlDeclaration("1.0", "utf-8", "");
  xmlDoc.LinkEndChild(declarationXml);

  // Collada element
  TiXmlElement *colladaXml = new TiXmlElement("COLLADA");
  xmlDoc.LinkEndChild(colladaXml);
  colladaXml->SetAttribute("version", "1.4.1");
  colladaXml->SetAttribute("xmlns",
      "http://www.collada.org/2005/11/COLLADASchema");

  // Asset element
  TiXmlElement *assetXml = new TiXmlElement("asset");
  this->ExportAsset(assetXml);
  colladaXml->LinkEndChild(assetXml);

  // Library geometries element
  TiXmlElement *libraryGeometriesXml = new TiXmlElement("library_geometries");
  this->ExportGeometries(libraryGeometriesXml);
  colladaXml->LinkEndChild(libraryGeometriesXml);

  if (this->materialCount != 0)
  {
    // Library images element
    TiXmlElement *libraryImagesXml = new TiXmlElement("library_images");
    int imageCount = this->ExportImages(libraryImagesXml);
    if (imageCount)
    {
      colladaXml->LinkEndChild(libraryImagesXml);
    }

    // Library materials element
    TiXmlElement *libraryMaterialsXml = new TiXmlElement("library_materials");
    this->ExportMaterials(libraryMaterialsXml);
    colladaXml->LinkEndChild(libraryMaterialsXml);

    // Library effects element
    TiXmlElement *libraryEffectsXml = new TiXmlElement("library_effects");
    this->ExportEffects(libraryEffectsXml);
    colladaXml->LinkEndChild(libraryEffectsXml);
  }

  // Library visual scenes element
  TiXmlElement *libraryVisualScenesXml =
      new TiXmlElement("library_visual_scenes");
  this->ExportVisualScenes(libraryVisualScenesXml);
  colladaXml->LinkEndChild(libraryVisualScenesXml);

  // Scene element
  TiXmlElement *sceneXml = new TiXmlElement("scene");
  this->ExportScene(sceneXml);
  colladaXml->LinkEndChild(sceneXml);

  xmlDoc.SaveFile(meshName+"_exported.dae");
}

//////////////////////////////////////////////////
void ColladaExporter::ExportAsset(TiXmlElement *_assetXml)
{
  TiXmlElement *unitXml = new TiXmlElement("unit");
  unitXml->SetAttribute("meter", "1");
  unitXml->SetAttribute("name", "meter");
  _assetXml->LinkEndChild(unitXml);

  TiXmlElement *upAxisXml = new TiXmlElement("up_axis");
  upAxisXml->LinkEndChild(new TiXmlText("Z_UP"));
  _assetXml->LinkEndChild(upAxisXml);
}

//////////////////////////////////////////////////
void ColladaExporter::FillSource(
    const gazebo::common::SubMesh *_subMesh,
    TiXmlElement *_meshXml, int _type, const char *_meshID)
{
  std::ostringstream sourceID;
  std::ostringstream sourceArrayID;
  std::ostringstream sourceArrayIdSelector;
  std::ostringstream fillData;
  fillData.precision(5);
  fillData << std::fixed;
  int stride;
  unsigned int count = 0;

  if (_type == 1)
  {
    sourceID << _meshID << "-Positions";
    count = _subMesh->GetVertexCount();
    stride = 3;
    gazebo::math::Vector3 vertex;
    for (unsigned int i = 0; i < count; ++i)
    {
      vertex = _subMesh->GetVertex(i);
      fillData << vertex.x << " " << vertex.y << " " << vertex.z << " ";
    }
  }
  if (_type == 2)
  {
    sourceID << _meshID << "-Normals";
    count = _subMesh->GetNormalCount();
    stride = 3;
    gazebo::math::Vector3 normal;
    for (unsigned int i = 0; i < count; ++i)
    {
      normal = _subMesh->GetNormal(i);
      fillData << normal.x << " " << normal.y << " " << normal.z << " ";
    }
  }
  if (_type == 3)
  {
    sourceID << _meshID << "-UVMap";
    count = _subMesh->GetVertexCount();
    stride = 2;
    gazebo::math::Vector2d inTexCoord;
    for (unsigned int i = 0; i < count; ++i)
    {
      inTexCoord = _subMesh->GetTexCoord(i);
      fillData << inTexCoord.x << " " << 1-inTexCoord.y << " ";
    }
  }
  sourceArrayID << sourceID.str() << "-array";
  sourceArrayIdSelector << "#" << sourceArrayID.str();

  TiXmlElement *sourceXml = new TiXmlElement("source");
  _meshXml->LinkEndChild(sourceXml);
  sourceXml->SetAttribute("id", sourceID.str().c_str());
  sourceXml->SetAttribute("name", sourceID.str().c_str());

  TiXmlElement *floatArrayXml = new TiXmlElement("float_array");
  floatArrayXml->SetAttribute("count", count *stride);
  floatArrayXml->SetAttribute("id", sourceArrayID.str().c_str());
  floatArrayXml->LinkEndChild(new TiXmlText(fillData.str().c_str()));
  sourceXml->LinkEndChild(floatArrayXml);

  TiXmlElement *techniqueCommonXml = new TiXmlElement("technique_common");
  sourceXml->LinkEndChild(techniqueCommonXml);

  TiXmlElement *accessorXml = new TiXmlElement("accessor");
  accessorXml->SetAttribute("count", count);
  accessorXml->SetAttribute("source", sourceArrayIdSelector.str().c_str());
  accessorXml->SetAttribute("stride", stride);
  techniqueCommonXml->LinkEndChild(accessorXml);

  TiXmlElement *paramXml = new TiXmlElement("param");
  if (_type == 1 || _type == 2)
  {
    paramXml->SetAttribute("type", "float");
    paramXml->SetAttribute("name", "X");
    accessorXml->LinkEndChild(paramXml);

    paramXml = new TiXmlElement("param");
    paramXml->SetAttribute("type", "float");
    paramXml->SetAttribute("name", "Y");
    accessorXml->LinkEndChild(paramXml);

    paramXml = new TiXmlElement("param");
    paramXml->SetAttribute("type", "float");
    paramXml->SetAttribute("name", "Z");
    accessorXml->LinkEndChild(paramXml);
  }
  if (_type == 3)
  {
    paramXml->SetAttribute("type", "float");
    paramXml->SetAttribute("name", "U");
    accessorXml->LinkEndChild(paramXml);

    paramXml = new TiXmlElement("param");
    paramXml->SetAttribute("type", "float");
    paramXml->SetAttribute("name", "V");
    accessorXml->LinkEndChild(paramXml);
  }
}

//////////////////////////////////////////////////
void ColladaExporter::ExportGeometries(TiXmlElement *_libraryGeometriesXml)
{
  for (unsigned int i = 0; i < this->mesh->GetSubMeshCount(); i++)
  {
    char meshId[100], materialId[100];
    snprintf(meshId, sizeof(meshId), "mesh_%d", i);
    snprintf(materialId, sizeof(materialId), "material_%d", i);

    TiXmlElement *geometryXml = new TiXmlElement("geometry");
    geometryXml->SetAttribute("id", meshId);
    _libraryGeometriesXml->LinkEndChild(geometryXml);

    TiXmlElement *meshXml = new TiXmlElement("mesh");
    geometryXml->LinkEndChild(meshXml);

    const gazebo::common::SubMesh *subMesh = this->mesh->GetSubMesh(i);

    // Position
    FillSource(subMesh, meshXml, 1, meshId);
    // Normals
    FillSource(subMesh, meshXml, 2, meshId);
    // Texture coordinates
    if (subMesh->GetTexCoordCount() != 0)
    {
      FillSource(subMesh, meshXml, 3, meshId);
    }

    // Vertices
    char attributeValue[100];

    TiXmlElement *verticesXml = new TiXmlElement("vertices");
    meshXml->LinkEndChild(verticesXml);
    snprintf(attributeValue, sizeof(attributeValue), "%s-Vertex", meshId);
    verticesXml->SetAttribute("id", attributeValue);
    verticesXml->SetAttribute("name", attributeValue);

    TiXmlElement *inputXml = new TiXmlElement("input");
    verticesXml->LinkEndChild(inputXml);
    inputXml->SetAttribute("semantic", "POSITION");
    snprintf(attributeValue, sizeof(attributeValue), "#%s-Positions", meshId);
    inputXml->SetAttribute("source", attributeValue);

    // Triangles
    unsigned int indexCount = subMesh->GetIndexCount();

    TiXmlElement *trianglesXml = new TiXmlElement("triangles");
    meshXml->LinkEndChild(trianglesXml);
    trianglesXml->SetAttribute("count", indexCount/3);
    trianglesXml->SetAttribute("material", materialId);

    inputXml = new TiXmlElement("input");
    trianglesXml->LinkEndChild(inputXml);
    inputXml->SetAttribute("offset", 0);
    inputXml->SetAttribute("semantic", "VERTEX");
    snprintf(attributeValue, sizeof(attributeValue), "#%s-Vertex", meshId);
    inputXml->SetAttribute("source", attributeValue);

    inputXml = new TiXmlElement("input");
    trianglesXml->LinkEndChild(inputXml);
    inputXml->SetAttribute("offset", 1);
    inputXml->SetAttribute("semantic", "NORMAL");
    snprintf(attributeValue, sizeof(attributeValue), "#%s-Normals", meshId);
    inputXml->SetAttribute("source", attributeValue);

    if (subMesh->GetTexCoordCount() != 0)
    {
      inputXml = new TiXmlElement("input");
      trianglesXml->LinkEndChild(inputXml);
      inputXml->SetAttribute("offset", 2);
      inputXml->SetAttribute("semantic", "TEXCOORD");
      snprintf(attributeValue, sizeof(attributeValue), "#%s-UVMap", meshId);
      inputXml->SetAttribute("source", attributeValue);
    }

    std::ostringstream fillData;
    for (unsigned int j = 0; j < indexCount; ++j)
    {
      fillData << subMesh->GetIndex(j) << " "
               << subMesh->GetIndex(j) << " ";
      if (subMesh->GetTexCoordCount() != 0)
      {
        fillData << subMesh->GetIndex(j) << " ";
      }
    }

    TiXmlElement *pXml = new TiXmlElement("p");
    trianglesXml->LinkEndChild(pXml);
    pXml->LinkEndChild(new TiXmlText(fillData.str().c_str()));
  }
}

//////////////////////////////////////////////////
int ColladaExporter::ExportImages(TiXmlElement *_libraryImagesXml)
{
  int imageCount = 0;
  for (unsigned int i = 0; i < this->materialCount; i++)
  {
    const gazebo::common::Material *material = this->mesh->GetMaterial(i);
    std::string imageString = material->GetTextureImage();

    if (imageString.find("meshes/") != std::string::npos)
    {
      char id[100];
      snprintf(id, sizeof(id), "image_%d", i);

      TiXmlElement *imageXml = new TiXmlElement("image");
      imageXml->SetAttribute("id", id);
      _libraryImagesXml->LinkEndChild(imageXml);

      TiXmlElement *initFromXml = new TiXmlElement("init_from");
      initFromXml->LinkEndChild(new TiXmlText(
        imageString.substr(imageString.find("meshes/")+7)));
      imageXml->LinkEndChild(initFromXml);

      imageCount++;
    }
  }

  return imageCount;
}

//////////////////////////////////////////////////
void ColladaExporter::ExportMaterials(TiXmlElement *_libraryMaterialsXml)
{
  for (unsigned int i = 0; i < this->materialCount; i++)
  {
    char id[100];
    snprintf(id, sizeof(id), "material_%d", i);

    TiXmlElement *materialXml = new TiXmlElement("material");
    materialXml->SetAttribute("id", id);
    _libraryMaterialsXml->LinkEndChild(materialXml);

    snprintf(id, sizeof(id), "#material_%d_fx", i);
    TiXmlElement *instanceEffectXml = new TiXmlElement("instance_effect");
    instanceEffectXml->SetAttribute("url", id);
    materialXml->LinkEndChild(instanceEffectXml);
  }
}

//////////////////////////////////////////////////
void ColladaExporter::ExportEffects(TiXmlElement *_libraryEffectsXml)
{
  for (unsigned int i = 0; i < this->materialCount; i++)
  {
    char id[100];
    snprintf(id, sizeof(id), "material_%d_fx", i);

    TiXmlElement *effectXml = new TiXmlElement("effect");
    effectXml->SetAttribute("id", id);
    _libraryEffectsXml->LinkEndChild(effectXml);

    TiXmlElement *profileCommonXml = new TiXmlElement("profile_COMMON");
    effectXml->LinkEndChild(profileCommonXml);

    // Image
    const gazebo::common::Material *material = this->mesh->GetMaterial(i);
    std::string imageString = material->GetTextureImage();

    if (imageString.find("meshes/") != std::string::npos)
    {
      TiXmlElement *newParamXml = new TiXmlElement("newparam");
      snprintf(id, sizeof(id), "image_%d_surface", i);
      newParamXml->SetAttribute("sid", id);
      profileCommonXml->LinkEndChild(newParamXml);

      TiXmlElement *surfaceXml = new TiXmlElement("surface");
      surfaceXml->SetAttribute("type", "2D");
      newParamXml->LinkEndChild(surfaceXml);

      TiXmlElement *initFromXml = new TiXmlElement("init_from");
      snprintf(id, sizeof(id), "image_%d", i);
      initFromXml->LinkEndChild(new TiXmlText(id));
      surfaceXml->LinkEndChild(initFromXml);

      newParamXml = new TiXmlElement("newparam");
      snprintf(id, sizeof(id), "image_%d_sampler", i);
      newParamXml->SetAttribute("sid", id);
      profileCommonXml->LinkEndChild(newParamXml);

      TiXmlElement *sampler2dXml = new TiXmlElement("sampler2D");
      newParamXml->LinkEndChild(sampler2dXml);

      TiXmlElement *sourceXml = new TiXmlElement("source");
      snprintf(id, sizeof(id), "image_%d_surface", i);
      sourceXml->LinkEndChild(new TiXmlText(id));
      sampler2dXml->LinkEndChild(sourceXml);

      TiXmlElement *minFilterXml = new TiXmlElement("minfilter");
      minFilterXml->LinkEndChild(new TiXmlText("LINEAR"));
      sampler2dXml->LinkEndChild(minFilterXml);

      TiXmlElement *magFilterXml = new TiXmlElement("magfilter");
      magFilterXml->LinkEndChild(new TiXmlText("LINEAR"));
      sampler2dXml->LinkEndChild(magFilterXml);
    }

    TiXmlElement *techniqueXml = new TiXmlElement("technique");
    techniqueXml->SetAttribute("sid", "COMMON");
    profileCommonXml->LinkEndChild(techniqueXml);

    // gazebo::common::Material::ShadeMode shadeMode = material->GetShadeMode();

    // Using phong for now
    TiXmlElement *phongXml = new TiXmlElement("phong");
    techniqueXml->LinkEndChild(phongXml);

    // ambient
    unsigned int RGBAcolor = material->GetAmbient().GetAsRGBA();
    float r = ((RGBAcolor >> 24) & 0xFF) / 255.0f;
    float g = ((RGBAcolor >> 16) & 0xFF) / 255.0f;
    float b = ((RGBAcolor >> 8) & 0xFF) / 255.0f;
    float a = (RGBAcolor & 0xFF) / 255.0f;

    TiXmlElement *ambientXml = new TiXmlElement("ambient");
    phongXml->LinkEndChild(ambientXml);

    TiXmlElement *colorXml = new TiXmlElement("color");
    snprintf(id, sizeof(id), "%f %f %f %f", r, g, b, a);
    colorXml->LinkEndChild(new TiXmlText(id));
    ambientXml->LinkEndChild(colorXml);

    // emission
    RGBAcolor = material->GetEmissive().GetAsRGBA();
    r = ((RGBAcolor >> 24) & 0xFF) / 255.0f;
    g = ((RGBAcolor >> 16) & 0xFF) / 255.0f;
    b = ((RGBAcolor >> 8) & 0xFF) / 255.0f;
    a = (RGBAcolor & 0xFF) / 255.0f;

    TiXmlElement *emissionXml = new TiXmlElement("emission");
    phongXml->LinkEndChild(emissionXml);

    colorXml = new TiXmlElement("color");
    snprintf(id, sizeof(id), "%f %f %f %f", r, g, b, a);
    colorXml->LinkEndChild(new TiXmlText(id));
    emissionXml->LinkEndChild(colorXml);

    // diffuse
    TiXmlElement *diffuseXml = new TiXmlElement("diffuse");
    phongXml->LinkEndChild(diffuseXml);

    if (imageString.find("meshes/") != std::string::npos)
    {
      TiXmlElement *textureXml = new TiXmlElement("texture");
      snprintf(id, sizeof(id), "image_%d", i);
      textureXml->SetAttribute("texture", id);
      textureXml->SetAttribute("texcoord", "UVSET0");
      diffuseXml->LinkEndChild(textureXml);
    }
    else
    {
      RGBAcolor = material->GetDiffuse().GetAsRGBA();
      r = ((RGBAcolor >> 24) & 0xFF) / 255.0f;
      g = ((RGBAcolor >> 16) & 0xFF) / 255.0f;
      b = ((RGBAcolor >> 8) & 0xFF) / 255.0f;
      a = (RGBAcolor & 0xFF) / 255.0f;

      colorXml = new TiXmlElement("color");
      snprintf(id, sizeof(id), "%f %f %f %f", r, g, b, a);
      colorXml->LinkEndChild(new TiXmlText(id));
      diffuseXml->LinkEndChild(colorXml);
    }

    // specular
    RGBAcolor = material->GetSpecular().GetAsRGBA();
    r = ((RGBAcolor >> 24) & 0xFF) / 255.0f;
    g = ((RGBAcolor >> 16) & 0xFF) / 255.0f;
    b = ((RGBAcolor >> 8) & 0xFF) / 255.0f;
    a = (RGBAcolor & 0xFF) / 255.0f;

    TiXmlElement *specularXml = new TiXmlElement("specular");
    phongXml->LinkEndChild(specularXml);

    colorXml = new TiXmlElement("color");
    snprintf(id, sizeof(id), "%f %f %f %f", r, g, b, a);
    colorXml->LinkEndChild(new TiXmlText(id));
    specularXml->LinkEndChild(colorXml);

    // transparency
    double transp = material->GetTransparency();

    TiXmlElement *transparencyXml = new TiXmlElement("transparency");
    phongXml->LinkEndChild(transparencyXml);

    TiXmlElement *floatXml = new TiXmlElement("float");
    snprintf(id, sizeof(id), "%f", transp);
    floatXml->LinkEndChild(new TiXmlText(id));
    transparencyXml->LinkEndChild(floatXml);

    // shininess
    double shine = material->GetShininess();

    TiXmlElement *shininessXml = new TiXmlElement("shininess");
    phongXml->LinkEndChild(shininessXml);

    colorXml = new TiXmlElement("color");
    snprintf(id, sizeof(id), "%f", shine);
    colorXml->LinkEndChild(new TiXmlText(id));
    shininessXml->LinkEndChild(colorXml);
  }
}

//////////////////////////////////////////////////
void ColladaExporter::ExportVisualScenes(
    TiXmlElement *_libraryVisualScenesXml)
{
  TiXmlElement *visualSceneXml = new TiXmlElement("visual_scene");
  _libraryVisualScenesXml->LinkEndChild(visualSceneXml);
  visualSceneXml->SetAttribute("name", "Scene");
  visualSceneXml->SetAttribute("id", "Scene");

  TiXmlElement *nodeXml = new TiXmlElement("node");
  visualSceneXml->LinkEndChild(nodeXml);
  nodeXml->SetAttribute("name", "node");
  nodeXml->SetAttribute("id", "node");

  for (unsigned int i = 0; i < this->mesh->GetSubMeshCount(); i++)
  {
    char meshId[100], materialId[100], attributeValue[100];
    snprintf(meshId, sizeof(meshId), "mesh_%d", i);
    snprintf(materialId, sizeof(materialId), "material_%d", i);

    TiXmlElement *instanceGeometryXml = new TiXmlElement("instance_geometry");
    nodeXml->LinkEndChild(instanceGeometryXml);
    snprintf(attributeValue, sizeof(attributeValue), "#%s", meshId);
    instanceGeometryXml->SetAttribute("url", attributeValue);

    const gazebo::common::Material *material = this->mesh->GetMaterial(i);

    if (material)
    {
      TiXmlElement *bindMaterialXml = new TiXmlElement("bind_material");
      instanceGeometryXml->LinkEndChild(bindMaterialXml);

      TiXmlElement *techniqueCommonXml = new TiXmlElement("technique_common");
      bindMaterialXml->LinkEndChild(techniqueCommonXml);

      TiXmlElement *instanceMaterialXml = new TiXmlElement("instance_material");
      techniqueCommonXml->LinkEndChild(instanceMaterialXml);
      instanceMaterialXml->SetAttribute("symbol", materialId);
      snprintf(attributeValue, sizeof(attributeValue), "#%s", materialId);
      instanceMaterialXml->SetAttribute("target", attributeValue);

      std::string imageString = material->GetTextureImage();

      if (imageString.find("meshes/") != std::string::npos)
      {
        TiXmlElement *bindVertexInputXml =
            new TiXmlElement("bind_vertex_input");
        instanceMaterialXml->LinkEndChild(bindVertexInputXml);
        bindVertexInputXml->SetAttribute("semantic", "UVSET0");
        bindVertexInputXml->SetAttribute("input_semantic", "TEXCOORD");
      }
    }
  }
}

//////////////////////////////////////////////////
void ColladaExporter::ExportScene(TiXmlElement *_sceneXml)
{
  TiXmlElement *instanceVisualSceneXml =
      new TiXmlElement("instance_visual_scene");
  _sceneXml->LinkEndChild(instanceVisualSceneXml);
  instanceVisualSceneXml->SetAttribute("url", "#Scene");
}
