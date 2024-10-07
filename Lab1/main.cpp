#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>

int main() {
    std::random_device rd; // Разные штуки для обеспечения случайного движения ломаной
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int> dis_elem(-30, 30);
    sf::Clock clock;

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode(); // создание окна
    sf::RenderWindow window(sf::VideoMode(desktop.width, desktop.height), "Program window");
    window.setVerticalSyncEnabled(true);

    std::vector<sf::CircleShape> points; // вектор вершин ломаной
    std::vector<int> edges; // вектор отрезков ломаной
    bool random_movement = false; // флаг случайного движения
    int mov_ind = -1; // индекс вершины в векторе, которую мы двигаем
    while (window.isOpen()) { // игровой цикл
        sf::Event event;
        while (window.pollEvent(event)) { // цикл событий
            if (event.type == sf::Event::Closed) { // закрытие окна
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) { // нажали клавишу мыши
                if (event.mouseButton.button == sf::Mouse::Button::Right) { // правая клавиша мыши - создаём вершину (и отрезок)
                    sf::CircleShape point(20.f);
                    point.setPointCount(100);
                    point.setFillColor(sf::Color::Green);
                    point.setOrigin(point.getRadius(), point.getRadius());
                    point.setPosition(event.mouseButton.x, event.mouseButton.y);
                    points.push_back(point);
                }
                if (event.mouseButton.button == sf::Mouse::Button::Left) { // левая клавиша мыши - "захватываем" вершину для движения
                    for (int i=0; i<points.size(); ++i) {
                        if (points[i].getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                            mov_ind = i;
                            break;
                        } 
                    }
                }
                if (event.mouseButton.button == sf::Mouse::Button::Middle) { // средняя клавиша мыши - удаляем вершину
                    for (auto it=points.begin(); it!=points.end(); ++it) {
                        if (it->getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                            points.erase(it);
                            break;
                        } 
                    }
                }
            }
            if (event.type == sf::Event::MouseMoved and mov_ind != -1) { // двигается мышь -> если "захвачена" вершина, двигаем и её
                points[mov_ind].setPosition(event.mouseMove.x, event.mouseMove.y);
            }
            if (event.type == sf::Event::MouseButtonReleased and event.mouseButton.button == sf::Mouse::Button::Left) { // отпускаем левую клавишу мыши - а с ней и вершину
                mov_ind = -1;
            } 
            if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Space) { // при нажатии пробела включаем случайное движение ломаной
                random_movement = true;
            }
            if (event.type == sf::Event::KeyReleased and event.key.code == sf::Keyboard::Space) { // при отпускании пробела отключаем случайное движение ломаной
                random_movement = false;
            }
        }
        window.clear(); // очищаем экран
        if (random_movement and (clock.getElapsedTime().asMilliseconds() >= 200)) { // цикл случайного движения, в нём делаем чтобы вершину не выходили за пределы экрана
            for (int i=0; i<points.size(); ++i) {
                int new_x = (static_cast<int>(points[i].getPosition().x) + dis_elem(gen)) % desktop.width;
                int new_y = (static_cast<int>(points[i].getPosition().x) + dis_elem(gen)) % desktop.height;
                points[i].setPosition(new_x, new_y);
            }
            clock.restart();
        }
        for (int i=0; i<points.size(); ++i) { // отрисовываем всё на экране
            window.draw(points[i]);
        }
        window.display(); // выводим изображение
    }
    return 0;
}