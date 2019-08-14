#define SCRWIDTH		400
#define SCRHEIGHT		400
// #define FULLSCREEN
 #define ADVANCEDGL	// faster if your system supports it

#include <inttypes.h>
extern "C" 
{ 
#include "glew.h" 
}
#include "gl.h"
#include "io.h"
#include <fstream>
#include <stdio.h>
#include "fcntl.h"
#include "SDL.h"
#include "wglext.h"
#include "freeimage.h"
#include "math.h"
#include "stdlib.h"
#include "emmintrin.h"
#include "immintrin.h"
#include "windows.h"
#include "template.h"
#include "surface.h"
#include "threads.h"
#include <assert.h>
#include <memory>
#include <vector>
#include <string>
#include <thread>

using namespace std;
using namespace Tmpl8;

#include "material.h"
#include "ray.h"
#include "primitive.h"
#include "camera.h"
#include "renderer.h"
#include "game.h"