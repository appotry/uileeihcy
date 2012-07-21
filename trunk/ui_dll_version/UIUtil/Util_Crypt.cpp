#include "stdafx.h"
#include "h\util.h"
namespace UI { namespace Util
{

#ifdef UTIL_CRYPT

#include "Wincrypt.h"
#pragma comment(lib, "Crypt32.lib")
//
//	函数:	void MD5_Digest( BYTE* bOrign, int nOriLen, CBuffer *pDigestBuf )
//	
//	目的:	对给定的字节流做摘要
//
//	参数:
//		bOrign
//			[in]:	要做摘要的字节流
//		nOriLen
//			[in]:	szOrign字节流的长度
//		pDigestBuf
//			[out]:	返回摘要值，33位的长度(包括NULL)
//
//	返回：	成功返回TRUE，失败返回FALSE;
//
//	备注：	支持ASCII和UNICODE
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

		// 做摘要
		::CryptHashData( hHash, bOrign, nOriLen, 0 );

		DWORD dwHashLen = 0;
		DWORD dwDataLen = sizeof(DWORD);
		CBufferT<BYTE> buf;

		// 先获取摘要的长度，再分配内存获取摘要值
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
//	函数:	void MD5_String_Digest( TCHAR* str, CBuffer* pDigestBuf )
//	
//	目的:	对给定的字符串做摘要
//
//	参数:
//		str
//			[in]:	要做摘要的字符串，兼容ASCII和UNICODE，对于UNICODE，
//					是将其转换为ASCII。因为做摘要它只是针对BYTE而言
//		pDigestBuf
//			[out]:	返回摘要值，33位的长度(包括NULL)
//
//	返回：	成功返回TRUE，失败返回FALSE;
//
//	备注：	内部调用MD5_Digest
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
//	函数:	BOOL MD5_File_Digest( TCHAR* szPath, CBuffer* pDigestBuf )
//
//	目的:	对一个文件做摘要
//
//	参数:	
//		szPath
//			[in]:	要做摘要的文件路径
//		pDigestBuf
//			[out]:	返回摘要内容
//
//	返回：	成功返回TRUE，失败返回FALSE;
//
BOOL MD5_File_Digest( TCHAR* szPath, CBuffer* pDigestBuf )
{
	CBufferT<char>	fileBuf;
	if( FALSE == GetFileBuffer( szPath, fileBuf ) )
		return FALSE;

	return MD5_Digest( (BYTE*)(char*) fileBuf, fileBuf.GetBufSize(), pDigestBuf );
}

//
//	函数：	void Encrypt( BYTE* bOrign, int nOriLen, TCHAR* szPassword, CBuffer* pEncryptBuf )
//
//	目的：	对字节流使用指定的密码进行加密
//
//	参数：
//		bOrign
//			[in]:	要加密的字节流
//		nOriLen
//			[in]:	要加密的字节流的长度
//		szPassword
//			[in]:	加密所用的密码
//		pEncryptBuf
//			[out]:	返回加密后的内容
//
//	备注：	该函数尚未完成
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

		// 对密码做一次HASH，用于产生会话密钥
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
		DWORD	dwBufferLen = 1000 - 1000%8 + 8;	// 每次加密的字节数
		DWORD	dwEncrypt	= 0;					// 到目前为止加密了多少数据

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
//	函数：DWORD WINAPI ImportRootCertAutoClickProc( LPVOID lpParameter )
//
//	目的：在导入一个根证书的时候，由程序来自动点击确定按钮
//
DWORD WINAPI ImportRootCertAutoClickProc( LPVOID lpParameter )
{
	int  nRetryTimes = 20;
	int  nSleepTime	= 200;

	HWND hWnd = NULL;
	bool bFinish = false;
	TCHAR	titleR[64] = _T("根证书存储");			// 卸载根证书的对话框标题
	TCHAR	titleI_xp[64] = _T("安全警告");			// xp下，导入根证书的对话框标题
	TCHAR	titleI_win7[64] = _T("安全性警告");		// win7下，导入根证书的对话框标题

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
//	函数：DWORD WINAPI RemoveRootCertAutoClickProc( LPVOID lpParameter )
//
//	目的：在删除一个根证书的时候，由程序来自动点击确定按钮
//
DWORD WINAPI RemoveRootCertAutoClickProc( LPVOID lpParameter )
{
	int  nRetryTimes = 20;
	int  nSleepTime	= 200;

	HWND hWnd = NULL;
	bool bFinish = false;
	TCHAR	titleR[64] = _T("根证书存储");			// 卸载根证书的对话框标题

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
//	函数：	bool ImportPfxCert( TCHAR* szCertPath, TCHAR* szPassword, WCHAR* wcsImportToWhere, TCHAR* szErrInfo = NULL )
//
//	目的：	导入一张PFX证书到IE中的wcsImportToWhere指定的区域
//
//	参数
//		szCertPath
//			[in]:	p12证书所在路径
//		szPassword
//			[in]:	p12证书的密码
//		wcsImportToWhere
//			[in]:	L"MY", L"CA", L"ROOT"
//		szErrInfo
//			[out]:	如果错误，返回错误信息
//
//	返回：	成功返回true,失败返回false
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
				_stprintf( szErrInfo, _T("读取证书文件数据失败") );
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
				_stprintf( szErrInfo, _T("PFXImportCertStore FAILED, Error Code: 0x%X。请确认密码是否正确。 "), dwError );
			}
			break;
		}

		// 从临时区找到刚才那张证书，导入到MY里面
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

		// 自动选择是按钮
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
//	函数：	bool RemoveCert( TCHAR* szSubject, TCHAR* szIssuer, WCHAR* wcsRemoveFromWhere, TCHAR* szErrInfo = NULL )
//
//	目的：	从系统中wcsImportToWhere指定区域删除一张证书，要求符合主题和颁发者
//
//	参数
//		szSubject
//			[in]:	要删除的证书的主题
//		szIssuer
//			[in]:	要删除的证书的颁发者
//		wcsImportToWhere
//			[in]:	L"MY", L"CA", L"ROOT"
//		szErrInfo
//			[out]:	如果错误，返回错误信息
//
//	返回：	成功返回true,失败返回false
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
				// 获取证书主题和颁发者
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

					// 在这里开一个线程来由程序点击“是”那个按钮
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
//	函数：bool ImportDerCert( TCHAR* szCertPath, WCHAR* wcsImportToWhere, TCHAR* szErrInfo = NULL )
//
//	目的：导入一张Der证书到IE中的区域
//
//	参数：
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
		// 读取证书内容
		CBufferT<char>	certBuf;

		if( FALSE == GetFileBuffer( szCertPath, certBuf ) )
		{
			if( szErrInfo != NULL )
			{
				_stprintf( szErrInfo, _T("读取证书文件数据失败") );
			}
			break;
		}

		// 打开STORE
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

		// 编码判断,如果是DER编码，那么它以 0x30 0x82开头
		// 如果是base64,那么将它转换成DER编码
		// 如果是base64中前两个字符不是'M''I',那么去掉它的前后格式
		CBufferT<BYTE>	derCertBuf;

		if( certBuf[0] == (char)0x30 && certBuf[1] == (char)0x82 )
		{
			// 创建证书Context
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

			// 创建证书Context
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

		// 导入
		/*
			如果向ROOT中导入证书的话，有可能会弹出“安全警告”，而却会阻塞在这里。
			如果已经存在该证书再导入的话，还会弹出“根证书存储”
			//因此在这里开一个线程来由程序点击“是”那个按钮
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