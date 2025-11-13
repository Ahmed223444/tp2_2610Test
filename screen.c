#include <stdio.h>
#include <unistd.h>
#include "screen.h"
#include "server.h"

void broadcastVideo(Video video) {
    
    printf("[SCREEN] Diffusion de la vidéo %d : %s (durée %d s)\n",
           video.id, video.content, video.duration);

    sleep(video.duration);
}

void* screenThread(void* arg) {
    ServerMonitor* server = (ServerMonitor*)arg;

    while (1) {
        Video video = getVideo(server);
        int index = server->out;

        broadcastVideo(video);

        completeVideo(server, index);

        sleep(video.duration);
    }

    return NULL;
}
