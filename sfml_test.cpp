#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace std;

struct Vector3 {
    double x, y, z;
};

struct Body {
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    double mass;
};

int main()
{
    const double PI = 3.141592653589793;
    const double G = 4.0 * PI * PI;
    const double dt = 1.0 / 730.5;

    sf::RenderWindow window(
        sf::VideoMode(800, 600),
        "N-body Simulation"
    );

    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

    sf::View view = window.getDefaultView();

    // FIZIK MOTORU
    Body sun{};
    Body earth{};

    sun.mass = 1.0;
    earth.mass = 3.003e-6;

    sun.position = {0.0, 0.0, 0.0};
    sun.velocity = {0.0, 0.0, 0.0};

    earth.position = {1.0, 0.0, 0.0};
    earth.velocity = {0.0, 2.0 * PI, 0.0};

    // Ekran koordinatlari
    const float centerX = 400.f;
    const float centerY = 300.f;
    const float scale = 180.f;

    auto toScreen = [&](const Vector3& position)
    {
        return sf::Vector2f(
            centerX + static_cast<float>(position.x) * scale,
            centerY - static_cast<float>(position.y) * scale
        );
    };

    // YILDIZLAR
    vector<sf::CircleShape> stars;

    mt19937 generator(42);

    uniform_real_distribution<float> xRandom(0.f, 800.f);
    uniform_real_distribution<float> yRandom(0.f, 600.f);
    uniform_real_distribution<float> sizeRandom(0.5f, 1.8f);

    for (int i = 0; i < 150; i++)
    {
        float size = sizeRandom(generator);

        sf::CircleShape star(size);

        star.setPosition(
            xRandom(generator),
            yRandom(generator)
        );

        star.setFillColor(sf::Color(180, 190, 220));

        stars.push_back(star);
    }

    // GUNES
    sf::CircleShape sunShape(45.f);

    sunShape.setOrigin(45.f, 45.f);
    sunShape.setPosition(centerX, centerY);
    sunShape.setFillColor(sf::Color(255, 140, 0));
    sunShape.setOutlineThickness(3.f);
    sunShape.setOutlineColor(sf::Color(255, 70, 0));

    sf::CircleShape outerGlow(68.f);

    outerGlow.setOrigin(68.f, 68.f);
    outerGlow.setPosition(centerX, centerY);
    outerGlow.setFillColor(sf::Color(255, 100, 0, 25));

    sf::CircleShape glow(55.f);

    glow.setOrigin(55.f, 55.f);
    glow.setPosition(centerX, centerY);
    glow.setFillColor(sf::Color(255, 170, 0, 50));

    sf::CircleShape sunSpot1(9.f);

    sunSpot1.setOrigin(9.f, 9.f);
    sunSpot1.setPosition(385.f, 283.f);
    sunSpot1.setFillColor(sf::Color(255, 195, 40, 180));

    sf::CircleShape sunSpot2(6.f);

    sunSpot2.setOrigin(6.f, 6.f);
    sunSpot2.setPosition(416.f, 310.f);
    sunSpot2.setFillColor(sf::Color(255, 210, 50, 180));

    // DUNYA'NIN YORUNGESI
    sf::CircleShape orbit(180.f);

    orbit.setOrigin(180.f, 180.f);
    orbit.setPosition(centerX, centerY);
    orbit.setFillColor(sf::Color::Transparent);
    orbit.setOutlineThickness(1.f);
    orbit.setOutlineColor(sf::Color(80, 80, 110));

    // DUNYA
    sf::CircleShape earthShape(8.f);

    earthShape.setOrigin(8.f, 8.f);
    earthShape.setFillColor(sf::Color(50, 140, 255));

    // YORUNGE IZI
    vector<sf::Vector2f> trailPoints;

    trailPoints.push_back(toScreen(earth.position));

    // SIMULASYON AYARLARI
    double simulationSpeed = 1.0;
    double accumulator = 0.0;
    double simulationTime = 0.0;

    bool paused = false;

    sf::Clock clock;

    // YAZI TIPINI YUKLE
    // Ubuntu'da yaygin font konumlarini dene.
    sf::Font font;

    bool fontLoaded =
        font.loadFromFile(
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
        );

    if (!fontLoaded)
    {
        fontLoaded =
            font.loadFromFile(
                "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf"
            );
    }

    if (!fontLoaded)
    {
        fontLoaded =
            font.loadFromFile(
                "/usr/share/fonts/truetype/freefont/FreeSans.ttf"
            );
    }

    // KONTROL PANELI ARKA PLANI
    sf::RectangleShape controlsBackground(
        sf::Vector2f(250.f, 145.f)
    );

    controlsBackground.setPosition(12.f, 12.f);
    controlsBackground.setFillColor(sf::Color(5, 10, 25, 220));
    controlsBackground.setOutlineThickness(1.f);
    controlsBackground.setOutlineColor(sf::Color(90, 110, 150));

    // KONTROL YAZILARI
    sf::Text controlsText;

    if (fontLoaded)
    {
        controlsText.setFont(font);

        controlsText.setString(
            "SIMULATION CONTROLS\n"
            "SPACE  - Pause / Resume\n"
            "UP       - Increase speed\n"
            "DOWN - Decrease speed\n"
            "R          - Reset simulation\n"
            "Mouse wheel - Zoom"
        );

        controlsText.setCharacterSize(13);
        controlsText.setFillColor(sf::Color(230, 235, 255));
        controlsText.setPosition(22.f, 20.f);
    }

    // ALT DURUM PANELI
    sf::RectangleShape statusBackground(
        sf::Vector2f(430.f, 30.f)
    );

    statusBackground.setPosition(12.f, 555.f);
    statusBackground.setFillColor(sf::Color(5, 10, 25, 220));
    statusBackground.setOutlineThickness(1.f);
    statusBackground.setOutlineColor(sf::Color(90, 110, 150));

    sf::Text statusText;

    if (fontLoaded)
    {
        statusText.setFont(font);
        statusText.setCharacterSize(13);
        statusText.setFillColor(sf::Color(230, 235, 255));
        statusText.setPosition(20.f, 560.f);
    }
    else
    {
        cerr << "Warning: Could not load a font. "
             << "On-screen text will not be displayed." << endl;
    }

    // ANA DONGU
    while (window.isOpen())
    {
        float elapsed = min(
            clock.restart().asSeconds(),
            0.1f
        );

        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            // KLAVYE KONTROLLERI
            if (event.type == sf::Event::KeyPressed)
            {
                // SPACE: DURAKLAT / DEVAM ET
                if (event.key.code == sf::Keyboard::Space)
                {
                    paused = !paused;
                }

                // UP: HIZLANDIR
                if (event.key.code == sf::Keyboard::Up)
                {
                    simulationSpeed *= 1.5;

                    if (simulationSpeed > 20.0)
                    {
                        simulationSpeed = 20.0;
                    }
                }

                // DOWN: YAVASLAT
                if (event.key.code == sf::Keyboard::Down)
                {
                    simulationSpeed /= 1.5;

                    if (simulationSpeed < 0.1)
                    {
                        simulationSpeed = 0.1;
                    }
                }

                // R: SIFIRLA
                if (event.key.code == sf::Keyboard::R)
                {
                    earth.position = {1.0, 0.0, 0.0};

                    earth.velocity = {
                        0.0, 2.0 * PI, 0.0
                    };

                    earth.acceleration = {0.0, 0.0, 0.0};

                    simulationTime = 0.0;
                    accumulator = 0.0;
                    simulationSpeed = 1.0;
                    paused = false;

                    trailPoints.clear();

                    trailPoints.push_back(
                        toScreen(earth.position)
                    );

                    view = window.getDefaultView();
                    window.setView(view);
                }
            }

            // FARE TEKERLEGI: YAKINLASTIR / UZAKLASTIR
            if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (event.mouseWheelScroll.delta > 0)
                {
                    view.zoom(0.9f);
                }
                else
                {
                    view.zoom(1.1f);
                }

                window.setView(view);
            }
        }

        // GERCEK FIZIK HESAPLAMASI
        if (!paused)
        {
            accumulator += elapsed * 0.1 * simulationSpeed;

            while (accumulator >= dt)
            {
                double dx =
                    sun.position.x - earth.position.x;

                double dy =
                    sun.position.y - earth.position.y;

                double dz =
                    sun.position.z - earth.position.z;

                double r = sqrt(
                    dx * dx +
                    dy * dy +
                    dz * dz
                );

                if (r < 1e-12)
                {
                    paused = true;
                    break;
                }

                earth.acceleration.x =
                    G * sun.mass * dx / (r * r * r);

                earth.acceleration.y =
                    G * sun.mass * dy / (r * r * r);

                earth.acceleration.z =
                    G * sun.mass * dz / (r * r * r);

                earth.velocity.x +=
                    earth.acceleration.x * dt;

                earth.velocity.y +=
                    earth.acceleration.y * dt;

                earth.velocity.z +=
                    earth.acceleration.z * dt;

                earth.position.x +=
                    earth.velocity.x * dt;

                earth.position.y +=
                    earth.velocity.y * dt;

                earth.position.z +=
                    earth.velocity.z * dt;

                simulationTime += dt;
                accumulator -= dt;

                trailPoints.push_back(
                    toScreen(earth.position)
                );

                if (trailPoints.size() > 6000)
                {
                    trailPoints.erase(
                        trailPoints.begin(),
                        trailPoints.begin() + 1000
                    );
                }
            }
        }

        // FIZIKTEN GELEN KONUMU CIZ
        earthShape.setPosition(
            toScreen(earth.position)
        );

        // YORUNGE IZINI HAZIRLA
        sf::VertexArray trail(sf::LineStrip);

        for (const auto& point : trailPoints)
        {
            trail.append(
                sf::Vertex(
                    point,
                    sf::Color(80, 170, 255)
                )
            );
        }

        // DURUM YAZISINI GUNCELLE
        if (fontLoaded)
        {
            ostringstream status;

            status << fixed << setprecision(2)
                   << (paused ? "PAUSED" : "RUNNING")
                   << " | Speed: " << simulationSpeed << "x"
                   << " | Time: " << simulationTime << " years";

            statusText.setString(status.str());
        }

        // EKRANI TEMIZLE
        window.clear(sf::Color(5, 5, 20));

        // YILDIZLAR
        for (const auto& star : stars)
        {
            window.draw(star);
        }

        // YORUNGE VE IZ
        window.draw(orbit);
        window.draw(trail);

        // GUNES
        window.draw(outerGlow);
        window.draw(glow);
        window.draw(sunShape);
        window.draw(sunSpot1);
        window.draw(sunSpot2);

        // DUNYA
        window.draw(earthShape);

        // KONTROL PANELINI SABIT EKRAN KOORDINATLARINDA CIZ
        window.setView(window.getDefaultView());

        if (fontLoaded)
        {
            window.draw(controlsBackground);
            window.draw(controlsText);

            window.draw(statusBackground);
            window.draw(statusText);
        }

        // Kamera görünümünü bir sonraki kare icin geri yukle
        window.setView(view);

        window.display();
    }

    return 0;
}