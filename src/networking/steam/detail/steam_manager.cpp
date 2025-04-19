#include "../steam_manager.h"
#include "core/globals.h"

void SteamManager::Init()
{
	SteamErrMsg msg;
	if (SteamAPI_InitEx(&msg) != k_ESteamAPIInitResult_OK)
		logger.fatal("Failed to init Steam: " + std::string(&msg[0]));
}

std::shared_ptr<Lobby> SteamManager::CreateLobbyObject(std::string lobbyName, int maxMembers, bool friendsOnly)
{
    curLobby = std::make_shared<Lobby>();

    curLobby->CreateLobby(lobbyName, maxMembers, friendsOnly);

    return curLobby;
}

std::shared_ptr<Lobby> SteamManager::JoinLobby(std::string lobbyName)
{

}

std::shared_ptr<Lobby> SteamManager::JoinLobby(steam_lobby_id_t lobbyID)
{

}