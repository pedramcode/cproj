#ifndef PACKET_TYPES_H
#define PACKET_TYPES_H

/// @brief packet command type
typedef enum {
    /// @brief first message for handshaking process
    CMD_HELLO = 1,
    /// @brief response of hello, send back to new node from seed node (bootstrapping node)
    CMD_WELCOME,
    /// @brief update other nodes about network, environment or changes
    CMD_UPDATE,
    /// @brief represents occured error (mainly use as response)
    CMD_ERROR,
} packet_type_t;

#endif // PACKET_TYPES_H