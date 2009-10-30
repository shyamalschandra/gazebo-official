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

/** @file Defines a post processing step to search all meshes for
  degenerated faces */
#ifndef AI_FINDDEGENERATESPROCESS_H_INC
#define AI_FINDDEGENERATESPROCESS_H_INC

#include "BaseProcess.h"
#include "aiMesh.h"

class FindDegeneratesProcessTest;
namespace Assimp	{


// ---------------------------------------------------------------------------
/** FindDegeneratesProcess: Searches a mesh for degenerated triangles.
*/
class ASSIMP_API FindDegeneratesProcess : public BaseProcess
{
	friend class Importer;
	friend class ::FindDegeneratesProcessTest; // grant the unit test full access to us

protected:
	/** Constructor to be privately used by Importer */
	FindDegeneratesProcess();

	/** Destructor, private as well */
	~FindDegeneratesProcess();

public:
	
	// -------------------------------------------------------------------
	// Check whether step is active
	bool IsActive( unsigned int pFlags) const;

	// -------------------------------------------------------------------
	// Execute step on a given scene
	void Execute( aiScene* pScene);

	// -------------------------------------------------------------------
	// Setup import settings
	void SetupProperties(const Importer* pImp);

	// -------------------------------------------------------------------
	// Execute step on a given mesh
	void ExecuteOnMesh( aiMesh* mesh);


	// -------------------------------------------------------------------
	/** @brief Enable the instant removal of degenerated primitives
	 *  @param d hm ... difficult to guess what this means, hu!?
	 */
	void EnableInstantRemoval(bool d) {
		configRemoveDegenerates = d;
	}

	// -------------------------------------------------------------------
	/** @brief Check whether instant removal is currently enabled
	 *  @return ...
	 */
	bool IsInstantRemoval() const {
		return configRemoveDegenerates;
	}

private:

	//! Configuration option: remove degenerates faces immediately
	bool configRemoveDegenerates;
};
}

#endif // !! AI_FINDDEGENERATESPROCESS_H_INC
