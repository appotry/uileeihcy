#ifndef SKINMANAGER_H_FE1D24AF_CC02_4cd2_8500_5026F671D4CC
#define SKINMANAGER_H_FE1D24AF_CC02_4cd2_8500_5026F671D4CC

#include "UISDK\Kernel\Src\Resource\uiresource.h"

namespace UI
{
class UIApplication;
class IUISkinDescParse;
enum  SKIN_PACKET_TYPE;

//////////////////////////////////////////////////////////////////////////
//
//	�������� -- ��Ӧ xxx.uiproj
//

class SkinManager
{
public:
	SkinManager(); 
	~SkinManager(); 
    void  Destroy();

public:
    void  SetSkinDirection(const TCHAR*  szDir);
    void  GetSkinDirection(TCHAR*  szOut);
    ISkinRes*  AddSkin(const TCHAR*  szName);

public:
    SkinRes*  OnFindSkinInSkinDir(SKIN_PACKET_TYPE, const TCHAR* szName, const TCHAR* szPath);
	
	UINT  GetSkinCount();
    ISkinRes*  GetActiveSkin();

	bool  Save(SkinRes* pSkinRes=NULL);
    ISkinManager*  GetISkinManager();
    IUIApplication*  GetUIApplication();

    HRESULT  ChangeSkin(ISkinRes*  pSkinRes, bool bSync=true); 
    HRESULT  ChangeSkinHLS( short h, short l, short s, int nFlag );
    HRESULT  SetActiveSkin(ISkinRes* pSkin);
    SkinRes*  GetSkinResByIndex(long lIndex);

public:
	void  SetUIApplication(IUIApplication* pUIApp);
	bool  LoadSkin(const TCHAR* szName);
	SkinRes*  AddSkinRes(const String&  strSkinName, const String&  strSkinXmlFullPath);
	bool  CreateSkinImageMgr(SkinRes*  pSkinRes, const String&  strXmlPath);
	bool  CreateSkinColorMgr(SkinRes*  pSkinRes, const String&  strXmlPath);
	bool  CreateSkinFontMgr (SkinRes*  pSkinRes, const String&  strXmlPath);
	bool  CreateSkinStyleMgr(SkinRes*  pSkinRes, const String&  strXmlPath);
	bool  CreateSkinLayoutMgr(SkinRes* pSkinRes, const String&  strXmlPath);

private:
	int  GetSkinResIndex(SkinRes* pSkinRes);
    SkinRes*  GetSkinResByName(const TCHAR* szName);

private:
    ISkinManager*  m_pISkinManager;

	// ���ݳ־ò�
    String  m_strSkinDir;
	bool  m_bDirty;

	// �־������Զ���
	SkinBuilderRes    m_SkinBuilderRes;

	// ����ʱ����
	vector<SkinRes*>  m_vSkinRes;           // ���е�Ƥ���б�
	SkinRes*          m_pCurActiveSkinRes;  // ��ǰ����ʹ�õ�Ƥ��

	IUIApplication*    m_pUIApplication;
};
}

#endif  // SKINMANAGER_H_FE1D24AF_CC02_4cd2_8500_5026F671D4CC