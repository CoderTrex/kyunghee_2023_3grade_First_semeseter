/*+===================================================================
  File:      COMMON.H

  Summary:   Common header file that contains common header files and 
			 macros used for the Library project of Game Graphics
			 Programming course.

  Functions:

  © 2022 Kyung Hee University
===================================================================+*/
#pragma once

#ifndef  UNICODE
#define UNICODE
#endif // ! UNICODE

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#include <windows.h>
#include <WinUser.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>

using namespace DirectX;