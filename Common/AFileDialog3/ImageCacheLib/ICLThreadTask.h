// ICLThreadTask.h: interface for the ICLThreadTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICLTHREADTASK_H__03D555FC_A888_4D07_83BD_927EA4CA2503__INCLUDED_)
#define AFX_ICLTHREADTASK_H__03D555FC_A888_4D07_83BD_927EA4CA2503__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ICLThreadTask  
{
public:
	ICLThreadTask();
	virtual ~ICLThreadTask();

public:
	virtual bool Do() = 0;
};

#endif // !defined(AFX_ICLTHREADTASK_H__03D555FC_A888_4D07_83BD_927EA4CA2503__INCLUDED_)
