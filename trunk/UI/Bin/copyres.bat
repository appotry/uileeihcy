REM  将Debug中的资源拷贝到Release当中


rd  Release /S /Q
mkdir  Release

REM 如果 Source 是一个目录或包含通配符，而 Destination 不存在，xcopy 会假定 destination 指定目录名并创建一个新目录。然后
REM xcopy 会将所有指定文件复制到新目录中。默认情况下，xcopy 将提示您指定 destination 是文件还是目录。 

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