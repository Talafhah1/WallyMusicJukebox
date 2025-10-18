#pragma once

#ifdef WALLYMUSICJUKEBOX_EXPORTS
    #define WALLYMUSICJUKEBOX_API __declspec(dllexport)
#else
    #define WALLYMUSICJUKEBOX_API __declspec(dllimport)
#endif
