#include "../lobby.h"
#include "core/globals.h"

SteamErr Lobby::JoinLobby(std::string lobbyName)
{
    return SteamErr::OK;
}

void Lobby::CreateLobby(std::string lobbyName, int maxMembers, bool friendsOnly)
{
    ELobbyType lobbyType;
    std::string lobbyTypeStr;

    if (friendsOnly)
    {
        lobbyType = k_ELobbyTypeFriendsOnly;
        lobbyTypeStr = "friends-only";
    }
    else
    {
        lobbyType = k_ELobbyTypePublic;
        lobbyTypeStr = "public";
    }

    this->lobbyName = lobbyName;

    logger.log("Creating " + lobbyTypeStr + " lobby with name " + lobbyName + ", max members " + std::to_string(maxMembers));

    if (!createLobbyCallResult.IsActive())
    {
        SteamAPICall_t ApiCall = SteamMatchmaking()->CreateLobby(lobbyType, maxMembers);
        createLobbyCallResult.Set(ApiCall, this, &Lobby::OnCreateLobby);
    }
    else
    {
        logger.log("Failed to create lobby, lobby was already created!");
    }

}

SteamErr Lobby::SendMessage(std::string content)
{
    return SteamErr::OK;
}

bool Lobby::IsOnline()
{
    return lobbyID.ConvertToUint64() != 0;
}

int Lobby::GetNumMembers()
{
    return SteamMatchmaking()->GetNumLobbyMembers(this->lobbyID);
}

CSteamID Lobby::GetMember(int index)
{
    return SteamMatchmaking()->GetLobbyMemberByIndex(this->lobbyID, index);
}

Lobby::~Lobby()
{
    logger.log("Lobby object destroyed, leaving lobby");
    SteamMatchmaking()->LeaveLobby(lobbyID);
    lobbyID = CSteamID();
}

void Lobby::OnCreateLobby(LobbyCreated_t* callback, bool ioFailure)
{
    if (ioFailure || (callback->m_eResult != k_EResultOK))
    {
        logger.err("Failed to create lobby!");
        lobbyID = CSteamID();
        return;
    }

    logger.log("Lobby created, lobby ID is " + std::to_string(callback->m_ulSteamIDLobby));
    lobbyID = callback->m_ulSteamIDLobby;
}