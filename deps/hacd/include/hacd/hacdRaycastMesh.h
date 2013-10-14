/* Copyright (c) 2011 Khaled Mamou (kmamou at gmail dot com)
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

   3. The names of the contributors may not be used to endorse or promote products derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
   IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   */

#ifndef _HACD_RAYCAST_MESH_H_
#define _HACD_RAYCAST_MESH_H_

#include <vector>
#include <stdint.h>

#include <hacd/hacdSArray.h>
#include <hacd/hacdVersion.h>
#include <hacd/hacdVector.h>
#include <hacd/hacdSArray.h>

namespace HACD
{
  class RaycastMesh;
  class RMNode;

  class BBox
  {
    public: BBox(void);

    public: ~BBox(void);

    public: bool Raycast(const Vec3<double> &origin,
                const Vec3<double> &dir, double &distance) const;

    private: Vec3<double> m_min;
    private: Vec3<double> m_max;
    private: friend class RMNode;
    private: friend class RaycastMesh;
  };

  enum RMSplitAxis
  {
    RMSplitAxis_X,
    RMSplitAxis_Y,
    RMSplitAxis_Z
  };
  class RMNode
  {
    public:
      void                  ComputeBB();
      bool                  Raycast(const Vec3<double> & from, const Vec3<double> & dir, long & triID, double & distance, Vec3<Real> & hitPoint, Vec3<Real> & hitNormal) const;
      void                  Create(size_t depth, size_t maxDepth, size_t minLeafSize, double minAxisSize);
      ~RMNode(void){}
      RMNode(void)
      {
        m_idRight = m_idLeft = m_id = -1;
        m_rm = 0;
        m_leaf = false;
      }
      long                  m_id;
      long                  m_idLeft;
      long                  m_idRight;
      BBox                  m_bBox;
      SArray<long, SARRAY_DEFAULT_MIN_SIZE>  m_triIDs;
      RaycastMesh *              m_rm;
      bool                  m_leaf;
  };

  class RaycastMesh
  {
    public:
      size_t                    GetNNodes() const { return m_nNodes;}
      size_t                    AddNode() { m_nNodes++; return m_nNodes-1; }
      void                    ComputeBB();
      bool                    Raycast(const Vec3<double> & from, const Vec3<double> & dir, long & triID, double & distance, Vec3<Real> & hitPoint, Vec3<Real> & hitNormal) const;
      void                    Initialize(size_t nVertices, size_t nTriangles,
          Vec3<double> *  vertices,  Vec3<int32_t> * triangles,
          size_t maxDepth=15, size_t minLeafSize = 4, double minAxisSize = 2.0);
      RaycastMesh(void);
      ~RaycastMesh(void);
    private :

    private:
      Vec3<int32_t> *              m_triangles;
      Vec3<double> *              m_vertices;
      size_t                  m_nVertices;
      size_t                  m_nTriangles;
      RMNode *                m_nodes;
      BBox                  m_bBox;
      size_t                  m_nNodes;
      size_t                  m_nMaxNodes;
      friend class RMNode;
  };
}
#endif
