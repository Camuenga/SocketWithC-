#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>

using namespace std;

int main()
{
    int sockfd, n;

    struct sockaddr_in serv_addr;
    struct hostent *server = nullptr;
    
    char* s1 = new char[sizeof(char)*64];
    char buffer[256];

    cout << "Enter server host name: ";
    cin.getline(s1,sizeof(buffer));
    server = gethostbyname(s1);

    if (server == nullptr) 
        cerr << "ERROR, no such host\n";

    while(true) {

        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0) 
            cerr << "ERROR opening socket" << endl;

        memset((char *) &serv_addr, 0 ,sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        memcpy((char *)&serv_addr.sin_addr.s_addr,(char *)server->h_addr, server->h_length);
        serv_addr.sin_port = htons(8080);
        serv_addr.sin_addr.s_addr = INADDR_ANY;

        if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
            cerr << "ERROR connecting" << endl;

        cout << "Enter a college major: ";
        cin.getline(buffer,256);
        
        strcat(buffer, "\n");
        n = write(sockfd,buffer,strlen(buffer));

        if (n < 0) 
            cerr << "ERROR writing to socket" << endl;
         
        n = read(sockfd,buffer,255);
        
        if (n < 0) 
            cerr << "ERROR reading from socket" << endl;
         
        cout  << buffer << '\n';

        string msg = buffer;

        if (msg == "You are disconnected from server.") 
            return -1;
    }
    
    delete [] s1;
    return 0;
}
