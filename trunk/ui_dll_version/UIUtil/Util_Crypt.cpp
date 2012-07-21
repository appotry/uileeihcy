#include "stdafx.h"
#include "h\util.h"
namespace UI { namespace Util
{

#ifdef UTIL_CRYPT

#include "Wincrypt.h"
#pragma comment(lib, "Crypt32.lib")
//
//	����:	void MD5_Digest( BYTE* bOrign, int nOriLen, CBuffer *pDigestBuf )
//	
//	Ŀ��:	�Ը������ֽ�����ժҪ
//
//	����:
//		bOrign
//			[in]:	Ҫ��ժҪ���ֽ���
//		nOriLen
//			[in]:	szOrign�ֽ����ĳ���
//		pDigestBuf
//			[out]:	����ժҪֵ��33λ�ĳ���(����NULL)
//
//	���أ�	�ɹ�����TRUE��ʧ�ܷ���FALSE;
//
//	��ע��	֧��ASCII��UNICODE
//		
BOOL MD5_Digest( BYTE* bOrign, int nOriLen, CBuffer *pDigestBuf )
{
	HCRYPTPROV		hCryptProv	= NULL;
	HCRYPTHASH		hHash		= NULL;
	BOOL			bRet		= FALSE;
	do
	{
		bRet = ::CryptAcquireContext( &hCryptProv, 0, NULL, PROV_RSA_FULL, 0 );
		if( FALSE == bRet )
			break;

		bRet = ::CryptCreateHash( hCryptProv, CALG_MD5, 0, 0, &hHash );
		if( FALSE == bRet )
			break;

		// ��ժҪ
		::CryptHashData( hHash, bOrign, nOriLen, 0 );

		DWORD dwHashLen = 0;
		DWORD dwDataLen = sizeof(DWORD);
		CBufferT<BYTE> buf;

		// �Ȼ�ȡժҪ�ĳ��ȣ��ٷ����ڴ��ȡժҪֵ
		if( FALSE == ::CryptGetHashParam( hHash, HP_HASHSIZE, (BYTE*)&dwHashLen, &dwDataLen, 0 ) )
			break;

		buf.malloc( dwHashLen );
		dwDataLen = dwHashLen;

		if( FALSE == ::CryptGetHashParam( hHash, HP_HASHVAL, buf, &dwDataLen, 0 ) )
			break;
	
		B2HS( (BYTE*)buf, buf.GetBufSize(), *pDigestBuf );
		bRet = TRUE;
	}
	while(false);

	if( hHash )
		::CryptDestroyHash( hHash );
	if( hCryptProv )
		::CryptReleaseContext( hCryptProv, 0 );

	return bRet;
}

//
//	����:	void MD5_String_Digest( TCHAR* str, CBuffer* pDigestBuf )
//	
//	Ŀ��:	�Ը������ַ�����ժҪ
//
//	����:
//		str
//			[in]:	Ҫ��ժҪ���ַ���������ASCII��UNICODE������UNICODE��
//					�ǽ���ת��ΪASCII����Ϊ��ժҪ��ֻ�����BYTE����
//		pDigestBuf
//			[out]:	����ժҪֵ��33λ�ĳ���(����NULL)
//
//	���أ�	�ɹ�����TRUE��ʧ�ܷ���FALSE;
//
//	��ע��	�ڲ�����MD5_Digest
//		
BOOL MD5_String_Digest( TCHAR* str, CBuffer* pDigestBuf )
{
#ifdef _UNICODE
	CBufferT<char>	buf;

	U2A( str, buf );
	return MD5_Digest( (BYTE*)(char*)buf, wcslen(str),  pDigestBuf );

#else
	return MD5_Digest( (BYTE*)str, strlen( str ),  pDigestBuf );
#endif
}



//
//	����:	BOOL MD5_File_Digest( TCHAR* szPath, CBuffer* pDigestBuf )
//
//	Ŀ��:	��һ���ļ���ժҪ
//
//	����:	
//		szPath
//			[in]:	Ҫ��ժҪ���ļ�·��
//		pDigestBuf
//			[out]:	����ժҪ����
//
//	���أ�	�ɹ�����TRUE��ʧ�ܷ���FALSE;
//
BOOL MD5_File_Digest( TCHAR* szPath, CBuffer* pDigestBuf )
{
	CBufferT<char>	fileBuf;
	if( FALSE == GetFileBuffer( szPath, fileBuf ) )
		return FALSE;

	return MD5_Digest( (BYTE*)(char*) fileBuf, fileBuf.GetBufSize(), pDigestBuf );
}

//
//	������	void Encrypt( BYTE* bOrign, int nOriLen, TCHAR* szPassword, CBuffer* pEncryptBuf )
//
//	Ŀ�ģ�	���ֽ���ʹ��ָ����������м���
//
//	������
//		bOrign
//			[in]:	Ҫ���ܵ��ֽ���
//		nOriLen
//			[in]:	Ҫ���ܵ��ֽ����ĳ���
//		szPassword
//			[in]:	�������õ�����
//		pEncryptBuf
//			[out]:	���ؼ��ܺ������
//
//	��ע��	�ú�����δ���
//
void Encrypt( BYTE* bOrign, int nOriLen, TCHAR* szPassword, CBuffer* pEncryptBuf )
{
	HCRYPTPROV		hCryptProv	= NULL;
	HCRYPTHASH		hHash		= NULL;
	HCRYPTKEY		hKey		= NULL;
	BOOL			bRet		= FALSE;

	do
	{
		bRet = ::CryptAcquireContext( &hCryptProv, 0, NULL, PROV_RSA_FULL, 0 );
		if( FALSE == bRet )
			break;

		bRet = ::CryptCreateHash( hCryptProv, CALG_MD5, 0, 0, &hHash );
		if( FALSE == bRet )
			break;

		// ��������һ��HASH�����ڲ����Ự��Կ
#ifdef _UNICODE
		CBufferT<char>	pswdBuf;
		U2A( szPassword, pswdBuf );
		::CryptHashData( hHash, (BYTE*)(char*)pswdBuf, pswdBuf.GetBufSize()-1, 0 );
#else
		::CryptHashData( hHash, (BYTE*)szPassword, strlen(szPassword), 0 );
#endif

		bRet = ::CryptDeriveKey( hCryptProv, CALG_RC2, hHash, 0x00800000|CRYPT_EXPORTABLE, &hKey );
		if( FALSE == bRet )
			break;
	
		::CryptDestroyHash( hHash );
		hHash = NULL;

		const int ENCRYPT_BLOCK_SIZE = 8;
		DWORD	dwBufferLen = 1000 - 1000%8 + 8;	// ÿ�μ��ܵ��ֽ���
		DWORD	dwEncrypt	= 0;					// ��ĿǰΪֹ�����˶�������

		do
		{
			BOOL	bFinal = dwEncrypt + dwBufferLen >= (DWORD)nOriLen;
			DWORD	dwDataLen = bFinal? nOriLen-dwEncrypt : dwBufferLen;

			bRet = ::CryptEncrypt( hKey, 0, bFinal, 0, bOrign+dwEncrypt, &dwDataLen, dwDataLen );
			if( FALSE == bRet )
			{
				DWORD dwErr = ::GetLastError();
				break;
			}

			dwEncrypt += dwBufferLen;
			if( bFinal )
				break;
		}
		while( true );
		
		

	}
	while( false );

	if( hKey )
		::CryptDestroyKey( hKey );
	if( hHash )
		::CryptDestroyHash( hHash );
	if( hCryptProv )
		::CryptReleaseContext( hCryptProv, 0 );

}


//
//	������DWORD WINAPI ImportRootCertAutoClickProc( LPVOID lpParameter )
//
//	Ŀ�ģ��ڵ���һ����֤���ʱ���ɳ������Զ����ȷ����ť
//
DWORD WINAPI ImportRootCertAutoClickProc( LPVOID lpParameter )
{
	int  nRetryTimes = 20;
	int  nSleepTime	= 200;

	HWND hWnd = NULL;
	bool bFinish = false;
	TCHAR	titleR[64] = _T("��֤��洢");			// ж�ظ�֤��ĶԻ������
	TCHAR	titleI_xp[64] = _T("��ȫ����");			// xp�£������֤��ĶԻ������
	TCHAR	titleI_win7[64] = _T("��ȫ�Ծ���");		// win7�£������֤��ĶԻ������

	for( int i = 0; i < nRetryTimes; i++ )
	{
		if( bFinish )
			break; 

		Sleep(nSleepTime);
		hWnd = FindWindow( _T("#32770"), titleR );
		if( !hWnd)
		{
			HWND hWndXp = FindWindow( _T("#32770"), titleI_xp );
			HWND hWndWin7 = FindWindow( _T("#32770"), titleI_win7 );
			if( hWndXp != NULL || hWndWin7 != NULL )
			{
				bFinish = true;
				hWnd = ( hWndXp == NULL ? hWndWin7:hWndXp );
			}
			else
				continue;
		}
		SendMessage(hWnd, WM_COMMAND, MAKELONG(IDYES, BN_CLICKED), (long)GetDlgItem( hWnd, IDYES ));
	}
	return 0;
} 
//
//	������DWORD WINAPI RemoveRootCertAutoClickProc( LPVOID lpParameter )
//
//	Ŀ�ģ���ɾ��һ����֤���ʱ���ɳ������Զ����ȷ����ť
//
DWORD WINAPI RemoveRootCertAutoClickProc( LPVOID lpParameter )
{
	int  nRetryTimes = 20;
	int  nSleepTime	= 200;

	HWND hWnd = NULL;
	bool bFinish = false;
	TCHAR	titleR[64] = _T("��֤��洢");			// ж�ظ�֤��ĶԻ������

	for( int i = 0; i < nRetryTimes; i++ )
	{
		Sleep(nSleepTime);
		hWnd = FindWindow( _T("#32770"), titleR );
		if( hWnd)
		{
			SendMessage(hWnd, WM_COMMAND, MAKELONG(IDYES, BN_CLICKED), (long)GetDlgItem( hWnd, IDYES ));
			break;
		}
		else
			continue;
	}
	return 0;
} 



//
//	������	bool ImportPfxCert( TCHAR* szCertPath, TCHAR* szPassword, WCHAR* wcsImportToWhere, TCHAR* szErrInfo = NULL )
//
//	Ŀ�ģ�	����һ��PFX֤�鵽IE�е�wcsImportToWhereָ��������
//
//	����
//		szCertPath
//			[in]:	p12֤������·��
//		szPassword
//			[in]:	p12֤�������
//		wcsImportToWhere
//			[in]:	L"MY", L"CA", L"ROOT"
//		szErrInfo
//			[out]:	������󣬷��ش�����Ϣ
//
//	���أ�	�ɹ�����true,ʧ�ܷ���false
//
bool ImportPfxCert( TCHAR* szCertPath, TCHAR* szPassword, WCHAR* wcsImportToWhere, TCHAR* szErrInfo )
{
	bool			nRet = false;
	HCERTSTORE		hCertStore = NULL;
	HCERTSTORE		hCertStoreToWhere = NULL; 
	PCCERT_CONTEXT	pCertContex = NULL;
	HCRYPTPROV		hCryptProv	= NULL;
	do
	{
		CBufferT<char>		bufCert;
		if( FALSE == GetFileBuffer( szCertPath, bufCert ) )
		{
			if( szErrInfo != NULL )
			{
				_stprintf( szErrInfo, _T("��ȡ֤���ļ�����ʧ��") );
			}
			break;
		}


		CRYPT_DATA_BLOB blob;
		blob.cbData = bufCert.GetBufSize();
		blob.pbData = (unsigned char *)(char*)bufCert;

#ifdef _UNICODE 
		hCertStore = PFXImportCertStore( &blob, szPassword, CRYPT_EXPORTABLE );
#else
		int nPswdLen = ::MultiByteToWideChar( CP_ACP, 0, szPassword, strlen( szPassword )+1, NULL, 0 );
		CBufferT<wchar_t>	wcsPassword;
		wcsPassword.malloc( nPswdLen );
		::MultiByteToWideChar( CP_ACP, 0, szPassword, strlen( szPassword )+1, wcsPassword, nPswdLen );

		hCertStore = PFXImportCertStore( &blob, wcsPassword, CRYPT_EXPORTABLE );
#endif
		if( NULL == hCertStore )
		{
			if( NULL != szErrInfo )
			{
				DWORD dwError = ::GetLastError();
				_stprintf( szErrInfo, _T("PFXImportCertStore FAILED, Error Code: 0x%X����ȷ�������Ƿ���ȷ�� "), dwError );
			}
			break;
		}

		// ����ʱ���ҵ��ղ�����֤�飬���뵽MY����
		BOOL			bCertFound	= FALSE;
		DWORD			dwKeySpec	= 0;
		BOOL			boCallerFreeProv = FALSE;
		do
		{                             
			pCertContex = CertEnumCertificatesInStore(hCertStore,pCertContex);
			if (!pCertContex)
				break;
			if (CryptAcquireCertificatePrivateKey(pCertContex, CRYPT_ACQUIRE_COMPARE_KEY_FLAG, 
				NULL,&hCryptProv,&dwKeySpec,&boCallerFreeProv) && hCryptProv)
			{
				bCertFound = TRUE;
				break;   		
			}
		}while(1);

		if( FALSE == bCertFound )
			break;
		
		hCertStoreToWhere = ::CertOpenStore(CERT_STORE_PROV_SYSTEM_W, 0, NULL, 
			CERT_SYSTEM_STORE_CURRENT_USER | CERT_STORE_OPEN_EXISTING_FLAG, wcsImportToWhere );
		if( NULL == hCertStoreToWhere )
		{
			if( NULL != szErrInfo )
			{
				_tcscpy( szErrInfo, _T("CertOpenStoren Failed") );
			}
			break;
		}

		// �Զ�ѡ���ǰ�ť
		if( 0 == wcscmp( L"ROOT", wcsImportToWhere ) )
		{
			CreateThread( NULL, 0, ImportRootCertAutoClickProc, 0, 0, 0 );
		}

		BOOL bRet = ::CertAddCertificateContextToStore( hCertStoreToWhere, pCertContex, CERT_STORE_ADD_REPLACE_EXISTING, NULL );
		if( !bRet )
		{
			if( NULL != szErrInfo )
			{
				DWORD dwError = ::GetLastError();
				_stprintf( szErrInfo, _T("CertAddCertificateContextToStore FAILED, Error Code: 0x%X "), dwError );
			}
			break;
		}

		nRet = true;
	}
	while(0);

	if(	pCertContex )
		CertFreeCertificateContext( pCertContex );

	if( hCryptProv )
		CryptReleaseContext( hCryptProv, 0 );

	if( hCertStoreToWhere )
		CertCloseStore( hCertStoreToWhere, 0 );

	if( hCertStore )
		CertCloseStore(hCertStore, 0);

	return nRet;
}


//
//	������	bool RemoveCert( TCHAR* szSubject, TCHAR* szIssuer, WCHAR* wcsRemoveFromWhere, TCHAR* szErrInfo = NULL )
//
//	Ŀ�ģ�	��ϵͳ��wcsImportToWhereָ������ɾ��һ��֤�飬Ҫ���������Ͱ䷢��
//
//	����
//		szSubject
//			[in]:	Ҫɾ����֤�������
//		szIssuer
//			[in]:	Ҫɾ����֤��İ䷢��
//		wcsImportToWhere
//			[in]:	L"MY", L"CA", L"ROOT"
//		szErrInfo
//			[out]:	������󣬷��ش�����Ϣ
//
//	���أ�	�ɹ�����true,ʧ�ܷ���false
//
bool RemoveCert( TCHAR* szSubject, TCHAR* szIssuer, WCHAR* wcsRemoveFromWhere, TCHAR* szErrInfo )
{
	bool			bRet = false;
	HCERTSTORE		hCertStore = NULL;
	PCCERT_CONTEXT	pCCertContext = NULL;
	do
	{
		hCertStore = ::CertOpenStore(CERT_STORE_PROV_SYSTEM_W, 0, NULL, 
			CERT_SYSTEM_STORE_CURRENT_USER | CERT_STORE_OPEN_EXISTING_FLAG, wcsRemoveFromWhere );
		if( NULL == hCertStore )
		{
			if( NULL != szErrInfo )
			{
				_tcscpy( szErrInfo, _T("CertOpenStoren Failed") );
			}
			break;
		}


		do
		{
			pCCertContext = ::CertEnumCertificatesInStore( hCertStore, pCCertContext );
			if( NULL == pCCertContext )
			{
				if( NULL != szErrInfo )
				{
					DWORD dwErrCode = ::GetLastError();
					_stprintf( szErrInfo, _T("CertFindCertificateInStore FAILED, Error Code: 0x%d "), dwErrCode );
				}
				break;
			}
			else
			{
				// ��ȡ֤������Ͱ䷢��
				CBuffer	subjectBuf;
				CBuffer	issuerBuf;

				DWORD dwSize = ::CertGetNameString( pCCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0/*CERT_NAME_ISSUER_FLAG*/, NULL, NULL, 0 );
				subjectBuf.malloc( dwSize );
				dwSize = ::CertGetNameString( pCCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0/*CERT_NAME_ISSUER_FLAG*/, NULL, subjectBuf, dwSize );
				
				dwSize = ::CertGetNameString( pCCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, CERT_NAME_ISSUER_FLAG, NULL, NULL, 0 );
				issuerBuf.malloc( dwSize );
				dwSize = ::CertGetNameString( pCCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, CERT_NAME_ISSUER_FLAG, NULL, issuerBuf, dwSize );

				if( 0 == _tcscmp( (TCHAR*)subjectBuf, szSubject ) &&
					0 == _tcscmp( (TCHAR*)issuerBuf, szIssuer ))
				{

					// �����￪һ���߳����ɳ��������ǡ��Ǹ���ť
					if( 0 == wcscmp( L"ROOT", wcsRemoveFromWhere ) )
					{
						CreateThread( NULL, 0, RemoveRootCertAutoClickProc, 0, 0, 0 );
					}
					if( FALSE == ::CertDeleteCertificateFromStore( pCCertContext ) )
					{
						if( NULL != szErrInfo )
						{
							DWORD dwErrCode = ::GetLastError();
							_stprintf( szErrInfo, _T("CertDeleteCertificateFromStore FAILED, Error Code: 0x% "), dwErrCode );
						}
					}
					break;
				}
			}
		}
		while( true );

		bRet =  true;
	}
	while(false);

	if(	pCCertContext )
		CertFreeCertificateContext( pCCertContext );

	if( hCertStore )
		::CertCloseStore( hCertStore, 0 );
	
	return bRet;
}

//
//	������bool ImportDerCert( TCHAR* szCertPath, WCHAR* wcsImportToWhere, TCHAR* szErrInfo = NULL )
//
//	Ŀ�ģ�����һ��Der֤�鵽IE�е�����
//
//	������
//		wcsImportToWhere
//			[in]	L"MY", L"CA", 
//
bool ImportDerCert( TCHAR* szCertPath, WCHAR* wcsImportToWhere, TCHAR* szErrInfo )
{

	bool			bRet = false;
	HCERTSTORE		hCertStore = NULL;
	PCCERT_CONTEXT	lpCertContext = NULL;

	do
	{
		// ��ȡ֤������
		CBufferT<char>	certBuf;

		if( FALSE == GetFileBuffer( szCertPath, certBuf ) )
		{
			if( szErrInfo != NULL )
			{
				_stprintf( szErrInfo, _T("��ȡ֤���ļ�����ʧ��") );
			}
			break;
		}

		// ��STORE
		hCertStore = ::CertOpenStore(CERT_STORE_PROV_SYSTEM_W, 0, NULL, 
			CERT_SYSTEM_STORE_CURRENT_USER | CERT_STORE_OPEN_EXISTING_FLAG, wcsImportToWhere );
		if( NULL == hCertStore )
		{
			if( NULL != szErrInfo )
			{
				_tcscpy( szErrInfo, _T("CertOpenStoren Failed") );
			}
			break;
		}

		// �����ж�,�����DER���룬��ô���� 0x30 0x82��ͷ
		// �����base64,��ô����ת����DER����
		// �����base64��ǰ�����ַ�����'M''I',��ôȥ������ǰ���ʽ
		CBufferT<BYTE>	derCertBuf;

		if( certBuf[0] == (char)0x30 && certBuf[1] == (char)0x82 )
		{
			// ����֤��Context
			lpCertContext = CertCreateCertificateContext(X509_ASN_ENCODING|PKCS_7_ASN_ENCODING,
				(const unsigned char*)(char*)certBuf, certBuf.GetBufSize() );
		}
		else
		{
			LONG	len = 4096;
			derCertBuf.malloc( 4096 );

			if( certBuf[0] == 'M' && certBuf[1] == 'I' )
			{
				base64_decode( (char*)certBuf, certBuf.GetBufSize(), (BYTE*)derCertBuf, &len );
			}
			else
			{
				base64_decode( (char*)certBuf+27, certBuf.GetBufSize()-25, (BYTE*)derCertBuf, &len );
			}

			// ����֤��Context
			lpCertContext = CertCreateCertificateContext(X509_ASN_ENCODING|PKCS_7_ASN_ENCODING,
				(BYTE*)derCertBuf, derCertBuf.GetBufSize() );
		}
		
		if( NULL == lpCertContext )
		{
			if( NULL != szErrInfo )
			{
				DWORD dwErrCode = ::GetLastError();
				_stprintf( szErrInfo, _T("CertCreateCertificateContext FAILED, Error Code: 0x% "), dwErrCode );
			}
			break;
		}

		// ����
		/*
			�����ROOT�е���֤��Ļ����п��ܻᵯ������ȫ���桱����ȴ�����������
			����Ѿ����ڸ�֤���ٵ���Ļ������ᵯ������֤��洢��
			//��������￪һ���߳����ɳ��������ǡ��Ǹ���ť
		*/
		if( 0 == wcscmp( L"ROOT", wcsImportToWhere ) )
		{
			CreateThread( NULL, 0, ImportRootCertAutoClickProc, 0, 0, 0 );
		}

		if( FALSE == CertAddCertificateContextToStore( hCertStore, lpCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL ) )
		{
			if( NULL != szErrInfo )
			{
				DWORD dwErrCode = ::GetLastError();
				_stprintf( szErrInfo, _T("CertAddCertificateContextToStore FAILED, Error Code: 0x% "), dwErrCode );
			}
			break;
		}
		
	
		bRet = true;
		break;
	}
	while(false);

	if( NULL != hCertStore )
		::CertCloseStore( hCertStore, 0 );

	if( NULL != lpCertContext )
		CertFreeCertificateContext( lpCertContext );

	return bRet;
	
}

#endif

}}