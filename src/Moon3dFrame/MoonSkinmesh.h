#pragma once
#include "MoonUtility.h"

//-----------------------------------------------------------------------------
// Name: struct D3DXFRAME_DERIVED
// Desc: Structure derived from D3DXFRAME so we can add some app-specific
//       info that will be stored with each frame
//-----------------------------------------------------------------------------
struct D3DXFRAME_DERIVED: public D3DXFRAME
{
    D3DXMATRIXA16        CombinedTransformationMatrix;
};

//-----------------------------------------------------------------------------
// Name: struct D3DXMESHCONTAINER_DERIVED
// Desc: Structure derived from D3DXMESHCONTAINER so we can add some app-specific
//       info that will be stored with each mesh
//-----------------------------------------------------------------------------
struct D3DXMESHCONTAINER_DERIVED: public D3DXMESHCONTAINER
{
    LPDIRECT3DTEXTURE9*  ppTextures;       // array of textures, entries are NULL if no texture specified    
                                
    // SkinMesh info             
    LPD3DXMESH           pOrigMesh;
    LPD3DXATTRIBUTERANGE pAttributeTable;
    DWORD                NumAttributeGroups; 
    DWORD                NumInfl;
    LPD3DXBUFFER         pBoneCombinationBuf;
    D3DXMATRIX**         ppBoneMatrixPtrs;
    D3DXMATRIX*          pBoneOffsetMatrices;
    DWORD                NumPaletteEntries;
    bool                 UseSoftwareVP;
    DWORD                iAttributeSW;     // used to denote the split between SW and HW if necessary for non-indexed skinning
};

class MoonSkinmesh;

class AllocateHierarchy: public ID3DXAllocateHierarchy
{
public:
    STDMETHOD(CreateFrame)(THIS_ LPCTSTR Name, LPD3DXFRAME *ppNewFrame);
    STDMETHOD(CreateMeshContainer)(THIS_ LPCTSTR Name, LPD3DXMESHDATA pMeshData,
                            LPD3DXMATERIAL pMaterials, LPD3DXEFFECTINSTANCE pEffectInstances, DWORD NumMaterials, 
                            DWORD *pAdjacency, LPD3DXSKININFO pSkinInfo, 
                            LPD3DXMESHCONTAINER *ppNewMeshContainer);
    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);
    AllocateHierarchy(MoonSkinmesh* pSkinmesh) :_pSkinmesh(pSkinmesh) {}

public:
    MoonSkinmesh* _pSkinmesh;
};

class MoonSkinmesh
{
	LPDIRECT3DDEVICE9				_pDevice;
	D3DCAPS9*						_d3dCaps;
	LPD3DXFRAME						_pFrameRoot;
	LPD3DXANIMATIONCONTROLLER		_pAnimController;
	string							_strMeshname;
	string							_strAnimName;
	DWORD							_currentTrack;

public:
	MoonSkinmesh(LPDIRECT3DDEVICE9 pDevice, D3DCAPS9* d3dCaps, const char* Meshname);
	~MoonSkinmesh(void);

	HRESULT		InitDeviceObjects(void);
	HRESULT		DeleteDeviceObjects(void);
	/*HRESULT		RestoreDeviceObjects(void);
	HRESULT		InvalidateDeviceObjects(void);*/

	HRESULT		GenerateSkinmesh(D3DXMESHCONTAINER_DERIVED *pMeshContainer);
	char*		GetMeshname();
	HRESULT		SetupBoneMatrixPointers(LPD3DXFRAME pFrame);
	HRESULT		SetupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainerBase);
	void		DrawFrame(LPD3DXFRAME pFrame);
	void		DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase);
	void		UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix);
	bool		SetAnimByName(const char* strAnimName);

	void		FrameMove(float ftime, D3DXMATRIX matWorld);
	void		Draw(void);
};
