/*
---------------------------------------------------------------------------
Open Asset Import Library (ASSIMP)
---------------------------------------------------------------------------

Copyright (c) 2006-2008, ASSIMP Development Team

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
/** @file  Assimp.cpp
 *  @brief Implementation of the Plain-C API
 */

#include "AssimpPCH.h"
#include "assimp.h"
#include "aiFileIO.h"

#include "GenericProperty.h"

// ------------------------------------------------------------------------------------------------
#ifdef AI_C_THREADSAFE
#	include <boost/thread/thread.hpp>
#	include <boost/thread/mutex.hpp>
#endif
// ------------------------------------------------------------------------------------------------
using namespace Assimp;

namespace Assimp
{
	/** Stores the importer objects for all active import processes */
	typedef std::map<const aiScene*, Assimp::Importer*> ImporterMap;

	/** Stores the LogStream objects for all active C log streams */
	struct mpred {
		bool operator  () (const aiLogStream& s0, const aiLogStream& s1) const  {
			return s0.callback<s1.callback&&s0.user<s1.user;
		}
	};
	typedef std::map<aiLogStream, Assimp::LogStream*, mpred> LogStreamMap;

	/** Stores the LogStream objects allocated by #aiGetPredefinedLogStream */
	typedef std::list<Assimp::LogStream*> PredefLogStreamMap;

	/** Local storage of all active import processes */
	static ImporterMap gActiveImports;

	/** Local storage of all active log streams */
	static LogStreamMap gActiveLogStreams;

	/** Local storage of LogStreams allocated by #aiGetPredefinedLogStream */
	static PredefLogStreamMap gPredefinedStreams;

	/** Error message of the last failed import process */
	static std::string gLastErrorString;

	/** Verbose logging active or not? */
	static aiBool gVerboseLogging = false;
}

/** Configuration properties */
static ImporterPimpl::IntPropertyMap gIntProperties;
static ImporterPimpl::FloatPropertyMap gFloatProperties;
static ImporterPimpl::StringPropertyMap	gStringProperties;

#ifdef AI_C_THREADSAFE
/** Global mutex to manage the access to the importer map */
static boost::mutex gMutex;

/** Global mutex to manage the access to the logstream map */
static boost::mutex gLogStreamMutex;
#endif

class CIOSystemWrapper;
class CIOStreamWrapper;

// ------------------------------------------------------------------------------------------------
// Custom IOStream implementation for the C-API
class CIOStreamWrapper : public IOStream
{
	friend class CIOSystemWrapper;
public:

	CIOStreamWrapper(aiFile* pFile)
		: mFile(pFile)
	{}

	// ...................................................................
	size_t Read(void* pvBuffer, 
		size_t pSize, 
		size_t pCount
	){
		// need to typecast here as C has no void*
		return mFile->ReadProc(mFile,(char*)pvBuffer,pSize,pCount);
	}

	// ...................................................................
	size_t Write(const void* pvBuffer, 
		size_t pSize,
		size_t pCount
	){
		// need to typecast here as C has no void*
		return mFile->WriteProc(mFile,(const char*)pvBuffer,pSize,pCount);
	}

	// ...................................................................
	aiReturn Seek(size_t pOffset,
		aiOrigin pOrigin
	){
		return mFile->SeekProc(mFile,pOffset,pOrigin);
	}

	// ...................................................................
	size_t Tell(void) const {
		return mFile->TellProc(mFile);
	}

	// ...................................................................
	size_t	FileSize() const {
		return mFile->FileSizeProc(mFile);
	}

	// ...................................................................
	void Flush () {
		return mFile->FlushProc(mFile);
	}

private:
	aiFile* mFile;
};

// ------------------------------------------------------------------------------------------------
// Custom IOStream implementation for the C-API
class CIOSystemWrapper : public IOSystem
{
public:
	CIOSystemWrapper(aiFileIO* pFile)
		: mFileSystem(pFile)
	{}

	// ...................................................................
	bool Exists( const char* pFile) const {
		CIOSystemWrapper* pip = const_cast<CIOSystemWrapper*>(this);
		IOStream* p = pip->Open(pFile);
		if (p){
			pip->Close(p);
			return true;
		}
		return false;
	}

	// ...................................................................
	char getOsSeparator() const {
#ifndef _WIN32
		return '/';
#else
		return '\\';
#endif
	}

	// ...................................................................
	IOStream* Open(const char* pFile,const char* pMode = "rb") {
		aiFile* p = mFileSystem->OpenProc(mFileSystem,pFile,pMode);
		if (!p) {
			return NULL;
		}
		return new CIOStreamWrapper(p);
	}

	// ...................................................................
	void Close( IOStream* pFile) {
		if (!pFile) {
			return;
		}
		mFileSystem->CloseProc(mFileSystem,((CIOStreamWrapper*) pFile)->mFile);
		delete pFile;
	}
private:
	aiFileIO* mFileSystem;
};

// ------------------------------------------------------------------------------------------------
// Custom LogStream implementation for the C-API
class LogToCallbackRedirector : public LogStream
{
public:
	LogToCallbackRedirector(const aiLogStream& s) 
		: stream (s)	{
			ai_assert(NULL != s.callback);
	}

	~LogToCallbackRedirector()	{
#ifdef AI_C_THREADSAFE
		boost::mutex::scoped_lock lock(gLogStreamMutex);
#endif
		// (HACK) Check whether the 'stream.user' pointer points to a
		// custom LogStream allocated by #aiGetPredefinedLogStream.
		// In this case, we need to delete it, too. Of course, this 
		// might cause strange problems, but the chance is quite low.

		PredefLogStreamMap::iterator it = std::find(gPredefinedStreams.begin(), 
			gPredefinedStreams.end(), (Assimp::LogStream*)stream.user);

		if (it != gPredefinedStreams.end()) {
			delete *it;
			gPredefinedStreams.erase(it);
		}
	}

	/** @copydoc LogStream::write */
	void write(const char* message)	{
		stream.callback(message,stream.user);
	}

private:
	aiLogStream stream;
};

// ------------------------------------------------------------------------------------------------
void ReportSceneNotFoundError()
{
	DefaultLogger::get()->error("Unable to find the Assimp::Importer for this aiScene. "
		"Are you playing fools with us? Don't mix cpp and c API. Thanks.");

	assert(false);
}

// ------------------------------------------------------------------------------------------------
// Reads the given file and returns its content. 
const aiScene* aiImportFile( const char* pFile, unsigned int pFlags)
{
	return aiImportFileEx(pFile,pFlags,NULL);
}

// ------------------------------------------------------------------------------------------------
const aiScene* aiImportFileEx( const char* pFile, unsigned int pFlags, 
	aiFileIO* pFS)
{
	ai_assert(NULL != pFile);
	// create an Importer for this file
	Assimp::Importer* imp = new Assimp::Importer();

#ifdef AI_C_THREADSAFE
	boost::mutex::scoped_lock lock(gMutex);
#endif
	// copy the global property lists to the Importer instance
	imp->pimpl->mIntProperties = gIntProperties;
	imp->pimpl->mFloatProperties = gFloatProperties;
	imp->pimpl->mStringProperties = gStringProperties;

#ifdef AI_C_THREADSAFE
	lock.unlock();
#endif

	// setup a custom IO system if necessary
	if (pFS)	{
		imp->SetIOHandler( new CIOSystemWrapper (pFS) );
	}

	// and have it read the file
	const aiScene* scene = imp->ReadFile( pFile, pFlags);

	// if succeeded, place it in the collection of active processes
	if( scene)	{
#ifdef AI_C_THREADSAFE
		lock.lock();
#endif
		gActiveImports[scene] = imp;
	} 
	else	{
		// if failed, extract error code and destroy the import
		gLastErrorString = imp->GetErrorString();
		delete imp;
	}

	// return imported data. If the import failed the pointer is NULL anyways
	return scene;
}

// ------------------------------------------------------------------------------------------------
const aiScene* aiImportFileFromMemory( 
	const char* pBuffer,
	unsigned int pLength,
	unsigned int pFlags,
	const char* pHint)
{
	ai_assert(NULL != pBuffer && 0 != pLength);

	// create an Importer for this file
	Assimp::Importer* imp = new Assimp::Importer();

#ifdef AI_C_THREADSAFE
	boost::mutex::scoped_lock lock(gMutex);
#endif
	// copy the global property lists to the Importer instance
	imp->pimpl->mIntProperties = gIntProperties;
	imp->pimpl->mFloatProperties = gFloatProperties;
	imp->pimpl->mStringProperties = gStringProperties;

#ifdef AI_C_THREADSAFE
	lock.unlock();
#endif

	// and have it read the file from the memory buffer
	const aiScene* scene = imp->ReadFileFromMemory( pBuffer, pLength, pFlags,pHint);

	// if succeeded, place it in the collection of active processes
	if( scene)	{
#ifdef AI_C_THREADSAFE
		lock.lock();
#endif
		gActiveImports[scene] = imp;
	} 
	else	{
		// if failed, extract error code and destroy the import
		gLastErrorString = imp->GetErrorString();
		delete imp;
	}
	// return imported data. If the import failed the pointer is NULL anyways
	return scene;
}

// ------------------------------------------------------------------------------------------------
// Releases all resources associated with the given import process. 
void aiReleaseImport( const aiScene* pScene)
{
	if (!pScene) {
		return;
	}

#ifdef AI_C_THREADSAFE
	boost::mutex::scoped_lock lock(gMutex);
#endif

	// find the importer associated with this data
	ImporterMap::iterator it = gActiveImports.find( pScene);
	// it should be there... else the user is playing fools with us
	if( it == gActiveImports.end())	{
		ReportSceneNotFoundError();
		return;
	}

	// kill the importer, the data dies with it
	delete it->second;
	gActiveImports.erase( it);
}

// ------------------------------------------------------------------------------------------------
ASSIMP_API const aiScene* aiApplyPostProcessing(const aiScene* pScene,
	unsigned int pFlags)
{
#ifdef AI_C_THREADSAFE
	boost::mutex::scoped_lock lock(gMutex);
#endif
	// find the importer associated with this data
	ImporterMap::iterator it = gActiveImports.find( pScene);
	// it should be there... else the user is playing fools with us
	if( it == gActiveImports.end())	{
		ReportSceneNotFoundError();
		return NULL;
	}
#ifdef AI_C_THREADSAFE
	lock.unlock();
#endif
	const aiScene* sc = it->second->ApplyPostProcessing(pFlags);
#ifdef AI_C_THREADSAFE
	lock.lock();
#endif
	if (!sc) {
		// kill the importer, the data dies with it
		delete it->second;
		gActiveImports.erase( it);
		return NULL;
	}

	return it->first;
}

// ------------------------------------------------------------------------------------------------
void CallbackToLogRedirector (const char* msg, char* dt)
{
	ai_assert(NULL != msg && NULL != dt);
	LogStream* s = (LogStream*)dt;

	s->write(msg);
}

// ------------------------------------------------------------------------------------------------
ASSIMP_API aiLogStream aiGetPredefinedLogStream(aiDefaultLogStream pStream,const char* file)
{
	aiLogStream sout;
	LogStream* stream = LogStream::createDefaultStream(pStream,file);
	if (!stream) {
		sout.callback = NULL;
	}
	else {
		sout.callback = &CallbackToLogRedirector;
		sout.user = (char*)stream;
	}
	gPredefinedStreams.push_back(stream);
	return sout;
}

// ------------------------------------------------------------------------------------------------
ASSIMP_API void aiAttachLogStream( const aiLogStream* stream )
{
#ifdef AI_C_THREADSAFE
	boost::mutex::scoped_lock lock(gLogStreamMutex);
#endif
	LogStream* lg = new LogToCallbackRedirector(*stream);
	gActiveLogStreams[*stream] = lg;

	if (DefaultLogger::isNullLogger()) {
		DefaultLogger::create(NULL,(gVerboseLogging == AI_TRUE ? Logger::VERBOSE : Logger::NORMAL));
	}
	DefaultLogger::get()->attachStream(lg);
}

// ------------------------------------------------------------------------------------------------
ASSIMP_API aiReturn aiDetachLogStream( const aiLogStream* stream)
{
#ifdef AI_C_THREADSAFE
	boost::mutex::scoped_lock lock(gLogStreamMutex);
#endif
	// find the logstream associated with this data
	LogStreamMap::iterator it = gActiveLogStreams.find( *stream);
	// it should be there... else the user is playing fools with us
	if( it == gActiveLogStreams.end())	{
		return AI_FAILURE;
	}
	delete it->second;
	gActiveLogStreams.erase( it);

	if (gActiveLogStreams.empty()) {
		DefaultLogger::kill();
	}
	return AI_SUCCESS;
}

// ------------------------------------------------------------------------------------------------
ASSIMP_API void aiDetachAllLogStreams(void)
{
#ifdef AI_C_THREADSAFE
	boost::mutex::scoped_lock lock(gLogStreamMutex);
#endif
	for (LogStreamMap::iterator it = gActiveLogStreams.begin(); it != gActiveLogStreams.end(); ++it) {
		delete it->second;
	}
	gActiveLogStreams.clear();
	DefaultLogger::kill();
}

// ------------------------------------------------------------------------------------------------
ASSIMP_API void aiEnableVerboseLogging(aiBool d)
{
	if (!DefaultLogger::isNullLogger()) {
		DefaultLogger::get()->setLogSeverity((d == AI_TRUE ? Logger::VERBOSE : Logger::NORMAL));
	}
	gVerboseLogging = d;
}

// ------------------------------------------------------------------------------------------------
// Returns the error text of the last failed import process. 
const char* aiGetErrorString()
{
	return gLastErrorString.c_str();
}

// ------------------------------------------------------------------------------------------------
// Returns the error text of the last failed import process. 
aiBool aiIsExtensionSupported(const char* szExtension)
{
	ai_assert(NULL != szExtension);
#ifdef AI_C_THREADSAFE
	boost::mutex::scoped_lock lock(gMutex);
#endif

	if (!gActiveImports.empty())	{
		return ((*(gActiveImports.begin())).second->IsExtensionSupported( szExtension )) ? AI_TRUE : AI_FALSE;
	}

	// fixme: no need to create a temporary Importer instance just for that .. 
	Assimp::Importer tmp;
	return tmp.IsExtensionSupported(std::string(szExtension)) ? AI_TRUE : AI_FALSE;
}

// ------------------------------------------------------------------------------------------------
// Get a list of all file extensions supported by ASSIMP
void aiGetExtensionList(aiString* szOut)
{
	ai_assert(NULL != szOut);
#ifdef AI_C_THREADSAFE
	boost::mutex::scoped_lock lock(gMutex);
#endif

	if (!gActiveImports.empty())
	{
		(*(gActiveImports.begin())).second->GetExtensionList(*szOut);
		return;
	}
	// fixme: no need to create a temporary Importer instance just for that .. 
	Assimp::Importer tmp;
	tmp.GetExtensionList(*szOut);
}

// ------------------------------------------------------------------------------------------------
// Get the memory requirements for a particular import.
void aiGetMemoryRequirements(const C_STRUCT aiScene* pIn,
	C_STRUCT aiMemoryInfo* in)
{
#ifdef AI_C_THREADSAFE
	boost::mutex::scoped_lock lock(gMutex);
#endif

	// find the importer associated with this data
	ImporterMap::iterator it = gActiveImports.find( pIn);
	// it should be there... else the user is playing fools with us
	if( it == gActiveImports.end())	{
		ReportSceneNotFoundError();
		return;
	}
	// get memory statistics
#ifdef AI_C_THREADSAFE
	lock.unlock();
#endif
	it->second->GetMemoryRequirements(*in);
}

// ------------------------------------------------------------------------------------------------
// Importer::SetPropertyInteger
ASSIMP_API void aiSetImportPropertyInteger(const char* szName, int value)
{
#ifdef AI_C_THREADSAFE
	boost::mutex::scoped_lock lock(gMutex);
#endif
	SetGenericProperty<int>(gIntProperties,szName,value,NULL);
}

// ------------------------------------------------------------------------------------------------
// Importer::SetPropertyFloat
ASSIMP_API void aiSetImportPropertyFloat(const char* szName, float value)
{
#ifdef AI_C_THREADSAFE
	boost::mutex::scoped_lock lock(gMutex);
#endif
	SetGenericProperty<float>(gFloatProperties,szName,value,NULL);
}

// ------------------------------------------------------------------------------------------------
// Importer::SetPropertyString
ASSIMP_API void aiSetImportPropertyString(const char* szName,
	const C_STRUCT aiString* st)
{
	if (!st) {
		return;
	}
#ifdef AI_C_THREADSAFE
	boost::mutex::scoped_lock lock(gMutex);
#endif
	SetGenericProperty<std::string>(gStringProperties,szName,
		std::string( st->data ),NULL);
}

// ------------------------------------------------------------------------------------------------
// Rotation matrix to quaternion
ASSIMP_API void aiCreateQuaternionFromMatrix(aiQuaternion* quat,const aiMatrix3x3* mat)
{
	ai_assert(NULL != quat && NULL != mat);
	*quat = aiQuaternion(*mat);
}

// ------------------------------------------------------------------------------------------------
// Matrix decomposition
ASSIMP_API void aiDecomposeMatrix(const aiMatrix4x4* mat,aiVector3D* scaling,
	aiQuaternion* rotation,
	aiVector3D* position)
{
	ai_assert(NULL != rotation && NULL != position && NULL != scaling && NULL != mat);
	mat->Decompose(*scaling,*rotation,*position);
}

// ------------------------------------------------------------------------------------------------
// Matrix transpose
ASSIMP_API void aiTransposeMatrix3(aiMatrix3x3* mat)
{
	ai_assert(NULL != mat);
	mat->Transpose();
}

// ------------------------------------------------------------------------------------------------
ASSIMP_API void aiTransposeMatrix4(aiMatrix4x4* mat)
{
	ai_assert(NULL != mat);
	mat->Transpose();
}

// ------------------------------------------------------------------------------------------------
// Vector transformation
ASSIMP_API void aiTransformVecByMatrix3(C_STRUCT aiVector3D* vec, 
	const C_STRUCT aiMatrix3x3* mat)
{
	ai_assert(NULL != mat && NULL != vec);
	*vec *= (*mat);
}

// ------------------------------------------------------------------------------------------------
ASSIMP_API void aiTransformVecByMatrix4(C_STRUCT aiVector3D* vec, 
	const C_STRUCT aiMatrix4x4* mat)
{
	ai_assert(NULL != mat && NULL != vec);
	*vec *= (*mat);
}

// ------------------------------------------------------------------------------------------------
// Matrix multiplication
ASSIMP_API void aiMultiplyMatrix4(
	C_STRUCT aiMatrix4x4* dst, 
	const C_STRUCT aiMatrix4x4* src)
{
	ai_assert(NULL != dst && NULL != src);
	*dst = (*dst) * (*src);
}

// ------------------------------------------------------------------------------------------------
ASSIMP_API void aiMultiplyMatrix3(
	C_STRUCT aiMatrix3x3* dst, 
	const C_STRUCT aiMatrix3x3* src)
{
	ai_assert(NULL != dst && NULL != src);
	*dst = (*dst) * (*src);
}

// ------------------------------------------------------------------------------------------------
// Matrix identity
ASSIMP_API void aiIdentityMatrix3(
	C_STRUCT aiMatrix3x3* mat)
{
	ai_assert(NULL != mat);
	*mat = aiMatrix3x3();
}

// ------------------------------------------------------------------------------------------------
ASSIMP_API void aiIdentityMatrix4(
	C_STRUCT aiMatrix4x4* mat)
{
	ai_assert(NULL != mat);
	*mat = aiMatrix4x4();
}


