#pragma once

namespace UI
{
    interface IImageResItem;
    interface IGifResItem;
    interface ICursorResItem;

	// ������༭�ӿ�
	enum UIEDITOR_ATTR_TYPE
	{
		ATTR_TYPE_NULL = 0,
		UIEDITOR_ATTR_TYPE_GROUP,
		UIEDITOR_ATTR_TYPE_TEXT,
		UIEDITOR_ATTR_TYPE_BOOL,
		UIEDITOR_ATTR_TYPE_COMBOBOX,
	};

	interface IPropertyCtrl;
	interface IUIEditorAttrBase
	{
		virtual void  Release() = 0;

		// ��ȡIAttribute������
		virtual UIEDITOR_ATTR_TYPE  GetAttrType() = 0;

		virtual const TCHAR* GetName() = 0;
		virtual const TCHAR* GetDesc() = 0;
		virtual void  SetName(const TCHAR*) = 0;
		virtual void  SetDesc(const TCHAR*) = 0;

		virtual void  Insert2PropertyCtrl(IPropertyCtrl* p, IListItemBase* pParent) = 0;

	};

	interface  IUIEditorGroupAttribute : public IUIEditorAttrBase
	{
		virtual UIEDITOR_ATTR_TYPE  GetAttrType() { return UIEDITOR_ATTR_TYPE_GROUP; }
		virtual void  AddAttribute(IUIEditorAttrBase* p) = 0;

	};

	interface IUIEditorAttribute : public IUIEditorAttrBase
	{
// 		virtual const TCHAR* GetValue() = 0;
// 		virtual void  SetValue(const TCHAR*) = 0;
		virtual void  SetDefaultValue(const TCHAR* szText) = 0;
		virtual void  SetKey(const TCHAR*) = 0;
	};

	
	interface IUIEditorTextAttribute : public IUIEditorAttribute
	{
		virtual UIEDITOR_ATTR_TYPE  GetAttrType() { return UIEDITOR_ATTR_TYPE_TEXT; }
	};

	interface IUIEditorBoolAttribute : public IUIEditorAttribute
	{
		virtual UIEDITOR_ATTR_TYPE  GetAttrType() { return UIEDITOR_ATTR_TYPE_BOOL; }
	};
    interface IUIEditorComboBoxAttribute : public IUIEditorAttribute
    {
        virtual UIEDITOR_ATTR_TYPE  GetAttrType() { return UIEDITOR_ATTR_TYPE_COMBOBOX; }
        virtual IUIEditorComboBoxAttribute*  AddOption(const TCHAR* szText, const TCHAR* szValue = NULL) = 0;
    };

// 	interface IComboboxAttribute : public IUIEditorAttr
// 	{
// 
// 	};
// 
// 	interface IRectAttribute : public IUIEditorAttr
// 	{
// 
// 	};
// 
// 	interface I9RegionAttribute : public IUIEditorAttr
// 	{
// 
// 	};
// 
// 	interface IImageAttribute : public IUIEditorAttr
// 	{
// 
// 	};
// 
// 	interface IFontAttribute : public IUIEditorAttr
// 	{
// 
// 	};
// 
// 	interface IColorAttribute : public IUIEditorAttr
// 	{
// 
// 	};
// 
// 	interface IStyleAttribute : public IUIEditorAttr
// 	{
// 
// 	};

	
	//  �ⲿ�ı༭��ָ�룬������Ϣ֪ͨ�����ݻ�ȡ

	interface IUIEditor
	{
		virtual ~IUIEditor() = 0{};

		// �������ļ��ж�ȡ��һ���������������
		virtual void  OnObjectAttributeLoad(__in IObject*  pObj, __in IUIElement* pXmlElem) = 0;
        virtual void  OnObjectDeleteInd(__in IObject* pObj) = 0;

        virtual void  OnImageItemLoad(__in IImageResItem*  pItem, __in IUIElement* pXmlElem) = 0;
        virtual void  OnGifItemLoad(__in IGifResItem*  pItem, __in IUIElement* pXmlElem) = 0;
        virtual void  OnCursorItemLoad(__in ICursorResItem*  pItem, __in IUIElement* pXmlElem) = 0;

		virtual IUIEditorAttrBase*  CreateAttribute(UIEDITOR_ATTR_TYPE, IUIEditorGroupAttribute* pParent) = 0;
		virtual IUIEditorGroupAttribute*  CreateGroupAttribute(
            IUIEditorGroupAttribute* pParent, 
            const TCHAR* szName, 
            const TCHAR* szDesc) 
            = 0;
        // ����һ���ַ������ԡ�pParentΪ����㣬szNameΪ�������б�����ʾ������, szDescΪ�����������Ϣ,
        //  szKeyPrefix1 + szKeyPrefix2 + szName�Ǹ�����xml�е������ֶ�
	    virtual IUIEditorTextAttribute*  CreateTextAttribute(
            IUIEditorGroupAttribute* pParent, 
            const TCHAR* szName, 
            const TCHAR* szKeyPrefix1,
            const TCHAR* szKeyPrefix2 = NULL,
            const TCHAR* szDesc = NULL) 
            = 0;

		virtual IUIEditorBoolAttribute*  CreateBoolAttribute(
			IUIEditorGroupAttribute* pParent, 
			const TCHAR* szName, 
            bool  bDefaultValue,
			const TCHAR* szKeyPrefix1,
			const TCHAR* szKeyPrefix2 = NULL,
			const TCHAR* szDesc = NULL) 
			= 0;

        virtual IUIEditorComboBoxAttribute*  CreateComboBoxAttribute(
            IUIEditorGroupAttribute* pParent, 
            const TCHAR* szName, 
            const TCHAR* szKeyPrefix1,
            const TCHAR* szKeyPrefix2 = NULL,
            const TCHAR* szDesc = NULL) = 0;

// 		// ��ȡһ������������б�ʱ����ȡ��һ�������������һ����ı�ʶ�����ڵ���OnGetAttribute_AddProperty
// 		virtual long  OnGetAttribute_AddGroupProperty(__in IObject* pObj, __in const TCHAR* szGroupName) = 0;
// 		// ��ȡһ������������б�ʱ����ȡ��һ��������
// 		virtual void  OnGetAttribute_AddProperty(__in IObject* pObj, __in long nGroupId, __in const TCHAR* szName) = 0;

        // ��ȡ�����б���ӵ��������С�����LoadUIObjectListToUIEdirotʱ����
        virtual void  OnToolBox_AddObject(__in const TCHAR* szObjName, __in const TCHAR* szCategory, __in UINT nObType) = 0;

		//////////////////////////////////////////////////////////////////////////

		// �ڱ���ʱ�����ض�������Լ���UI
//		virtual HRESULT GetObjectLastAttribute(__in Object* pObj, __out ATTRMAP** ppRetAttr) = 0;
	};

	
};

