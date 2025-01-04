
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

int main(int argc, char* argv[]) {
    std::cout << "Application started" << std::endl;
    
    #ifdef _WIN32
    Sleep(1000); // Windows sleep in milliseconds
    #else
    sleep(1);    // Unix sleep in seconds
    #endif
    
    return 0;
}
