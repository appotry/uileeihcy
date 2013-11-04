#pragma once

// ����MFC COleDataSource,  oledobj2.cpp

namespace UI
{
    interface IDataObjectDelayCommit
    {

    };
    struct OleDataObjectItem
    {
        FORMATETC  formatetc;
        STGMEDIUM  stgmedium;
    };


	// ʵ��IDataObject��������WindowlessRichEdit::GetClipboardData
	class OleDataObject : public IDataObject
	{
	public:
		OleDataObject();
		~OleDataObject();

		// IUnknown  Interface
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void **ppvObject);
		virtual ULONG   STDMETHODCALLTYPE AddRef(void);
		virtual ULONG   STDMETHODCALLTYPE Release(void);

#pragma region // IDataObject
		virtual  HRESULT STDMETHODCALLTYPE GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);
		virtual  HRESULT STDMETHODCALLTYPE GetDataHere(FORMATETC *pformatetc, STGMEDIUM *pmedium);
		virtual  HRESULT STDMETHODCALLTYPE QueryGetData(FORMATETC *pformatetc);
		virtual  HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(FORMATETC *pformatectIn, FORMATETC *pformatetcOut);
		virtual  HRESULT STDMETHODCALLTYPE SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease);
		virtual  HRESULT STDMETHODCALLTYPE EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc);
		virtual  HRESULT STDMETHODCALLTYPE DAdvise(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection);
		virtual  HRESULT STDMETHODCALLTYPE DUnadvise(DWORD dwConnection);
		virtual  HRESULT STDMETHODCALLTYPE EnumDAdvise(IEnumSTATDATA **ppenumAdvise);
#pragma endregion

	protected:
		list<OleDataObjectItem*>   m_list;
		long     m_dwRef;
//		/*IMarshal*/IUnknown* m_pMarshal;

		friend class IEnumFORMATETCImpl;
	};

	// ����IDataObject::EnumFormatEtc�У�ʵ�ָ��ָ�ʽ�ı���
	// ͬʱ�Լ�ʵ��һ��IEnumFORMATETC����ֱ������OleDataObject
	// �е����ݣ����⿽��
	class IEnumFORMATETCImpl : public IEnumFORMATETC
	{
	public:
		IEnumFORMATETCImpl(OleDataObject* pDataObj);
		~IEnumFORMATETCImpl();

		// IUnknown  Interface
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void **ppvObject);
		virtual ULONG   STDMETHODCALLTYPE AddRef(void);
		virtual ULONG   STDMETHODCALLTYPE Release(void);

#pragma region
		virtual HRESULT STDMETHODCALLTYPE Next(ULONG celt, FORMATETC *rgelt, ULONG *pceltFetched);
		virtual HRESULT STDMETHODCALLTYPE Skip(ULONG celt);
		virtual HRESULT STDMETHODCALLTYPE Reset(void);
		virtual HRESULT STDMETHODCALLTYPE Clone(IEnumFORMATETC **ppenum);
#pragma endregion

	private:
		OleDataObject*  m_pDataObject;
		long     m_dwRef;
		int      m_nCurIndex;
	}; 

	
}