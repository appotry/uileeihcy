#pragma once

namespace UI
{

// ���пؼ��Ļ���
class UIAPI Control : public Object
{
public:
	Control(void);
	~Control(void);

	UI_DECLARE_OBJECT( Control, OBJ_CONTROL )

protected:

public:

	// Object �麯��
public:
	virtual      SIZE        GetDesiredSize( HRDC hRDC ); // �ڲ�����GetAutoSize
	// ��û��ָ���ؼ��Ĵ�С(width or height)������£����Լ������Լ�����Ҫ�Ĵ�С��
	// ����ֵ������margin��margin�ļ�����GetDesiredSize��ɣ���padding�����Ӷ����Լ����
	virtual      SIZE        GetAutoSize( HRDC hRDC ) = 0;

};


}