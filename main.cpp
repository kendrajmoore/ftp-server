#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <filesystem>
#include <fstream>



class FTPServer {
private:
    int controlSocket;
    int dataSocket;
    struct sockaddr_in serverAddress;
    static const int BUFFER_SIZE = 1024;
    std::string currentDirectory;

public:
    FTPServer(int port = 21) {
        controlSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (controlSocket < 0) {
            throw std::runtime_error("Error creating control socket");
        }
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(port);

        if (bind(controlSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
            throw std::runtime_error("Error binding socket");
        }
        currentDirectory = std::filesystem::current_path().string();

    }

    void start(){
        listen(controlSocket, 5);
        std::cout << "FTP Server listening on port " << ntohs(serverAddress.sin_port) << std::endl;

        while (true) {
            struct sockaddr_in clientAddress;
            socklen_t clientLength = sizeof(clientAddress);
            int clientSocket = accept(controlSocket, (struct sockaddr*)&clientAddress, &clientLength);
            if (clientSocket < 0) {
                std::cerr << "Error accepting connection" << std::endl;
                continue;
            }
            sendResponse(clientSocket, "FTP Server\r\n");
            handleClient(clientSocket);

        }

    }

public:
    ~FTPServer() {
        close(controlSocket);
    }

};

int main() {

    try {
        FTPServer server(2121);
        server.start();
    }
    catch (const std::exception& e) {
        return 1;
    }
    return 0;
}
