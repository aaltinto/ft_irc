# ft_irc

## Overview
`ft_irc` is a custom implementation of an IRC (Internet Relay Chat) server and bot. It is designed to provide a lightweight and efficient IRC server that adheres to the IRC protocol, along with a bot that can connect to the server and interact with channels and users. This project is implemented in C++ and follows the C++98 standard.

## Features
- **IRC Server**:
  - Handles multiple clients and channels.
  - Supports basic IRC commands such as `JOIN`, `PART`, `PRIVMSG`, `NICK`, `USER`, `TOPIC`, `KICK`, and more.
  - Implements channel modes and user permissions.
  - File tranfer via using IRC clients like HexChat.
  - Error handling for invalid commands, channels, and users.

- **IRC Bot**:
  - Connects to the IRC server and joins specified channels.
  - Processes and responds to messages in real-time.
  - Maintains a blacklist and whitelist for user interactions.
  - Tracks user activity and enforces custom rules.

## Project Structure
```
ft_irc/
├── bot/
│   ├── bot.cpp
│   ├── bot.hpp
│   └── main.cpp
├── channel/
│   ├── channel.cpp
│   ├── channel_mods.cpp
│   ├── channel_operations.cpp
│   └── mode.cpp
├── communication/
│   └── send_message.cpp
├── errors/
│   └── server_errors.cpp
├── includes/
│   ├── channel.hpp
│   ├── client.hpp
│   ├── mode.hpp
│   └── server.hpp
├── server/
│   ├── client.cpp
│   ├── main.cpp
│   ├── server.cpp
│   └── server_build.cpp
└── Makefile
```

## Getting Started

### Prerequisites
- A C++ compiler that supports the C++98 standard (e.g., `g++`).
- A Unix-based operating system (e.g., macOS, Linux).

### Building the Project
To build the IRC server and bot, use the provided `Makefile`:

1. Build the server:
   ```bash
   make
   ```

2. Build the bot:
   ```bash
   make bot
   ```

3. Clean build artifacts:
   ```bash
   make clean
   ```

4. Remove all build artifacts and executables:
   ```bash
   make fclean
   ```

5. Rebuild the project:
   ```bash
   make re
   ```

6. Rebuild the bot:
   ```bash
   make rebot
   ```

### Running the Server
To start the IRC server, run the following command:
```bash
./ircserv <port> <password>
```
- `<port>`: The port number on which the server will listen (e.g., `6667`).
- `<password>`: The password required for clients to connect to the server.

### Running the Bot
To start the bot, run the following command:
```bash
./ircbot <server> <port> <password> <nickname> "[#channel1,#channel2,...]"
```
- `<server>`: The IP address or hostname of the IRC server.
- `<port>`: The port number of the IRC server.
- `<password>`: The password for the IRC server.
- `<nickname>`: The nickname of the bot.
- `[#channel1,#channel2,...]`: A comma-separated list of channels for the bot to join.

## Code Structure

### Server
The server code is located in the `server/` directory and includes the following key components:
- `server.cpp`: Core server logic.
- `server_build.cpp`: Handles server initialization and client connections.
- `client.cpp`: Manages client-related operations.
- `main.cpp`: Entry point for the server application.

### Bot
The bot code is located in the `bot/` directory and includes the following key components:
- `bot.cpp`: Core bot logic.
- `bot.hpp`: Bot class definition.
- `main.cpp`: Entry point for the bot application.

### Channels and Modes
The `channel/` directory contains code for managing channels and their modes:
- `channel.cpp`: Core channel logic.
- `channel_mods.cpp`: Handles channel modes.
- `channel_operations.cpp`: Implements channel-related commands.
- `mode.cpp`: Defines the `Mode` class for channel modes.

### Communication
The `communication/` directory contains code for sending messages:
- `send_message.cpp`: Implements message-sending functionality.

### Error Handling
The `errors/` directory contains code for handling server errors:
- `server_errors.cpp`: Implements error messages for invalid commands, channels, and users.

## Contributing
Contributions are welcome! If you would like to contribute to this project, please follow these steps:
1. Fork the repository.
2. Create a new branch for your feature or bug fix.
3. Commit your changes and push them to your fork.
4. Submit a pull request with a detailed description of your changes.

## Acknowledgments
- The IRC protocol documentation and community for guidance.
- Open-source contributors for inspiration and support.

---

Thank you for using `ft_irc`! If you have any questions or feedback, feel free to reach out.
