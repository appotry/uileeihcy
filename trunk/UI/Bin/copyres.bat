REM  ��Debug�е���Դ������Release����


rd  Release /S /Q
mkdir  Release

REM ��� Source ��һ��Ŀ¼�����ͨ������� Destination �����ڣ�xcopy ��ٶ� destination ָ��Ŀ¼��������һ����Ŀ¼��Ȼ��
REM xcopy �Ὣ����ָ���ļ����Ƶ���Ŀ¼�С�Ĭ������£�xcopy ����ʾ��ָ�� destination ���ļ�����Ŀ¼�� 

mkdir  Release\Player
xcopy  Debug\Player\*.*  Release\Player /E

mkdir  Release\IM
xcopy  Debug\IM\*.*  Release\IM /E

mkdir  Release\HourGlass
xcopy  Debug\HourGlass\*.*  Release\HourGlass /E

mkdir  Release\UI
xcopy  Debug\UI\*.*  Release\UI /E

mkdir  Release\Demo
xcopy  Debug\Demo\*.* Release\Demo /E

xcopy  3rd_depent\*.*  Release /E

copy  Debug\reg.bat  Release\reg.bat
copy  Debug\unreg.bat  Release\unreg.bat

pause