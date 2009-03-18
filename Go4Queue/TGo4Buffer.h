#ifndef TGO4BUFFER_HEADER
#define TGO4BUFFER_HEADER

#include "RVersion.h"

#if ROOT_VERSION_CODE < ROOT_VERSION(5,15,1)

#include "TBuffer.h"
#define TGo4Buffer TBuffer

#else

#include "TBufferFile.h"
#define TGo4Buffer TBufferFile

#endif

#endif
