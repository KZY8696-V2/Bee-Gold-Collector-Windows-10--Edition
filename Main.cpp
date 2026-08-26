#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <cmath>
#include <optional>
#include <algorithm>
#include <cstdint>

enum Language { EN, TR };
Language currentLang = EN;

std::string getLocalizedText(std::string key, long long val1 = 0, long long val2 = 0, long long val3 = 0) {
    if (currentLang == EN) {
        if (key == "title") return "BEE GOLD ADVENTURE";
        if (key == "start") return "START GAME";
        if (key == "lang") return "Language: English";
        if (key == "exit") return "EXIT";
        if (key == "total") return "Total Score: " + std::to_string(val1);
        if (key == "high") return "High Score: " + std::to_string(val1);
        if (key == "hud") return "Score: " + std::to_string(val1) + " | Lives: " + std::to_string(val2) + " | Bonus in: " + std::to_string(val3) + "s";
        if (key == "gameover") return "GAME OVER";
        if (key == "restart") return "Press [SPACE] to return to Menu";
        if (key == "pause_title") return "GAME PAUSED";
        if (key == "pause_msg") return "Return to Main Menu?";
        if (key == "yes") return "YES [Y]";
        if (key == "no") return "NO [N]";
    }
    else {
        if (key == "title") return "BEE GOLD ADVENTURE";
        if (key == "start") return "OYUNA BASLA";
        if (key == "lang") return "Dil: Turkce";
        if (key == "exit") return "CIKIS";
        if (key == "total") return "Toplam Skor: " + std::to_string(val1);
        if (key == "high") return "En Yuksek Skor: " + std::to_string(val1);
        if (key == "hud") return "Skor: " + std::to_string(val1) + " | Can: " + std::to_string(val2) + " | Can Suresi: " + std::to_string(val3) + "s";
        if (key == "gameover") return "OYUN BITTI";
        if (key == "restart") return "Menuye donmek icin [SPACE] tusuna basin";
        if (key == "pause_title") return "OYUN DURAKLATILDI";
        if (key == "pause_msg") return "Ana Menuye Donulsun mu?";
        if (key == "yes") return "EVET [Y]";
        if (key == "no") return "HAYIR [N]";
    }
    return "";
}

std::string encryptDecrypt(const std::string& toEncrypt) {
    std::string output = toEncrypt;
    char key = 'K';
    for (size_t i = 0; i < toEncrypt.size(); i++) {
        output[i] = toEncrypt[i] ^ key;
    }
    return output;
}

struct ScoreData {
    long long totalScore = 0;
    int highScore = 0;
};

ScoreData loadScores() {
    ScoreData data;
    std::ifstream file("C:\\ProgramData\\BeeGoldCollector\\scores.dat");
    if (file.is_open()) {
        std::string line;
        std::getline(file, line);
        std::string decrypted = encryptDecrypt(line);
        std::stringstream ss(decrypted);
        ss >> data.totalScore >> data.highScore;
        file.close();
    }
    return data;
}

void saveScores(long long total, int high) {
    system("mkdir C:\\ProgramData\\BeeGoldCollector >nul 2>&1");
    std::ofstream file("C:\\ProgramData\\BeeGoldCollector\\scores.dat");
    if (file.is_open()) {
        std::stringstream ss;
        ss << total << " " << high;
        std::string encrypted = encryptDecrypt(ss.str());
        file << encrypted;
        file.close();
    }
}

enum GameState { MENU, PLAYING, PAUSED, GAMEOVER };

bool isButtonClicked(const sf::RectangleShape& button, const sf::RenderWindow& window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    return button.getGlobalBounds().contains(mousePosF);
}

struct SceneryObject {
    sf::Vector2f pos;
    int type; // 0: Çam, 1: Meşe, 2: Kavak
    float scale;
};

struct CoinObject {
    sf::Vector2f pos;
    float scale;
};

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    sf::RenderWindow window(sf::VideoMode({ 1000, 600 }), "Bee Gold Adventure");
    window.setFramerateLimit(60);

    GameState state = MENU;
    ScoreData scores = loadScores();
    int currentScore = 0;
    int lives = 3;
    float healthTimer = 0.0f;
    int menuIndex = 0;
    int pauseMenuIndex = 0;
    float beeFacingDir = 1.0f;
    float wingAnimTimer = 0.0f;

    sf::RectangleShape startButton(sf::Vector2f(280.f, 52.f));
    startButton.setFillColor(sf::Color(46, 204, 113));
    startButton.setOutlineThickness(3.f);
    startButton.setOutlineColor(sf::Color(30, 130, 76));
    startButton.setOrigin({ 140.f, 26.f });
    startButton.setPosition({ 500.f, 230.f });

    sf::RectangleShape startShadow(sf::Vector2f(280.f, 52.f));
    startShadow.setFillColor(sf::Color(0, 0, 0, 70));
    startShadow.setOrigin({ 140.f, 26.f });
    startShadow.setPosition({ 504.f, 234.f });

    sf::RectangleShape langButton(sf::Vector2f(280.f, 52.f));
    langButton.setFillColor(sf::Color(52, 152, 219));
    langButton.setOutlineThickness(3.f);
    langButton.setOutlineColor(sf::Color(31, 97, 141));
    langButton.setOrigin({ 140.f, 26.f });
    langButton.setPosition({ 500.f, 305.f });

    sf::RectangleShape langShadow(sf::Vector2f(280.f, 52.f));
    langShadow.setFillColor(sf::Color(0, 0, 0, 70));
    langShadow.setOrigin({ 140.f, 26.f });
    langShadow.setPosition({ 504.f, 309.f });

    sf::RectangleShape exitButton(sf::Vector2f(280.f, 52.f));
    exitButton.setFillColor(sf::Color(231, 76, 60));
    exitButton.setOutlineThickness(3.f);
    exitButton.setOutlineColor(sf::Color(146, 43, 33));
    exitButton.setOrigin({ 140.f, 26.f });
    exitButton.setPosition({ 500.f, 380.f });

    sf::RectangleShape exitButtonShadow(sf::Vector2f(280.f, 52.f));
    exitButtonShadow.setFillColor(sf::Color(0, 0, 0, 70));
    exitButtonShadow.setOrigin({ 140.f, 26.f });
    exitButtonShadow.setPosition({ 504.f, 384.f });

    sf::RectangleShape inGamePauseButton(sf::Vector2f(90.f, 36.f));
    inGamePauseButton.setFillColor(sf::Color(241, 196, 15));
    inGamePauseButton.setOutlineThickness(2.f);
    inGamePauseButton.setOutlineColor(sf::Color(125, 102, 8));
    inGamePauseButton.setPosition({ 890.f, 20.f });

    sf::RectangleShape pauseYesButton(sf::Vector2f(200.f, 45.f));
    pauseYesButton.setFillColor(sf::Color(46, 204, 113));
    pauseYesButton.setOutlineThickness(3.f);
    pauseYesButton.setOutlineColor(sf::Color(30, 130, 76));
    pauseYesButton.setOrigin({ 100.f, 22.5f });
    pauseYesButton.setPosition({ 500.f, 280.f });

    sf::RectangleShape pauseNoButton(sf::Vector2f(200.f, 45.f));
    pauseNoButton.setFillColor(sf::Color(231, 76, 60));
    pauseNoButton.setOutlineThickness(3.f);
    pauseNoButton.setOutlineColor(sf::Color(146, 43, 33));
    pauseNoButton.setOrigin({ 100.f, 22.5f });
    pauseNoButton.setPosition({ 500.f, 345.f });

    sf::ConvexShape beeBody;
    beeBody.setPointCount(16);
    for (int i = 0; i < 16; ++i) {
        float angle = i * 6.2831853f / 16.f;
        float x = std::cos(angle) * 24.f;
        float y = std::sin(angle) * 18.f;
        beeBody.setPoint(i, sf::Vector2f(x, y));
    }
    beeBody.setFillColor(sf::Color(255, 221, 51));
    beeBody.setOutlineThickness(2.f);
    beeBody.setOutlineColor(sf::Color(90, 70, 10));
    beeBody.setPosition({ 150.f, 300.f });

    sf::RectangleShape beeStripe1(sf::Vector2f(6.f, 32.f));
    beeStripe1.setFillColor(sf::Color(60, 40, 20));
    beeStripe1.setOrigin({ 3.f, 16.f });

    sf::RectangleShape beeStripe2(sf::Vector2f(6.f, 28.f));
    beeStripe2.setFillColor(sf::Color(60, 40, 20));
    beeStripe2.setOrigin({ 3.f, 14.f });

    sf::ConvexShape beeStinger;
    beeStinger.setPointCount(3);
    beeStinger.setPoint(0, sf::Vector2f(0.f, -4.f));
    beeStinger.setPoint(1, sf::Vector2f(-10.f, 0.f));
    beeStinger.setPoint(2, sf::Vector2f(0.f, 4.f));
    beeStinger.setFillColor(sf::Color(60, 40, 20));

    sf::CircleShape beeEye(7.f);
    beeEye.setFillColor(sf::Color::White);
    beeEye.setOutlineThickness(1.5f);
    beeEye.setOutlineColor(sf::Color(60, 40, 20));
    beeEye.setOrigin({ 7.f, 7.f });

    sf::CircleShape beePupil(3.5f);
    beePupil.setFillColor(sf::Color(30, 30, 30));
    beePupil.setOrigin({ 3.5f, 3.5f });

    sf::CircleShape beeEyeHighlight(1.5f);
    beeEyeHighlight.setFillColor(sf::Color::White);
    beeEyeHighlight.setOrigin({ 1.5f, 1.5f });

    sf::VertexArray beeAntenna1(sf::PrimitiveType::Lines, 2);
    beeAntenna1[0].color = sf::Color(60, 40, 20);
    beeAntenna1[1].color = sf::Color(60, 40, 20);

    sf::VertexArray beeAntenna2(sf::PrimitiveType::Lines, 2);
    beeAntenna2[0].color = sf::Color(60, 40, 20);
    beeAntenna2[1].color = sf::Color(60, 40, 20);

    sf::CircleShape beeAntennaTip1(2.f);
    beeAntennaTip1.setFillColor(sf::Color(60, 40, 20));
    beeAntennaTip1.setOrigin({ 2.f, 2.f });

    sf::CircleShape beeAntennaTip2(2.f);
    beeAntennaTip2.setFillColor(sf::Color(60, 40, 20));
    beeAntennaTip2.setOrigin({ 2.f, 2.f });

    sf::VertexArray beeLeg1(sf::PrimitiveType::Lines, 2);
    beeLeg1[0].color = sf::Color(60, 40, 20);
    beeLeg1[1].color = sf::Color(60, 40, 20);

    sf::VertexArray beeLeg2(sf::PrimitiveType::Lines, 2);
    beeLeg2[0].color = sf::Color(60, 40, 20);
    beeLeg2[1].color = sf::Color(60, 40, 20);

    sf::ConvexShape beeWing;
    beeWing.setPointCount(4);
    beeWing.setPoint(0, sf::Vector2f(0.f, 0.f));
    beeWing.setPoint(1, sf::Vector2f(-8.f, -32.f));
    beeWing.setPoint(2, sf::Vector2f(12.f, -38.f));
    beeWing.setPoint(3, sf::Vector2f(22.f, -14.f));
    beeWing.setFillColor(sf::Color(240, 248, 255, 220));
    beeWing.setOutlineThickness(1.5f);
    beeWing.setOutlineColor(sf::Color(130, 165, 200));

    std::vector<sf::ConvexShape> bugs;
    std::vector<CoinObject> coins;
    std::vector<SceneryObject> sceneries;

    float spawnTimer = 0.0f;
    float sceneryTimer = 0.0f;

    sf::Font font;
    if (!font.openFromFile("C:\\Windows\\Fonts\\arial.ttf")) {}

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Time elapsed = clock.restart();
        float dt = elapsed.asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (state == MENU) {
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->code == sf::Keyboard::Key::Up || keyPressed->code == sf::Keyboard::Key::W) {
                        menuIndex--;
                        if (menuIndex < 0) menuIndex = 2;
                    }
                    else if (keyPressed->code == sf::Keyboard::Key::Down || keyPressed->code == sf::Keyboard::Key::S) {
                        menuIndex++;
                        if (menuIndex > 2) menuIndex = 0;
                    }
                    else if (keyPressed->code == sf::Keyboard::Key::Enter) {
                        if (menuIndex == 0) {
                            state = PLAYING;
                            currentScore = 0;
                            lives = 3;
                            healthTimer = 0.0f;
                            bugs.clear();
                            coins.clear();
                            sceneries.clear();
                        }
                        else if (menuIndex == 1) {
                            currentLang = (currentLang == EN) ? TR : EN;
                        }
                        else if (menuIndex == 2) {
                            window.close();
                        }
                    }
                }
                else if (event->is<sf::Event::MouseMoved>()) {
                    if (isButtonClicked(startButton, window)) menuIndex = 0;
                    else if (isButtonClicked(langButton, window)) menuIndex = 1;
                    else if (isButtonClicked(exitButton, window)) menuIndex = 2;
                }
                else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                        if (isButtonClicked(startButton, window)) {
                            state = PLAYING;
                            currentScore = 0;
                            lives = 3;
                            healthTimer = 0.0f;
                            bugs.clear();
                            coins.clear();
                            sceneries.clear();
                        }
                        else if (isButtonClicked(langButton, window)) {
                            currentLang = (currentLang == EN) ? TR : EN;
                        }
                        else if (isButtonClicked(exitButton, window)) {
                            window.close();
                        }
                    }
                }
            }
            else if (state == PLAYING) {
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->code == sf::Keyboard::Key::Escape) {
                        state = PAUSED;
                        pauseMenuIndex = 0;
                    }
                }
                else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                        if (inGamePauseButton.getGlobalBounds().contains(mousePosF)) {
                            state = PAUSED;
                            pauseMenuIndex = 0;
                        }
                    }
                }
            }
            else if (state == PAUSED) {
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->code == sf::Keyboard::Key::Up || keyPressed->code == sf::Keyboard::Key::W ||
                        keyPressed->code == sf::Keyboard::Key::Down || keyPressed->code == sf::Keyboard::Key::S) {
                        pauseMenuIndex = 1 - pauseMenuIndex;
                    }
                    else if (keyPressed->code == sf::Keyboard::Key::Enter) {
                        if (pauseMenuIndex == 0) {
                            state = MENU;
                            scores = loadScores();
                        }
                        else {
                            state = PLAYING;
                        }
                    }
                    else if (keyPressed->code == sf::Keyboard::Key::Escape) {
                        state = PLAYING;
                    }
                }
                else if (event->is<sf::Event::MouseMoved>()) {
                    if (isButtonClicked(pauseYesButton, window)) pauseMenuIndex = 0;
                    else if (isButtonClicked(pauseNoButton, window)) pauseMenuIndex = 1;
                }
                else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                        if (isButtonClicked(pauseYesButton, window)) {
                            state = MENU;
                            scores = loadScores();
                        }
                        else if (isButtonClicked(pauseNoButton, window)) {
                            state = PLAYING;
                        }
                    }
                }
            }
            else if (state == GAMEOVER) {
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->code == sf::Keyboard::Key::Space) {
                        state = MENU;
                        scores = loadScores();
                    }
                }
            }
        }

        if (state == PLAYING) {
            healthTimer += dt;
            wingAnimTimer += dt * 25.0f;
            if (healthTimer >= 25.0f) {
                lives += 2;
                healthTimer = 0.0f;
            }

            float beeSpeed = 520.f;
            sf::Vector2f movement(0.f, 0.f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) movement.y -= beeSpeed * dt;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) movement.y += beeSpeed * dt;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
                movement.x -= beeSpeed * dt;
                beeFacingDir = -1.0f;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
                movement.x += beeSpeed * dt;
                beeFacingDir = 1.0f;
            }

            beeBody.move(movement);

            sf::Vector2f pos = beeBody.getPosition();
            if (pos.x < 40.f) beeBody.setPosition({ 40.f, pos.y });
            if (pos.x > 960.f) beeBody.setPosition({ 960.f, pos.y });
            if (pos.y < 40.f) beeBody.setPosition({ pos.x, 40.f });
            if (pos.y > 500.f) beeBody.setPosition({ pos.x, 500.f });

            sf::Vector2f curPos = beeBody.getPosition();

            beeStripe1.setPosition({ curPos.x - (6.f * beeFacingDir), curPos.y });
            beeStripe2.setPosition({ curPos.x + (10.f * beeFacingDir), curPos.y });

            beeStinger.setPosition({ curPos.x + (-24.f * beeFacingDir), curPos.y });
            beeStinger.setScale({ beeFacingDir, 1.f });

            beeEye.setPosition({ curPos.x + (12.f * beeFacingDir), curPos.y - 4.f });
            beePupil.setPosition({ curPos.x + (13.f * beeFacingDir), curPos.y - 4.f });
            beeEyeHighlight.setPosition({ curPos.x + (11.5f * beeFacingDir), curPos.y - 5.5f });

            beeAntenna1[0].position = sf::Vector2f(curPos.x + (8.f * beeFacingDir), curPos.y - 14.f);
            beeAntenna1[1].position = sf::Vector2f(curPos.x + (16.f * beeFacingDir), curPos.y - 23.f);
            beeAntennaTip1.setPosition(beeAntenna1[1].position);

            beeAntenna2[0].position = sf::Vector2f(curPos.x + (4.f * beeFacingDir), curPos.y - 15.f);
            beeAntenna2[1].position = sf::Vector2f(curPos.x + (10.f * beeFacingDir), curPos.y - 25.f);
            beeAntennaTip2.setPosition(beeAntenna2[1].position);

            beeLeg1[0].position = sf::Vector2f(curPos.x - 6.f * beeFacingDir, curPos.y + 16.f);
            beeLeg1[1].position = sf::Vector2f(curPos.x - 10.f * beeFacingDir, curPos.y + 26.f);
            beeLeg2[0].position = sf::Vector2f(curPos.x + 6.f * beeFacingDir, curPos.y + 16.f);
            beeLeg2[1].position = sf::Vector2f(curPos.x + 10.f * beeFacingDir, curPos.y + 26.f);

            beeWing.setPosition({ curPos.x, curPos.y - 12.f });
            beeWing.setScale({ beeFacingDir, 1.0f + 0.35f * std::sin(wingAnimTimer) });

            sceneryTimer += dt;
            if (sceneryTimer > 1.2f) {
                SceneryObject obj;
                obj.pos = sf::Vector2f(1100.f, 540.f);
                obj.type = std::rand() % 3;
                obj.scale = static_cast<float>(std::rand() % 40 + 80) / 100.f;
                sceneries.push_back(obj);
                sceneryTimer = 0.0f;
            }

            spawnTimer += dt;
            if (spawnTimer > 0.5f) {
                sf::ConvexShape bug;
                bug.setPointCount(12);
                for (int i = 0; i < 12; ++i) {
                    float angle = i * 6.2831853f / 12.f;
                    float x = std::cos(angle) * 16.f;
                    float y = std::sin(angle) * 11.f;
                    bug.setPoint(i, sf::Vector2f(x, y));
                }
                bug.setFillColor(sf::Color(40, 30, 30));
                bug.setOutlineThickness(1.5f);
                bug.setOutlineColor(sf::Color::Black);
                bug.setPosition({ 1050.f, static_cast<float>(std::rand() % 420 + 50) });
                bugs.push_back(bug);

                CoinObject coin;
                coin.pos = sf::Vector2f(1050.f, static_cast<float>(std::rand() % 420 + 70));
                coin.scale = static_cast<float>(std::rand() % 40 + 80) / 100.f;
                coins.push_back(coin);

                spawnTimer = 0.0f;
            }

            for (auto& s : sceneries) s.pos.x -= 200.f * dt;
            for (auto& bug : bugs) bug.move({ -320.f * dt, 0.f });
            for (auto& coin : coins) coin.pos.x -= 250.f * dt;

            sf::FloatRect beeBounds = beeBody.getGlobalBounds();
            for (auto it = bugs.begin(); it != bugs.end();) {
                if (beeBounds.findIntersection(it->getGlobalBounds()).has_value()) {
                    lives--;
                    it = bugs.erase(it);
                    if (lives <= 0) {
                        state = GAMEOVER;
                        scores.totalScore += currentScore;
                        if (currentScore > scores.highScore) scores.highScore = currentScore;
                        saveScores(scores.totalScore, scores.highScore);
                    }
                }
                else {
                    ++it;
                }
            }

            for (auto it = coins.begin(); it != coins.end();) {
                sf::FloatRect coinBox({ it->pos.x - 16.f * it->scale, it->pos.y - 16.f * it->scale }, { 32.f * it->scale, 32.f * it->scale });
                if (beeBounds.findIntersection(coinBox).has_value()) {
                    currentScore += 15;
                    it = coins.erase(it);
                }
                else {
                    ++it;
                }
            }
        }

        window.clear(sf::Color(135, 206, 250));

        if (state == MENU) {
            startButton.setOutlineColor(menuIndex == 0 ? sf::Color::Yellow : sf::Color(30, 130, 76));
            langButton.setOutlineColor(menuIndex == 1 ? sf::Color::Yellow : sf::Color(31, 97, 141));
            exitButton.setOutlineColor(menuIndex == 2 ? sf::Color::Yellow : sf::Color(146, 43, 33));

            sf::Text title(font, getLocalizedText("title"), 38);
            title.setFillColor(sf::Color::Black);
            title.setOrigin({ title.getLocalBounds().size.x / 2.f, title.getLocalBounds().size.y / 2.f });
            title.setPosition({ 500.f, 90.f });

            sf::Text startText(font, getLocalizedText("start"), 18);
            startText.setFillColor(sf::Color::White);
            startText.setOrigin({ startText.getLocalBounds().size.x / 2.f, startText.getLocalBounds().size.y / 2.f });
            startText.setPosition(startButton.getPosition());

            sf::Text langText(font, getLocalizedText("lang"), 18);
            langText.setFillColor(sf::Color::White);
            langText.setOrigin({ langText.getLocalBounds().size.x / 2.f, langText.getLocalBounds().size.y / 2.f });
            langText.setPosition(langButton.getPosition());

            sf::Text exitText(font, getLocalizedText("exit"), 18);
            exitText.setFillColor(sf::Color::White);
            exitText.setOrigin({ exitText.getLocalBounds().size.x / 2.f, exitText.getLocalBounds().size.y / 2.f });
            exitText.setPosition(exitButton.getPosition());

            std::stringstream ss;
            ss << getLocalizedText("total", scores.totalScore) << "    |    " << getLocalizedText("high", scores.highScore);
            sf::Text scoreBoard(font, ss.str(), 16);
            scoreBoard.setFillColor(sf::Color::Black);
            scoreBoard.setOrigin({ scoreBoard.getLocalBounds().size.x / 2.f, scoreBoard.getLocalBounds().size.y / 2.f });
            scoreBoard.setPosition({ 500.f, 480.f });

            window.draw(title);
            window.draw(startShadow);
            window.draw(startButton);
            window.draw(startText);
            window.draw(langShadow);
            window.draw(langButton);
            window.draw(langText);
            window.draw(exitButtonShadow);
            window.draw(exitButton);
            window.draw(exitText);
            window.draw(scoreBoard);

        }
        else if (state == PLAYING || state == PAUSED) {
            sf::RectangleShape ground(sf::Vector2f(1000.f, 60.f));
            ground.setFillColor(sf::Color(76, 175, 80));
            ground.setPosition({ 0.f, 540.f });
            window.draw(ground);

            sf::RectangleShape grassStripe(sf::Vector2f(1000.f, 10.f));
            grassStripe.setFillColor(sf::Color(129, 199, 132));
            grassStripe.setPosition({ 0.f, 540.f });
            window.draw(grassStripe);

            for (const auto& s : sceneries) {
                if (s.type == 0) {
                    sf::RectangleShape trunk(sf::Vector2f(10.f * s.scale, 70.f * s.scale));
                    trunk.setFillColor(sf::Color(90, 50, 20));
                    trunk.setOrigin({ 5.f * s.scale, 70.f * s.scale });
                    trunk.setPosition(s.pos);
                    window.draw(trunk);

                    sf::ConvexShape leaves1;
                    leaves1.setPointCount(3);
                    leaves1.setPoint(0, sf::Vector2f(0.f, -50.f * s.scale));
                    leaves1.setPoint(1, sf::Vector2f(-30.f * s.scale, 0.f));
                    leaves1.setPoint(2, sf::Vector2f(30.f * s.scale, 0.f));
                    leaves1.setFillColor(sf::Color(34, 115, 34));
                    leaves1.setPosition({ s.pos.x, s.pos.y - 60.f * s.scale });
                    window.draw(leaves1);
                }
                else if (s.type == 1) {
                    sf::RectangleShape trunk(sf::Vector2f(12.f * s.scale, 60.f * s.scale));
                    trunk.setFillColor(sf::Color(100, 60, 30));
                    trunk.setOrigin({ 6.f * s.scale, 60.f * s.scale });
                    trunk.setPosition(s.pos);
                    window.draw(trunk);

                    sf::CircleShape crown(30.f * s.scale);
                    crown.setFillColor(sf::Color(60, 140, 60));
                    crown.setOrigin({ 30.f * s.scale, 30.f * s.scale });
                    crown.setPosition({ s.pos.x, s.pos.y - 70.f * s.scale });
                    window.draw(crown);
                }
                else {
                    sf::RectangleShape trunk(sf::Vector2f(8.f * s.scale, 90.f * s.scale));
                    trunk.setFillColor(sf::Color(130, 130, 110));
                    trunk.setOrigin({ 4.f * s.scale, 90.f * s.scale });
                    trunk.setPosition(s.pos);
                    window.draw(trunk);

                    sf::ConvexShape leaves;
                    leaves.setPointCount(5);
                    leaves.setPoint(0, sf::Vector2f(0.f, -60.f * s.scale));
                    leaves.setPoint(1, sf::Vector2f(20.f * s.scale, -30.f * s.scale));
                    leaves.setPoint(2, sf::Vector2f(15.f * s.scale, 10.f * s.scale));
                    leaves.setPoint(3, sf::Vector2f(-15.f * s.scale, 10.f * s.scale));
                    leaves.setPoint(4, sf::Vector2f(-20.f * s.scale, -30.f * s.scale));
                    leaves.setFillColor(sf::Color(107, 142, 35));
                    leaves.setPosition({ s.pos.x, s.pos.y - 70.f * s.scale });
                    window.draw(leaves);
                }
            }

            for (const auto& bug : bugs) {
                window.draw(bug);
                sf::Vector2f bPos = bug.getPosition();

                sf::VertexArray bLegs(sf::PrimitiveType::Lines, 4);
                bLegs[0].color = sf::Color::Black; bLegs[1].color = sf::Color::Black;
                bLegs[2].color = sf::Color::Black; bLegs[3].color = sf::Color::Black;
                bLegs[0].position = sf::Vector2f(bPos.x - 5.f, bPos.y + 11.f);
                bLegs[1].position = sf::Vector2f(bPos.x - 9.f, bPos.y + 20.f);
                bLegs[2].position = sf::Vector2f(bPos.x + 5.f, bPos.y + 11.f);
                bLegs[3].position = sf::Vector2f(bPos.x + 9.f, bPos.y + 20.f);
                window.draw(bLegs);

                sf::VertexArray bAntennas(sf::PrimitiveType::Lines, 4);
                bAntennas[0].color = sf::Color::Black; bAntennas[1].color = sf::Color::Black;
                bAntennas[2].color = sf::Color::Black; bAntennas[3].color = sf::Color::Black;
                bAntennas[0].position = sf::Vector2f(bPos.x - 10.f, bPos.y - 8.f);
                bAntennas[1].position = sf::Vector2f(bPos.x - 18.f, bPos.y - 15.f);
                bAntennas[2].position = sf::Vector2f(bPos.x - 7.f, bPos.y - 10.f);
                bAntennas[3].position = sf::Vector2f(bPos.x - 14.f, bPos.y - 19.f);
                window.draw(bAntennas);

                sf::CircleShape bEye(2.f);
                bEye.setFillColor(sf::Color::White);
                bEye.setOrigin({ 2.f, 2.f });
                bEye.setPosition({ bPos.x - 10.f, bPos.y - 3.f });
                window.draw(bEye);
            }

            for (const auto& coin : coins) {
                sf::CircleShape coinBase(16.f * coin.scale);
                coinBase.setFillColor(sf::Color(255, 215, 0));
                coinBase.setOutlineThickness(2.f * coin.scale);
                coinBase.setOutlineColor(sf::Color(218, 165, 32));
                coinBase.setOrigin({ 16.f * coin.scale, 16.f * coin.scale });
                coinBase.setPosition(coin.pos);

                sf::CircleShape coinInner(11.f * coin.scale);
                coinInner.setFillColor(sf::Color::Transparent);
                coinInner.setOutlineThickness(1.5f * coin.scale);
                coinInner.setOutlineColor(sf::Color(238, 201, 0));
                coinInner.setOrigin({ 11.f * coin.scale, 11.f * coin.scale });
                coinInner.setPosition(coin.pos);

                window.draw(coinBase);
                window.draw(coinInner);
            }

            window.draw(beeBody);
            window.draw(beeStripe1);
            window.draw(beeStripe2);
            window.draw(beeStinger);
            window.draw(beeEye);
            window.draw(beePupil);
            window.draw(beeEyeHighlight);
            window.draw(beeAntenna1);
            window.draw(beeAntenna2);
            window.draw(beeAntennaTip1);
            window.draw(beeAntennaTip2);
            window.draw(beeLeg1);
            window.draw(beeLeg2);
            window.draw(beeWing);

            int remainingTime = std::max(0, static_cast<int>(25.0f - healthTimer));
            sf::Text hud(font, getLocalizedText("hud", currentScore, lives, remainingTime), 18);
            hud.setFillColor(sf::Color::Black);
            hud.setPosition({ 20.f, 20.f });
            window.draw(hud);

            window.draw(inGamePauseButton);
            sf::Text pauseBtnText(font, "PAUSE", 15);
            pauseBtnText.setFillColor(sf::Color::Black);
            pauseBtnText.setOrigin({ pauseBtnText.getLocalBounds().size.x / 2.f, pauseBtnText.getLocalBounds().size.y / 2.f });
            pauseBtnText.setPosition({ 890.f + 45.f, 20.f + 18.f });
            window.draw(pauseBtnText);

            if (state == PAUSED) {
                sf::RectangleShape overlay(sf::Vector2f(1000.f, 600.f));
                overlay.setFillColor(sf::Color(0, 0, 0, 150));
                window.draw(overlay);

                sf::RectangleShape dialogBox(sf::Vector2f(440.f, 240.f));
                dialogBox.setFillColor(sf::Color(250, 250, 250));
                dialogBox.setOutlineThickness(3.f);
                dialogBox.setOutlineColor(sf::Color(80, 80, 80));
                dialogBox.setOrigin({ 220.f, 120.f });
                dialogBox.setPosition({ 500.f, 300.f });
                window.draw(dialogBox);

                sf::Text pauseTitle(font, getLocalizedText("pause_title"), 22);
                pauseTitle.setFillColor(sf::Color::Black);
                pauseTitle.setOrigin({ pauseTitle.getLocalBounds().size.x / 2.f, pauseTitle.getLocalBounds().size.y / 2.f });
                pauseTitle.setPosition({ 500.f, 215.f });
                window.draw(pauseTitle);

                sf::Text pauseMsg(font, getLocalizedText("pause_msg"), 18);
                pauseMsg.setFillColor(sf::Color(60, 60, 60));
                pauseMsg.setOrigin({ pauseMsg.getLocalBounds().size.x / 2.f, pauseMsg.getLocalBounds().size.y / 2.f });
                pauseMsg.setPosition({ 500.f, 245.f });
                window.draw(pauseMsg);

                pauseYesButton.setOutlineColor(pauseMenuIndex == 0 ? sf::Color::Yellow : sf::Color(30, 130, 76));
                pauseNoButton.setOutlineColor(pauseMenuIndex == 1 ? sf::Color::Yellow : sf::Color(146, 43, 33));

                sf::Text yesText(font, getLocalizedText("yes"), 16);
                yesText.setFillColor(sf::Color::White);
                yesText.setOrigin({ yesText.getLocalBounds().size.x / 2.f, yesText.getLocalBounds().size.y / 2.f });
                yesText.setPosition(pauseYesButton.getPosition());

                sf::Text noText(font, getLocalizedText("no"), 16);
                noText.setFillColor(sf::Color::White);
                noText.setOrigin({ noText.getLocalBounds().size.x / 2.f, noText.getLocalBounds().size.y / 2.f });
                noText.setPosition(pauseNoButton.getPosition());

                window.draw(pauseYesButton);
                window.draw(yesText);
                window.draw(pauseNoButton);
                window.draw(noText);
            }
        }
        else if (state == GAMEOVER) {
            sf::Text goText(font, getLocalizedText("gameover"), 40);
            goText.setFillColor(sf::Color::Red);
            goText.setOrigin({ goText.getLocalBounds().size.x / 2.f, goText.getLocalBounds().size.y / 2.f });
            goText.setPosition({ 500.f, 200.f });

            sf::Text restartText(font, getLocalizedText("restart"), 18);
            restartText.setFillColor(sf::Color::White);
            restartText.setOrigin({ restartText.getLocalBounds().size.x / 2.f, restartText.getLocalBounds().size.y / 2.f });
            restartText.setPosition({ 500.f, 280.f });

            window.draw(goText);
            window.draw(restartText);
        }

        window.display();
    }

    return 0;
}