#pragma once
#include <string>
#include <memory>
#include <vector>

#include "lobby.h"

/*!
 * @brief Manages Steam API calls and abstracts away some of Steam's callback nonsense.
 * Most Steam objects should be created through SteamManager.
 */
class SteamManager
{
public:
    /*!
     * @brief Initializes SteamManager and the Steam API
     */
    void Init();

    /*!
     * @brief Creates a lobby object.
     * This is asyncronous, so make sure to check Lobby::IsOnline before using the lobby.
     * @param lobbyName The lobby name
     * @param maxMembers The maximum number of people that can be in the lobby
     * @param friendsOnly Whether the lobby should be friends only or public
     * @return A `std::shared_ptr` to the lobby object. Not thread safe!
     */
    std::shared_ptr<Lobby> CreateLobbyObject(std::string lobbyName, int maxMembers, bool friendsOnly);

    /*!
     * @brief Joins a lobby and creates a lobby object for it
     * @param lobbyName The lobby to join
     * @return A `std::shared_ptr` to the lobby object. Not thread safe!
     */
    std::shared_ptr<Lobby> JoinLobby(std::string lobbyName);

    /*!
     * @brief Joins a lobby and creates a lobby object for it
     * @param lobbyID The lobby to join
     * @return A `std::shared_ptr` to the lobby object. Not thread safe!
     */
    std::shared_ptr<Lobby> JoinLobby(steam_lobby_id_t lobbyID);

private:
    std::shared_ptr<Lobby> curLobby;
};