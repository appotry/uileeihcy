REM  ��Debug�е���Դ������Release����


rd  Release /S /Q
mkdir  Release

REM ��� Source ��һ��Ŀ¼�����ͨ������� Destination �����ڣ�xcopy ��ٶ� destination ָ��Ŀ¼��������һ����Ŀ¼��Ȼ��
REM xcopy �Ὣ����ָ���ļ����Ƶ���Ŀ¼�С�Ĭ������£�xcopy ����ʾ��ָ�� destination ���ļ�����Ŀ¼�� 

mkdir  Release\TTPlayer
xcopy  Debug\TTPlayer\*.*  Release\TTPlayer /E

mkdir  Release\IM
xcopy  Debug\IM\*.*  Release\IM /E

mkdir  Release\Editor
xcopy  Debug\Editor\*.*  Release\Editor  /E

mkdir  Release\controldemo
xcopy  Debug\controldemo\*.* Release\controldemo /E

xcopy  3rd_depent\*.*  Release /E

copy  Debug\reg.bat  Release\reg.bat
copy  Debug\unreg.bat  Release\unreg.bat

pause