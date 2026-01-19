/**
 * FMOD Streaming Audio Example
 * 
 * This example demonstrates how to stream audio files from disk.
 * Streaming is ideal for:
 * - Background music
 * - Long audio files
 * - Reducing memory usage
 * 
 * The key difference from loading sounds into memory is using
 * createStream() instead of createSound().
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
    FMOD::Sound* musicStream = nullptr;
    FMOD::Channel* musicChannel = nullptr;
    FMOD_RESULT result;
    
    std::cout << "=== FMOD Streaming Audio Example ===" << std::endl;
    
    // Initialize FMOD system
    std::cout << "Initializing FMOD..." << std::endl;
    result = FMOD::System_Create(&system);
    CheckError(result, "System_Create");
    
    result = system->init(512, FMOD_INIT_NORMAL, nullptr);
    CheckError(result, "init");
    
    // Create a streaming sound
    const char* musicFile = (argc > 1) ? argv[1] : "music.mp3";
    std::cout << "Creating stream for: " << musicFile << std::endl;
    
    // FMOD_CREATESTREAM: Stream from disk (don't load entire file into memory)
    // FMOD_LOOP_NORMAL: Enable looping
    result = system->createStream(musicFile, FMOD_LOOP_NORMAL | FMOD_2D, nullptr, &musicStream);
    CheckError(result, "createStream");
    
    // Get stream information
    FMOD_SOUND_TYPE soundType;
    FMOD_SOUND_FORMAT soundFormat;
    int channels, bits;
    
    result = musicStream->getFormat(&soundType, &soundFormat, &channels, &bits);
    CheckError(result, "getFormat");
    std::cout << "Stream format: " << channels << " channels, " << bits << " bits" << std::endl;
    
    // Get length
    unsigned int lengthMs;
    musicStream->getLength(&lengthMs, FMOD_TIMEUNIT_MS);
    std::cout << "Stream duration: " << (lengthMs / 1000) << " seconds" << std::endl;
    
    // Play the stream
    std::cout << "Playing streamed music (looping)..." << std::endl;
    result = system->playSound(musicStream, nullptr, false, &musicChannel);
    CheckError(result, "playSound");
    
    // Set initial volume
    musicChannel->setVolume(0.5f);
    std::cout << "Volume set to 50%" << std::endl;
    
    // Demonstrate volume control
    std::cout << "\nDemonstrating volume fade in/out..." << std::endl;
    std::cout << "Music will play for 20 seconds with volume changes" << std::endl;
    
    float elapsedTime = 0.0f;
    const float totalTime = 20.0f;
    const float updateInterval = 0.1f; // Update every 100ms
    
    while (elapsedTime < totalTime) {
        // Update FMOD system
        system->update();
        
        // Check if still playing
        bool isPlaying = false;
        result = musicChannel->isPlaying(&isPlaying);
        if (result != FMOD_OK && result != FMOD_ERR_INVALID_HANDLE) {
            CheckError(result, "isPlaying");
        }
        
        if (!isPlaying) {
            std::cout << "\nMusic stopped unexpectedly!" << std::endl;
            break;
        }
        
        // Demonstrate volume control with a sine wave
        float volume = 0.3f + 0.3f * std::sin(elapsedTime * 2.0f);
        musicChannel->setVolume(volume);
        
        // Get playback position
        unsigned int position;
        musicChannel->getPosition(&position, FMOD_TIMEUNIT_MS);
        
        // Display status
        std::cout << "\rTime: " << static_cast<int>(elapsedTime) 
                  << "s | Position: " << (position / 1000) 
                  << "s | Volume: " << static_cast<int>(volume * 100) << "%   " 
                  << std::flush;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(updateInterval * 1000)));
        elapsedTime += updateInterval;
    }
    
    std::cout << "\n\nStopping music..." << std::endl;
    musicChannel->stop();
    
    // Clean up
    std::cout << "Cleaning up..." << std::endl;
    result = musicStream->release();
    CheckError(result, "musicStream->release");
    
    result = system->release();
    CheckError(result, "system->release");
    
    std::cout << "Done!" << std::endl;
    return 0;
}

/**
 * Key differences between createSound() and createStream():
 * 
 * createSound():
 * - Loads entire file into memory
 * - Fast access, no disk I/O during playback
 * - Uses more memory
 * - Good for: Sound effects, short audio clips
 * 
 * createStream():
 * - Streams from disk in small chunks
 * - Minimal memory usage
 * - Some disk I/O during playback
 * - Good for: Music, long audio, voice-over
 * 
 * Compilation instructions:
 * 
 * Windows (Visual Studio):
 *   cl streaming.cpp /I"C:\FMOD\api\core\inc" /link /LIBPATH:"C:\FMOD\api\core\lib\x64" fmod_vc.lib
 * 
 * Linux:
 *   g++ streaming.cpp -o streaming -I/path/to/fmod/api/core/inc -L/path/to/fmod/api/core/lib/x86_64 -lfmod -lpthread
 * 
 * macOS:
 *   clang++ streaming.cpp -o streaming -I/path/to/fmod/api/core/inc -L/path/to/fmod/api/core/lib -lfmod
 * 
 * Usage:
 *   ./streaming [path/to/music/file.mp3]
 */
