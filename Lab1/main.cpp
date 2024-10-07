#include <SFML/Graphics.hpp>

int main() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    //sf::RenderWindow window(sf::VideoMode(desktop.width, desktop.height), "Program window");
    sf::RenderWindow window(sf::VideoMode(300, 300), "Program window");
    //window.setVerticalSyncEnabled(true);
    std::vector<sf::CircleShape> points;
    sf::CircleShape small(0.f);
    sf::CircleShape& moving_point = small;
    int mov_ind = -1;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Button::Left) {
                    for (auto elem: points) {
                        if (elem.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                            moving_point = elem;
                            break;
                        } 
                    }
                } else if (event.mouseButton.button == sf::Mouse::Button::Right) {
                    sf::CircleShape point(5.f);
                    point.setFillColor(sf::Color::Green);
                    point.setPosition(event.mouseButton.x, event.mouseButton.y);
                    points.push_back(point);
                }
                if (event.mouseButton.button == sf::Mouse::Button::Middle) {
                    moving_point.setPosition(event.mouseButton.x, event.mouseButton.y);
                    moving_point = sf::CircleShape(0.f);
                }
            }
            /*if (event.type == sf::Event::MouseMoved) {
                moving_point.setPosition(event.mouseButton.x, event.mouseButton.y);
                break;
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Button::Left) {
                    moving_point.setPosition(event.mouseButton.x, event.mouseButton.y);
                    moving_point = sf::CircleShape(0.f);
                }
            }*/
        }
        window.clear();
        auto position = sf::Mouse::getPosition();
        moving_point.setPosition(position.x, position.y);
        for (auto elem: points) {
            window.draw(elem);
        }
        window.display();
    }
    return 0;
}