#include <SFML/Graphics.hpp>
#include <TichuLib/card.h>

int main()
{
    Tichu::Card card;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Tichu!");

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        // Do drawing here...




        window.display();
    }

    return 0;
}