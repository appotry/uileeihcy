REM 清理临时文件，用于代码转移
del  ..\..\UI.ncb

rd  ..\..\App\ControlDemo\Debug /S /Q
rd  ..\..\App\ControlDemo\Release /S /Q
del ..\..\App\ControlDemo\ControlDemo.vcproj.*.user


rd  ..\..\App\Player\Player_UI\Debug /S /Q
rd  ..\..\App\Player\Player_UI\Release /S /Q
del ..\..\App\Player\Player_UI\Player_UI.vcproj.*.user

rd  ..\..\App\Player\Player_Sound\Debug /S /Q
rd  ..\..\App\Player\Player_Sound\Release /S /Q
del ..\..\App\Player\Player_Sound\Player_Sound.vcproj.*.user

rd  ..\..\App\Player\Player_Ctrls\Debug /S /Q
rd  ..\..\App\Player\Player_Ctrls\Release /S /Q
del ..\..\App\Player\Player_Ctrls\Player_Ctrls.vcproj.*.user

rd  ..\..\App\IM\IM_UI\Debug /S /Q
rd  ..\..\App\IM\IM_UI\Release /S /Q
del ..\..\App\IM\IM_UI\IM_UI.vcproj.*.user

rd  ..\..\App\IM\IM_Ctrls\Debug /S /Q
rd  ..\..\App\IM\IM_Ctrls\Release /S /Q
del ..\..\App\IM\IM_Ctrls\IM_Ctrls.vcproj.*.user


rd  ..\..\Bin\Debug\pdb /S /Q
del ..\..\Bin\Debug\lib\*.exp

rd  ..\..\Bin\Release\pdb /S /Q
del ..\..\Bin\Release\lib\*.exp

rd  ..\..\UISDK\Project\UICtrls\Debug /S /Q
rd  ..\..\UISDK\Project\UICtrls\Release /S /Q
del ..\..\UISDK\Project\UICtrls\UICtrls.vcproj.*.user

rd  ..\..\UISDK\Project\UIDLL\Debug /S /Q
rd  ..\..\UISDK\Project\UIDLL\Release /S /Q
del ..\..\UISDK\Project\UIDLL\UIDLL.vcproj.*.user

rd  ..\..\UISDK\Project\UILog\Debug /S /Q
rd  ..\..\UISDK\Project\UILog\Release /S /Q
del ..\..\UISDK\Project\UILog\UILog.vcproj.*.user

rd  ..\..\UISDK\Project\UIUtil\Debug /S /Q
rd  ..\..\UISDK\Project\UIUtil\Release /S /Q
del ..\..\UISDK\Project\UIUtil\UIUtil.vcproj.*.user

rd  ..\..\UISDK\Project\UIEditor\Debug /S /Q
rd  ..\..\UISDK\Project\UIEditor\Release /S /Q
del ..\..\UISDK\Project\UIEditor\UIEditor.vcproj.*.user
pause