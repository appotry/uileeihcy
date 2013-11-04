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
        _control87(_MCW_RC, _RC_DOWN); //设置FPU的舍入模式，在Bilinear函数中需要使用汇编加快float->int

        Build_Sin_Cos_Tables();
        g_bInitSinCosTable = true;
    }
}

// 使用3D图形算法来获取图片旋转后的四个顶点位置
void Window3DAnimate::GetQuadByAnimateValue(int nDegreeX, int nDegreeY, int nDegreeZ, int nZOffset, Quad* pOutQuad)
{
	// .局部坐标(以矩形的中心作为局部坐标的原点)
	float x = (float)m_nSrcWndWidth/2;
	float y = (float)m_nSrcWndHeight/2;

	POINT3D  pt3dModel[4] = { 
		{-x, y, 0}, {x, y, 0}, {x, -y, 0}, {-x, -y, 0}
	};
	POINT3D pt3DWorld[4] = {0};
	POINT3D pt3DCamera[4] = {0};

#pragma region // .局部坐标->世界坐标
	{
		// .以当前值作为角度进行旋转
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

		// .由于仍然是平移到世界坐标的 （0，0，0）位置，因此不用计算平移
		for (int i = 0; i < 4; i++)
		{
			pt3DWorld[i].z += nZOffset;
		}
	}
#pragma endregion

	// 相机位置(这里默认将视平面放在与矩形所在面的同一位置，这样透视出来的坐标直接就可以
	// 当成屏幕坐标来用了，省了一步操作。
	// 但是需要注意的是，如果nCameraPos过小的话看到的图像就会缩小，
	CAMERA camerpos = {0};
	float  fCameraPos = 2000.0f;  // 相机位置
	float  d = fCameraPos;      // 相机与视平面的距离。将两值设成一样，避免了一次到屏幕坐标的转换

	VECTOR4D_INITXYZ(&camerpos.WorldPos, 0,0, -fCameraPos);

#pragma region // 世界坐标转换为相机坐标
	{
		// 平移矩阵
		MATRIX_4_4  matCameraTrans = {0};
		MAT_IDENTITY_4_4(&matCameraTrans);
		matCameraTrans.M30 = -camerpos.WorldPos.x;
		matCameraTrans.M31 = -camerpos.WorldPos.y;
		matCameraTrans.M32 = -camerpos.WorldPos.z;
		
		// 相机角度为0，不旋转

		for (int i = 0; i < 4; i++)
		{
			Mat_Mul_VECTOR3D_4X4(&pt3DWorld[i], &matCameraTrans, &pt3DCamera[i]);
		}
	}
#pragma endregion

#pragma region // 相机坐标转换为透视坐标
	POINT3D pt3DPerspectivePos[4];
	for (int i = 0; i < 4; i++)
	{
		float z = pt3DCamera[i].z;  // 这里的z是用于和d相比的距离，不是坐标. 当d值取的比较小时，会导致z为负
		float i_z = 1/z;
		if (pt3DCamera[i].z != 0)
		{
			pt3DPerspectivePos[i].x = d * pt3DCamera[i].x * i_z;  // nCameraPos相当于d
			pt3DPerspectivePos[i].y = d * pt3DCamera[i].y * i_z;  // 
		}
	}
#pragma endregion

	// 转换到屏幕坐标上
	for (int i = 0; i < 4; ++i)
	{
		pt3DPerspectivePos[i].x += (m_nSrcWndWidth>>1);
		pt3DPerspectivePos[i].y = -pt3DPerspectivePos[i].y;
		pt3DPerspectivePos[i].y += (m_nSrcWndHeight>>1);
	}

	// 赋值给返回值
	for (int i = 0; i < 4; i++)
	{
		pOutQuad->pos[2*i]   = (int)pt3DPerspectivePos[i].x;
		pOutQuad->pos[2*i+1] = (int)pt3DPerspectivePos[i].y;
	}
}


// 将函数调用改成宏定义，因为这个函数调用太频繁了
#define GetPixelValue(pBits, nPitch, x, y) \
	(((DWORD*)(pBits + nPitch*(y)))[(x)])

void Window3DAnimate::OnTick(int nCount, IStoryboard** ppTimerArray)
{
	//RECT rc = {-m_nSrcWndWidth>>1, -m_nSrcWndHeight>>1, m_nSrcWndWidth>>1, m_nSrcWndHeight>>1};
	RECT rc = {0, 0, m_nSrcWndWidth, m_nSrcWndHeight};

	// 计算当前的动画窗口屏幕坐标
	Quad quad;
	GetQuadByAnimateValue(m_nRotateX, m_nRotateY, m_nRotateZ, m_nOffsetZ, &quad);

	// 用于计算矩形->任意四边形的对应关系
	PerspectiveTransform perspective;
	perspective.SetQuad(&quad, &rc);
	perspective.ChangeToFixedPoint();

	BYTE* pbSrcBits = NULL;
	BYTE* pDstBits = m_pLayeredWindow->m_pBits;
	int   nDstPitch = m_pLayeredWindow->m_nPitch;

	int   nWidthLimit  = m_nSrcWndWidth-1;   // (300->299)，数据范围是它的大小减一
	int   nHeightLimit = m_nSrcWndHeight-1; //

	float fxSrc = 0;
	float fySrc = 0;
	float *pfxSrc = &fxSrc;  // 为了在perspective_transform中使用指针
	float *pfySrc = &fySrc;

	Color Color0, Color1, Color2, Color3, ColorResult;  // 二次线性插值数据

#if 1  // 在大循环之前剔除掉一些空白区域
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
			//perspective.transform(X, Y, &xSrc, &ySrc); // (TODO: 需要优化，该函数内部需要8次乘法，一次除法...了解下3D引擎是怎么实现的)
			perspective_transform_fp(perspective, X, Y, pfxSrc, pfySrc);

			int nx = 0; //fx;   // 注: 直接赋值 int i = floatvalue; 会产生一个 _ftol()调用
			int ny = 0; //fy;   //     导致效率降低。这里使用内嵌汇编指令。(3D游戏编程大师技巧 P918)

			// 浮点数转整数。 注意：默认的fistp是四舍五入模式。需要通过调用_control87(_MCW_RC, _RC_DOWN);进行调整
			__asm	fld    fxSrc;
			__asm	fistp  nx;

			__asm	fld    fySrc;
			__asm	fistp  ny;

			// 1. ceil的效率非常非常低!千万别在这用
			// 但是有一个问题如果height为300，ySrc=299.99999999时，转成(int)得到的结果是300，
			// 但在这里判断的话是成功的，导致最后崩溃，因此当ySrc>299时即退出
			if (nx < 0 || nx > nWidthLimit || ny < 0 || ny > nHeightLimit)
			{
				//	((DWORD*)pDstBits)[X] = 0;
				continue;
			}

			// 目标与源图像素对应
#if 0
			//////////////////////////////////////////////////////////////////////////
			// 邻近取样, 速度快，但有锯齿
			((DWORD*)pDstBits)[X] = ((DWORD*)(m_pSrcBits + m_nSrcPitch*(int)ySrc))[(int)xSrc];
#else
			//////////////////////////////////////////////////////////////////////////
			// 二次线性插值，能有效去掉锯齿，但速度非常慢
			//((DWORD*)pDstBits)[X] = Bilinear(m_pSrcBits, m_nSrcPitch, nx, ny, (float)fxSrc, (float)fySrc);

			//
			// 二次线性插值公式推导
			// http://blog.csdn.net/dakistudio/article/details/1767100 二次线性插值公式推导
			//
			// x为fx的向下取整，y为fy的向下取整
			//
			//inline DWORD& Bilinear(BYTE* pBits, const int& nPitch, const int& x, const int& y, const float& fx, const float& fy)
			{
				// 已将原始图片的right/bottom扩大1px，这样在获取 x+1, y+1时达到数组边缘也不会崩溃
				// 取附近的四个像素的颜色值(x,y) (x+1, y) (x, y+1) (x+1, y+1)
				// PS: 这四个调用也非常耗CPU，但是不知道怎么优化。越优化点的CPU越高了

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

				// 将浮点数乘法转为定点数乘法。同时优化浮点数转成整数。
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
