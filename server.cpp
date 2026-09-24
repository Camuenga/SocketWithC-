#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>

using namespace std;

struct Salary 
{
    string Name;
    string Avg;
    string Mid;
};

typedef vector<struct Salary>::iterator sa_it;

void readSalary(std::vector<Salary>& r, std::string filename) {
    std::ifstream ifs(filename.c_str());
    if (ifs){
        string line;
        struct Salary s;
        while(getline(ifs, line)) {
            char * str = new char[line.length()+1];
            std::strcpy(str,line.c_str());
            s.Name = strtok(str, "\t");
            s.Avg = strtok(nullptr, "\t");
            s.Mid = strtok(nullptr, "\t");
            r.push_back(s);
        }
    }
    else {
        cout << "Cannot open file." <<endl;
    }
    ifs.close();
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd;
     socklen_t clilen;
     char buffer[256];
     char fileName[100];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     std::vector<Salary> salaries;
     string msg;

     cout << "Enter name of file: ";
     cin >> fileName;

     readSalary(salaries, fileName);

     sockfd = socket(AF_INET, SOCK_STREAM, 0);

     if (sockfd < 0) 
        cerr << "ERROR opening socket" << endl;
        
     memset((char *) &serv_addr, 0 , sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(8080);

     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        cerr << "ERROR on binding" << endl;
              
     listen(sockfd,5);

     clilen = sizeof(cli_addr);

     while(true){
        bool found = false;
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (newsockfd < 0) 
            cerr << "ERROR on accept" << endl;

      //  memset(buffer,0,256);
        n = read(newsockfd,buffer,255);

        if (n < 0) 
            cerr << "ERROR reading from socket" << endl;

        msg = buffer;
        msg.erase(msg.length()-1);

        for (sa_it it = salaries.begin(); it != salaries.end(); ++it)
        {
            if (it->Name == msg ){
                std::ostringstream oss;
                oss << "The average early career pay for a " << msg << " major is $" << it->Avg << "\n" << "The corresponding mid‐career pay is $" << it->Mid << "\n";
                string reply = oss.str();
                char * str = new char[reply.length()+1];
                std::strcpy(str,reply.c_str());
                n = write(newsockfd,str,256);
                found = true;
                break;
            }
            else {
                continue;
            }
        }

        if(msg.empty()) 
            n = write(newsockfd,"You are disconnected from server.",256);

        if (!found)
            n = write(newsockfd,"That major is not in the table.",256);

        if (n < 0) 
            cerr << "ERROR writing to socket" << endl; 

    }
    return 0;
}
