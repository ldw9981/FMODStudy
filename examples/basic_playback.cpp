/**
 * FMOD Basic Playback Example
 * 
 * This example demonstrates the fundamental workflow of using FMOD:
 * 1. Initialize the FMOD system
 * 2. Load a sound file
 * 3. Play the sound
 * 4. Wait for playback to complete
 * 5. Clean up resources
 */

#include <fmod.hpp>
#include <fmod_errors.h>
#include <iostream>
#include <thread>
#include <chrono>

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
    FMOD::Sound* sound = nullptr;
    FMOD::Channel* channel = nullptr;
    FMOD_RESULT result;
    
    std::cout << "=== FMOD Basic Playback Example ===" << std::endl;
    
    // Step 1: Create the FMOD system object
    std::cout << "Creating FMOD system..." << std::endl;
    result = FMOD::System_Create(&system);
    CheckError(result, "System_Create");
    
    // Get FMOD version
    unsigned int version;
    result = system->getVersion(&version);
    CheckError(result, "getVersion");
    std::cout << "FMOD Version: " << std::hex << version << std::dec << std::endl;
    
    // Step 2: Initialize the system
    // Parameters: maxChannels (512), initFlags (FMOD_INIT_NORMAL), extraDriverData (nullptr)
    std::cout << "Initializing FMOD system..." << std::endl;
    result = system->init(512, FMOD_INIT_NORMAL, nullptr);
    CheckError(result, "init");
    
    // Get audio driver information
    int numDrivers;
    system->getNumDrivers(&numDrivers);
    std::cout << "Number of audio drivers: " << numDrivers << std::endl;
    
    // Step 3: Load a sound file
    // Note: Replace "sound.wav" with an actual audio file path
    const char* soundFile = (argc > 1) ? argv[1] : "sound.wav";
    std::cout << "Loading sound: " << soundFile << std::endl;
    
    // FMOD_DEFAULT: Default mode (not 3D, not streaming, uncompressed/compressed based on file format)
    result = system->createSound(soundFile, FMOD_DEFAULT, nullptr, &sound);
    CheckError(result, "createSound");
    
    // Get sound information
    float frequency;
    int priority;
    sound->getDefaults(&frequency, &priority);
    std::cout << "Sound loaded - Frequency: " << frequency << " Hz" << std::endl;
    
    unsigned int lengthMs;
    sound->getLength(&lengthMs, FMOD_TIMEUNIT_MS);
    std::cout << "Sound duration: " << lengthMs << " ms" << std::endl;
    
    // Step 4: Play the sound
    std::cout << "Playing sound..." << std::endl;
    
    // Parameters: sound, channelGroup (nullptr = default), paused (false), channel (output)
    result = system->playSound(sound, nullptr, false, &channel);
    CheckError(result, "playSound");
    
    // Optionally adjust volume (0.0 = silent, 1.0 = full volume)
    channel->setVolume(0.8f);
    
    // Step 5: Wait for the sound to finish playing
    bool isPlaying = true;
    while (isPlaying) {
        // Update FMOD system (required to be called regularly)
        system->update();
        
        // Check if the channel is still playing
        result = channel->isPlaying(&isPlaying);
        if (result != FMOD_OK && result != FMOD_ERR_INVALID_HANDLE) {
            CheckError(result, "isPlaying");
        }
        
        // Get playback position
        unsigned int position;
        if (channel->getPosition(&position, FMOD_TIMEUNIT_MS) == FMOD_OK) {
            std::cout << "\rPlayback position: " << position << " / " 
                      << lengthMs << " ms  " << std::flush;
        }
        
        // Sleep to avoid busy-waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    std::cout << std::endl << "Playback finished!" << std::endl;
    
    // Step 6: Clean up resources
    std::cout << "Cleaning up..." << std::endl;
    
    // Release the sound
    result = sound->release();
    CheckError(result, "sound->release");
    
    // Release the system (this also releases all sounds and channels)
    result = system->release();
    CheckError(result, "system->release");
    
    std::cout << "Done!" << std::endl;
    return 0;
}

/**
 * Compilation instructions:
 * 
 * Windows (Visual Studio):
 *   cl basic_playback.cpp /I"C:\FMOD\api\core\inc" /link /LIBPATH:"C:\FMOD\api\core\lib\x64" fmod_vc.lib
 * 
 * Linux:
 *   g++ basic_playback.cpp -o basic_playback -I/path/to/fmod/api/core/inc -L/path/to/fmod/api/core/lib/x86_64 -lfmod -lpthread
 * 
 * macOS:
 *   clang++ basic_playback.cpp -o basic_playback -I/path/to/fmod/api/core/inc -L/path/to/fmod/api/core/lib -lfmod
 * 
 * Usage:
 *   ./basic_playback [path/to/audio/file.wav]
 */
