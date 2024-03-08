#pragma once

#include "al.h"
#include "alc.h"
struct ALState {
    ALboolean enumeration;
    const ALchar *devices;
    int ret;
    char *bufferData;
    ALCdevice *device;
    ALCcontext *context;
    ALuint buffer, source;
    ALfloat listenerOri[6] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
    ALCenum error;
    ALint source_state;
};

struct WavFile {
    ALenum format;
    ALsizei size, freq;
    ALvoid *data;
    ALboolean loop = AL_TRUE;
};
