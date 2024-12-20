#include "Scene.hpp"
#include <filesystem>

int main() {
    //std::cout << std::filesystem::current_path().parent_path().string() << std::endl;
    Scene scene(std::filesystem::current_path().parent_path().string());
    std::cout << "END!";
}