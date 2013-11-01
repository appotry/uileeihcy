#pragma once

namespace UI
{
    interface IImageResItem;
    interface IGifResItem;
    interface ICursorResItem;

	// 属性项编辑接口
	enum UIEDITOR_ATTR_TYPE
	{
		ATTR_TYPE_NULL = 0,
		UIEDITOR_ATTR_TYPE_GROUP,
		UIEDITOR_ATTR_TYPE_TEXT,
	};

	interface IPropertyCtrl;
	interface IUIEditorAttrBase
	{
		virtual void  Release() = 0;

		// 获取IAttribute的类型
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
		virtual const TCHAR* GetValue() = 0;
		virtual void  SetValue(const TCHAR*) = 0;
		virtual void  SetDefaultValue(const TCHAR* szText) = 0;
		virtual void  SetKey(const TCHAR*) = 0;
	};

	
	interface IUIEditorTextAttribute : public IUIEditorAttribute
	{
		virtual UIEDITOR_ATTR_TYPE  GetAttrType() { return UIEDITOR_ATTR_TYPE_TEXT; }
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

	
	//  外部的编辑器指针，用于消息通知和数据获取

	interface IUIEditor
	{
		virtual ~IUIEditor() = 0{};

		// 从配置文件中读取完一个对象的属性配置
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
        // 创建一个字符串属性。pParent为父结点，szName为该项在列表中显示的名称, szDesc为该项的描述信息,
        //  szKeyPrefix1 + szKeyPrefix2 + szName是该项在xml中的属性字段
	    virtual IUIEditorTextAttribute*  CreateTextAttribute(
            IUIEditorGroupAttribute* pParent, 
            const TCHAR* szName, 
            const TCHAR* szKeyPrefix1,
            const TCHAR* szKeyPrefix2 = NULL,
            const TCHAR* szDesc = NULL) 
            = 0;

// 		// 读取一个对象的属性列表时，获取到一个属性组项，返回一个组的标识，用于调用OnGetAttribute_AddProperty
// 		virtual long  OnGetAttribute_AddGroupProperty(__in IObject* pObj, __in const TCHAR* szGroupName) = 0;
// 		// 读取一个对象的属性列表时，获取到一个属性项
// 		virtual void  OnGetAttribute_AddProperty(__in IObject* pObj, __in long nGroupId, __in const TCHAR* szName) = 0;

        // 获取对象列表，添加到工具箱中。调用LoadUIObjectListToUIEdirot时触发
        virtual void  OnToolBox_AddObject(__in const TCHAR* szObjName, __in const TCHAR* szCategory, __in UINT nObType) = 0;

		//////////////////////////////////////////////////////////////////////////

		// 在保存时，返回对象的属性集给UI
//		virtual HRESULT GetObjectLastAttribute(__in Object* pObj, __out ATTRMAP** ppRetAttr) = 0;
	};

	
};

