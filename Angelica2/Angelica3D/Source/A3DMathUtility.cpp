#include "A3DMathUtility.h"
#include "A3DPI.h"
#include "A3DMacros.h"

A3DMathUtility * g_pA3DMathUtility = NULL;

A3DMathUtility::A3DMathUtility()
{
	m_pSinTable = NULL;
	m_pCosTable = NULL;
}

A3DMathUtility::~A3DMathUtility()
{
}

bool A3DMathUtility::Init(int nTableLength)
{
	m_nTableLength = nTableLength;
	m_vPrecision = 360.0f / m_nTableLength;

	m_pSinTable = (FLOAT *) malloc(m_nTableLength * sizeof(FLOAT));
	if( NULL == m_pSinTable )
	{
		g_A3DErrLog.Log("A3DMathUtility::Init Not enough memory!");
		return false;
	}
	m_pCosTable = (FLOAT *) malloc(m_nTableLength * sizeof(FLOAT));
	if( NULL == m_pCosTable )
	{
		g_A3DErrLog.Log("A3DMathUtility::Init Not enough memory!");
		return false;
	}

	for(int i=0; i<m_nTableLength; i++)
	{
		FLOAT vDegree = i * m_vPrecision;
		m_pSinTable[i] = (FLOAT) sin(DEG2RAD(vDegree));
		m_pCosTable[i] = (FLOAT) cos(DEG2RAD(vDegree));
	}
	return true;
}

bool A3DMathUtility::Release()
{
	if( m_pSinTable )
	{
		free(m_pSinTable);
		m_pSinTable = NULL;
	}
	if( m_pCosTable )
	{
		free(m_pCosTable);
		m_pCosTable = NULL;
	}

	return true;
}
