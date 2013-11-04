#pragma once

namespace UI
{
class UISDKAPI RenderContext
{
public:
	RenderContext(RECT* prcRenderRegion, bool bClip);

    void  DrawClient(IObject* pObject);
	void  Scroll(IObject* pObjScroll);
    bool  DrawChild(IObject* pChild);
	bool  DrawListItem(IListItemBase* pItem);
	
	void  Update(IRenderTarget* pRenderTarget);
	void  Reset(IRenderTarget* pRenderTarget);

public:
	bool      m_bUpdateClip;   // �Ƿ���Ҫά��CLIP����

	CRect     m_rcDrawRegion;  // ��������
	POINT     m_ptOffset;      // ��ǰҪ���ƵĶ���ƫ��������ʵ�����豸����
};

}

