#include <iostream>
#include <SFML/Graphics.hpp>

int main(int argc, char** argv)
{
    sf::RenderWindow window(sf::VideoMode(640,320), "JC8E");

    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)){
            // Close window
            if (event.type == sf::Event::Closed) window.close();
        }

        // Frame pipeline
        window.clear(sf::Color::Black);
        // Draw stuff...
        window.display();
    }

    return 0;
}