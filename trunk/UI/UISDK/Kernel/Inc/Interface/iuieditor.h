#pragma once

namespace UI
{

	//  外部的编辑器指针，用于消息通知和数据获取

	interface IUIEditor
	{
		virtual ~IUIEditor() = 0{};

		// 从配置文件中读取完一个对象的属性配置
		virtual void  OnObjectAttributeLoad(__in IObject*  pObj, __in IUIElement* pXmlElem) = 0;
        virtual void  OnObjectDeleteInd(__in IObject* pObj) = 0;

		// 读取一个对象的属性列表时，获取到一个属性组项，返回一个组的标识，用于调用OnGetAttribute_AddProperty
		virtual int   OnGetAttribute_AddGroupProperty(__in IObject* pObj, __in const TCHAR* szGroupName) = 0;
		// 读取一个对象的属性列表时，获取到一个属性项
		virtual void  OnGetAttribute_AddProperty(__in IObject* pObj, __in int nGroupId, __in const TCHAR* szName) = 0;

        // 获取对象列表，添加到工具箱中。调用LoadUIObjectListToUIEdirot时触发
        virtual void  OnToolBox_AddObject(__in const TCHAR* szObjName, __in const TCHAR* szCategory, __in UINT nObType) = 0;

		//////////////////////////////////////////////////////////////////////////

		// 在保存时，返回对象的属性集给UI
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
// 		// 该属性是否配置，没有配置的属性，不会写入xml中。该值与内容为空不是一个概念
// 		virtual bool IsConfiged() = 0;   
// 
// 		// 获取IAttribute的类型
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

