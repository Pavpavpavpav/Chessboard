#pragma once
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <string>
#include <cstdlib>

struct engine {
    pid_t pid = -1;
    int toE = -1;
    int fromE = -1;
    std::string buffer;

    std::string name;
    bool helo = false;
    int elomin = 0;
    int elomax = 0;

    bool start(const std::string& path){
        int a[2];
        int b[2];
        if (pipe(a) != 0 || pipe(b) != 0){
            return false;
        }

        pid = fork();
        if (pid < 0){
            return false;
        }
        if (pid == 0){
            dup2(a[0], 0);
            dup2(b[1], 1);
            close(a[0]);
            close(a[1]);
            close(b[0]);
            close(b[1]);
            execlp(path.c_str(), path.c_str(), (char*)nullptr);
            _exit(127);
        }

        close(a[0]);
        close(b[1]);
        toE = a[1];
        fromE = b[0];
        fcntl(fromE, F_SETFL, O_NONBLOCK);
        signal(SIGPIPE, SIG_IGN);
        return true;
    }

    void send(const std::string& s){
        if (toE < 0){
            return;
        }
        std::string line = s + "\n";
        (void)!write(toE, line.data(), line.size());
    }

    bool rLine(std::string& line){
        if (fromE < 0){
            return false;
        }

        char tmp[4096];
        ssize_t n;
        while((n = read(fromE, tmp, sizeof tmp)) > 0){
            buffer.append(tmp, n);
        }
        size_t p = buffer.find('\n');
        if (p == std::string::npos){
            return false; 
        }
        line = buffer.substr(0, p);
        buffer.erase(0, p + 1);
        if (line.empty() == false && line.back() == '\r'){
            line.pop_back();
        }
        return true;
    }

    bool wait(const std::string& tkn){
        std::string l;
        for (int t = 0; t < 5000; t += 10){
            while(rLine(l) == true){
                if (l == tkn){
                    return true;
                }
            }
            usleep(10000);
        }
        return false;
    }

    bool hshake(){
        send("uci");

        std::string l;
        bool ok = false;
        for (int t = 0; t < 5000 && ok == false; t+= 10){
            while (rLine(l) == true){
                if (l.rfind("id name ", 0) == 0){
                    name = l.substr(8); 
                }
                else if (l.rfind("option name UCI_Elo ", 0) == 0){
                    size_t p = l.find(" min ");
                    size_t q = l.find(" max ");
                    if (p != std::string::npos && q != std::string::npos){
                        helo = true;
                        elomin = atoi(l.c_str() + p + 5);
                        elomax = atoi(l.c_str() + q + 5);
                    }
                }
                else if (l == "uciok"){
                    ok = true;
                }
            }
            if (ok == false){
                usleep(10000);
            }
        }
        if (ok == false){
            return false;
        }

        send("isready");
        if (wait("readyok") == false){
            return false;
        }
        send("ucinewgame");
        return true;
    }

    void stop(){
        if (pid > 0){
            send("quit");
            usleep(10000);
            kill(pid, SIGTERM);
            waitpid(pid, nullptr, 0);
        }
        if (toE >= 0){
            close(toE);
        }
        if (fromE >= 0){
            close(fromE);
        }
        pid = -1;
        toE = -1;
        fromE = -1;
        buffer.clear();
        name.clear();
        hElo = false;
    }
};