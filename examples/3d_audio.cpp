/**
 * FMOD 3D Audio Example
 * 
 * This example demonstrates how to use FMOD's 3D audio capabilities:
 * - Setting up a 3D listener (represents the player/camera)
 * - Creating 3D sounds
 * - Positioning sounds in 3D space
 * - Moving sounds and the listener
 * 
 * 3D audio provides spatial positioning, distance attenuation, and doppler effects.
 */

#include <fmod.hpp>
#include <fmod_errors.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Helper function to check FMOD results
void CheckError(FMOD_RESULT result, const char* description) {
    if (result != FMOD_OK) {
        std::cerr << "FMOD Error at " << description << ": (" 
                  << result << ") " << FMOD_ErrorString(result) << std::endl;
        exit(-1);
    }
}

int main(int argc, char* argv[]) {
    FMOD::System* system = nullptr;
    FMOD::Sound* sound3D = nullptr;
    FMOD::Channel* channel = nullptr;
    FMOD_RESULT result;
    
    std::cout << "=== FMOD 3D Audio Example ===" << std::endl;
    
    // Initialize FMOD system
    std::cout << "Initializing FMOD..." << std::endl;
    result = FMOD::System_Create(&system);
    CheckError(result, "System_Create");
    
    result = system->init(512, FMOD_INIT_NORMAL, nullptr);
    CheckError(result, "init");
    
    // Set 3D settings
    // Parameters: dopplerscale, distancefactor, rolloffscale
    // - dopplerscale: 1.0 = normal doppler effect
    // - distancefactor: Units per meter (1.0 = 1 unit = 1 meter)
    // - rolloffscale: How quickly sound attenuates with distance
    result = system->set3DSettings(1.0f, 1.0f, 1.0f);
    CheckError(result, "set3DSettings");
    
    // Create a 3D sound
    const char* soundFile = (argc > 1) ? argv[1] : "sound.wav";
    std::cout << "Loading 3D sound: " << soundFile << std::endl;
    
    // FMOD_3D: Enable 3D positioning
    // FMOD_LOOP_NORMAL: Loop the sound so we can hear the 3D effect
    result = system->createSound(soundFile, FMOD_3D | FMOD_LOOP_NORMAL, nullptr, &sound3D);
    CheckError(result, "createSound");
    
    // Set 3D min/max distance for the sound
    // minDistance: Distance at which sound is at full volume
    // maxDistance: Distance at which sound stops attenuating (or becomes silent)
    result = sound3D->set3DMinMaxDistance(1.0f, 100.0f);
    CheckError(result, "set3DMinMaxDistance");
    
    // Play the sound
    std::cout << "Playing 3D sound..." << std::endl;
    result = system->playSound(sound3D, nullptr, false, &channel);
    CheckError(result, "playSound");
    
    // Initial listener position (represents the player/camera)
    FMOD_VECTOR listenerPos = {0.0f, 0.0f, 0.0f};
    FMOD_VECTOR listenerVel = {0.0f, 0.0f, 0.0f};
    FMOD_VECTOR listenerForward = {0.0f, 0.0f, 1.0f};  // Looking down Z axis
    FMOD_VECTOR listenerUp = {0.0f, 1.0f, 0.0f};       // Up is Y axis
    
    std::cout << "\nSimulating 3D audio with moving sound source..." << std::endl;
    std::cout << "Listener is at origin (0, 0, 0)" << std::endl;
    std::cout << "Sound will orbit around the listener" << std::endl;
    
    // Simulate movement for demonstration
    float angle = 0.0f;
    const float radius = 10.0f;
    const float duration = 15.0f; // Run for 15 seconds
    const float updateInterval = 0.05f; // Update every 50ms
    
    float elapsedTime = 0.0f;
    
    while (elapsedTime < duration) {
        // Update listener attributes (position, velocity, orientation)
        result = system->set3DListenerAttributes(
            0,                  // Listener index (0 = first listener)
            &listenerPos,       // Position
            &listenerVel,       // Velocity (for doppler effect)
            &listenerForward,   // Forward vector
            &listenerUp         // Up vector
        );
        CheckError(result, "set3DListenerAttributes");
        
        // Calculate sound position in a circular orbit
        FMOD_VECTOR soundPos;
        soundPos.x = radius * std::cos(angle);
        soundPos.y = 0.0f;
        soundPos.z = radius * std::sin(angle);
        
        // Calculate velocity for doppler effect
        const float angularSpeed = 2.0f * M_PI / 10.0f; // One rotation every 10 seconds
        FMOD_VECTOR soundVel;
        soundVel.x = -radius * angularSpeed * std::sin(angle);
        soundVel.y = 0.0f;
        soundVel.z = radius * angularSpeed * std::cos(angle);
        
        // Set 3D attributes for the channel (sound position)
        result = channel->set3DAttributes(&soundPos, &soundVel);
        if (result != FMOD_OK && result != FMOD_ERR_INVALID_HANDLE) {
            CheckError(result, "set3DAttributes");
        }
        
        // Update FMOD system (this processes 3D calculations)
        system->update();
        
        // Calculate distance from listener
        float distance = std::sqrt(soundPos.x * soundPos.x + 
                                   soundPos.y * soundPos.y + 
                                   soundPos.z * soundPos.z);
        
        // Display information
        std::cout << "\rTime: " << static_cast<int>(elapsedTime) << "s "
                  << "| Pos: (" << static_cast<int>(soundPos.x) 
                  << ", " << static_cast<int>(soundPos.y)
                  << ", " << static_cast<int>(soundPos.z) << ") "
                  << "| Distance: " << static_cast<int>(distance) << "m   "
                  << std::flush;
        
        // Update angle for next iteration
        angle += angularSpeed * updateInterval;
        if (angle > 2.0f * M_PI) {
            angle -= 2.0f * M_PI;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(updateInterval * 1000)));
        elapsedTime += updateInterval;
    }
    
    std::cout << "\n\nStopping sound..." << std::endl;
    channel->stop();
    
    // Clean up
    std::cout << "Cleaning up..." << std::endl;
    result = sound3D->release();
    CheckError(result, "sound3D->release");
    
    result = system->release();
    CheckError(result, "system->release");
    
    std::cout << "Done!" << std::endl;
    return 0;
}

/**
 * Key concepts for 3D audio:
 * 
 * 1. Listener: Represents the player/camera position and orientation
 *    - Position: Where the listener is in 3D space
 *    - Velocity: Movement speed (for doppler effect)
 *    - Forward: Direction listener is facing
 *    - Up: Up direction for listener orientation
 * 
 * 2. Sound position: Where the sound source is in 3D space
 *    - Position: Sound location
 *    - Velocity: Sound movement (for doppler effect)
 * 
 * 3. Distance attenuation:
 *    - minDistance: Full volume within this distance
 *    - maxDistance: Volume drops to min/silence beyond this
 *    - Volume automatically calculated based on distance
 * 
 * 4. Doppler effect:
 *    - Automatic pitch shifting based on relative velocity
 *    - Sounds higher when moving toward listener
 *    - Sounds lower when moving away from listener
 * 
 * Compilation instructions:
 * 
 * Windows (Visual Studio):
 *   cl 3d_audio.cpp /I"C:\FMOD\api\core\inc" /link /LIBPATH:"C:\FMOD\api\core\lib\x64" fmod_vc.lib
 * 
 * Linux:
 *   g++ 3d_audio.cpp -o 3d_audio -I/path/to/fmod/api/core/inc -L/path/to/fmod/api/core/lib/x86_64 -lfmod -lpthread
 * 
 * macOS:
 *   clang++ 3d_audio.cpp -o 3d_audio -I/path/to/fmod/api/core/inc -L/path/to/fmod/api/core/lib -lfmod
 * 
 * Usage:
 *   ./3d_audio [path/to/audio/file.wav]
 */
