#include <algorithm>
#include <iostream>
#include <float.h>
#include <list>

#include "Mesh.hh"
#include "GazeboMessage.hh"
#include "OgreLoader.hh"

using namespace gazebo;

////////////////////////////////////////////////////////////////////////////////
/// Constructor
OgreLoader::OgreLoader()
  : MeshLoader()
{
  this->gSubMesh = NULL;
}

////////////////////////////////////////////////////////////////////////////////
/// Destructor
OgreLoader::~OgreLoader()
{
  if (this->gSubMesh)
    delete this->gSubMesh;
}

////////////////////////////////////////////////////////////////////////////////
// Load a mesh from file
Mesh *OgreLoader::Load(const std::string &filename)
{
  FILE *file = fopen(filename.c_str(), "rb");

  Mesh *mesh = new Mesh();

  if (this->ReadFileHeader(file))
  {
    MeshChunk chunk;
    chunk.Read(file);

    if (chunk.id == 0x3000)
      this->ReadMesh(file, mesh);
    else
      printf("Unable to process chunk with id[%d]\n", chunk.id);
  }

  fclose(file);
  file = NULL;

  return mesh;
}

////////////////////////////////////////////////////////////////////////////////
// Read a submesh
void OgreLoader::ReadSubMesh(FILE *file, Mesh *mesh)
{
  // Ignore the material name
  std::string materialName = ReadString(file);

  bool useSharedVertices = ReadValue<bool>(file);

  int indexCount = ReadValue<int>(file);

  bool idx32Bit = ReadValue<bool>(file);

  SubMesh *subMesh = new SubMesh();
  mesh->AddSubMesh( subMesh );

  if (indexCount > 0)
  {
    /*unsigned int offset = 0;
   
    if (mesh->GetIndexCount() > 0)
      offset = mesh->GetMaxIndex() + 1;
      */

    if (idx32Bit)
    {
      std::list<unsigned int> values;
      std::list<unsigned int>::iterator iter;

      values = ReadValues<unsigned int>(file, indexCount);

      for (iter = values.begin(); iter != values.end(); iter++)
        subMesh->AddIndex( (*iter) );
    }
    else
    {
      std::list<unsigned short> values;
      std::list<unsigned short>::iterator iter;
      values = ReadValues<unsigned short>(file, indexCount);

      for (iter = values.begin(); iter != values.end(); iter++)
        subMesh->AddIndex( (*iter) );
    }
  }

  // Global vertices have not been specified, so load this submesh's vertex
  // data
  if (!useSharedVertices)
  {
    MeshChunk chunk;
    chunk.Read(file);
    if (chunk.id != M_GEOMETRY)
    {
      printf("Missing geometry data in mesh file");
      exit(0);
    }

    this->ReadGeometry(file, mesh, subMesh);
  }
  // Else use the global vertex data
  else
  {
    for (unsigned int i=0; i < subMesh->GetIndexCount(); i++)
    {
      subMesh->AddVertex( gSubMesh->GetVertex( subMesh->GetIndex(i) ) );
    }
  }

  // Skip the remaining data
  if (!feof(file))
  {
    MeshChunk chunk;
    chunk.Read(file);

    while (!feof(file) &&
        (chunk.id == M_SUBMESH_BONE_ASSIGNMENT ||
         chunk.id == M_SUBMESH_OPERATION ||
         chunk.id == M_SUBMESH_TEXTURE_ALIAS))
    {
      // Skip this data
      fseek(file, chunk.length - chunk.SizeOf(), SEEK_CUR);

      if (!feof(file))
        chunk.Read(file);
    }

    // Backpedal a little to the start of the chunk
    if (!feof(file))
      fseek(file, -chunk.SizeOf(), SEEK_CUR);
  }

}

////////////////////////////////////////////////////////////////////////////////
// Read the geometry (vertices, normals, and optionally texture coords)
void OgreLoader::ReadGeometry(FILE *file, Mesh *mesh, SubMesh *subMesh)
{ 
  unsigned short index;
  MeshChunk chunk;
  std::vector<unsigned short> types;
  std::vector<unsigned short> semantics;
  std::vector<unsigned short> offsets;
  std::vector<unsigned short> sources;

  unsigned int vertexCount = this->ReadValue<unsigned int>(file);

  // Read optional geom parameters
  if (!feof(file))
  {
    chunk.Read(file);
    while (!feof(file) &&
        (chunk.id == M_GEOMETRY_VERTEX_DECLRATION ||
         chunk.id == M_GEOMETRY_VERTEX_BUFFER))
    {

      // VERTEX_DECLARTION tells what type of information is provided
      if (chunk.id == M_GEOMETRY_VERTEX_DECLRATION)
      {
        chunk.Read(file);
        while (!feof(file) && chunk.id == M_GEOMETRY_VERTEX_ELEMENT)
        {
          // The source
          unsigned short source = ReadValue<unsigned short>(file);

          // The data type of the data used to store the vertex info
          unsigned short type = ReadValue<unsigned short>(file);

          // The meaning of the data (position, normal, etc)
          unsigned short semantic = ReadValue<unsigned short>(file);

          // Offset in bytes where this element is located in the buffer
          unsigned short offset = ReadValue<unsigned short>(file);

          // We can ignore the index
          index = ReadValue<unsigned short>(file);

          sources.push_back(source);
          types.push_back(type);
          semantics.push_back(semantic);
          offsets.push_back(offset);

          /*printf("Source[%d] Type[%d] Semantic[%d] Offset[%d] Index[%d]\n",
              source, type, semantic,offset, index);
              */

          if (!feof(file))
            chunk.Read(file);
        }

        // backpeddle to start of non-submesh stream
        if (!feof(file))
          fseek(file, -chunk.SizeOf(), SEEK_CUR);
      }
      // VERTEX_BUFFER contains the vertex data
      else if (chunk.id == M_GEOMETRY_VERTEX_BUFFER)
      {
        unsigned short bindIndex, vertexSize;

        bindIndex = ReadValue<unsigned short>(file);
        vertexSize = ReadValue<unsigned short>(file);

        chunk.Read(file);
        if (chunk.id != M_GEOMETRY_VERTEX_BUFFER_DATA)
        {
          printf("Can't find vertex buffer data area\n");
          exit(0);
        }

        // Read the data buffer 
        unsigned char *vbuf = new unsigned char[vertexCount * vertexSize];
        if (fread(vbuf, vertexSize, vertexCount, file) < vertexCount)
          printf("Error reading the vertex buffer\n");

        Vector3 vec;
        unsigned short semantic, type, offset;

        // Extract the information from the buffer
        for (unsigned int v = 0; v < vertexCount; v++)
        {
          for (unsigned int i = 0; i < types.size(); i++)
          {
            if (sources[i] != bindIndex)
              continue;

            semantic = semantics[i];
            type = types[i];
            offset = offsets[i];
            switch (semantic)
            {
              // VES_POSITION
              case 1:
                {
                  // TODO: I'm assuming that all vertex data will be
                  // a three tuple of floats. Should use the "type"
                  // variable
                  float *ptr = (float*)(vbuf + offset);
                  vec.x = *(ptr++); vec.y = *(ptr++); vec.z = *(ptr++);
                  subMesh->AddVertex(vec);
                  break;
                }
                // VES_NORMAL
              case 4:
                {
                  // TODO: I'm assuming that all normal data will be
                  // a three tuple of floats. Should use the "type"
                  // variable
                  float *ptr = (float*)(vbuf + offset);
                  vec.x = *(ptr++); vec.y = *(ptr++); vec.z = *(ptr++);
                  subMesh->AddNormal(vec);
                  break;
                }
                // Ignore all other values
              default:
                break;
            }
          }
          vbuf += vertexSize;
        }
      }

      if (!feof(file))
        chunk.Read(file);
    }

    if (!feof(file))
      fseek(file, -chunk.SizeOf(), SEEK_CUR);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Read the mesh. A mesh can be composed of multiple submeshes. Each submesh
// contains a list of indices, and optionally a set of vertices. A global
// set of vertices can be specified, and then each submesh would just have
// a list of indices that reference the global vertices.
bool OgreLoader::ReadMesh(FILE *file, Mesh *mesh)
{
  MeshChunk chunk;

  if (ReadValue<bool>(file))
    printf("Warning: this doesn't fully support animated meshes\n");

  if (!feof(file))
  {
    chunk.Read(file);

    while (!feof(file) &&
          (chunk.id == M_GEOMETRY ||
           chunk.id == M_SUBMESH ||
           chunk.id == M_MESH_SKELETON_LINK ||
           chunk.id == M_MESH_BONE_ASSIGNMENT ||
           chunk.id == M_MESH_LOD ||
           chunk.id == M_MESH_BOUNDS ||
           chunk.id == M_SUBMESH_NAME_TABLE ||
           chunk.id == M_EDGE_LISTS ||
           chunk.id == M_POSES ||
           chunk.id == M_ANIMATIONS ||
           chunk.id == M_TABLE_EXTREMES))
    {
      switch (chunk.id)
      {
        case M_GEOMETRY:
          {
            if (this->gSubMesh == NULL)
              this->gSubMesh = new SubMesh();

            // Global vertex data
            this->ReadGeometry(file, mesh, gSubMesh);
            break;
          }
        case M_SUBMESH:
          {
            // Load a submesh
            this->ReadSubMesh(file, mesh);
            break;
          }
        // Skip all this data
        case M_MESH_SKELETON_LINK:
        case M_MESH_BONE_ASSIGNMENT:
        case M_MESH_BOUNDS:
        case M_POSES:
        case M_ANIMATIONS:
        case M_TABLE_EXTREMES:
        case M_SUBMESH_NAME_TABLE:
        case M_EDGE_LISTS:
        case M_MESH_LOD:
          fseek(file, chunk.length - chunk.SizeOf(), SEEK_CUR);
          break;
      };

      if (!feof(file))
        chunk.Read(file);
    }

    if (!feof(file))
      fseek(file, -chunk.SizeOf(), SEEK_CUR);
  }

  return true;
}

