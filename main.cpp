#include "graphics.hpp"

int main()
{
    sf::Vector3f lightPoint(80,384,2000);
    sf::Vector3f lightColor(185,190,120);

    gp::mainWindow window(1024,768);
    window.clearWindow();
    window.setVerticalSyncEnabled(false);

    sf::Vector3f viewPortPoint(0,0,10);

    window.setViewPoint(viewPortPoint);
    window.setLookAtPoint(sf::Vector3f(0,0,0));

    window.setProjection(sf::Vector3f(512,384,2000),500);
    window.setViewVolume(sf::Vector3f(0,0,400),sf::Vector3f(1024,768,-400));
    window.calcTransformation();

    window.setAmbientLight(sf::Vector3f(255,255,255));
    window.addLightSource(lightPoint,lightColor);

    bool showShadow = false;
    bool showOther = false;

    vector <gp::object3d> objects = gp::parseObj("Objects.obj");

    for (unsigned i = 0; i<objects.size(); i++)
    {
        float x=0,y=0,z=0;
        if (objects[i].name == "Chair")
        {
            x = 542.7, y = 330, z = 1400;
        }
        else if (objects[i].name == "Room")
        {
            x = 330, y = 280, z = 1600;
        }
        else if (objects[i].name == "Lamp")
        {
            x = 330, y = 280, z = 1700;
        }
        else if (objects[i].name == "Table")
        {
            x = 500, y = 350, z = 1260;
        }
        else if (objects[i].name == "Vase")
        {
            x = 500, y = 363, z = 1260;
        }
        else if (objects[i].name == "Door")
        {
            x = 657, y = 285, z = 1420;
        }
        else if (objects[i].name == "WindowFrame")
        {
            x = 356, y = 390, z = 1337;
        }
        else if (objects[i].name == "WindowSlab")
        {
            x = 346, y = 338, z = 1337;
        }

        objects[i].setPosition(sf::Vector3f(x,y,z));
        objects[i].setScale(sf::Vector3f(100,100,100));
    }
    for (unsigned i = 0; i<objects.size(); i++)
    {
        objects[i].setWindow(&window);

//        gp::object3d::allTriangleStrips.clear();
        objects[i].mapTriangleStrip();
        objects[i].calculateintensity();
    }

    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::KeyPressed)
            {
                if(event.key.code == sf::Keyboard::P)
                    window.saveScreenshot();
                if(event.key.code == sf::Keyboard::Escape)
                    window.close();
                if(event.key.code == sf::Keyboard::W)
                    viewPortPoint.z+=0.10;
                if(event.key.code == sf::Keyboard::S)
                    viewPortPoint.z-=0.10;
                if(event.key.code == sf::Keyboard::Up)
                    viewPortPoint.y+=0.10;
                if(event.key.code == sf::Keyboard::K)
                    if (showShadow) showShadow = false; else showShadow = true;
                if(event.key.code == sf::Keyboard::L)
                    if (showShadow) showOther = false; else showOther = true;
                if(event.key.code == sf::Keyboard::Down)
                    viewPortPoint.y-=0.10;
                if(event.key.code == sf::Keyboard::Left)
                    viewPortPoint.x+=0.10;
                if(event.key.code == sf::Keyboard::Right)
                    viewPortPoint.x-=0.10;

                window.setViewPoint(viewPortPoint);
                window.calcTransformation();
            }
        }
        window.clearWindow();

        window.plotPoint(window.transformPoint(lightPoint - sf::Vector3f(1,0,0)),sf::Vector3f(255,0,0));
        window.plotPoint(window.transformPoint(lightPoint + sf::Vector3f(1,0,0)),sf::Vector3f(0,0,255));
        window.plotPoint(window.transformPoint(lightPoint - sf::Vector3f(0,1,0)),sf::Vector3f(0,255,0));
        window.plotPoint(window.transformPoint(lightPoint + sf::Vector3f(0,1,0)),sf::Vector3f(255,255,255));
        window.plotPoint(window.transformPoint(lightPoint),sf::Vector3f(0,0,0));

        for (unsigned i=0; i<objects.size(); i++)
        {
            if (objects[i].name == "Room")
                objects[i].draw();
            else if (showOther)
                objects[i].draw();
        }
        if (showShadow) window.displayShadow();
        window.updateWindow();
    }
    return 0;
}
