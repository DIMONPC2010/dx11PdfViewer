#pragma once
#define PAGE_NUM 7

#include <clocale>
#include <ctime>

#include <string>
#include <list>
#include <vector>
#include <deque>
#include <memory>
#include <fstream>
#include <thread>
#include <mutex>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "WICTextureLoader.h"
#include <mupdf/fitz.h>
#include <wrl.h>