/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

	Define some macro stuff for singleton pattern
----------------------------------------------------------------------*/


#ifndef _APHYSXSINGLETON_H_
#define _APHYSXSINGLETON_H_

//--------------------------------------------------------------
// all in one macro in .h file
#define APHYSX_SINGLETON(class_name)				\
	public:											\
		static class_name* GetPtrInstance()			\
		{											\
			static class_name s_Instance;			\
			return &s_Instance;						\
		}											\
	private:


//--------------------------------------------------------------
// declaration macro in .h file
#define APHYSX_DECLARE_SINGLETON(class_name)		\
	public:											\
		static class_name* GetPtrInstance();		\
	private:


// definition macro in .cpp file
#define APHYSX_DEFINE_SINGLETON(class_name)			\
	class_name* class_name::GetPtrInstance()		\
	{												\
		static class_name s_Instance;				\
		return &s_Instance;							\
	}

//--------------------------------------------------------------



#endif
