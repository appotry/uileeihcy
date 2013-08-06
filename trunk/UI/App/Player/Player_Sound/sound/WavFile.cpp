#include "StdAfx.h"
#include "WavFile.h"


CWavFile::CWavFile()
{
	m_hMmio = NULL;
	m_pResourceBuffer =	NULL;
	m_dwSize = 0;
	memset(&m_ck, 0, sizeof(MMCKINFO));
	memset(&m_ckRiff, 0, sizeof(MMCKINFO));
}	
CWavFile::~CWavFile()
{
	this->Release();
}
void CWavFile::Release()
{
 	SAFE_ARRAY_DELETE(m_pResourceBuffer);
 	m_dwSize = 0;
	memset(&m_ck, 0, sizeof(MMCKINFO));
	memset(&m_ckRiff, 0, sizeof(MMCKINFO));
	this->Close();
}

HRESULT CWavFile::RenderFile(const TCHAR* szFile)
{
	if (m_pResourceBuffer == (CHAR*)1)
	{
		int a = 0;
	}
	if (NULL == szFile)
		return E_INVALIDARG;

	this->Release();

	HRESULT hr = E_FAIL;
	m_hMmio = mmioOpen((TCHAR*)szFile, NULL, MMIO_ALLOCBUF | MMIO_READ);

#if 0
	if (NULL == m_hMmio)  // ���Դ�RC��Դ�ж�ȡ...
	{
		HRSRC   hResInfo = NULL;
		HGLOBAL hResData = NULL;
		DWORD   dwSize   = 0;
		VOID*   pvRes    = NULL;

		// Loading it as a file failed, so try it as a resource
		if( NULL == ( hResInfo = FindResource( NULL, szFile, L"WAVE" ) ) )
		{
			if( NULL == ( hResInfo = FindResource( NULL, szFile, L"WAV" ) ) )
				return E_FAIL;
		}

		if( NULL == ( hResData = LoadResource( GetModuleHandle( NULL ), hResInfo ) ) )
			return E_FAIL;

		if( 0 == ( dwSize = SizeofResource( GetModuleHandle( NULL ), hResInfo ) ) )
			return E_FAIL;

		if( NULL == ( pvRes = LockResource( hResData ) ) )
			return E_FAIL;

		m_pResourceBuffer = new CHAR[ dwSize ];
		if( m_pResourceBuffer == NULL )
			return  E_OUTOFMEMORY;

		memcpy( m_pResourceBuffer, pvRes, dwSize );

		MMIOINFO mmioInfo;
		ZeroMemory( &mmioInfo, sizeof( mmioInfo ) );
		mmioInfo.fccIOProc = FOURCC_MEM;
		mmioInfo.cchBuffer = dwSize;
		mmioInfo.pchBuffer = ( CHAR* )m_pResourceBuffer;

		m_hMmio = mmioOpen( NULL, &mmioInfo, MMIO_ALLOCBUF | MMIO_READ );
	}
#endif

	if( FAILED( hr = ReadMMIO() ) )
	{
		// ReadMMIO will fail if its an not a wave file
		mmioClose( m_hMmio, 0 );
		m_hMmio = NULL;
		return hr;
	}

	if( FAILED( hr = ResetFile() ) )
		return hr;

	// After the reset, the size of the wav file is m_ck.cksize so store it now
	m_dwSize = m_ck.cksize;

	return hr;
}

HRESULT CWavFile::Close()
{
	if (m_hMmio)
	{
		mmioClose( m_hMmio, 0 );
		m_hMmio = NULL;

		return S_OK;
	}
	return S_FALSE;
}


//-----------------------------------------------------------------------------
// Name: CWaveFile::ReadMMIO()
// Desc: Support function for reading from a multimedia I/O stream.
//       m_hMmio must be valid before calling.  This function uses it to
//       update m_ckRiff, and m_pwfx.
//-----------------------------------------------------------------------------
HRESULT CWavFile::ReadMMIO()
{
	MMCKINFO ckIn;           // chunk info. for general use.
	PCMWAVEFORMAT pcmWaveFormat;  // Temp PCM structure to load in.

	if( ( 0 != mmioDescend( m_hMmio, &m_ckRiff, NULL, 0 ) ) )   // ������mmioOpen�����򿪵�RIFF�ļ��Ŀ��У�������һ����
		return E_FAIL;

	// Check to make sure this is a valid wave file
	if( ( m_ckRiff.ckid != FOURCC_RIFF ) ||
		( m_ckRiff.fccType != mmioFOURCC( 'W', 'A', 'V', 'E' ) ) )
		return E_FAIL;

	// Search the input file for for the 'fmt ' chunk.
	ckIn.ckid = mmioFOURCC( 'f', 'm', 't', ' ' );
	if( 0 != mmioDescend( m_hMmio, &ckIn, &m_ckRiff, MMIO_FINDCHUNK ) )
		return E_FAIL;

	// Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
	// if there are extra parameters at the end, we'll ignore them
	if( ckIn.cksize < ( LONG )sizeof( PCMWAVEFORMAT ) )
		return E_FAIL;

	// Read the 'fmt ' chunk into <pcmWaveFormat>.
	if( mmioRead( m_hMmio, ( HPSTR )&pcmWaveFormat,
		sizeof( pcmWaveFormat ) ) != sizeof( pcmWaveFormat ) )
		return E_FAIL;

	// Allocate the waveformatex, but if its not pcm format, read the next
	// word, and thats how many extra bytes to allocate.
	if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
	{
		// Copy the bytes from the pcm structure to the waveformatex structure
		memcpy( &m_wfx, &pcmWaveFormat, sizeof( pcmWaveFormat ) );
		m_wfx.cbSize = 0;
	}
	else
	{
		assert(0);
		return E_FAIL;  // ������δ��������⣬�ᵼ���������ڴ汻���ǣ�NND��
#if 0
		// Read in length of extra bytes.
		WORD cbExtraBytes = 0L;
		if( mmioRead( m_hMmio, ( CHAR* )&cbExtraBytes, sizeof( WORD ) ) != sizeof( WORD ) )
			return E_FAIL;


		// Copy the bytes from the pcm structure to the waveformatex structure
		memcpy( &m_wfx, &pcmWaveFormat, sizeof( pcmWaveFormat ) );
		m_wfx.cbSize = cbExtraBytes;

		// Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
		if( mmioRead( m_hMmio, ( CHAR* )( ( ( BYTE* )&( m_wfx.cbSize ) ) + sizeof( WORD ) ),
			cbExtraBytes ) != cbExtraBytes )
		{
			return E_FAIL;
		}
#endif
	}

	// Ascend the input file out of the 'fmt ' chunk.
	if( 0 != mmioAscend( m_hMmio, &ckIn, 0 ) )  // ȡ��һ��RIFF�ļ���
	{
		return E_FAIL;
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CWaveFile::GetSize()
// Desc: Retuns the size of the read access wave file
//-----------------------------------------------------------------------------
DWORD CWavFile::GetSize()
{
	return m_dwSize;
}


//-----------------------------------------------------------------------------
// Name: CWaveFile::ResetFile()
// Desc: Resets the internal m_ck pointer so reading starts from the
//       beginning of the file again
//       ����ǰλ��ָ��wave�ļ������ݲ���
//-----------------------------------------------------------------------------
HRESULT CWavFile::ResetFile()
{
	if( m_hMmio == NULL )
		return CO_E_NOTINITIALIZED;

	// Seek to the data
	if( -1 == mmioSeek( m_hMmio, m_ckRiff.dwDataOffset + sizeof( FOURCC ), SEEK_SET ) )
		return E_FAIL;

	// Search the input file for the 'data' chunk.
	m_ck.ckid = mmioFOURCC( 'd', 'a', 't', 'a' );
	if( 0 != mmioDescend( m_hMmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) )
		return E_FAIL;

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CWaveFile::Read()
// Desc: Reads section of data from a wave file into pBuffer and returns
//       how much read in pdwSizeRead, reading not more than dwSizeToRead.
//       This uses m_ck to determine where to start reading from.  So
//       subsequent calls will be continue where the last left off unless
//       Reset() is called.
//-----------------------------------------------------------------------------
HRESULT CWavFile::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead )
{

	if( m_hMmio == NULL )
		return CO_E_NOTINITIALIZED;
	if( pBuffer == NULL || pdwSizeRead == NULL )
		return E_INVALIDARG;

	*pdwSizeRead = 0;
	if (m_ck.cksize <= 0)
	{
		return E_FAIL;   // ��ȡ����
	}

	MMIOINFO mmioinfoIn; // current status of m_hMmio
	if( 0 != mmioGetInfo( m_hMmio, &mmioinfoIn, 0 ) )
		return E_FAIL;

	UINT cbDataIn = dwSizeToRead;
	if( cbDataIn > m_ck.cksize )
		cbDataIn = m_ck.cksize;

	m_ck.cksize -= cbDataIn;

	for( DWORD cT = 0; cT < cbDataIn; cT++ )
	{
		// Copy the bytes from the io to the buffer.
		if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
		{
			if( 0 != mmioAdvance( m_hMmio, &mmioinfoIn, MMIO_READ ) )
				return E_FAIL;

			if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
				return E_FAIL;
		}

		// Actual copy.
		*( ( BYTE* )pBuffer + cT ) = *( ( BYTE* )mmioinfoIn.pchNext );
		mmioinfoIn.pchNext++;
	}

	if( 0 != mmioSetInfo( m_hMmio, &mmioinfoIn, 0 ) )
		return E_FAIL;

	*pdwSizeRead = cbDataIn;

	return S_OK;
}

HRESULT CWavFile::SetCurPos(double percent) 
{
	int nOffset = (DWORD)(m_dwSize*percent);  // TODO: TNND�����offset����һ��ż���Ļ���mmioseek�ͻὫ�����ƻ���ʲô����?
	                                          //       wfx.wBitsPerSample==16��ÿ������ռ��λ��ԭ��ô��
	if (nOffset%2!=0)
	{
		nOffset--;
		if (nOffset < 0)
		{
			nOffset=0;
		}
	}
	this->ResetFile();

	// Seek to the data
	if( -1 == mmioSeek( m_hMmio, nOffset, SEEK_CUR ) )
		return E_FAIL;

	m_ck.cksize = m_dwSize - nOffset;

	return S_OK;
}
HRESULT CWavFile::GetCurPos(int nWriteBufferSize, double* pdSeconds, double* pdPercent) 
{
	if (m_dwSize == 0)
		return E_FAIL;

	int bytesPerSec = (m_wfx.nChannels*(m_wfx.wBitsPerSample>>3)*m_wfx.nSamplesPerSec);
	int nPlayedBytes = m_dwSize-(m_ck.cksize+nWriteBufferSize);    // ��ȥ�Ѿ����뻺����������

	if (0 != bytesPerSec)
		*pdSeconds = nPlayedBytes/bytesPerSec;

	*pdPercent = nPlayedBytes*1.0/m_dwSize;
	return S_OK;
}