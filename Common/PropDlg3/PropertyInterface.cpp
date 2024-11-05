#include "PropertyInterface.h"

APropertyInterface* APropertyInterface::s_Interface;

APropertyInterface* APropertyInterface::GetPropertyInterface()
{
	return s_Interface;
}

void APropertyInterface::SetPropertyInterface(APropertyInterface* pInterface)
{
	s_Interface = pInterface;
}
