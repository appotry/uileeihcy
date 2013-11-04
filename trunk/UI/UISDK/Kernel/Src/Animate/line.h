#pragma once

// �Լ�д�Ĵ��룬�ѷ����� 20130129

#if 0
POINT pt[4] = 
{
	{0,50}, {100,-50},{0,100}, {100,200}
};
// 	int nDestW = 100;
// 	int nDestH = 200;

Line* pAB = Line::CreateLine(pt[0], pt[1]);
Line* pBD = Line::CreateLine(pt[1], pt[3]);
Line* pAC = Line::CreateLine(pt[0], pt[2]);
Line* pCD = Line::CreateLine(pt[2], pt[3]);

for (int Y = -50; Y < 200; Y++)
{
	// 1. �жϸõ��Ƿ�λ�ڶ�����ڲ�

	HorzLine horzline;
	horzline.y = Y;

	DPOINT ptAC, ptBD, ptAB, ptCD;
	LINE_INTERSECT eIntersectAC = pAC->Calc2lineIntersect(&horzline, &ptAC);
	LINE_INTERSECT eIntersectBD = pBD->Calc2lineIntersect(&horzline, &ptBD);
	LINE_INTERSECT eIntersectAB = pAB->Calc2lineIntersect(&horzline, &ptAB);
	LINE_INTERSECT eIntersectCD = pCD->Calc2lineIntersect(&horzline, &ptCD);

	DPOINT dp[4] = {0};
	int nCount = 0;

	if (eIntersectAB == LINE_INTERSECT_IN && AddPoint(dp, nCount, ptAB))
		nCount++;

	if (eIntersectBD == LINE_INTERSECT_IN && AddPoint(dp, nCount, ptBD))
		nCount++;

	if (eIntersectAC == LINE_INTERSECT_IN && AddPoint(dp, nCount, ptAC))
		nCount++;

	if (eIntersectCD == LINE_INTERSECT_IN && AddPoint(dp, nCount, ptCD))
		nCount++;

	if (0 == nCount)
		continue;
	if (1 == nCount)
	{
		dp[1].x = dp[0].x;
		dp[1].y = dp[0].y;
	}

	for (int X = 0; X < 100; X++)
	{

		// x����λ������dp�м䣬�����ʾ�õ㲻���ı��з�Χ��
		if ((X > dp[0].x && X > dp[1].x) ||
			(X < dp[0].x && X < dp[1].x))
			continue;



		// 			// 2. ��������������ϵ�ͶӰ�㣬ӳ�䵽ԭ��������
		// 			double pAB_pos = pAB->CalcPosPercent(ptAB);
		// 			DPOINT ptSrcAB = {nSrcW*pAB_pos, 0};
		// 
		// 			double pCD_pos = pCD->CalcPosPercent(ptCD);
		// 			DPOINT ptSrcCD = {nSrcW*pCD_pos, nSrcH};
		// 
		// 			double pAC_pos = pAC->CalcPosPercent(ptAC);
		// 			DPOINT ptSrcAC = {0, nSrcH*pAC_pos};
		// 
		// 			double pBD_pos = pBD->CalcPosPercent(ptBD);
		// 			DPOINT ptSrcBD = {nSrcW, nSrcH*pBD_pos};
		// 
		// 			// 3. ��ԭ�����н��ĸ����γ�����ֱ�߽����ཻ���ó�ӳ���
		// 			Line* pSrcVLine = Line::CreateLine(ptSrcAB, ptSrcCD);
		// 			Line* pSrcHLine = Line::CreateLine(ptSrcAC, ptSrcBD);
		// 
		// 			DPOINT ptSrc;
		// 			pSrcVLine->Calc2lineIntersect(pSrcHLine, &ptSrc);
	}
}

SAFE_DELETE(pAB);
SAFE_DELETE(pBD);
SAFE_DELETE(pAC);
SAFE_DELETE(pCD);
#endif
namespace UI
{
// ��������ֱ�ߵĽ���
enum LINE_TYPE
{
	LINE_POINT,
	LINE_HORZ,
	LINE_VERT,
	LINE_NORMAL
};

enum LINE_INTERSECT
{
	LINE_INTERSECT_NO,  // �����߲��ཻ
	LINE_INTERSECT_OUT, // �������ཻ�������㲻���߶���
	LINE_INTERSECT_IN,  // �������ཻ����ս���߶���
};


struct DPOINT
{
	double x;
	double y;
};

class Line
{
public:
	Line();
	virtual ~Line() {};

	virtual LINE_INTERSECT    Calc2lineIntersect(Line* pLine, DPOINT* pPt) = 0;
	virtual double  CalcPosPercent(const DPOINT& pt) = 0;

	static Line* CreateLine(const DPOINT& pt1, const DPOINT& pt2);
	static Line* CreateLine(const POINT& pt1, const POINT& pt2);

public:
	LINE_TYPE  m_eLineType;
	DPOINT  m_pt1;
	DPOINT  m_pt2;
};
class PointLine : public Line
{
public:
	PointLine() 
	{ 
		m_eLineType = LINE_NORMAL;
		pt.x = pt.y = 0;
	}
	virtual LINE_INTERSECT    Calc2lineIntersect(Line* pLine, DPOINT* pPt);
	virtual double  CalcPosPercent(const DPOINT& pt) { return 0; }
public:
	DPOINT pt;
};

class HorzLine : public Line
{
public:
	HorzLine() { m_eLineType = LINE_HORZ; y = 0;}
	virtual LINE_INTERSECT    Calc2lineIntersect(Line* pLine, DPOINT* pPt);
	virtual double  CalcPosPercent(const DPOINT& pt);
public:
	double y;
};

class VertLine : public Line
{
public:
	VertLine() { m_eLineType = LINE_VERT; x = 0; }
	virtual LINE_INTERSECT    Calc2lineIntersect(Line* pLine, DPOINT* pPt);
	virtual double  CalcPosPercent(const DPOINT& pt);
public:
	double x;
};

class NormalLine : public Line
{
public:
	NormalLine()
	{
		m_eLineType = LINE_NORMAL;
		k = 0; b = 0;
	}
	virtual LINE_INTERSECT  Calc2lineIntersect(Line* pLine, DPOINT* pPt);
	virtual double  CalcPosPercent(const DPOINT& pt);

	double GetX(double y)
	{
		return (y - b) / k; // kΪ0ʱ�Ͳ�����normalline��
	}
	double GetY(double x)
	{
		return k*x + b;
	}
public:
	double k;
	double b;
};




}