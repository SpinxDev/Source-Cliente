#ifndef _ICL_NOTIFIER_H_
#define _ICL_NOTIFIER_H_

class ICLNotifier
{
public:
	virtual void FolderProcComplete(const TCHAR * szFolder) = 0;
	virtual ~ICLNotifier() = 0 {}
};

#endif