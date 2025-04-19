#include "../steam_net.h"
#include <chrono>
#include <thread>
#include "core/globals.h"
#include "networking/utils.h"

SteamNet::SteamNet()
{
    InitRelayNetworkAccess();
    
    InitAuthentication();
}

void SteamNet::SetupLobby(std::shared_ptr<Lobby> lobby)
{
    if (this->curLobby)
    {
        logger.log("Attempted to set up a new lobby while already in a lobby, leaving current lobby");
        LeaveLobby();
    }

    this->curLobby = lobby;

    using namespace std::chrono_literals;

    bool failed = !RetryAndWait([&](){
        SteamAPI_RunCallbacks();

        bool isOnline = this->curLobby->IsOnline();

        if (!isOnline)
            logger.warn("Attempted to set up lobby that is not connected yet, retrying...");

        return isOnline;
    }, 3, 500ms);

    if (failed)
    {
        logger.warn("Failed to join lobby, bailing!");
        curLobby = nullptr;
        return;
    }

    logger.log("Lobby connected, creating listen socket and poll group");
    
    // Create a listen socket
    socketHandle = SteamNetworkingSockets()->CreateListenSocketP2P(0, 0, nullptr);

    // Create a poll group
    pollGroup = SteamNetworkingSockets()->CreatePollGroup();

    logger.log("Lobby has " + std::to_string(curLobby->GetNumMembers()) + " member(s)");

    // Iterate through each member of the lobby (minus self) and connect to each one
    for (size_t i = 0; i < curLobby->GetNumMembers(); i++)
    {
        CSteamID id = curLobby->GetMember(i);

        // Make sure to skip over the local user
        if (id == SteamUser()->GetSteamID())
        {
            continue;
        }

        SteamNetworkingIdentity identity;
        identity.SetSteamID(id);
        
        // Connect to the member
        HSteamNetConnection handle = SteamNetworkingSockets()->ConnectP2P(identity, 0, 0, nullptr);
        
        // Add them to the poll group
        SteamNetworkingSockets()->SetConnectionPollGroup(handle, pollGroup);

        SteamConnection connection;
        connection.handle = handle;
        connection.identity = identity;
        connection.isConnected = true;
        connection.playername = SteamFriends()->GetFriendPersonaName(identity.GetSteamID());

        logger.log("Connected to Steam ID " + std::to_string(identity.GetSteamID64()) + ", alias " + connection.playername);

        connections.push_back(connection);
    }
    

}

void SteamNet::LeaveLobby()
{
    if (!curLobby)
    {
        logger.warn("Attempted to leave a lobby that is already disconnected!");
        return;
    }

    for (auto &c : connections)
    {
        SteamNetworkingSockets()->CloseConnection(c.handle, 0, nullptr, true);
    }
    
    SteamNetworkingSockets()->CloseListenSocket(socketHandle);

    SteamNetworkingSockets()->DestroyPollGroup(pollGroup);
    curLobby = nullptr;
}

void SteamNet::ProcessIncomingMessages()
{
    if (!curLobby)
    {
        logger.warn("Attempted to process incoming messages from nonexistent lobby!");
        return;
    }

    SteamNetworkingMessage_t* messages[128];

    int numMessages = SteamNetworkingSockets()->ReceiveMessagesOnPollGroup(pollGroup, &messages[0], 128);

    if (numMessages == -1)
    {
        logger.err("Failed to process incoming messages, poll group invalid!");
        return;
    }

    if (numMessages == 0)
        return;
    
    for (size_t i = 0; i < numMessages; i++)
    {
        size_t peerID = GetConnectionByIdentity(messages[i]->m_identityPeer);

        logger.log("Got a message from " + connections[peerID].playername);

        messages[i]->Release();
    }
}

SteamErr SteamNet::SendMessageRaw(const std::vector<uint8_t> data, size_t peerIndex)
{
    if (peerIndex >= connections.size())
        return SteamErr::PEER_NOT_FOUND;

    EResult result = SteamNetworkingSockets()->SendMessageToConnection(connections[peerIndex].handle, data.data(), data.size(), 0, nullptr);

    switch (result)
    {
    case k_EResultInvalidParam:
        return SteamErr::API_PARAM_INVALID;
        break;
    case k_EResultInvalidState:
        return SteamErr::CONNECTION_INVALID;
        break;
    case k_EResultNoConnection:
        return SteamErr::NO_CONNECTION;
        break;
    default:
        return SteamErr::API_REQ_FAILED;
        break;
    }
}

SteamErr SteamNet::BroadcastMessageRaw(const std::vector<uint8_t> data)
{
    if (connections.size() == 0)
        return SteamErr::OK;

    for (size_t i = 0; i < connections.size(); i++)
    {
        if (!connections[i].isConnected)
            continue;

        SteamErr err = SendMessageRaw(data, i);

        if (err == SteamErr::NO_CONNECTION || err == SteamErr::CONNECTION_INVALID)
        {
            logger.err("Tried to broadcast a message to a disconnected peer, removing peer from list");
            connections[i].isConnected = false;
        }

        if (err != SteamErr::OK)
            logger.err("Failed to broadcast a message to peer " + std::to_string(i));
    }

    return SteamErr::OK;
}

size_t SteamNet::GetPeerIndex(CSteamID id)
{
    for (size_t i = 0; i < connections.size(); i++)
    {
        if (connections[i].identity.GetSteamID() == id)
            return i;
    }

    return (size_t)-1;
}

size_t SteamNet::GetHostIndex()
{
    return hostConnectionIndex;
}

void SteamNet::InitRelayNetworkAccess()
{
    using namespace std::this_thread;
    using namespace std::chrono_literals;
    using std::chrono::steady_clock;

    logger.log("Inititalizing relay network access");
    SteamNetworkingUtils()->InitRelayNetworkAccess();

    bool failed = true;
    for (size_t i = 0; i < 3; i++)
    {
        SteamAPI_RunCallbacks();
        ESteamNetworkingAvailability status = SteamNetworkingUtils()->GetRelayNetworkStatus(nullptr);

        // If the relay network is initialized, break out of the loop
        if (status == k_ESteamNetworkingAvailability_Current)
        {
            failed = false;
            break;
        }

        // Statuses that are positive aren't an error state, nor is retrying, so just wait
        if (status > 0 || status == k_ESteamNetworkingAvailability_Retrying)
        {
            sleep_for(0.25s);
            // Waiting doesn't count as retrying, so just subtract 1 from the index count
            i--;
        }
        else
        {
            // Otherwise, an error occurred, so log it and retry
            logger.err("An error occurred while trying to initialize the relay network, retrying (" + std::to_string(3 - i) + " tries left)");
            SteamNetworkingUtils()->InitRelayNetworkAccess();
            sleep_for(1s);
        }
    }

    if (failed)
    {
        logger.fatal("Failed to initialize the relay network, Steam is likely unresponsive");
    }
}

void SteamNet::InitAuthentication()
{
    using namespace std::this_thread;
    using namespace std::chrono_literals;
    using std::chrono::steady_clock;

    logger.log("Inititalizing authentication");
    SteamNetworkingSockets()->InitAuthentication();

    bool failed = true;
    for (size_t i = 0; i < 3; i++)
    {
        SteamAPI_RunCallbacks();
        ESteamNetworkingAvailability status = SteamNetworkingSockets()->GetAuthenticationStatus(nullptr);

        // If the authentication is initialized, break out of the loop
        if (status == k_ESteamNetworkingAvailability_Current)
        {
            failed = false;
            break;
        }

        // Statuses that are positive aren't an error state, nor is retrying, so just wait
        if (status > 0 || status == k_ESteamNetworkingAvailability_Retrying)
        {
            sleep_for(1s);
            // Waiting doesn't count as retrying, so just subtract 1 from the index count
            i--;
        }
        else
        {
            // Otherwise, an error occurred, so log it and retry
            logger.err("An error occurred while trying to initialize authentication, retrying (" + std::to_string(3 - i) + " tries left)");
            SteamNetworkingSockets()->InitAuthentication();
            sleep_for(1s);
        }
    }

    if (failed)
        logger.fatal("Failed to initialize authentication, Steam is likely unresponsive");
}

size_t SteamNet::GetConnectionByIdentity(SteamNetworkingIdentity identity)
{
    for (size_t i = 0; i < connections.size(); i++)
    {
        if (connections[i].identity == identity)
            return i;
    }
}

void SteamNet::OnConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* param)
{
    
}