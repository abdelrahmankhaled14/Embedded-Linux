#include<iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <initializer_list>
#include <vector>


class myClass 
{
    private:
           int *fdptr = nullptr;
           int *&fdRef = fdptr;
           std::string path;
           std::vector<std::pair<std::string,int>> actions;
           bool OWNERSHIP = true;

    public:
        myClass()
        {
            std::cout << "Default Constructor Called" << std::endl;
            path = "/home/abdo/projects/linux/C++/task1/myfile";
            fdptr = (int*) new int;
            *fdptr = open(path.c_str(), O_RDWR);
        }
        ~myClass()
        {
            std::cout << "Destructor Called" << std::endl;
            if(fdptr != nullptr && OWNERSHIP)
            {
                close(*fdRef);
                delete fdptr;
            }
        }
        myClass(const myClass& other)
        {
            std::cout << "Copy Constructor Called" << std::endl;
            fdptr=other.fdptr;
            path=other.path;
            fdRef=fdptr;
            actions=other.actions;
            executeActions();
            OWNERSHIP=false;
        }
        myClass(std::string& obj)
        {
            std::cout << "Parameterized Constructor Called with string: " << obj << std::endl;
        }
        void registerAction(std::initializer_list<std::pair <std::string,int>>actions)
        {
            for (const auto& action : actions) {
                this->actions.push_back(action);
            }
        }
        void executeActions()
        {
            for (const auto& action : actions)
            {
                std::cout << "Executing action: " << action.first << " with code: " << action.second << std::endl;
                if(action.first == "write")
                {
                     write(*fdRef, "3uyweutu", 7);
                }
                else if(action.first == "read")
                {
                    char buffer[110] = {0}; 
                    lseek(*fdRef, 0, SEEK_SET);
                    ssize_t bytesRead = read(*fdRef, buffer, sizeof(buffer)-1); 
                    if (bytesRead > 0)
                        std::cout << "Read data: " << buffer << std::endl;
                    else
                        std::cout << "Read failed or empty" << std::endl;
                }
                else if(action.first == "close")
                {
                    //close(*fdRef);
                    std::cout << "File descriptor closed." << std::endl;
                    OWNERSHIP = false;
                }
            }
        }
};

int main()
{
    myClass obj;
    obj.registerAction({{"write", 1}, {"read", 2}, {"close", 3}});
    myClass copyObj(obj); 
    obj.executeActions();
    while(true);

    return 0;
}