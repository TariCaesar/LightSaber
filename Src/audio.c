#include "audio.h"

int32_t audioStartAddr[] = {1};

//audio length in pages
int32_t audioLen[] = {1};

int32_t AudioInit()
{
    ExternFlashInit();
}

int32_t AudioStore(int16_t* addrSrc, int32_t Size, int32_t addrDst)
{
}
