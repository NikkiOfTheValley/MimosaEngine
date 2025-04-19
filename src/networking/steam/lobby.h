#pragma once
#include <string>
#include <functional>
#include <memory>

#pragma warning(disable : 4996)
#include <vendor/steam/steam_api.h>

#include "steam.h"

class Lobby
{
public:
    /*!
     * @brief Joins a lobby asyncronously
     * @param lobbyName The lobby to join
     * @return The error that occurred, or `SteamErr::OK` if no error occurred
     */
    SteamErr JoinLobby(std::string lobbyName);

    /*!
     * @brief Joins a lobby asyncronously
     * @param lobbyID The lobby to join
     * @return The error that occurred, or `SteamErr::OK` if no error occurred
     */
    SteamErr JoinLobby(CSteamID lobbyID);

    /*!
     * @brief Creates a lobby asyncronously
     * @param lobbyName The lobby name
     * @param maxMembers The maximum number of people that can be in the lobby
     * @param friendsOnly Whether the lobby should be friends only or public
     */
    void CreateLobby(std::string lobbyName, int maxMembers = 2, bool friendsOnly = true);

    /*!
     * @brief Sends a message to this lobby
     * @param content The content of the message
     * @return The error that occurred, or `SteamErr::OK` if no error occurred
     */
    SteamErr SendMessage(std::string content);

    /*!
     * @brief Returns true if the lobby is online
     */
    bool IsOnline();

    int GetNumMembers();

    CSteamID GetMember(int index);

    /*!
     * @brief Called when this lobby is created
     */
    std::function<void()> lobbyCreatedCallback;

    /*!
     * @brief Called when a message is received (this includes messages from the local user)
     */
    std::function<void(std::string, steam_user_id_t)> messageCallback;

    /*!
     * @brief Called when a user joins the lobby
     */
    std::function<void(steam_user_id_t)> userJoinedCallback;

    /*!
     * @brief Called when a user leaves the lobby
     */
    std::function<void(steam_user_id_t)> userLeftCallback;

    /*!
     * @brief Handles cleaning up the Lobby on the Steam side
     */
    ~Lobby();

private:
    CSteamID lobbyID;
    std::string lobbyName = "";

    void OnCreateLobby(LobbyCreated_t* callback, bool ioFailure);
    CCallResult<Lobby, LobbyCreated_t> createLobbyCallResult;
};