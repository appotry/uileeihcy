#ifndef GLOBALFUNC_H_A42C0C89_F00A_4255_8B73_CA5BAA18302A
#define GLOBALFUNC_H_A42C0C89_F00A_4255_8B73_CA5BAA18302A

// 内部使用的全局方法
namespace UI
{
void  UI_Split(const String& str, TCHAR szSep, vector<String>& vRet);
void  UI_ExtendPath(String& strPath);
void  UI_AttachFont(IRenderFont** pOut, HFONT hFont, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType);
long  UISendMessage(Message* pMsgTo, UINT message,  WPARAM wParam = 0, LPARAM lParam = 0);

}



#endif