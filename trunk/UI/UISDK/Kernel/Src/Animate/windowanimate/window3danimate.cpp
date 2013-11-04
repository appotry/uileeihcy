#include "stdafx.h"
#include "window3danimate.h"

#include "UISDK\Kernel\Src\Animate\3dlib\3dlib.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\Animate\animatemgr.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\layeredanimatewindow.h"

#include <float.h>
bool  g_bInitSinCosTable = false;

Window3DAnimate::Window3DAnimate()
{
	m_nRotateX = 0;
	m_nRotateY = 0;
	m_nRotateZ = 0;
	m_nOffsetX = 0;
	m_nOffsetY = 0;
	m_nOffsetZ = 0;

    m_pIWindow3DAnimate = NULL;
}
Window3DAnimate::~Window3DAnimate()
{
}


void  Window3DAnimate::Initialize()
{
    __super::Initialize();

    if (false == g_bInitSinCosTable)
    {
        _control87(_MCW_RC, _RC_DOWN); //����FPU������ģʽ����Bilinear��������Ҫʹ�û��ӿ�float->int

        Build_Sin_Cos_Tables();
        g_bInitSinCosTable = true;
    }
}

// ʹ��3Dͼ���㷨����ȡͼƬ��ת����ĸ�����λ��
void Window3DAnimate::GetQuadByAnimateValue(int nDegreeX, int nDegreeY, int nDegreeZ, int nZOffset, Quad* pOutQuad)
{
	// .�ֲ�����(�Ծ��ε�������Ϊ�ֲ������ԭ��)
	float x = (float)m_nSrcWndWidth/2;
	float y = (float)m_nSrcWndHeight/2;

	POINT3D  pt3dModel[4] = { 
		{-x, y, 0}, {x, y, 0}, {x, -y, 0}, {-x, -y, 0}
	};
	POINT3D pt3DWorld[4] = {0};
	POINT3D pt3DCamera[4] = {0};

#pragma region // .�ֲ�����->��������
	{
		// .�Ե�ǰֵ��Ϊ�ǶȽ�����ת
		float fDegreeX = (float)nDegreeX;
		float fDegreeY = (float)nDegreeY;
		float fDegreeZ = (float)nDegreeZ;

		MATRIX_4_4  matTemp1, matTemp2;
		MATRIX_4_4  matRotateY, matRotateX, matRotateZ;

		MATRIX_4_4_PTR pLeftArg = NULL;
		if (0 != fDegreeY)
		{
// 			matRotateY = {
// 				Fast_Cos(fDegreeY),  0, -Fast_Sin(fDegreeY), 0,
// 				0, 1, 0, 0,
// 				Fast_Sin(fDegreeY),  0,  Fast_Cos(fDegreeY), 0,
// 				0, 0, 0, 1
//			};
			MAT_IDENTITY_4_4(&matRotateY);
			matRotateY.M00 = Fast_Cos(fDegreeY);
			matRotateY.M02 = -Fast_Sin(fDegreeY);
			matRotateY.M20 = Fast_Sin(fDegreeY);
			matRotateY.M22 = Fast_Cos(fDegreeY);
			
			pLeftArg = &matRotateY;
		}

		if (0 != fDegreeX)
		{
// 			MATRIX_4_4 matRotateX= {
// 				1, 0, 0, 0,
// 				0,  Fast_Cos(fDegreeX), Fast_Sin(fDegreeX), 0,
// 				0, -Fast_Sin(fDegreeX), Fast_Cos(fDegreeX), 0,
// 				0, 0, 0, 1
// 			};

			MAT_IDENTITY_4_4(&matRotateX);
			matRotateX.M11 = Fast_Cos(fDegreeX);
			matRotateX.M12 = Fast_Sin(fDegreeX);
			matRotateX.M21 = -Fast_Sin(fDegreeX);
			matRotateX.M22 = Fast_Cos(fDegreeX);

			if (NULL == pLeftArg)
			{
				pLeftArg = &matRotateX;
			}
			else
			{
				Mat_Mul_4X4(pLeftArg, &matRotateX, &matTemp1);
				pLeftArg = &matTemp1;
			}
		}

		if (0 != fDegreeZ)
		{
// 			MATRIX_4_4 matRotateZ= {
//  			Fast_Cos(fDegreeZ), Fast_Sin(fDegreeZ), 0, 0
// 				-Fast_Sin(fDegreeZ), Fast_Cos(fDegreeZ), 1, 0,
// 				0, 0, 1, 0,
// 				0, 0, 0, 1
// 			};

			MAT_IDENTITY_4_4(&matRotateZ);
			matRotateZ.M00 = Fast_Cos(fDegreeZ);
			matRotateZ.M01 = Fast_Sin(fDegreeZ);
			matRotateZ.M10 = -Fast_Sin(fDegreeZ);
			matRotateZ.M11 = Fast_Cos(fDegreeZ);

			if (NULL == pLeftArg)
			{
				pLeftArg = &matRotateZ;
			}
			else
			{
				Mat_Mul_4X4(pLeftArg, &matRotateZ, &matTemp2);
				pLeftArg = &matTemp2;
			}
		}

		if (pLeftArg)
		{
			for (int i = 0; i < 4; i++)
				Mat_Mul_VECTOR3D_4X4(&pt3dModel[i], pLeftArg, &pt3DWorld[i]);
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				pt3DWorld[i].x = pt3dModel[i].x;
				pt3DWorld[i].y = pt3dModel[i].y;
			}
		}

		// .������Ȼ��ƽ�Ƶ���������� ��0��0��0��λ�ã���˲��ü���ƽ��
		for (int i = 0; i < 4; i++)
		{
			pt3DWorld[i].z += nZOffset;
		}
	}
#pragma endregion

	// ���λ��(����Ĭ�Ͻ���ƽ�����������������ͬһλ�ã�����͸�ӳ���������ֱ�ӾͿ���
	// ������Ļ���������ˣ�ʡ��һ��������
	// ������Ҫע����ǣ����nCameraPos��С�Ļ�������ͼ��ͻ���С��
	CAMERA camerpos = {0};
	float  fCameraPos = 2000.0f;  // ���λ��
	float  d = fCameraPos;      // �������ƽ��ľ��롣����ֵ���һ����������һ�ε���Ļ�����ת��

	VECTOR4D_INITXYZ(&camerpos.WorldPos, 0,0, -fCameraPos);

#pragma region // ��������ת��Ϊ�������
	{
		// ƽ�ƾ���
		MATRIX_4_4  matCameraTrans = {0};
		MAT_IDENTITY_4_4(&matCameraTrans);
		matCameraTrans.M30 = -camerpos.WorldPos.x;
		matCameraTrans.M31 = -camerpos.WorldPos.y;
		matCameraTrans.M32 = -camerpos.WorldPos.z;
		
		// ����Ƕ�Ϊ0������ת

		for (int i = 0; i < 4; i++)
		{
			Mat_Mul_VECTOR3D_4X4(&pt3DWorld[i], &matCameraTrans, &pt3DCamera[i]);
		}
	}
#pragma endregion

#pragma region // �������ת��Ϊ͸������
	POINT3D pt3DPerspectivePos[4];
	for (int i = 0; i < 4; i++)
	{
		float z = pt3DCamera[i].z;  // �����z�����ں�d��ȵľ��룬��������. ��dֵȡ�ıȽ�Сʱ���ᵼ��zΪ��
		float i_z = 1/z;
		if (pt3DCamera[i].z != 0)
		{
			pt3DPerspectivePos[i].x = d * pt3DCamera[i].x * i_z;  // nCameraPos�൱��d
			pt3DPerspectivePos[i].y = d * pt3DCamera[i].y * i_z;  // 
		}
	}
#pragma endregion

	// ת������Ļ������
	for (int i = 0; i < 4; ++i)
	{
		pt3DPerspectivePos[i].x += (m_nSrcWndWidth>>1);
		pt3DPerspectivePos[i].y = -pt3DPerspectivePos[i].y;
		pt3DPerspectivePos[i].y += (m_nSrcWndHeight>>1);
	}

	// ��ֵ������ֵ
	for (int i = 0; i < 4; i++)
	{
		pOutQuad->pos[2*i]   = (int)pt3DPerspectivePos[i].x;
		pOutQuad->pos[2*i+1] = (int)pt3DPerspectivePos[i].y;
	}
}


// ���������øĳɺ궨�壬��Ϊ�����������̫Ƶ����
#define GetPixelValue(pBits, nPitch, x, y) \
	(((DWORD*)(pBits + nPitch*(y)))[(x)])

void Window3DAnimate::OnTick(int nCount, IStoryboard** ppTimerArray)
{
	//RECT rc = {-m_nSrcWndWidth>>1, -m_nSrcWndHeight>>1, m_nSrcWndWidth>>1, m_nSrcWndHeight>>1};
	RECT rc = {0, 0, m_nSrcWndWidth, m_nSrcWndHeight};

	// ���㵱ǰ�Ķ���������Ļ����
	Quad quad;
	GetQuadByAnimateValue(m_nRotateX, m_nRotateY, m_nRotateZ, m_nOffsetZ, &quad);

	// ���ڼ������->�����ı��εĶ�Ӧ��ϵ
	PerspectiveTransform perspective;
	perspective.SetQuad(&quad, &rc);
	perspective.ChangeToFixedPoint();

	BYTE* pbSrcBits = NULL;
	BYTE* pDstBits = m_pLayeredWindow->m_pBits;
	int   nDstPitch = m_pLayeredWindow->m_nPitch;

	int   nWidthLimit  = m_nSrcWndWidth-1;   // (300->299)�����ݷ�Χ�����Ĵ�С��һ
	int   nHeightLimit = m_nSrcWndHeight-1; //

	float fxSrc = 0;
	float fySrc = 0;
	float *pfxSrc = &fxSrc;  // Ϊ����perspective_transform��ʹ��ָ��
	float *pfySrc = &fySrc;

	Color Color0, Color1, Color2, Color3, ColorResult;  // �������Բ�ֵ����

#if 1  // �ڴ�ѭ��֮ǰ�޳���һЩ�հ�����
	int nMinX = max(0, min(min(min(quad.Ax,quad.Bx),quad.Cx),quad.Dx));
	int nMinY = max(0, min(min(min(quad.Ay,quad.By),quad.Cy),quad.Dy));
	int nMaxX = min(m_nSrcWndWidth,  max(max(max(quad.Ax,quad.Bx),quad.Cx),quad.Dx));
	int nMaxY = min(m_nSrcWndHeight, max(max(max(quad.Ay,quad.By),quad.Cy),quad.Dy));

	if (nDstPitch < 0)
	{
		int nBitsSize = nDstPitch * m_nSrcWndHeight;
		pDstBits = m_pLayeredWindow->m_pBits + (nBitsSize - nDstPitch);
		memset(pDstBits, 0, -nBitsSize);
	}
	else
	{
		pDstBits = m_pLayeredWindow->m_pBits;
		memset(pDstBits, 0, nDstPitch * m_nSrcWndHeight);
	}

	pDstBits = m_pLayeredWindow->m_pBits + (nMinY*nDstPitch);
	for (int X = 0, Y = nMinY; Y < nMaxY; Y++)
	{
		for (X = nMinX; X < nMaxX; X++)
		{
#else
	for (int X = 0, Y = 0; Y < m_nSrcWndHeight; Y++)
	{
		for (X = 0; X < m_nSrcWndWidth; X++)
		{
#endif
			//perspective.transform(X, Y, &xSrc, &ySrc); // (TODO: ��Ҫ�Ż����ú����ڲ���Ҫ8�γ˷���һ�γ���...�˽���3D��������ôʵ�ֵ�)
			perspective_transform_fp(perspective, X, Y, pfxSrc, pfySrc);

			int nx = 0; //fx;   // ע: ֱ�Ӹ�ֵ int i = floatvalue; �����һ�� _ftol()����
			int ny = 0; //fy;   //     ����Ч�ʽ��͡�����ʹ����Ƕ���ָ�(3D��Ϸ��̴�ʦ���� P918)

			// ������ת������ ע�⣺Ĭ�ϵ�fistp����������ģʽ����Ҫͨ������_control87(_MCW_RC, _RC_DOWN);���е���
			__asm	fld    fxSrc;
			__asm	fistp  nx;

			__asm	fld    fySrc;
			__asm	fistp  ny;

			// 1. ceil��Ч�ʷǳ��ǳ���!ǧ���������
			// ������һ���������heightΪ300��ySrc=299.99999999ʱ��ת��(int)�õ��Ľ����300��
			// ���������жϵĻ��ǳɹ��ģ���������������˵�ySrc>299ʱ���˳�
			if (nx < 0 || nx > nWidthLimit || ny < 0 || ny > nHeightLimit)
			{
				//	((DWORD*)pDstBits)[X] = 0;
				continue;
			}

			// Ŀ����Դͼ���ض�Ӧ
#if 0
			//////////////////////////////////////////////////////////////////////////
			// �ڽ�ȡ��, �ٶȿ죬���о��
			((DWORD*)pDstBits)[X] = ((DWORD*)(m_pSrcBits + m_nSrcPitch*(int)ySrc))[(int)xSrc];
#else
			//////////////////////////////////////////////////////////////////////////
			// �������Բ�ֵ������Чȥ����ݣ����ٶȷǳ���
			//((DWORD*)pDstBits)[X] = Bilinear(m_pSrcBits, m_nSrcPitch, nx, ny, (float)fxSrc, (float)fySrc);

			//
			// �������Բ�ֵ��ʽ�Ƶ�
			// http://blog.csdn.net/dakistudio/article/details/1767100 �������Բ�ֵ��ʽ�Ƶ�
			//
			// xΪfx������ȡ����yΪfy������ȡ��
			//
			//inline DWORD& Bilinear(BYTE* pBits, const int& nPitch, const int& x, const int& y, const float& fx, const float& fy)
			{
				// �ѽ�ԭʼͼƬ��right/bottom����1px�������ڻ�ȡ x+1, y+1ʱ�ﵽ�����ԵҲ�������
				// ȡ�������ĸ����ص���ɫֵ(x,y) (x+1, y) (x, y+1) (x+1, y+1)
				// PS: ���ĸ�����Ҳ�ǳ���CPU�����ǲ�֪����ô�Ż���Խ�Ż����CPUԽ����

//  			pbSrcBits = m_pSrcBits + (ny*m_nSrcPitch + (nx<<2));
//  			Color0.m_col = *((DWORD*)(pbSrcBits));
//  			Color2.m_col = (((DWORD*)(pbSrcBits))[1]);
//  			pbSrcBits += m_nSrcPitch;
// 				Color1.m_col = *((DWORD*)(pbSrcBits));
// 				Color3.m_col = (((DWORD*)(pbSrcBits))[1]);

 				Color0.m_col = GetPixelValue(m_pSrcBits, m_nSrcPitch, nx,  ny);
 				Color2.m_col = GetPixelValue(m_pSrcBits, m_nSrcPitch, nx+1,ny);
 				Color1.m_col = GetPixelValue(m_pSrcBits, m_nSrcPitch, nx,  ny+1);
 				Color3.m_col = GetPixelValue(m_pSrcBits, m_nSrcPitch, nx+1,ny+1);

				// ���������˷�תΪ�������˷���ͬʱ�Ż�������ת��������
				float u = (float)fxSrc - nx;
				float v = (float)fySrc - ny;
			
				float fpm3 = FLOAT_TO_FIXP16(u*v);
				float fpm2 = FLOAT_TO_FIXP16(u*(1.0f-v));
				float fpm1 = FLOAT_TO_FIXP16(v*(1.0f-u));
				float fpm0 = FLOAT_TO_FIXP16((1.0f-u)*(1.0f-v));
				int pm3_16 = 0;
				int pm2_16 = 0;
				int pm1_16 = 0;
				int pm0_16 = 0;
				__asm
				{
					fld    fpm3;
					fistp  pm3_16;
					fld    fpm2;
					fistp  pm2_16;
					fld    fpm1;
					fistp  pm1_16;
					fld    fpm0;
					fistp  pm0_16;
				}

				ColorResult.a = (byte)((pm0_16*Color0.a + pm1_16*Color1.a + pm2_16*Color2.a + pm3_16*Color3.a) >> FIXP16_SHIFT);
				ColorResult.r = (byte)((pm0_16*Color0.r + pm1_16*Color1.r + pm2_16*Color2.r + pm3_16*Color3.r) >> FIXP16_SHIFT);
				ColorResult.g = (byte)((pm0_16*Color0.g + pm1_16*Color1.g + pm2_16*Color2.g + pm3_16*Color3.g) >> FIXP16_SHIFT);
				ColorResult.b = (byte)((pm0_16*Color0.b + pm1_16*Color1.b + pm2_16*Color2.b + pm3_16*Color3.b) >> FIXP16_SHIFT);
				((DWORD*)pDstBits)[X] = ColorResult.m_col;

// 				float u = (float)fxSrc - nx;
// 				float v = (float)fySrc - ny;
// 				float pm3 = u*v;
// 				float pm2 = u*(1.0f-v);
// 				float pm1 = v*(1.0f-u);
// 				float pm0 = (1.0f-u)*(1.0f-v);
// 
// 				ColorResult.a = (byte)(pm0*Color0.a + pm1*Color1.a + pm2*Color2.a + pm3*Color3.a);
// 				ColorResult.r = (byte)(pm0*Color0.r + pm1*Color1.r + pm2*Color2.r + pm3*Color3.r);
// 				ColorResult.g = (byte)(pm0*Color0.g + pm1*Color1.g + pm2*Color2.g + pm3*Color3.g);
// 				ColorResult.b = (byte)(pm0*Color0.b + pm1*Color1.b + pm2*Color2.b + pm3*Color3.b);
// 				((DWORD*)pDstBits)[X] = ColorResult.m_col;
			}
#endif
		}
		pDstBits += nDstPitch;
	}
}
