//
//  AEUtilities.m
//  The Amazing Audio Engine
//
//  Created by Michael Tyson on 23/03/2012.
//  Copyright (c) 2012 A Tasty Pixel. All rights reserved.
//

#include "AEUtilities.h"

AudioBufferList *AEAllocateAndInitAudioBufferList(AudioStreamBasicDescription *audioFormat, int frameCount) {
    int numberOfBuffers = audioFormat->mFormatFlags & kAudioFormatFlagIsNonInterleaved ? audioFormat->mChannelsPerFrame : 1;
    int channelsPerBuffer = audioFormat->mFormatFlags & kAudioFormatFlagIsNonInterleaved ? 1 : audioFormat->mChannelsPerFrame;
    int bytesPerBuffer = audioFormat->mBytesPerFrame * frameCount;
    
    AudioBufferList *audio = malloc(sizeof(AudioBufferList) + (numberOfBuffers-1)*sizeof(AudioBuffer));
    if ( !audio ) {
        return NULL;
    }
    audio->mNumberBuffers = numberOfBuffers;
    for ( int i=0; i<numberOfBuffers; i++ ) {
        if ( bytesPerBuffer > 0 ) {
            audio->mBuffers[i].mData = malloc(bytesPerBuffer);
            if ( !audio->mBuffers[i].mData ) {
                for ( int j=0; j<i; j++ ) free(audio->mBuffers[j].mData);
                free(audio);
                return NULL;
            }
        } else {
            audio->mBuffers[i].mData = NULL;
        }
        audio->mBuffers[i].mDataByteSize = bytesPerBuffer;
        audio->mBuffers[i].mNumberChannels = channelsPerBuffer;
    }
    return audio;
}

AudioBufferList *AECopyAudioBufferList(AudioBufferList *original) {
    AudioBufferList *audio = malloc(sizeof(AudioBufferList) + (original->mNumberBuffers-1)*sizeof(AudioBuffer));
    if ( !audio ) {
        return NULL;
    }
    audio->mNumberBuffers = original->mNumberBuffers;
    for ( int i=0; i<original->mNumberBuffers; i++ ) {
        audio->mBuffers[i].mData = malloc(original->mBuffers[i].mDataByteSize);
        if ( !audio->mBuffers[i].mData ) {
            for ( int j=0; j<i; j++ ) free(audio->mBuffers[j].mData);
            free(audio);
            return NULL;
        }
        audio->mBuffers[i].mDataByteSize = original->mBuffers[i].mDataByteSize;
        audio->mBuffers[i].mNumberChannels = original->mBuffers[i].mNumberChannels;
        memcpy(audio->mBuffers[i].mData, original->mBuffers[i].mData, original->mBuffers[i].mDataByteSize);
    }
    return audio;
}

void AEFreeAudioBufferList(AudioBufferList *bufferList ) {
    for ( int i=0; i<bufferList->mNumberBuffers; i++ ) {
        if ( bufferList->mBuffers[i].mData ) free(bufferList->mBuffers[i].mData);
    }
    free(bufferList);
}

void AEInitAudioBufferList(AudioBufferList *list, int listSize, AudioStreamBasicDescription *audioFormat, void *data, int dataSize) {
    list->mNumberBuffers = audioFormat->mFormatFlags & kAudioFormatFlagIsNonInterleaved ? audioFormat->mChannelsPerFrame : 1;
    assert(list->mNumberBuffers == 1 || listSize >= (sizeof(AudioBufferList)+sizeof(AudioBuffer)) );
    
    for ( int i=0; i<list->mNumberBuffers; i++ ) {
        list->mBuffers[0].mNumberChannels = audioFormat->mFormatFlags & kAudioFormatFlagIsNonInterleaved ? 1 : audioFormat->mChannelsPerFrame;
        list->mBuffers[0].mData = (char*)data + (i * (dataSize/list->mNumberBuffers));
        list->mBuffers[0].mDataByteSize = dataSize/list->mNumberBuffers;
    }
}

int AEGetNumberOfFramesInAudioBufferList(AudioBufferList *list, AudioStreamBasicDescription *audioFormat, int *oNumberOfChannels) {
    int channelCount = audioFormat->mFormatFlags & kAudioFormatFlagIsNonInterleaved ? list->mNumberBuffers : list->mBuffers[0].mNumberChannels;
    if ( oNumberOfChannels ) {
        *oNumberOfChannels = channelCount;
    }
    return list->mBuffers[0].mDataByteSize / ((audioFormat->mBitsPerChannel/8) * channelCount);
}
