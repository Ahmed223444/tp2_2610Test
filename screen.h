#ifndef SCREEN_H
#define SCREEN_H

#include "server.h"

void* screenThread(void* arg);

void broadcastVideo(Video video);

#endif
