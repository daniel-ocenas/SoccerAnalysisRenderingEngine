#include "Controller.hpp"

/**
* @brief main
*/
int main(int argc, char* argv[]) {
    try 
    {
        auto app = Controller(argv);
        
        if (argc > 1 && strcmp(argv[1], "-p") == 0)
            app.runPerformanceTest(argv);
        else
            app.start();



    }
    catch (std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
