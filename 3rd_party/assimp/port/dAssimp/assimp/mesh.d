/*
---------------------------------------------------------------------------
Open Asset Import Library (ASSIMP)
---------------------------------------------------------------------------

Copyright (c) 2006-2009, ASSIMP Development Team

All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the following
conditions are met:

 * Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

 * Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

 * Neither the name of the ASSIMP team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the ASSIMP Development Team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/

/**
 * Contains the data structures in which the imported geometry is returned by
 * Assimp.
 */
module assimp.mesh;

import assimp.math;
import assimp.types;

extern ( C ) {
   /**
    * A single face in a mesh, referring to multiple vertices.
    *
    * If <code>mNumIndices</code> is 3, we call the face <em>triangle</em>, for
    * for <code>mNumIndices > 3</code> it's called <em>polygon</em>.
    *
    * <code>aiMesh.mPrimitiveTypes</code> can be queried to quickly examine
    * which types of primitive are actually present in a mesh. The
    * <code>aiProcess.SortByPType</code> flag post-processing step splits
    * meshes containing different primitive types (e.g. lines and triangles) in
    * several "clean" submeshes.
    *
    * Furthermore, there is a configuration option
    * (<code>AI_CONFIG_PP_SBP_REMOVE</code>) to force <code>SortByPType</code>
    * to completely remove specific kinds of primitives from the imported scene.
    * In many cases you'll probably want to set this setting to
    * <code>aiPrimitiveType.LINE | aiPrimitiveType.POINT</code>. Together with
    * the <code>aiProcess.Triangulate</code> flag you can then be sure that
    * <code>mNumIndices</code> is always 3.
    */
   struct aiFace {
      /**
       * Number of indices defining this face (3 for a triangle, >3 for a
       * polygon).
       */
      uint mNumIndices;

      /**
       * Array of the indicies defining the face.
       *
       * The size is given in <code>mNumIndices</code>.
       */
      uint* mIndices;
   }

   /**
    * A single influence of a bone on a vertex.
    */
   struct aiVertexWeight {
      /**
       * Index of the vertex which is influenced by the bone.
       */
      uint mVertexId;

      /**
       * The strength of the influence in the range <code>[0..1]</code>.
       *
       * The influence from all bones at one vertex sums up to 1.
       */
      float mWeight;
   }

   /**
    * A single bone of a mesh.
    *
    * A bone has a name by which it can be found in the frame hierarchy and by
    * which it can be addressed by animations. In addition it has a number of
    * influences on vertices.
    */
   struct aiBone {
      /**
       * The name of the bone.
       */
      aiString mName;

      /**
       * The number of vertices affected by this bone.
       */
      uint mNumWeights;

      /**
       * The vertices affected by this bone.
       *
       * This array is <code>mNumWeights</code> entries in size.
       */
      aiVertexWeight* mWeights;

      /**
       * Matrix that transforms from mesh space to bone space (in the bind
       * pose).
       */
      aiMatrix4x4 mOffsetMatrix;
   }

   /**
    * Maximum number of vertex color sets per mesh.
    *
    * Normally: Diffuse, specular, ambient and emissive. However, one could use
    * the vertex color sets for any other purpose, too.
    *
    * Note: Some internal structures expect (and assert) this value to be at
    *    least 4. For the moment it is absolutely safe to assume that this will
    *    not change.
    */
   const uint AI_MAX_NUMBER_OF_COLOR_SETS = 0x4;

   /**
    * Maximum number of texture coord sets (UV(W) channels) per mesh
    *
    * The material system uses the <code>AI_MATKEY_UVWSRC_XXX</code> keys to
    * specify which UVW channel serves as data source for a texture.
    *
    * Note: Some internal structures expect (and assert) this value to be at
    *    least 4. For the moment it is absolutely safe to assume that this will
    *    not change.
    */
   const uint AI_MAX_NUMBER_OF_TEXTURECOORDS = 0x4;

   /**
    * Enumerates the types of geometric primitives supported by Assimp.
    *
    * See: <code>aiFace</code>, <code>aiProcess.SortByPType</code>,
    *    <code>aiProcess.Triangulate</code>,
    *    <code>AI_CONFIG_PP_SBP_REMOVE</code>.
    */
   enum aiPrimitiveType : uint {
      /** A point primitive.
       *
       * This is just a single vertex in the virtual world,
       * <code>aiFace</code> contains just one index for such a primitive.
       */
      POINT = 0x1,

      /** A line primitive.
       *
       * This is a line defined through a start and an end position.
       * <code>aiFace</code> contains exactly two indices for such a primitive.
       */
      LINE = 0x2,

      /** A triangular primitive.
       *
       * A triangle consists of three indices.
       */
      TRIANGLE = 0x4,

      /** A higher-level polygon with more than 3 edges.
       *
       * A triangle is a polygon, but in this context, polygon means
       * "all polygons that are not triangles". The <code>Triangulate</code>
       * post processing step is provided for your convinience, it splits all
       * polygons in triangles (which are much easier to handle).
       */
      POLYGON = 0x8
   }

   // Note: The AI_PRIMITIVE_TYPE_FOR_N_INDICES(n) macro from the C headers is
   // missing since there is probably not much use for it when just reading
   // scene files.

   /**
    * A mesh represents a geometry or model with a single material.
    *
    * It usually consists of a number of vertices and a series
    * primitives/faces referencing the vertices. In addition there might be a
    * series of bones, each of them addressing a number of vertices with a
    * certain weight. Vertex data is presented in channels with each channel
    * containing a single per-vertex information such as a set of texture
    * coords or a normal vector. If a data pointer is non-null, the
    * corresponding data stream is present.
    *
    * A mesh uses only a single material which is referenced by a material ID.
    *
    * Note: The <code>mPositions</code> member is usually not optional.
    *    However, vertex positions <em>could</em> be missing if the
    *    <code>AI_SCENE_FLAGS_INCOMPLETE</code> flag is set in
    *    <code>aiScene.mFlags</code>.
    */
   struct aiMesh {
      /**
       * Bitwise combination of <code>aiPrimitiveType</code> members.
       *
       * This specifies which types of primitives are present in the mesh.
       * The <code>SortByPrimitiveType</code> post processing step can be used
       * to make sure the output meshes consist of one primitive type each.
       */
      uint mPrimitiveTypes;

      /**
       * The number of vertices in this mesh.
       *
       * This is also the size of all of the per-vertex data arrays.
       */
      uint mNumVertices;

      /**
       * The number of primitives (triangles, polygons, lines) in this  mesh.
       * This is also the size of the <code>mFaces</code> array.
       */
      uint mNumFaces;

      /**
       * Vertex positions.
       *
       * This array is always present in a mesh. The array is
       * <code>mNumVertices</code> in size.
       */
      aiVector3D* mVertices;

      /**
       * Vertex normals.
       *
       * The array contains normalized vectors, null if not present.
       * The array is <code>mNumVertices</code> in size.
       *
       * Normals are undefined for point and line primitives. A mesh
       * consisting of points and lines only may not have normal vectors.
       * Meshes with mixed primitive types (i.e. lines and triangles) may have
       * normals, but the normals for vertices that are only referenced by
       * point or line primitives are undefined and set to <code>QNAN</code>.
       *
       * Note: Normal vectors computed by Assimp are always unit-length.
       *    However, this needn't apply for normals that have been taken
       *    directly from the model file.
       */
      aiVector3D* mNormals;

      /**
       * Vertex tangents.
       *
       * The tangent of a vertex points in the direction of the positive x
       * texture axis. The array contains normalized vectors, null if
       * not present. The array is <code>mNumVertices</code> in size.
       *
       * A mesh consisting of points and lines only may not have normal
       * vectors. Meshes with mixed primitive types (i.e. lines and triangles)
       * may have normals, but the normals for vertices that are only
       * referenced by point or line primitives are undefined and set to
       * <code>QNAN</code>.
       *
       * Note: If the mesh contains tangents, it automatically also contains
       *    bitangents (the bitangent is just the cross product of tangent and
       *    normal vectors).
       */
      aiVector3D* mTangents;

      /**
       * Vertex bitangents.
       *
       * The bitangent of a vertex points in the direction of the positive Y
       * texture axis. The array contains normalized vectors, null if not
       * present. The array is <code>mNumVertices</code> in size.
       *
       * Note: If the mesh contains tangents, it automatically also contains
       *    bitangents.
       */
      aiVector3D* mBitangents;

      /**
       * Vertex color sets.
       *
       * A mesh may contain 0 to <code>AI_MAX_NUMBER_OF_COLOR_SETS</code>
       * vertex colors per vertex. null if not present.
       *
       * Each array is <code>mNumVertices</code> in size if present.
       */
      aiColor4D* mColors[ AI_MAX_NUMBER_OF_COLOR_SETS ];

      /**
       * Vertex texture coords, also known as UV channels.
       * A mesh may contain 0 to <code>AI_MAX_NUMBER_OF_TEXTURECOORDS</code>
       * per vertex. null if not present.
       *
       * Each array is <code>mNumVertices</code> in size.
       */
      aiVector3D* mTextureCoords[ AI_MAX_NUMBER_OF_TEXTURECOORDS ];

      /**
       * Specifies the number of components for a given UV channel.
       *
       * Up to three channels are supported (UVW, for accessing volume or cube
       * maps). If the value is 2 for a given channel <code>n</code>, the
       * component <code>p.z</code> of <code>mTextureCoords[n][p]</code> is set
       * to 0. If the value is 1 for a given channel, <code>p.y</code> is set
       * to 0, too. If this value is 0, 2 should be assumed.
       *
       * Note: 4D coords are not supported.
       */
      uint mNumUVComponents[ AI_MAX_NUMBER_OF_TEXTURECOORDS ];

      /**
       * The faces the mesh is contstructed from.
       *
       * Each face referrs to a number of vertices by their indices.
       * This array is always present in a mesh, its size is given
       * in <code>mNumFaces</code>. If the
       * <code>AI_SCENE_FLAGS_NON_VERBOSE_FORMAT</code> is <em>not</em> set,
       * each face references an unique set of vertices.
       */
      aiFace* mFaces;

      /**
       * The number of bones this mesh contains.
       *
       * Can be 0, in which case the <code>mBones</code> array is null.
       */
      uint mNumBones;

      /**
       * The bones of this mesh.
       *
       * A bone consists of a name by which it can be found in the frame
       * hierarchy and a set of vertex weights.
       */
      aiBone** mBones;

      /**
       * The material used by this mesh.
       *
       * A mesh does use only a single material. If an imported model uses
       * multiple materials, the import splits up the mesh. Use this value as
       * index into the scene's material list.
       */
      uint mMaterialIndex;
   }
}
