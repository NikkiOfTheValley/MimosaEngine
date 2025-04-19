#pragma once
#include <functional>
#include <vector>
#include <memory>

#pragma warning(disable : 4996)
#include <vendor/steam/steam_api.h>

#include "steam.h"
#include "lobby.h"

/*!
 * @brief This is a Steam connection handle with extra information
 */
struct SteamConnection
{
    HSteamNetConnection handle;
    SteamNetworkingIdentity identity;
    std::string playername;
    bool isConnected;
};

struct Message
{
    std::vector<uint8_t> data;

    /*!
     * @brief Is either the peer that sent this message, or the peer to send this message to
     */
    size_t peerID;
};

/*!
 * @brief Handles Steam networking. Only operates in P2P mode.
 */
class SteamNet
{
public:
    SteamNet();

    /*!
     * @brief Sets up a given lobby
     * @param lobby The lobby to set up
     */
    void SetupLobby(std::shared_ptr<Lobby> lobby);

    /*!
     * @brief Leaves the current lobby
     */
    void LeaveLobby();

    /*!
     * @brief Processes incoming messages
     */
    void ProcessIncomingMessages();

    /*!
     * @brief Sends raw data to a peer
     * @param data The data to send
     * @param peerIndex The index in the connections vector to send the message to
     * @return The error that occurred, or `SteamErr::OK` if no error occurred
     */
    SteamErr SendMessageRaw(const std::vector<uint8_t> data, size_t peerIndex);

    /*!
     * @brief Broadcasts a message to all peers
     * @param data The data to broadcast
     * @return The error that occurred, or `SteamErr::OK` if no error occurred
     */
    SteamErr BroadcastMessageRaw(const std::vector<uint8_t> data);

    /*!
     * @brief Gets a peer index from a Steam ID
     * @param id The Steam ID to search for
     * @return The peer index, or -1 if no peer was found
     */
    size_t GetPeerIndex(CSteamID id);

    /*!
     * @brief Gets the host peer index
     * @return The index
     */
    size_t GetHostIndex();

    std::function<void(CSteamID from, void* data, size_t size)> messageHandler;
private:
    std::shared_ptr<Lobby> curLobby = nullptr;

    HSteamListenSocket socketHandle;
    HSteamNetPollGroup pollGroup;

    std::vector<SteamConnection> connections = {};
    size_t hostConnectionIndex;

    void InitRelayNetworkAccess();

    void InitAuthentication();

    size_t GetConnectionByIdentity(SteamNetworkingIdentity identity);

    STEAM_CALLBACK(SteamNet, OnConnectionStatusChanged, SteamNetConnectionStatusChangedCallback_t);
};