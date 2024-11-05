
#include <stdio.h>
#include "APhysXStdAfx.h"

//-------------------------------------------------------------------------------------------------

APhysXFileStream::APhysXFileStream(const char* filename, bool load) : fp(NULL)
	{
	fp = fopen(filename, load ? "rb" : "wb");
	APHYSX_ASSERT(fp);
	}

APhysXFileStream::~APhysXFileStream()
	{
	if(fp)	fclose(fp);
	}

// Loading API
NxU8 APhysXFileStream::readByte() const
	{
	NxU8 b;
	size_t r = fread(&b, sizeof(NxU8), 1, fp);
	NX_ASSERT(r);
	return b;
	}

NxU16 APhysXFileStream::readWord() const
	{
	NxU16 w;
	size_t r = fread(&w, sizeof(NxU16), 1, fp);
	NX_ASSERT(r);
	return w;
	}

NxU32 APhysXFileStream::readDword() const
	{
	NxU32 d;
	size_t r = fread(&d, sizeof(NxU32), 1, fp);
	NX_ASSERT(r);
	return d;
	}

float APhysXFileStream::readFloat() const
	{
	NxReal f;
	size_t r = fread(&f, sizeof(NxReal), 1, fp);
	NX_ASSERT(r);
	return f;
	}

double APhysXFileStream::readDouble() const
	{
	NxF64 f;
	size_t r = fread(&f, sizeof(NxF64), 1, fp);
	NX_ASSERT(r);
	return f;
	}

void APhysXFileStream::readBuffer(void* buffer, NxU32 size)	const
	{
	size_t w = fread(buffer, size, 1, fp);
	NX_ASSERT(w);
	}

// Saving API
NxStream& APhysXFileStream::storeByte(NxU8 b)
	{
	size_t w = fwrite(&b, sizeof(NxU8), 1, fp);
	NX_ASSERT(w);
	return *this;
	}

NxStream& APhysXFileStream::storeWord(NxU16 w)
	{
	size_t ww = fwrite(&w, sizeof(NxU16), 1, fp);
	NX_ASSERT(ww);
	return *this;
	}

NxStream& APhysXFileStream::storeDword(NxU32 d)
	{
	size_t w = fwrite(&d, sizeof(NxU32), 1, fp);
	NX_ASSERT(w);
	return *this;
	}

NxStream& APhysXFileStream::storeFloat(NxReal f)
	{
	size_t w = fwrite(&f, sizeof(NxReal), 1, fp);
	NX_ASSERT(w);
	return *this;
	}

NxStream& APhysXFileStream::storeDouble(NxF64 f)
	{
	size_t w = fwrite(&f, sizeof(NxF64), 1, fp);
	NX_ASSERT(w);
	return *this;
	}

NxStream& APhysXFileStream::storeBuffer(const void* buffer, NxU32 size)
	{
	size_t w = fwrite(buffer, size, 1, fp);
	NX_ASSERT(w);
	return *this;
	}

//-------------------------------------------------------------------------------------------------

APhysXCacheFileStream::APhysXCacheFileStream(const char* filename, bool load) : fp(NULL), m_pMemoryStream(NULL), m_pData(NULL)
{
	fp = fopen(filename, load ? "rb" : "wb");
	APHYSX_ASSERT(fp);
	
	if(!fp) return;

	if(load)
	{
		// read data once from the file...
		fseek(fp, 0, SEEK_END);
		int iLen = ftell(fp);
		
		m_pData = new NxU8[iLen + 1];
		
		fseek(fp, 0, SEEK_SET);
		size_t r = fread(m_pData, iLen, 1, fp);
		NX_ASSERT(r);

		fclose(fp);
		fp = NULL;

		m_pMemoryStream = new APhysXMemoryReadBuffer(m_pData);

	}
	else
	{
		// write data to the buffer firstly...
		m_pMemoryStream = new APhysXMemoryWriteBuffer;
	}
}

APhysXCacheFileStream::~APhysXCacheFileStream()
{
	if(fp)
	{
		APhysXMemoryWriteBuffer* memWriteBuf = (APhysXMemoryWriteBuffer*) m_pMemoryStream;
		// write mode...
		size_t w = fwrite(memWriteBuf->data, memWriteBuf->currentSize, 1, fp);
		NX_ASSERT(w);
		
		fclose(fp);
	}

	if(m_pMemoryStream) delete m_pMemoryStream;

	if(m_pData) delete [] m_pData;
}

// Loading API
NxU8 APhysXCacheFileStream::readByte() const
{
	if (m_pMemoryStream)
		return m_pMemoryStream->readByte();
	else
		return 0;
}

NxU16 APhysXCacheFileStream::readWord() const
{
	if (m_pMemoryStream)
		return m_pMemoryStream->readWord();
	else
		return 0;
}

NxU32 APhysXCacheFileStream::readDword() const
{
	if (m_pMemoryStream)
		return m_pMemoryStream->readDword();
	else
		return 0;
}

float APhysXCacheFileStream::readFloat() const
{
	if (m_pMemoryStream)
		return m_pMemoryStream->readFloat();
	else
		return 0.0f;
}

double APhysXCacheFileStream::readDouble() const
{
	if (m_pMemoryStream)
		return m_pMemoryStream->readDouble();
	else
		return 0.0;
}

void APhysXCacheFileStream::readBuffer(void* buffer, NxU32 size) const
{
	if (m_pMemoryStream)
		m_pMemoryStream->readBuffer(buffer, size);
}

// Saving API
NxStream& APhysXCacheFileStream::storeByte(NxU8 b)
{
	if (m_pMemoryStream)
		m_pMemoryStream->storeByte(b);
	return *this;
}

NxStream& APhysXCacheFileStream::storeWord(NxU16 w)
{
	if (m_pMemoryStream)
		m_pMemoryStream->storeWord(w);
	return *this;
}

NxStream& APhysXCacheFileStream::storeDword(NxU32 d)
{
	if (m_pMemoryStream)
		m_pMemoryStream->storeDword(d);
	return *this;
}

NxStream& APhysXCacheFileStream::storeFloat(NxReal f)
{
	if (m_pMemoryStream)
		m_pMemoryStream->storeFloat(f);
	return *this;
}

NxStream& APhysXCacheFileStream::storeDouble(NxF64 f)
{
	if (m_pMemoryStream)
		m_pMemoryStream->storeDouble(f);
	return *this;
}

NxStream& APhysXCacheFileStream::storeBuffer(const void* buffer, NxU32 size)
{
	if (m_pMemoryStream)
		m_pMemoryStream->storeBuffer(buffer, size);
	return *this;
}

//-------------------------------------------------------------------------------------------------

APhysXFileImageStream::APhysXFileImageStream(const char* filename, bool load)
{
#if 0
	DWORD dwFlags = (load)? (AFILE_OPENEXIST|AFILE_BINARY):(AFILE_CREATENEW|AFILE_BINARY);
#else
	DWORD dwFlags = AFILE_OPENEXIST|AFILE_BINARY;
#endif

	m_pFileImage = new AFileImage;
	APHYSX_ASSERT(m_pFileImage);

	if(!m_pFileImage->Open("", filename, dwFlags))
	{
		delete m_pFileImage;
		m_pFileImage = NULL;

		m_bValid = false;
	}
	else
		m_bValid = true;

}

APhysXFileImageStream::~APhysXFileImageStream()
{
	if(m_pFileImage)
	{
		m_pFileImage->Close();
		delete m_pFileImage;
	}
}

// Loading API
NxU8 APhysXFileImageStream::readByte() const
{

	if(m_pFileImage)
	{
		NxU8 b = 0;
		DWORD r = 0;
		m_pFileImage->Read(&b, sizeof(NxU8), &r);
		NX_ASSERT(r == sizeof(NxU8));
		return b;

	}
	else
		return 0;
}

NxU16 APhysXFileImageStream::readWord() const
{
	if(m_pFileImage)
	{
		NxU16 w = 0;
		DWORD r = 0;
		m_pFileImage->Read(&w, sizeof(NxU16), &r);
		NX_ASSERT(r == sizeof(NxU16));
		return w;

	}
	else
		return 0;

}

NxU32 APhysXFileImageStream::readDword() const
{
	if(m_pFileImage)
	{
		NxU32 d = 0;
		DWORD r = 0;
		m_pFileImage->Read(&d, sizeof(NxU32), &r);
		NX_ASSERT(r == sizeof(NxU32));
		return d;
	}
	else
		return 0;
}

float APhysXFileImageStream::readFloat() const
{
	if(m_pFileImage)
	{
		NxReal f = 0;
		DWORD r = 0;
		m_pFileImage->Read(&f, sizeof(NxReal), &r);
		NX_ASSERT(r == sizeof(NxReal));
		return f;
	}
	else
		return 0.0f;
}

double APhysXFileImageStream::readDouble() const
{
	if(m_pFileImage)
	{
		NxF64 f = 0;
		DWORD r = 0;
		m_pFileImage->Read(&f, sizeof(NxF64), &r);
		NX_ASSERT(r == sizeof(NxF64));
		return f;
	}
	else
		return 0.0;
}

void APhysXFileImageStream::readBuffer(void* buffer, NxU32 size)	const
{
	if(m_pFileImage)
	{
		DWORD w = 0;
		m_pFileImage->Read(buffer, size, &w);

		NX_ASSERT(w == size);
	}

}

// Saving API
NxStream& APhysXFileImageStream::storeByte(NxU8 b)
{
#if 0
	
	if(m_pFileImage)
	{
		DWORD w;
		m_pFileImage->Write(&b, sizeof(NxU8), &w);
		NX_ASSERT(w);
	}

#else
	NX_ASSERT(0);
#endif

	return *this;
}

NxStream& APhysXFileImageStream::storeWord(NxU16 w)
{
#if 0

	if(m_pFileImage)
	{
		DWORD ww;
		m_pFileImage->Write(&w, sizeof(NxU16), &ww);
		NX_ASSERT(ww);
	}
#else
	NX_ASSERT(0);
#endif

	return *this;


}

NxStream& APhysXFileImageStream::storeDword(NxU32 d)
{
#if 0

	if(m_pFileImage)
	{
		DWORD w;
		m_pFileImage->Write(&d, sizeof(NxU32), &w);
		NX_ASSERT(w);
	}
#else
	NX_ASSERT(0);
#endif

	return *this;
}

NxStream& APhysXFileImageStream::storeFloat(NxReal f)
{
#if 0	
	
	if(m_pFileImage)
	{

		DWORD w;
		m_pFileImage->Write(&f, sizeof(NxReal), &w);
		NX_ASSERT(w);
	}

#else
	NX_ASSERT(0);
#endif

	return *this;
}

NxStream& APhysXFileImageStream::storeDouble(NxF64 f)
{
#if 0

	if(m_pFileImage)
	{

		DWORD w;
		m_pFileImage->Write(&f, sizeof(NxF64), &w);
		NX_ASSERT(w);
	}

#else
	NX_ASSERT(0);
#endif

	return *this;
}

NxStream& APhysXFileImageStream::storeBuffer(const void* buffer, NxU32 size)
{
#if 0	
	
	if(m_pFileImage)
	{
		DWORD w;
		m_pFileImage->Write((LPVOID)buffer, size, &w);
		NX_ASSERT(w);
	}

#else
	NX_ASSERT(0);
#endif

	return *this;

}



//-------------------------------------------------------------------------------------------------

APhysXMemoryWriteBuffer::APhysXMemoryWriteBuffer() : currentSize(0), maxSize(0), data(NULL)
	{
	}

APhysXMemoryWriteBuffer::~APhysXMemoryWriteBuffer()
	{
	NX_DELETE_ARRAY(data);
	}

void APhysXMemoryWriteBuffer::clear()
	{
	currentSize = 0;
	}

NxStream& APhysXMemoryWriteBuffer::storeByte(NxU8 b)
	{
	storeBuffer(&b, sizeof(NxU8));
	return *this;
	}
NxStream& APhysXMemoryWriteBuffer::storeWord(NxU16 w)
	{
	storeBuffer(&w, sizeof(NxU16));
	return *this;
	}
NxStream& APhysXMemoryWriteBuffer::storeDword(NxU32 d)
	{
	storeBuffer(&d, sizeof(NxU32));
	return *this;
	}
NxStream& APhysXMemoryWriteBuffer::storeFloat(NxReal f)
	{
	storeBuffer(&f, sizeof(NxReal));
	return *this;
	}
NxStream& APhysXMemoryWriteBuffer::storeDouble(NxF64 f)
	{
	storeBuffer(&f, sizeof(NxF64));
	return *this;
	}
NxStream& APhysXMemoryWriteBuffer::storeBuffer(const void* buffer, NxU32 size)
	{
	NxU32 expectedSize = currentSize + size;
	if(expectedSize > maxSize)
		{
		maxSize = expectedSize + 4096;

		NxU8* newData = new NxU8[maxSize];
		NX_ASSERT(newData!=NULL);

		if(data)
			{
			memcpy(newData, data, currentSize);
			delete[] data;
			}
		data = newData;
		}
	memcpy(data+currentSize, buffer, size);
	currentSize += size;
	return *this;
	}


APhysXMemoryReadBuffer::APhysXMemoryReadBuffer(const NxU8* data) : buffer(data)
	{
	}

APhysXMemoryReadBuffer::~APhysXMemoryReadBuffer()
	{
	// We don't own the data => no delete
	}

NxU8 APhysXMemoryReadBuffer::readByte() const
	{
	NxU8 b;
	memcpy(&b, buffer, sizeof(NxU8));
	buffer += sizeof(NxU8);
	return b;
	}

NxU16 APhysXMemoryReadBuffer::readWord() const
	{
	NxU16 w;
	memcpy(&w, buffer, sizeof(NxU16));
	buffer += sizeof(NxU16);
	return w;
	}

NxU32 APhysXMemoryReadBuffer::readDword() const
	{
	NxU32 d;
	memcpy(&d, buffer, sizeof(NxU32));
	buffer += sizeof(NxU32);
	return d;
	}

float APhysXMemoryReadBuffer::readFloat() const
	{
	float f;
	memcpy(&f, buffer, sizeof(float));
	buffer += sizeof(float);
	return f;
	}

double APhysXMemoryReadBuffer::readDouble() const
	{
	double f;
	memcpy(&f, buffer, sizeof(double));
	buffer += sizeof(double);
	return f;
	}

void APhysXMemoryReadBuffer::readBuffer(void* dest, NxU32 size) const
	{
	memcpy(dest, buffer, size);
	buffer += size;
	}
