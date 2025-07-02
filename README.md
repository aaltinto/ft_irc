# ft_irc

<div align="center">
  <img src="https://img.shields.io/badge/language-C%2B%2B-blue.svg" alt="Language">
  <img src="https://img.shields.io/badge/standard-C%2B%2B98-blue.svg" alt="C++ Standard">
  <img src="https://img.shields.io/badge/license-MIT-green.svg" alt="License">
</div>

## Overview

`ft_irc` is a robust implementation of an Internet Relay Chat (IRC) server and bot, built from scratch following the IRC protocol specifications. This project provides a lightweight, efficient, and standards-compliant IRC server capable of handling multiple client connections simultaneously, along with a versatile bot that can connect to the server and interact with channels and users.

Developed in compliance with the C++98 standard, this project demonstrates advanced network programming concepts, concurrent connection handling, and protocol implementation.

## Features

### IRC Server
- **Connection Management**: Handles multiple simultaneous client connections using non-blocking I/O with `poll()`.
- **Channel Operations**: Robust channel creation, joining, and management with admin privileges.
- **Comprehensive Command Support**: Implements essential IRC commands:
  - `JOIN`: Connect to channels
  - `PART`: Leave channels
  - `PRIVMSG`: Send messages to channels or users
  - `NICK`: Set or change nickname
  - `USER`: Set username and real name
  - `TOPIC`: Set or view channel topics
  - `KICK`: Remove users from channels
  - `MODE`: Set channel modes and user permissions
  - `INVITE`: Invite users to channels
  - `QUIT`: Disconnect from server
- **Channel Modes**: Implements various channel modes:
  - `+i`: Invite-only channels
  - `+t`: Topic protection
  - `+k`: Password-protected channels
  - `+o`: Channel operator status
  - `+l`: User limit enforcement
- **File Transfer**: Supports file transfer capabilities via IRC clients like HexChat.
- **Error Handling**: Comprehensive error handling with appropriate IRC numeric replies.

### IRC Bot
- **Auto-Connection**: Seamlessly connects to the IRC server and joins specified channels.
- **Real-Time Processing**: Monitors and responds to messages in real-time.
- **User Management**: Maintains blacklist and whitelist for user interactions.
- **Activity Monitoring**: Tracks user activity and can enforce custom channel rules.

## Project Structure

```
ft_irc/
├── bot/                      # Bot implementation
│   ├── bot.cpp               # Bot core functionality
│   ├── bot.hpp               # Bot class definition
│   └── main.cpp              # Bot entry point
├── channel/                  # Channel management
│   ├── channel.cpp           # Channel core functionality
│   ├── channel_mods.cpp      # Channel mode management
│   ├── channel_operations.cpp # Channel command handlers
│   └── mode.cpp              # Mode class implementation
├── communication/            # Communication utilities
│   └── send_message.cpp      # Message sending implementation
├── errors/                   # Error handling
│   └── server_errors.cpp     # Error messages and responses
├── includes/                 # Header files
│   ├── channel.hpp           # Channel class definition
│   ├── client.hpp            # Client class definition
│   ├── mode.hpp              # Mode class definition
│   └── server.hpp            # Server class definition
├── server/                   # Server implementation
│   ├── client.cpp            # Client functionality
│   ├── main.cpp              # Server entry point
│   ├── server.cpp            # Core server functionality
│   └── server_build.cpp      # Server initialization
└── Makefile                  # Build configuration
```

## Getting Started

### Prerequisites

- A C++ compiler that supports the C++98 standard (e.g., `g++`, `clang++`)
- A Unix-based operating system (macOS, Linux)
- Network connectivity for multi-client testing (optional)

### Building the Project

The project uses a comprehensive `Makefile` to simplify the build process:

#### Server

```bash
# Build the IRC server
make

# Clean object files
make clean

# Remove all build artifacts and executables
make fclean

# Rebuild the entire project
make re
```

#### Bot

```bash
# Build the IRC bot
make bot

# Rebuild the bot
make rebot
```

All build commands use the flags `-Wall -Wextra -Werror -std=c++98` to ensure strict compliance with the C++98 standard.

### Running the Server

Start the IRC server by executing:

```bash
./ircserv <port> <password>
```

Parameters:
- **`<port>`**: The port number for the server to listen on (must be in range 1024-65535)
- **`<password>`**: The server password that clients need to provide (must be 1-16 characters)

Example:
```bash
./ircserv 6667 serverpass123
```

### Running the Bot

Launch the IRC bot with:

```bash
./ircbot <server> <port> <password> <nickname> "[#channel1,#channel2,...]"
```

Parameters:
- **`<server>`**: The IP address or hostname of the IRC server (e.g., `127.0.0.1`, `localhost`)
- **`<port>`**: The port number of the IRC server
- **`<password>`**: The server password
- **`<nickname>`**: The bot's nickname on the network
- **`"[#channel1,#channel2,...]"`**: Comma-separated list of channels for the bot to join (must be in quotes)

Example:
```bash
./ircbot localhost 6667 serverpass123 ircbot "#channel1,#channel2"
```

### Connecting with IRC Clients

You can connect to the server using popular IRC clients such as:

- **HexChat**: Set server address to your IP/hostname and port from command line
- **irssi**: Connect using `/connect <server> <port> <password>`
- **WeeChat**: Connect using `/server add myserver <server>/<port> -password=<password>`

Once connected, use standard IRC commands to interact with the server:
```
/nick your_nickname
/user username * * :Real Name
/join #channel
/msg #channel Hello, world!
```

## Technical Implementation

### Network Architecture

The server implements a non-blocking I/O model using the `poll()` system call, enabling it to efficiently handle multiple client connections simultaneously without resource-intensive threading. This architecture allows the server to:

1. Accept new client connections
2. Read incoming data from connected clients
3. Process IRC commands
4. Send appropriate responses to clients

### Server Components

The `Server` class forms the core of the application and manages:

- **Socket Management**: Creates and binds to the server socket, handles client connections
- **Client Tracking**: Maintains a collection of connected clients and their states
- **Channel Management**: Creates, modifies, and removes channels as needed
- **Command Processing**: Parses and executes IRC commands from clients
- **Error Handling**: Generates appropriate error responses following the IRC protocol

### IRC Protocol Implementation

The server implements essential IRC commands according to RFC specifications:

- **Connection Registration**: `PASS`, `NICK`, `USER` commands for client authentication
- **Channel Operations**: `JOIN`, `PART`, `TOPIC`, `KICK`, `INVITE` commands
- **Messaging**: `PRIVMSG` command for channel and private messages
- **Mode Management**: `MODE` command with support for various channel modes
- **Disconnection**: `QUIT` command for graceful disconnection

### Bot Implementation

The IRC bot demonstrates a client implementation that can connect to the server and perform automated tasks:

- **Network Client**: Implements socket connections to the IRC server
- **Message Processing**: Parses and responds to IRC protocol messages
- **User Management**: Tracks users and enforces custom rules
- **Command Automation**: Can execute commands based on predefined triggers

### Error Handling

The server includes comprehensive error handling with proper IRC numeric replies:

- Invalid command parameters
- Non-existent channels or users
- Permission violations
- Connection and authentication errors

## Supported IRC Commands

| Command   | Description                                   | Format                                      |
|-----------|-----------------------------------------------|---------------------------------------------|
| `PASS`    | Authenticate with server password             | `/PASS <password>`                          |
| `NICK`    | Set or change nickname                        | `/NICK <nickname>`                          |
| `USER`    | Set username and real name                    | `/USER <username> 0 * :<realname>`          |
| `JOIN`    | Join a channel                                | `/JOIN <channel> [<key>]`                   |
| `PART`    | Leave a channel                               | `/PART <channel> [<message>]`               |
| `TOPIC`   | Set or view channel topic                     | `/TOPIC <channel> [:<topic>]`               |
| `PRIVMSG` | Send message to channel or user               | `/PRIVMSG <target> :<message>`              |
| `KICK`    | Remove user from channel                      | `/KICK <channel> <nickname> [<reason>]`     |
| `INVITE`  | Invite user to channel                        | `/INVITE <nickname> <channel>`              |
| `MODE`    | Set channel modes                             | `/MODE <channel> <+/-><modes> [parameters]` |
| `QUIT`    | Disconnect from server                        | `/QUIT [<message>]`                         |

### Supported Channel Modes

| Mode | Description          | Format                        |
|------|----------------------|-------------------------------|
| `+i` | Invite-only channel  | `/MODE <channel> +i`          |
| `+t` | Topic protection     | `/MODE <channel> +t`          |
| `+k` | Password protection  | `/MODE <channel> +k <key>`    |
| `+o` | Channel operator     | `/MODE <channel> +o <nick>`   |
| `+l` | User limit           | `/MODE <channel> +l <limit>`  |

## Contributing

Contributions are welcome! To contribute to this project:

1. Fork the repository
2. Create a feature branch: `git checkout -b feature-name`
3. Implement your changes with proper testing
4. Follow the C++98 standard and existing code style
5. Commit your changes: `git commit -m "Add feature-name"`
6. Push to your branch: `git push origin feature-name`
7. Submit a pull request with detailed description

## License

This project is available under the MIT License. See the LICENSE file for details.

## Acknowledgments

- RFC documentation for the IRC protocol
- Open-source IRC server implementations for reference
- Testing and feedback from fellow developers

---

Thank you for using `ft_irc`! For questions, issues or contributions, please open an issue or pull request in the repository.
