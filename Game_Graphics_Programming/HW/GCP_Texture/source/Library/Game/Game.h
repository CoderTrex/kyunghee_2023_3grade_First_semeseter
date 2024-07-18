/*+===================================================================
  File:      GAME.H

  Summary:   Game header file that contains declarations of functinos
			 used for the lab samples of Game Graphics Programming 
             course.

  Functions: PrintHi

  © 2022 Kyung Hee University
===================================================================+*/
#pragma once

#include "Common.h"
#include "DDSTextureLoader11.h"

HRESULT InitWindow(_In_ HINSTANCE hInstance, _In_ INT nCmdShow);
HRESULT InitDevice();
void CleanupDevice();
void Render();

struct SimpleVertex 
{
	XMFLOAT3 Position;
	XMFLOAT2 Material;
};

struct ConstantBuffer
{
	XMMATRIX A;
	XMMATRIX B;
	XMMATRIX C;
};

struct cbView
{
	XMMATRIX View;
}; 

struct cbProjection
{
	XMMATRIX Projection;
}; 

struct cbWorld
{
	XMMATRIX World;
};

HRESULT CompileShaderFromFile(
	_In_ PCWSTR pszFileName, // FileName
	_In_ PCSTR pszEntryPoint, // EntryPoint
	_In_ PCSTR pszShaderModel, // Shader target
	_Outptr_ ID3DBlob** ppBlobOut // ID3DBlob out
);

