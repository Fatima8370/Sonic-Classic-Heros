#pragma once
#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class Animation {
public:
    Animation(const Texture& texture, int frameWidth, int frameHeight, int totalFrames, float frameDuration)
        : m_texture(texture), m_frameWidth(frameWidth), m_frameHeight(frameHeight),
        m_totalFrames(totalFrames), m_frameDuration(frameDuration), m_currentFrame(0), m_elapsedTime(0)
    {
        m_sprite.setTexture(texture);
        m_sprite.setTextureRect(IntRect(0, 0, frameWidth, frameHeight)); // start at first frame
    }

    void update(float deltaTime) {
        m_elapsedTime += deltaTime;
        if (m_elapsedTime >= m_frameDuration) {
            m_elapsedTime -= m_frameDuration;  // reset the elapsed time
            m_currentFrame = (m_currentFrame + 1) % m_totalFrames;  // move to the next frame

            int frameX = (m_currentFrame * m_frameWidth) % m_texture.getSize().x;
            int frameY = (m_currentFrame * m_frameWidth / m_texture.getSize().x) * m_frameHeight;

            m_sprite.setTextureRect(IntRect(frameX, frameY, m_frameWidth, m_frameHeight));
        }
    }

    void draw(RenderWindow& window) {
        window.draw(m_sprite);
    }

    void setPosition(float x, float y) {
        m_sprite.setPosition(x, y);
    }

private:
    Sprite m_sprite;
    Texture m_texture;
    int m_frameWidth;
    int m_frameHeight;
    int m_totalFrames;
    float m_frameDuration;  // Time in seconds per frame
    int m_currentFrame;
    float m_elapsedTime;
};


