/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXSERIALIZERBASE_H_
#define _APHYSXSERIALIZERBASE_H_

#include "NxStream.h"

//-----------------------------------------------------------------
// Note: 
//		if you declare one class's own version number, you have to 
// override the Save() and Load() function to save and load its
// version number.

// the default macro will make Save()/Load() function public...
#define DeclareVersion(VerNumber)								\
	public:														\
		static APhysXU32 GetVersion()							\
			{ return (VerNumber); }								\
		virtual bool Save(NxStream* pNxStream);					\
		virtual bool Load(NxStream* pNxStream);					\
	protected:

// this macro will make Save()/Load() function protected...
#define DeclareVersion_ProtectedSaveLoad(VerNumber)				\
	public:														\
		static APhysXU32 GetVersion()							\
			{ return (VerNumber); }								\
	protected:													\
		virtual bool Save(NxStream* pNxStream);					\
		virtual bool Load(NxStream* pNxStream);					


#define Begin_Save(class_name, base_class_name)					\
	bool class_name::Save(NxStream* pNxStream)					\
	{															\
		if(!base_class_name::Save(pNxStream))					\
			return false;										\
		SaveVersion(pNxStream);									

#define End_Save()	 return true; }


#define Begin_Load(class_name, base_class_name)					\
	bool class_name::Load(NxStream* pNxStream)					\
	{															\
		if(!base_class_name::Load(pNxStream))					\
			return false;										\
		LoadVersion(pNxStream);									

#define End_Load()	 return true; }


// a simple implementation of Save() and Load() functions...
#define ImplementVersion(class_name, base_class_name)			\
	Begin_Save(class_name, base_class_name)						\
	End_Save()													\
	Begin_Load(class_name, base_class_name)						\
	End_Load()


//-----------------------------------------------------------------
// this macro will clone this object and return the cloned pointer.
// make sure that operator= execute the correct stuff...
#define CloneThis(class_name, returned_class_name)				\
	returned_class_name* pRet = new class_name;					\
	*(class_name*)pRet = *this;									\
	return pRet;


#define DeclareCloneAPhysXObjectDesc(class_name)				\
	virtual APhysXObjectDesc* Clone() const						\
	{															\
		CloneThis(class_name, APhysXObjectDesc)					\
	}		

// disable the C++ default copy constructor
#define DisableCopyConstructor(class_name)						\
	private:													\
		class_name(const class_name&);

//-----------------------------------------------------------------
// the high 24 bits of VerNum is the prefix, and the low 8 bits is
// the change version number...

#define GetVersionPrefixNumber(VerNum) ((VerNum) & 0xffffff00)
#define CheckVersionPrefix(VerNum) ( GetVersionPrefixNumber(VerNum) == GetVersionPrefixNumber(GetVersion()) )

//-----------------------------------------------------------------
// X is a pointer to NxStream, call this macro to Save/Load my own 
// version number but not the version of my base or child class...
// these two macros only can be used in Save/Load functions...

#define SaveVersion(X)  (X)##->storeDword(GetVersion());

#define LoadVersion(X)  m_StreamObjVer = (X)##->readDword();	\
						if(!CheckVersionPrefix(m_StreamObjVer)) \
							return false;
						
//-----------------------------------------------------------------

class APHYSX_DLL_EXPORT APhysXSerializerBase
{
	DeclareVersion(0xcc000001);

public:

//---------------------------------------------------------------------

	static void StoreString(NxStream* pNxStream, const char* str);
	static const char* ReadString(NxStream* pNxStream);

//---------------------------------------------------------------------

public:

	APhysXSerializerBase():m_StreamObjVer(0), m_szName(NULL){}
		
	void SetName(const char* szName)
	{
		m_szName = gGlobalStringManager->GetString(szName);
	}

	const char* GetName() const
	{
		return m_szName;
	}


protected:
	

	virtual void SaveName(NxStream* pNxStream)
	{
		StoreString(pNxStream, m_szName);
	}

	virtual void LoadName(NxStream* pNxStream)
	{
		m_szName = ReadString(pNxStream);
	}

protected:
	
	APhysXU32 m_StreamObjVer;
	const char* m_szName;
};



#endif