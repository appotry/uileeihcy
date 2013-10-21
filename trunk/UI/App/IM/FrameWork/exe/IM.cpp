// IM.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "IM.h"

#include "App/IM/include/framework_inc.h"
#pragma comment(lib, "framework.lib")


#include <fstream>
using namespace std;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    IM::GetFramework()->Run(0);
    return 0;
}
