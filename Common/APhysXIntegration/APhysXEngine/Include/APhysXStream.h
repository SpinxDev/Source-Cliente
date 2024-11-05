/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXSTREAM_H_
#define _APHYSXSTREAM_H_

#include "NxStream.h"

//-----------------------------------------------------------------------------------
// user stream based on FILE...
class APHYSX_DLL_EXPORT APhysXFileStream : public NxStream
	{
	public:
								APhysXFileStream(const char* filename, bool load);
	virtual						~APhysXFileStream();

	virtual		NxU8			readByte()								const;
	virtual		NxU16			readWord()								const;
	virtual		NxU32			readDword()								const;
	virtual		float			readFloat()								const;
	virtual		double			readDouble()							const;
	virtual		void			readBuffer(void* buffer, NxU32 size)	const;

	virtual		NxStream&		storeByte(NxU8 b);
	virtual		NxStream&		storeWord(NxU16 w);
	virtual		NxStream&		storeDword(NxU32 d);
	virtual		NxStream&		storeFloat(NxReal f);
	virtual		NxStream&		storeDouble(NxF64 f);
	virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size);

	bool IsValid() { return (fp != NULL); }

				FILE*			fp;
	};

//-----------------------------------------------------------------------------------
// user stream based on AFileImage...
// Note: we can only use this class to Load data from Package, so we should not provide
//    store functions for it!
class APHYSX_DLL_EXPORT APhysXFileImageStream : public NxStream
{

public:

	APhysXFileImageStream(const char* filename, bool load);
	virtual						~APhysXFileImageStream();

	virtual		NxU8			readByte()								const;
	virtual		NxU16			readWord()								const;
	virtual		NxU32			readDword()								const;
	virtual		float			readFloat()								const;
	virtual		double			readDouble()							const;
	virtual		void			readBuffer(void* buffer, NxU32 size)	const;

	virtual		NxStream&		storeByte(NxU8 b);
	virtual		NxStream&		storeWord(NxU16 w);
	virtual		NxStream&		storeDword(NxU32 d);
	virtual		NxStream&		storeFloat(NxReal f);
	virtual		NxStream&		storeDouble(NxF64 f);
	virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size);

	bool IsValid() { return m_bValid; }

protected:	
	
	AFileImage* m_pFileImage;

	bool m_bValid;
};

//-----------------------------------------------------------------------------------
class APHYSX_DLL_EXPORT APhysXMemoryWriteBuffer : public NxStream
	{
	public:
								APhysXMemoryWriteBuffer();
	virtual						~APhysXMemoryWriteBuffer();
				void			clear();

	virtual		NxU8			readByte()								const	{ NX_ASSERT(0);	return 0;	}
	virtual		NxU16			readWord()								const	{ NX_ASSERT(0);	return 0;	}
	virtual		NxU32			readDword()								const	{ NX_ASSERT(0);	return 0;	}
	virtual		float			readFloat()								const	{ NX_ASSERT(0);	return 0.0f;}
	virtual		double			readDouble()							const	{ NX_ASSERT(0);	return 0.0;	}
	virtual		void			readBuffer(void* buffer, NxU32 size)	const	{ NX_ASSERT(0);				}

	virtual		NxStream&		storeByte(NxU8 b);
	virtual		NxStream&		storeWord(NxU16 w);
	virtual		NxStream&		storeDword(NxU32 d);
	virtual		NxStream&		storeFloat(NxReal f);
	virtual		NxStream&		storeDouble(NxF64 f);
	virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size);

				NxU32			currentSize;
				NxU32			maxSize;
				NxU8*			data;
	};

//-----------------------------------------------------------------------------------
class APHYSX_DLL_EXPORT APhysXMemoryReadBuffer : public NxStream
	{
	public:
								APhysXMemoryReadBuffer(const NxU8* data = NULL);
	virtual						~APhysXMemoryReadBuffer();

	virtual		NxU8			readByte()								const;
	virtual		NxU16			readWord()								const;
	virtual		NxU32			readDword()								const;
	virtual		float			readFloat()								const;
	virtual		double			readDouble()							const;
	virtual		void			readBuffer(void* buffer, NxU32 size)	const;

	virtual		NxStream&		storeByte(NxU8 b)							{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeWord(NxU16 w)							{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeDword(NxU32 d)							{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeFloat(NxReal f)						{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeDouble(NxF64 f)						{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size)	{ NX_ASSERT(0);	return *this;	}

	mutable		const NxU8*		buffer;
	};


//-----------------------------------------------------------------------------------
// cache the direct file reading/writing operation by memory stream

class APHYSX_DLL_EXPORT APhysXCacheFileStream : public NxStream
{
public:
	APhysXCacheFileStream(const char* filename, bool load);
	virtual						~APhysXCacheFileStream();

	virtual		NxU8			readByte()								const;
	virtual		NxU16			readWord()								const;
	virtual		NxU32			readDword()								const;
	virtual		float			readFloat()								const;
	virtual		double			readDouble()							const;
	virtual		void			readBuffer(void* buffer, NxU32 size)	const;

	virtual		NxStream&		storeByte(NxU8 b);
	virtual		NxStream&		storeWord(NxU16 w);
	virtual		NxStream&		storeDword(NxU32 d);
	virtual		NxStream&		storeFloat(NxReal f);
	virtual		NxStream&		storeDouble(NxF64 f);
	virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size);

	bool IsValid() { return (m_pMemoryStream != NULL); }

protected:

	FILE*			fp;
	NxStream*		m_pMemoryStream;
	NxU8*			m_pData;
};

//-----------------------------------------------------------------------------------
// Define the default user stream: APhysXFileStream
#define	APX_USE_CACHE_FILE_STREAM

#ifdef	APX_USE_CACHE_FILE_STREAM
	typedef APhysXCacheFileStream APhysXUserStream;
#else
	typedef APhysXFileStream APhysXUserStream;
#endif




#endif
