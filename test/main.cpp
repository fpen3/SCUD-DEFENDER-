#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <time.h>
#include "irrKlang.h" // Include the irrKlang header for sound functionality

using namespace irrklang;
using namespace std::chrono;

struct Rectangle {
    float x, y;       // Current position of the rectangle
    float targetX, targetY; // Target position
    float speed;      // Speed of the rectangle
    bool isMovingAcrossTop; // Flag to indicate movement type
    ISound* sound;    // Sound associated with the rectangle
};

std::vector<Rectangle> rectangles;
const float squareWidth = 0.2f; // Width and height of the moving rectangles
const int cooldown = 2000; // Cooldown in milliseconds for sound playback
auto lastSoundTime = high_resolution_clock::now(); // Last sound played time

ISoundEngine* soundEngine = createIrrKlangDevice(); // Create sound engine

// Check if two rectangles intersect
bool intersect(const Rectangle& a, const Rectangle& b) {
    return (std::abs(a.x - b.x) * 2 < squareWidth) && (std::abs(a.y - b.y) * 2 < squareWidth);
}


// PATRIOT MISSILE LAUNCHER
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // Convert screen coordinates to OpenGL coordinates
        float x = (float)mouseX / (float)width * 2.0f - 1.0f;
        float y = (float)(height - mouseY) / (float)height * 2.0f - 1.0f;

        ISound* sound = nullptr;
  
        // Play sound
        int pRandomNumber = (rand() % 5) + 1;
        switch (pRandomNumber) {
            case 1:
                sound = soundEngine->play2D("sounds/patriotFire1.mp3", false, false, true, ESM_AUTO_DETECT, true);
                break;
            case 2:
                sound = soundEngine->play2D("sounds/patriotFire2.mp3", false, false, true, ESM_AUTO_DETECT, true);
                break;
            case 3:
                sound = soundEngine->play2D("sounds/patriotFire3.mp3", false, false, true, ESM_AUTO_DETECT, true);
                break;
            case 4:
                sound = soundEngine->play2D("sounds/patriotFire4.mp3", false, false, true, ESM_AUTO_DETECT, true);
                break;
            case 5:
                sound = soundEngine->play2D("sounds/patriotFire5.mp3", false, false, true, ESM_AUTO_DETECT, true);
                break;
        }
        std::cout << sound << std::endl;
        
        // Add new rectangle
        rectangles.push_back({ -squareWidth / 2, -1.0f + squareWidth / 2, x, y, 0.02f, false, sound });
        std::cout << "New rectangle added by mouse click. Total rectangles: " << rectangles.size() << std::endl;
        rectangles.back().sound = sound;

    }
}

// SCUD MISSILE
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        auto currentTime = high_resolution_clock::now();
        long long duration = duration_cast<milliseconds>(currentTime - lastSoundTime).count();

        if (duration > cooldown) {
            // Add a new rectangle moving across the top
            if (rectangles.size() < 3) { // Keep the cap for space bar generated rectangles
                rectangles.push_back({ -1.0f, 0.9f - squareWidth, 0.0f, 0.0f, 0.01f + static_cast<float>(rand()) / RAND_MAX * 0.003f, true, nullptr });
                std::cout << "New rectangle added by space bar. Total rectangles: " << rectangles.size() << std::endl;

                // Play sound
                int sRandomNumber = (rand() % 3) + 1;

				ISound* sound = nullptr;

                switch (sRandomNumber) {
                    case 1:
                        sound = soundEngine->play2D("sounds/Scud1.mp3", false, false, true, ESM_AUTO_DETECT, true);
                        break;
                    case 2:
                        sound = soundEngine->play2D("sounds/Scud2.mp3", false, false, true, ESM_AUTO_DETECT, true);
                        break;
                    case 3:
                        sound = soundEngine->play2D("sounds/Scud3.mp3", false, false, true, ESM_AUTO_DETECT, true);
                        break;
                    }
                
                lastSoundTime = currentTime; // Update last sound played time
                rectangles.back().sound = sound;
            }
            else {
                std::cout << "Maximum number of rectangles reached." << std::endl;
            }
        }
    }
}



int main(void) {

    GLFWwindow* window;

    srand(static_cast<unsigned int>(time(NULL)));
    int hitRandomNumber;

    // Initialize the GLFW library
    if (!glfwInit()) {
        if (soundEngine) {
            soundEngine->drop();
        }
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(1920, 1080, "SCUD DEFENDER", NULL, NULL);
    if (!window) {
        glfwTerminate();
        if (soundEngine) {
            soundEngine->drop();
        }
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Set the mouse button and key callback
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT);

        // Update and draw rectangles
        for (auto it = rectangles.begin(); it != rectangles.end();) {
            // Draw rectangle
            glBegin(GL_QUADS);
            glVertex2f(it->x - squareWidth / 2, it->y - squareWidth / 2);
            glVertex2f(it->x + squareWidth / 2, it->y - squareWidth / 2);
            glVertex2f(it->x + squareWidth / 2, it->y + squareWidth / 2);
            glVertex2f(it->x - squareWidth / 2, it->y + squareWidth / 2);
            glEnd();

            // Update position
            if (it->isMovingAcrossTop) {
                it->x += it->speed; // Move to the right each frame
                if (it->x > 1.0f) { // If completely off screen
                    it = rectangles.erase(it); // Remove rectangle
                }
                else {
                    ++it;
                }
            }
            else {
                float dx = it->targetX - it->x;
                float dy = it->targetY - it->y;
                float distance = std::sqrt(dx * dx + dy * dy);
                if (distance < it->speed) {
                    it = rectangles.erase(it); // Remove rectangle if it has reached its destination
                }
                else {
                    it->x += dx / distance * it->speed;
                    it->y += dy / distance * it->speed;
                    ++it;
                }
            }
        }

        // Collision Detection
        for (auto it1 = rectangles.begin(); it1 != rectangles.end() && it1 + 1 != rectangles.end(); ++it1) {
			
            for (auto it2 = it1 + 1; it2 != rectangles.end(); ) {
				
                if (it1->isMovingAcrossTop != it2->isMovingAcrossTop && intersect(*it1, *it2)) {
                    // Handle collision
                    std::cout << "Collision detected!" << std::endl;               

					//Stop and drop the sound associated with the rectangle
					it1->sound->stop(), it1->sound->drop();
					it2->sound->stop(), it2->sound->drop();

                    it2 = rectangles.erase(it2); // Erase the collided rectangle
                    it1 = rectangles.erase(it1); // Erase the collided rectangle

                    hitRandomNumber = (rand() % 6) + 1;
	                
                    switch (hitRandomNumber) {
                        case 1:
                            soundEngine->stopAllSounds();
                            soundEngine->play2D("sounds/hit1.mp3", false);
                            break;
                        case 2:
                            soundEngine->stopAllSounds();
                            soundEngine->play2D("sounds/hit2.mp3", false);
                            break;
                        case 3:
                            soundEngine->stopAllSounds();
                            soundEngine->play2D("sounds/hit3.mp3", false);
                            break;
                        case 4:
                            soundEngine->stopAllSounds();
                            soundEngine->play2D("sounds/hit4.mp3", false);
                            break;
                        case 5:
                            soundEngine->stopAllSounds();
                            soundEngine->play2D("sounds/hit5.mp3", false);
                            break;
                        case 6:
                            soundEngine->stopAllSounds();
                            soundEngine->play2D("sounds/hit6.mp3", false);
                            break;
                        }
                    break; // Break out of the inner loop as it1 is invalidated
                }
                else {
                    ++it2;
                }
            }
            if (it1 == rectangles.end()) break; // Check if it1 is invalidated
        }

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    if (soundEngine) {
        soundEngine->drop(); // Release sound engine resources
    }
    return 0;
}
