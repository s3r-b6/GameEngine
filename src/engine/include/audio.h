#pragma once

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alut.h"
#include "engine_lib.h"
#include <cstdint>
#include <cstring>
#include <string>

struct ALState {
    ALboolean enumeration;
    const ALchar *devices;
    int ret;
    char *bufferData;
    ALCdevice *device;
    ALCcontext *context;
    ALuint buffer, bgBuffer, source, bgSource;
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

bool displayErrorsAL(const std::string &filename, const std::uint_fast32_t line) {
    ALenum error = alGetError();

    if (error == AL_NO_ERROR) return true;

    switch (error) {
    case AL_INVALID_NAME:
        _log(filename, line, "AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function");
        break;
    case AL_INVALID_ENUM:
        _log(filename, line,
             "AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function");
        break;
    case AL_INVALID_VALUE:
        _log(filename, line, "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function");
        break;
    case AL_INVALID_OPERATION:
        _log(filename, line, "AL_INVALID_OPERATION: the requested operation is not valid");
        break;
    case AL_OUT_OF_MEMORY:
        _log(filename, line,
             "AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of "
             "memory");
        break;
    default:
        _log(filename, line, "UNKNOWN AL ERROR: ", error);
    }

    return false;
}

// TODO: just one wavFile
global WavFile wavFile = {};
WavFile *loadAudio(const char *originalFilename) {
    signed char filename[64];

    strncpy((char *)filename, originalFilename, 64);
    filename[64 - 1] = '\0';

    alutLoadWAVFile(filename, &wavFile.format, &wavFile.data, &wavFile.size, &wavFile.freq,
                    &wavFile.loop);
    displayErrorsAL(__FILE__, __LINE__);

    return &wavFile;
}

void playBackground(ALState *alState, WavFile *wavFile) {
    alBufferData(alState->buffer, wavFile->format, wavFile->data, wavFile->size, wavFile->freq);
    alSourcei(alState->bgSource, AL_BUFFER, alState->bgBuffer);
    alSourcei(alState->bgSource, AL_LOOPING, AL_TRUE);
    alSourcePlay(alState->bgSource);
}

void playSoundEffects(ALState *alState, WavFile *wavFile) {
    alBufferData(alState->buffer, wavFile->format, wavFile->data, wavFile->size, wavFile->freq);
    alSourcei(alState->source, AL_BUFFER, alState->buffer);
    alSourcePlay(alState->source);
}
