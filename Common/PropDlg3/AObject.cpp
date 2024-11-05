#include "AObject.h"

bool APropertyObject::InitStateTable()
{
	int count;
	count = PropertiesCount();
	for(int i = 0; i < count; i++)
	{
		int state = Properties(i)->GetState();
		m_StateList.push_back(state&AProperty::DEFAULT_VALUE?1:0);
	}
	return true;
}

ADynPropertyObject::ADynPropertyObject()
{
	m_PropLst.reserve(16);
	m_VarLst.reserve(16);
}

bool ADynPropertyObject::is_valid() const
{
	return m_PropLst.size() == m_VarLst.size();
}

int ADynPropertyObject::PropertiesCount() 
{
	assert(is_valid());
	return (int)m_PropLst.size(); 
}

AProperty * ADynPropertyObject::Properties(int index) 
{ 
	assert(is_valid());
	return &m_PropLst[index]; 
}

AProperty * ADynPropertyObject::Properties(const char * name, int *index)
{
	assert(is_valid());
	assert(name);
	for (int i = 0; i < (int)m_PropLst.size(); i++)
	{
		if (strcmp(m_PropLst[i].GetName(), name) == 0)
		{
			if (index) *index = i;
			return &m_PropLst[i];
		}
	}
	return NULL;
}

AVariant& ADynPropertyObject::Impl_GetPropVal(int nIndex)
{
	assert(is_valid());
	assert(nIndex >= 0 && nIndex < (int)m_VarLst.size());
	return m_VarLst[nIndex];
}

void ADynPropertyObject::Impl_SetPropVal(int nIndex, const AVariant& var)
{
	assert(is_valid());
	m_VarLst[nIndex] = var;
}

void ADynPropertyObject::DynAddProperty(const AVariant& varValue
										, const char* szName
										, ASet* pSet /*= NULL*/
										, ARange* pRange /*= NULL*/
										, int way/* = WAY_DEFAULT*/
										, const char* szPath /*= NULL*/
										, bool isVisable /*= true*/
										, int iState /*= 0*/
										, const char* szDesc /*= 0*/)
{
	assert(is_valid());
	m_VarLst.push_back(varValue);
	m_PropLst.push_back(VarPropTemp("", 
		szName, 
		m_VarLst.size() - 1,
		pSet,
		pRange,
		way,
		szPath,
		isVisable,
		iState,
		szDesc));
	m_StateList.push_back(m_PropLst.back().GetState() & AProperty::DEFAULT_VALUE ? 1 : 0);
}

const char* ADynPropertyObject::GetPropDesc(int nIndex)
{
	assert(is_valid());
	return m_PropLst[nIndex].GetDesc();
}

void ADynPropertyObject::SetPropDesc(int nIndex, const char* szDesc)
{
	assert(is_valid());
	m_PropLst.at(nIndex).SetDesc(szDesc);
}

void ADynPropertyObject::SetPropName(int nIndex, const char* name)
{
	assert(is_valid());
	m_PropLst.at(nIndex).SetName(name);
}

void ADynPropertyObject::Clear()
{
	assert(is_valid());
	m_PropLst.clear();
	m_VarLst.clear();
}