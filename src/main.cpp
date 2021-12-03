#include <Application.hpp>

int main(int argc, char** argv) {
    std::string setupFile{"setup.txt"};
    if(argc >= 2) {
        setupFile = argv[1];
    }

    Application app{setupFile};
    app.run();

    return EXIT_SUCCESS;
}
