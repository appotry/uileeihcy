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
}