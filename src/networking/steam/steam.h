#pragma once
#include <cstdint>

typedef uint64 steam_user_id_t;
typedef uint64 steam_lobby_id_t;

typedef enum SteamErr
{
    OK,
    LOGIC_FAILED,
    LOBBY_EXISTS,
    PEER_NOT_FOUND,
    ALREADY_IN_LOBBY,
    NO_LOBBY_FOUND,
    API_REQ_FAILED,
    API_PARAM_INVALID,
    NO_CONNECTION,
    CONNECTION_INVALID,
};