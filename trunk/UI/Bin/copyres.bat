REM  将Debug中的资源拷贝到Release当中


rd  Release /S /Q
mkdir  Release

REM 如果 Source 是一个目录或包含通配符，而 Destination 不存在，xcopy 会假定 destination 指定目录名并创建一个新目录。然后
REM xcopy 会将所有指定文件复制到新目录中。默认情况下，xcopy 将提示您指定 destination 是文件还是目录。 

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