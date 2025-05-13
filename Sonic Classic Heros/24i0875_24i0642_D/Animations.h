#pragma once
#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

class Animation {
public:
    Animation() = default; 

    Animation(const Texture& texture, int sheetWidth, int frameHeight, int totalFrames, float frameDuration)
        : m_texture(texture), m_totalFrames(totalFrames), m_frameDuration(frameDuration),
        m_currentFrame(0), m_elapsedTime(0)
    {

        m_frameWidth = sheetWidth / totalFrames;
        m_frameHeight = frameHeight;

        m_sprite.setTexture(texture);
        updateFrameRect(); 
    }

    void update(float deltaTime) {
        m_elapsedTime += deltaTime;
        if (m_elapsedTime >= m_frameDuration) {
            m_elapsedTime -= m_frameDuration;  
            m_currentFrame = (m_currentFrame + 1) % m_totalFrames;  
            updateFrameRect(); 
        }
    }

    void draw(RenderWindow& window) {
        window.draw(m_sprite);
    }

    void setPosition(float x, float y) {
        m_sprite.setPosition(x, y);
    }


    Sprite getSprite() const {
        return m_sprite;
    }

private:

    void updateFrameRect() {

        int frameX = m_currentFrame * m_frameWidth;
        int frameY = 0;


        m_sprite.setTextureRect(IntRect(frameX, frameY, m_frameWidth, m_frameHeight));
    }

    Sprite m_sprite;
    Texture m_texture;
    int m_frameWidth;
    int m_frameHeight;
    int m_totalFrames;
    float m_frameDuration;  
    int m_currentFrame;
    float m_elapsedTime;
};