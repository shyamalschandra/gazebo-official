/*
Open Asset Import Library (ASSIMP)
----------------------------------------------------------------------

Copyright (c) 2006-2008, ASSIMP Development Team
All rights reserved.

Redistribution and use of this software in source and binary forms, 
with or without modification, are permitted provided that the 
following conditions are met:

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

----------------------------------------------------------------------
*/

/** Small helper classes to optimise finding vertizes close to a given location */
#ifndef AI_SPATIALSORT_H_INC
#define AI_SPATIALSORT_H_INC

#include <vector>
#include "aiTypes.h"

namespace Assimp
{

// ------------------------------------------------------------------------------------------------
/** A little helper class to quickly find all vertices in the epsilon environment of a given
 * position. Construct an instance with an array of positions. The class stores the given positions
 * by their indices and sorts them by their distance to an arbitrary chosen plane.
 * You can then query the instance for all vertices close to a given position in an average O(log n) 
 * time, with O(n) worst case complexity when all vertices lay on the plane. The plane is chosen
 * so that it avoids common planes in usual data sets.
 */
class ASSIMP_API SpatialSort
{
public:

	SpatialSort() {/* This is unintialized. This is evil. This is OK. */}

	/** Constructs a spatially sorted representation from the given position array.
	 * Supply the positions in its layout in memory, the class will only refer to them
	 * by index.
	 * @param pPositions Pointer to the first position vector of the array.
	 * @param pNumPositions Number of vectors to expect in that array.
	 * @param pElementOffset Offset in bytes from the beginning of one vector in memory 
	 *   to the beginning of the next vector.
	 */
	SpatialSort( const aiVector3D* pPositions, unsigned int pNumPositions, 
		unsigned int pElementOffset);


	/** Sets the input data for the SpatialSort. This replaces the old data.
	 *
	 * @param pPositions Pointer to the first position vector of the array.
	 * @param pNumPositions Number of vectors to expect in that array.
	 * @param pElementOffset Offset in bytes from the beginning of one vector in memory 
	 *   to the beginning of the next vector.
	 */
	void Fill( const aiVector3D* pPositions, unsigned int pNumPositions, 
		unsigned int pElementOffset);


	/** Destructor */
	~SpatialSort();

	/** Returns an iterator for all positions close to the given position.
	 * @param pPosition The position to look for vertices.
	 * @param pRadius Maximal distance from the position a vertex may have to be counted in.
	 * @param poResults The container to store the indices of the found positions. Will be emptied
	 *   by the call so it may contain anything.
	 * @return An iterator to iterate over all vertices in the given area.
	 */
	void FindPositions( const aiVector3D& pPosition, float pRadius, std::vector<unsigned int>& poResults) const;

protected:
	/** Normal of the sorting plane, normalized. The center is always at (0, 0, 0) */
	aiVector3D mPlaneNormal;

	/** An entry in a spatially sorted position array. Consists of a vertex index,
	 * its position and its precalculated distance from the reference plane */
	struct Entry
	{
		unsigned int mIndex; ///< The vertex referred by this entry
		aiVector3D mPosition; ///< Position
		float mDistance; ///< Distance of this vertex to the sorting plane

		Entry() { /** intentionally not initialized.*/ }
		Entry( unsigned int pIndex, const aiVector3D& pPosition, float pDistance) 
			: mIndex( pIndex), mPosition( pPosition), mDistance( pDistance)
		{ 	}

		bool operator < (const Entry& e) const { return mDistance < e.mDistance; }
	};

	// all positions, sorted by distance to the sorting plane
	std::vector<Entry> mPositions;
};

} // end of namespace Assimp

#endif // AI_SPATIALSORT_H_INC
