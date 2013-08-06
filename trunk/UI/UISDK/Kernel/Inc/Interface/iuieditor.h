#pragma once

namespace UI
{

	//  �ⲿ�ı༭��ָ�룬������Ϣ֪ͨ�����ݻ�ȡ

	interface IUIEditor
	{
		virtual ~IUIEditor() = 0{};

		// �������ļ��ж�ȡ��һ���������������
		virtual void  OnObjectAttributeLoad(__in IObject*  pObj, __in IUIElement* pXmlElem) = 0;
        virtual void  OnObjectDeleteInd(__in IObject* pObj) = 0;

		// ��ȡһ������������б�ʱ����ȡ��һ�������������һ����ı�ʶ�����ڵ���OnGetAttribute_AddProperty
		virtual int   OnGetAttribute_AddGroupProperty(__in IObject* pObj, __in const TCHAR* szGroupName) = 0;
		// ��ȡһ������������б�ʱ����ȡ��һ��������
		virtual void  OnGetAttribute_AddProperty(__in IObject* pObj, __in int nGroupId, __in const TCHAR* szName) = 0;

        // ��ȡ�����б���ӵ��������С�����LoadUIObjectListToUIEdirotʱ����
        virtual void  OnToolBox_AddObject(__in const TCHAR* szObjName, __in const TCHAR* szCategory, __in UINT nObType) = 0;

		//////////////////////////////////////////////////////////////////////////

		// �ڱ���ʱ�����ض�������Լ���UI
//		virtual HRESULT GetObjectLastAttribute(__in Object* pObj, __out ATTRMAP** ppRetAttr) = 0;
	};


// 	enum ATTR_TYPE
// 	{
// 		ATTR_TYPE_NULL = 0,
// 		ATTR_TYPE_GROUP,
// 		ATTR_TYPE_EDIT,
// 	};
// 	interface IAttribute
// 	{
// 		// �������Ƿ����ã�û�����õ����ԣ�����д��xml�С���ֵ������Ϊ�ղ���һ������
// 		virtual bool IsConfiged() = 0;   
// 
// 		// ��ȡIAttribute������
// 		virtual ATTR_TYPE    GetAttrType() = 0;
// 
// 		virtual const TCHAR* GetAttrName() = 0;
// 		virtual const TCHAR* GetAttrDesc() = 0;
// 		virtual const TCHAR* GetAttrValue() = 0;
// 	};
// 
// 	interface  IAttributeGroup
// 	{
// 		virtual ATTR_TYPE  GetAttrType() { return ATTR_TYPE_GROUP; }
// 	};
// 
// 	interface IEditAttribute
// 	{
// 		virtual ATTR_TYPE  GetAttrType() { return ATTR_TYPE_EDIT; }
// 	};
// 
// 	interface IComboboxAttribute
// 	{
// 
// 	};
// 
// 	interface IRectAttribute
// 	{
// 
// 	};
// 
// 	interface I9RegionAttribute
// 	{
// 
// 	};
// 
// 	interface IImageAttribute
// 	{
// 
// 	};
// 
// 	interface IFontAttribute
// 	{
// 
// 	};
// 
// 	interface IColorAttribute
// 	{
// 
// 	};
// 
// 	interface IStyleAttribute
// 	{
// 
// 	};
};

