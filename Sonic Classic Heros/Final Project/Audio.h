#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

class MusicManager {

    float x, y;

    Music music[3];
    float volume = 50.f;
    int currentTrackIndex = -1;

    Font font;
    Text text[5];
    int selectedOption = 0;

    RectangleShape sliderBackground;
    RectangleShape sliderIndicator;

    Texture backgroundTexture;
    Sprite backgroundSprite;

    RectangleShape whiteOverlay;

    Texture b[3];
    Sprite a[3];

public:
    MusicManager(float x, float y) : x(x), y(y) {

        cout << "Loading Music...\n";

        if (!font.loadFromFile("OOP-Final/SyneMono-Regular.ttf")) {
            cout << "Error loading font" << endl;
        }

        if (!music[0].openFromFile("Data/music/stronger-monsters.wav")) {
            cout << "Failed to load stronger-monsters.wav" << endl;
        }
        if (!music[1].openFromFile("Data/music/megalovania.wav")) {
            cout << "Failed to load megalovania.wav" << endl;
        }
        if (!music[2].openFromFile("Data/music/labrynth.wav")) {
            cout << "Failed to load labrynth.wav" << endl;
        }

        for (int i = 0; i < 3; ++i) {
            music[i].setVolume(volume);
            music[i].setLoop(true);
        }

        music[0].play();
        currentTrackIndex = 0;

        b[0].loadFromFile("Data/sonic_music.png");
        a[0].setTexture(b[0]);
        a[0].setPosition((x / 3) * 2, (y / 7) * 1);
        a[0].setScale(2.5, 2.5);

        text[0].setFont(font);
        text[0].setString("Play Track 1");
        text[0].setCharacterSize(30);
        text[0].setPosition(100.f, (y / 7) * 1);

        b[1].loadFromFile("Data/Tail_Music.png");
        a[1].setTexture(b[1]);
        a[1].setPosition((x / 3) * 2, (y / 7) * 2);
        a[1].setScale(2.5, 2.5);

        text[1].setFont(font);
        text[1].setString("Play Track 2");
        text[1].setCharacterSize(30);
        text[1].setPosition(100.f, (y / 7) * 2);

        b[2].loadFromFile("Data/kunckles_music.png");
        a[2].setTexture(b[2]);
        a[2].setPosition((x / 3) * 2, (y / 7) * 3);
        a[2].setScale(2.5, 2.5);

        text[2].setFont(font);
        text[2].setString("Play Track 3");
        text[2].setCharacterSize(30);
        text[2].setPosition(100.f, (y / 7) * 3);

        text[3].setFont(font);
        text[3].setString("Volume: " + to_string(int(volume)) + "%");
        text[3].setCharacterSize(30);
        text[3].setPosition(100.f, (y / 7) * 4);

        text[4].setFont(font);
        text[4].setString(" Press Enter to Select\n Up/Down to Navigate\n Use Left/Right to adjust Volume");
        text[4].setCharacterSize(20);
        text[4].setPosition(100.f, (y / 7) * 5.75);

        sliderBackground.setSize(Vector2f(300.f, 10.f));
        sliderBackground.setFillColor(Color::White);
        sliderBackground.setPosition(100.f, (y / 7) * 4.5);

        sliderIndicator.setSize(Vector2f(20.f, 20.f));
        sliderIndicator.setFillColor(Color::Blue);
        updateSlider();

        if (!backgroundTexture.loadFromFile("Data/background.jpg")) {
            cout << "Error loading background image!" << endl;
        }
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(0.2f, 0.2f);

        whiteOverlay.setSize(Vector2f(1200, 900));
        whiteOverlay.setFillColor(Color(255, 255, 255, 100));
    }

    void setVolume(float vol) {
        if (vol < 0) vol = 0;
        if (vol > 100) vol = 100;

        volume = vol;

        for (int i = 0; i < 3; ++i) {
            music[i].setVolume(volume);
        }

        text[3].setString("Volume: " + to_string(int(volume)) + "%");
        updateSlider();
    }

    void increaseVolume(float step) { setVolume(volume + step); }

    void decreaseVolume(float step) { setVolume(volume - step); }

    float getVolume() const { return volume; }

    void playTrack(int trackIndex) {
        if (trackIndex >= 0 && trackIndex < 3) {

            if (currentTrackIndex == trackIndex && music[trackIndex].getStatus() == SoundSource::Playing) {
                music[trackIndex].pause();
                return;
            }

            if (currentTrackIndex != -1 && music[currentTrackIndex].getStatus() == SoundSource::Playing) {
                music[currentTrackIndex].stop();
            }

            music[trackIndex].play();
            currentTrackIndex = trackIndex;
        }
    }

    void updateSlider() {
        float sliderWidth = sliderBackground.getSize().x;
        float indicatorX = (volume / 100.f) * sliderWidth;
        sliderIndicator.setPosition(100.f + indicatorX - sliderIndicator.getSize().x / 2.f, (y / 7) * 4.5);
    }

    void handleInput(Event event) {
        if (event.type == Event::KeyPressed) {
            if (event.key.code == Keyboard::Up) {
                selectedOption = (selectedOption == 0) ? 3 : selectedOption - 1;
            }

            if (event.key.code == Keyboard::Down) {
                selectedOption = (selectedOption == 3) ? 0 : selectedOption + 1;
            }

            if (event.key.code == Keyboard::Enter) {
                switch (selectedOption) {
                case 0:
                    playTrack(0);
                    break;
                case 1:
                    playTrack(1);
                    break;
                case 2:
                    playTrack(2);
                    break;
                }
            }

            if (Keyboard::isKeyPressed(Keyboard::Left)) {
                decreaseVolume(10);
            }

            if (Keyboard::isKeyPressed(Keyboard::Right)) {
                increaseVolume(10);
            }

        }
    }

    void drawMenu(RenderWindow& window) {
        window.draw(backgroundSprite);
        window.draw(whiteOverlay);

        for (int i = 0; i < 5; ++i) {
            if (i == selectedOption) {
                text[i].setFillColor(Color::Blue);
            }
            else {
                text[i].setFillColor(Color::Black);
            }
            window.draw(text[i]);

            if (i < 3) window.draw(a[i]);
        }

        window.draw(sliderBackground);
        window.draw(sliderIndicator);
    }
};
