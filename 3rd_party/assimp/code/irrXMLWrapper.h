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

#ifndef INCLUDED_AI_IRRXML_WRAPPER
#define INCLUDED_AI_IRRXML_WRAPPER

// some long includes ....
#include "./../contrib/irrXML/irrXML.h"
#include "./IOStream.h"
namespace Assimp	{

// ---------------------------------------------------------------------------------
/** @brief Utility class to make IrrXML work together with our custom IO system
 *  See the IrrXML docs for more details.
 *
 *  Construct IrrXML-Reader in BaseImporter::InternReadFile():
 *  @code
 * // open the file
 * boost::scoped_ptr<IOStream> file( pIOHandler->Open( pFile));
 * if( file.get() == NULL) {
 *	  throw new ImportErrorException( "Failed to open file " + pFile + ".");
 * }
 *
 * // generate a XML reader for it
 * boost::scoped_ptr<CIrrXML_IOStreamReader> mIOWrapper( new CIrrXML_IOStreamReader( file.get()));
 * mReader = irr::io::createIrrXMLReader( mIOWrapper.get());
 * if( !mReader) {
 *    ThrowException( "xxxx: Unable to open file.");
 * }
 * @endcode
 **/
class CIrrXML_IOStreamReader 
	: public irr::io::IFileReadCallBack
{
public:

	// ----------------------------------------------------------------------------------
	//! Construction from an existing IOStream
	CIrrXML_IOStreamReader(IOStream* _stream)
		: stream (_stream)
		, t (0)
	{

		// Map the buffer into memory and convert it to UTF8. IrrXML provides its
		// own conversion, which is merely a cast from uintNN_t to uint8_t. Thus,
		// it is not suitable for our purposes and we have to do it BEFORE IrrXML
		// gets the buffer. Sadly, this forces as to map the whole file into
		// memory.

		data.resize(stream->FileSize());
		stream->Read(&data[0],data.size(),1);

		BaseImporter::ConvertToUTF8(data);
	}

	// ----------------------------------------------------------------------------------
	//! Virtual destructor
	virtual ~CIrrXML_IOStreamReader() {};

	// ----------------------------------------------------------------------------------
	//!   Reads an amount of bytes from the file.
	/**  @param buffer:       Pointer to output buffer.
	 *   @param sizeToRead:   Amount of bytes to read 
	 *   @return              Returns how much bytes were read.  */
	virtual int read(void* buffer, int sizeToRead)	{
		if(sizeToRead<0) {
			return 0;
		}
		if(t+sizeToRead>data.size()) {
			sizeToRead = data.size()-t;
		}

		memcpy(buffer,&data.front()+t,sizeToRead);

		t += sizeToRead;
		return sizeToRead;
	}

	// ----------------------------------------------------------------------------------
	//! Returns size of file in bytes
	virtual int getSize()	{
		return (int)data.size();
	}

private:
	IOStream* stream;
	std::vector<char> data;
	size_t t;

}; // ! class CIrrXML_IOStreamReader

} // ! Assimp

#endif // !! INCLUDED_AI_IRRXML_WRAPPER
