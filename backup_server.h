#ifndef BACKUP_SERVER_H
#define BACKUP_SERVER_H

#include "server.h"

// Thread du serveur de sauvegarde
void* backup_server_thread(void* arg);

// Sauvegarde et restauration
int sauvegarder_serveur(ServerMonitor* serveur);
int restaurer_serveur(ServerMonitor* serveur);

// Journalisation des vidéos (persistance)
int persist_video_to_journal(const Video *v);             // Ajoute une vidéo au journal
int mark_video_processed_in_journal(int video_id);         // Marque une vidéo comme traitée
int load_videos_from_journal(ServerMonitor* mainServer);   // Recharge les vidéos depuis le journal
int compact_journal(void);                                 // Optionnel : nettoie le journal

#endif
