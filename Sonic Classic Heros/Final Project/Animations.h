#pragma once
#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

class Animation {
public:
    Animation() = default; // Default constructor   

    // Constructor now takes sheet dimensions rather than individual frame dimensions
    Animation(const Texture& texture, int sheetWidth, int frameHeight, int totalFrames, float frameDuration)
        : m_texture(texture), m_totalFrames(totalFrames), m_frameDuration(frameDuration),
        m_currentFrame(0), m_elapsedTime(0)
    {
        // Calculate individual frame width from sheet width and total frames
        m_frameWidth = sheetWidth / totalFrames;
        m_frameHeight = frameHeight;

        m_sprite.setTexture(texture);
        updateFrameRect(); // Set initial frame rectangle
    }

    void update(float deltaTime) {
        m_elapsedTime += deltaTime;
        if (m_elapsedTime >= m_frameDuration) {
            m_elapsedTime -= m_frameDuration;  // reset the elapsed time
            m_currentFrame = (m_currentFrame + 1) % m_totalFrames;  // move to the next frame
            updateFrameRect(); // Update the texture rectangle for the new frame
        }
    }

    void draw(RenderWindow& window) {
        window.draw(m_sprite);
    }

    void setPosition(float x, float y) {
        m_sprite.setPosition(x, y);
    }

    // Get the current sprite to be used in the Enemies class
    Sprite getSprite() const {
        return m_sprite;
    }

private:
    // Helper method to update the frame rectangle based on the current frame
    void updateFrameRect() {
        // Calculate frame position (assuming horizontal sprite sheet)
        int frameX = m_currentFrame * m_frameWidth;
        int frameY = 0; // Assume single row

        // Update the sprite's texture rectangle
        m_sprite.setTextureRect(IntRect(frameX, frameY, m_frameWidth, m_frameHeight));
    }

    Sprite m_sprite;
    Texture m_texture;
    int m_frameWidth;
    int m_frameHeight;
    int m_totalFrames;
    float m_frameDuration;  // Time in seconds per frame
    int m_currentFrame;
    float m_elapsedTime;
};