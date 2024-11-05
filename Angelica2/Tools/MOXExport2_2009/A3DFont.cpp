#include "A3DFont.h"
#include "resource.h"

A3DFont::A3DFont()
{
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;
	m_nStringLength = 0;
}

A3DFont::~A3DFont()
{
}

bool A3DFont::Init(HINSTANCE hInstance, IDirect3DDevice8 * pDevice, char * szFontMap, int chWidth, int chHeight, int cellWidth, int cellHeight)
{
	HRESULT			hval;
	
	m_pDevice = pDevice;

	strcpy(m_szFontMap, szFontMap);

	if( !m_Texture.Init(pDevice) )
		return false;

	if( !m_Texture.LoadFromResource(hInstance, IDB_CONSOLEFONT) )
		return false;

	m_nMapWidth = 128;//Should get from the texture;
	m_nMapHeight = 256;//Should get from the texture;

	m_nCharWidth = chWidth;
	m_nCharHeight = chHeight;

	m_nCellWidth = cellWidth;
	m_nCellHeight = cellHeight;

	m_fCellWidth = m_nCellWidth * 1.0f / m_nMapWidth;
	m_fCellHeight = m_nCellHeight * 1.0f / m_nMapHeight;

	hval = m_pDevice->CreateVertexBuffer(MAX_STRING * 4 * sizeof(A3DTLVERTEX), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_A3DTLVERTEX, D3DPOOL_DEFAULT, &m_pVertexBuffer);
	if( D3D_OK != hval ) return false;

	hval = m_pDevice->CreateIndexBuffer(MAX_STRING * 6 * sizeof(WORD), NULL, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIndexBuffer);
	if( D3D_OK != hval ) return false;

	WORD * pVBIndices;
	hval = m_pIndexBuffer->Lock(0, 0, (LPBYTE *)&pVBIndices, NULL);
	if( D3D_OK != hval ) return false;

	for(int n=0; n<MAX_STRING; n++)
	{
		pVBIndices[n * 6] = n * 4;
		pVBIndices[n * 6 + 1] = n * 4 + 1;
		pVBIndices[n * 6 + 2] = n * 4 + 2;
		pVBIndices[n * 6 + 3] = n * 4 + 2;
		pVBIndices[n * 6 + 4] = n * 4 + 1;
		pVBIndices[n * 6 + 5] = n * 4 + 3;
	}

	hval = m_pIndexBuffer->Unlock();
	if( D3D_OK != hval ) return false;

	m_nStringLength = 0;
	return true;
}

bool A3DFont::TextOut(int x, int y, char * szText, A3DCOLOR color)
{
	HRESULT			hval;
	A3DTLVERTEX	*	pVBVerts;

	m_nStringLength = strlen(szText);

	if( m_nStringLength > MAX_STRING )
		m_nStringLength = MAX_STRING;

	if( m_nStringLength == 0 )
		return true;

	hval = m_pVertexBuffer->Lock(0, m_nStringLength * 4 * sizeof(A3DTLVERTEX), (LPBYTE *)&pVBVerts, NULL);
	if( D3D_OK != hval ) return false;

	for(int n=0; n<m_nStringLength; n++)
	{
		if( szText[n] == ' ' )
			szText[n] = '~' + 1;

		char ch = szText[n] - '!';

		//Left up corner [0];
		pVBVerts[n * 4].x = x * 1.0f + n * m_nCharWidth;
		pVBVerts[n * 4].y = y * 1.0f;
		pVBVerts[n * 4].z = 0.0f;
		pVBVerts[n * 4].rhw = 1.0f;
		pVBVerts[n * 4].tu = (ch % 10) * m_fCellWidth;
		pVBVerts[n * 4].tv = (ch / 10) * m_fCellHeight;
		pVBVerts[n * 4].color = color;

		//Right up corner [1];
		pVBVerts[n * 4 + 1] = pVBVerts[n * 4];
		pVBVerts[n * 4 + 1].x += m_nCharWidth;
		pVBVerts[n * 4 + 1].tu += m_fCellWidth;
		
		//Left bottom corner [2];
		pVBVerts[n * 4 + 2] = pVBVerts[n * 4];
		pVBVerts[n * 4 + 2].y += m_nCharHeight;
		pVBVerts[n * 4 + 2].tv += m_fCellHeight;

		//Right bottom corner [3];
		pVBVerts[n * 4 + 3] = pVBVerts[n * 4];
		pVBVerts[n * 4 + 3].x += m_nCharWidth;
		pVBVerts[n * 4 + 3].tu += m_fCellWidth;
		pVBVerts[n * 4 + 3].y += m_nCharHeight;
		pVBVerts[n * 4 + 3].tv += m_fCellHeight;
	}

	hval = m_pVertexBuffer->Unlock();
	if( D3D_OK != hval ) return false;
	return true;
}

bool A3DFont::Release()
{
	if( m_pVertexBuffer )
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}

	if( m_pIndexBuffer )
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = NULL;
	}

	m_Texture.Release();
	return true;
}

bool A3DFont::Render()
{
	HRESULT	hval;

	if( m_nStringLength == 0 )
		return true;

	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);

	m_pDevice->SetStreamSource(0, m_pVertexBuffer, sizeof(A3DTLVERTEX));
	m_pDevice->SetVertexShader(D3DFVF_A3DTLVERTEX);
	m_pDevice->SetIndices(m_pIndexBuffer, 0);

	m_Texture.Appear(0);

	hval = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_nStringLength * 4, 0, m_nStringLength * 2);
	if( D3D_OK != hval ) return false;
	
	m_Texture.Disappear(0);
	m_pDevice->SetStreamSource(0, NULL, 0);
	m_pDevice->SetIndices(NULL, 0);

	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	return true;
}
