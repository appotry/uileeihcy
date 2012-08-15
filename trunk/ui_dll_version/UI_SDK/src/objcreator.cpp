#include "stdafx.h"



HRESULT UICreateInstance(const String& strXmlName, Object** pOut)
{
	if (NULL == pOut)
		return E_INVALIDARG;
	*pOut = NULL;

	s_UICreateInstancePtr  funcptr;
	if (false == g_pUIApplication->GetUICreateInstanceFuncPtr(strXmlName, &funcptr))
	{
		return E_FAIL;
	}

	return funcptr((void**)pOut);

// 	BEGIN_XML2CLASS
// 		XML2CLASS( Panel )
// 		XML2CLASS( GroupBox )
// 		XML2CLASS( ScrollPanel )
// 		XML2CLASS( HwndHost )
// 		XML2CLASS( Button )
// 		XML2CLASS( CheckButton )
// 		XML2CLASS( RadioButton )
// 		XML2CLASS( HyperLink )
// 		XML2CLASS( Label )
// 		XML2CLASS( PictureCtrl )
// 		XML2CLASS( Edit )
// 		XML2CLASS( SliderCtrl )
// 		XML2CLASS( ListBox )
// //		XML2CLASS( TTPlayerPlaylistCtrl )
// 		XML2CLASS( HScrollBar )
// 		XML2CLASS( VScrollBar )
// 		XML2CLASS( Combobox )
// 	END_XML2CLASS;
// 
// 	HRESULT hr = S_OK;
// 	hr = pObj->InitialConstruct();
// 	if (SUCCEEDED(hr))
// 		hr = pObj->FinalConstruct();
// 
// 	if (FAILED(hr))
// 		delete pObj;
// 	else
// 		*pOut = static_cast<Object*>(pObj);
// 
// 	return hr;
}