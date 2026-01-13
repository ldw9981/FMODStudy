/**
 * FMOD Audio Manager Example
 * 
 * This example demonstrates a practical audio manager pattern that can be
 * used in games or applications. It shows:
 * - Singleton pattern for global audio access
 * - Sound loading and caching
 * - Channel groups for audio categories
 * - Volume control per category
 * - Resource management
 */

#include <fmod.hpp>
#include <fmod_errors.h>
#include <iostream>
#include <map>
#include <string>

class AudioManager {
private:
    FMOD::System* system;
    std::map<std::string, FMOD::Sound*> sounds;
    
    // Channel groups for different audio categories
    FMOD::ChannelGroup* sfxGroup;
    FMOD::ChannelGroup* musicGroup;
    FMOD::ChannelGroup* voiceGroup;
    
    // Master channel group (controls all audio)
    FMOD::ChannelGroup* masterGroup;
    
    // Current music channel (to control music playback)
    FMOD::Channel* currentMusicChannel;
    
    // Singleton instance
    static AudioManager* instance;
    
    // Private constructor for singleton
    AudioManager() : system(nullptr), sfxGroup(nullptr), musicGroup(nullptr), 
                     voiceGroup(nullptr), masterGroup(nullptr), currentMusicChannel(nullptr) {}
    
    // Helper to check FMOD results
    bool checkError(FMOD_RESULT result, const char* description) {
        if (result != FMOD_OK) {
            std::cerr << "FMOD Error at " << description << ": (" 
                      << result << ") " << FMOD_ErrorString(result) << std::endl;
            return false;
        }
        return true;
    }
    
public:
    // Audio categories enum
    enum Category {
        CATEGORY_SFX,
        CATEGORY_MUSIC,
        CATEGORY_VOICE
    };
    
    // Get singleton instance
    static AudioManager* getInstance() {
        if (!instance) {
            instance = new AudioManager();
        }
        return instance;
    }
    
    // Initialize the audio system
    bool initialize() {
        std::cout << "Initializing Audio Manager..." << std::endl;
        
        // Create FMOD system
        FMOD_RESULT result = FMOD::System_Create(&system);
        if (!checkError(result, "System_Create")) return false;
        
        // Initialize with 512 channels
        result = system->init(512, FMOD_INIT_NORMAL, nullptr);
        if (!checkError(result, "init")) return false;
        
        // Create channel groups for audio categories
        result = system->createChannelGroup("SFX", &sfxGroup);
        if (!checkError(result, "createChannelGroup SFX")) return false;
        
        result = system->createChannelGroup("Music", &musicGroup);
        if (!checkError(result, "createChannelGroup Music")) return false;
        
        result = system->createChannelGroup("Voice", &voiceGroup);
        if (!checkError(result, "createChannelGroup Voice")) return false;
        
        // Get master channel group
        result = system->getMasterChannelGroup(&masterGroup);
        if (!checkError(result, "getMasterChannelGroup")) return false;
        
        std::cout << "Audio Manager initialized successfully!" << std::endl;
        return true;
    }
    
    // Load a sound file and cache it
    bool loadSound(const std::string& name, const std::string& filepath, bool isStream = false) {
        // Check if already loaded
        if (sounds.find(name) != sounds.end()) {
            std::cout << "Sound '" << name << "' already loaded." << std::endl;
            return true;
        }
        
        FMOD::Sound* sound = nullptr;
        FMOD_RESULT result;
        
        if (isStream) {
            // Stream from disk (for music, long audio)
            result = system->createStream(filepath.c_str(), FMOD_DEFAULT, nullptr, &sound);
        } else {
            // Load into memory (for SFX)
            result = system->createSound(filepath.c_str(), FMOD_DEFAULT, nullptr, &sound);
        }
        
        if (!checkError(result, ("load " + name).c_str())) {
            return false;
        }
        
        sounds[name] = sound;
        std::cout << "Loaded sound: " << name << std::endl;
        return true;
    }
    
    // Play a sound effect
    void playSFX(const std::string& name, float volume = 1.0f) {
        if (sounds.find(name) == sounds.end()) {
            std::cerr << "Sound '" << name << "' not loaded!" << std::endl;
            return;
        }
        
        FMOD::Channel* channel = nullptr;
        system->playSound(sounds[name], sfxGroup, false, &channel);
        
        if (channel) {
            channel->setVolume(volume);
        }
    }
    
    // Play music (stops current music if any)
    void playMusic(const std::string& name, bool loop = true, float volume = 1.0f) {
        if (sounds.find(name) == sounds.end()) {
            std::cerr << "Music '" << name << "' not loaded!" << std::endl;
            return;
        }
        
        // Stop current music if playing
        if (currentMusicChannel) {
            bool isPlaying = false;
            currentMusicChannel->isPlaying(&isPlaying);
            if (isPlaying) {
                currentMusicChannel->stop();
            }
        }
        
        // Play new music
        system->playSound(sounds[name], musicGroup, false, &currentMusicChannel);
        
        if (currentMusicChannel) {
            currentMusicChannel->setVolume(volume);
            
            // Set loop mode
            if (loop) {
                currentMusicChannel->setMode(FMOD_LOOP_NORMAL);
            } else {
                currentMusicChannel->setMode(FMOD_LOOP_OFF);
            }
        }
    }
    
    // Stop music
    void stopMusic() {
        if (currentMusicChannel) {
            currentMusicChannel->stop();
            currentMusicChannel = nullptr;
        }
    }
    
    // Pause/unpause music
    void pauseMusic(bool pause) {
        if (currentMusicChannel) {
            currentMusicChannel->setPaused(pause);
        }
    }
    
    // Set volume for a category (0.0 to 1.0)
    void setCategoryVolume(Category category, float volume) {
        FMOD::ChannelGroup* group = getChannelGroup(category);
        if (group) {
            group->setVolume(volume);
        }
    }
    
    // Set master volume (0.0 to 1.0)
    void setMasterVolume(float volume) {
        if (masterGroup) {
            masterGroup->setVolume(volume);
        }
    }
    
    // Update audio system (call every frame!)
    void update() {
        if (system) {
            system->update();
        }
    }
    
    // Clean up resources
    void shutdown() {
        std::cout << "Shutting down Audio Manager..." << std::endl;
        
        // Release all sounds
        for (auto& pair : sounds) {
            pair.second->release();
        }
        sounds.clear();
        
        // Release system
        if (system) {
            system->release();
            system = nullptr;
        }
        
        std::cout << "Audio Manager shut down." << std::endl;
    }
    
    // Destructor
    ~AudioManager() {
        shutdown();
    }
    
private:
    FMOD::ChannelGroup* getChannelGroup(Category category) {
        switch (category) {
            case CATEGORY_SFX: return sfxGroup;
            case CATEGORY_MUSIC: return musicGroup;
            case CATEGORY_VOICE: return voiceGroup;
            default: return nullptr;
        }
    }
};

// Initialize static instance
AudioManager* AudioManager::instance = nullptr;

// Example usage
int main() {
    std::cout << "=== FMOD Audio Manager Example ===" << std::endl;
    
    // Get audio manager instance
    AudioManager* audio = AudioManager::getInstance();
    
    // Initialize
    if (!audio->initialize()) {
        std::cerr << "Failed to initialize audio manager!" << std::endl;
        return -1;
    }
    
    // Load some sounds (you would replace these with actual files)
    std::cout << "\nLoading audio files..." << std::endl;
    audio->loadSound("explosion", "explosion.wav");
    audio->loadSound("laser", "laser.wav");
    audio->loadSound("bgmusic", "background.mp3", true); // Stream music
    
    // Set category volumes
    audio->setCategoryVolume(AudioManager::CATEGORY_SFX, 0.8f);
    audio->setCategoryVolume(AudioManager::CATEGORY_MUSIC, 0.5f);
    audio->setMasterVolume(1.0f);
    
    // Example: Play sounds in a simulated game loop
    std::cout << "\nSimulating gameplay..." << std::endl;
    
    // Start background music
    std::cout << "Starting background music..." << std::endl;
    audio->playMusic("bgmusic", true, 0.5f);
    
    // Simulate some game events
    for (int i = 0; i < 5; i++) {
        std::cout << "\nEvent " << (i + 1) << ":" << std::endl;
        
        // Play sound effects
        if (i % 2 == 0) {
            std::cout << "  - Playing explosion sound" << std::endl;
            audio->playSFX("explosion", 0.7f);
        } else {
            std::cout << "  - Playing laser sound" << std::endl;
            audio->playSFX("laser", 0.8f);
        }
        
        // Update audio system
        for (int j = 0; j < 20; j++) {
            audio->update();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    
    std::cout << "\nPausing music..." << std::endl;
    audio->pauseMusic(true);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    std::cout << "Resuming music..." << std::endl;
    audio->pauseMusic(false);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    std::cout << "\nStopping music..." << std::endl;
    audio->stopMusic();
    
    // Cleanup
    audio->shutdown();
    
    std::cout << "\nExample complete!" << std::endl;
    return 0;
}

/**
 * This audio manager pattern provides:
 * 
 * 1. Centralized audio control through singleton pattern
 * 2. Sound caching to avoid loading the same file multiple times
 * 3. Category-based volume control (SFX, Music, Voice)
 * 4. Easy-to-use interface for common audio operations
 * 5. Proper resource management and cleanup
 * 
 * Usage in your game/application:
 * 
 * // During initialization
 * AudioManager* audio = AudioManager::getInstance();
 * audio->initialize();
 * audio->loadSound("jump", "sounds/jump.wav");
 * audio->loadSound("bgm", "music/level1.mp3", true);
 * 
 * // During gameplay
 * audio->playMusic("bgm");
 * audio->playSFX("jump");
 * audio->update(); // Call every frame!
 * 
 * // During shutdown
 * audio->shutdown();
 * 
 * Compilation instructions:
 * 
 * Windows (Visual Studio):
 *   cl audio_manager.cpp /I"C:\FMOD\api\core\inc" /link /LIBPATH:"C:\FMOD\api\core\lib\x64" fmod_vc.lib
 * 
 * Linux:
 *   g++ audio_manager.cpp -o audio_manager -I/path/to/fmod/api/core/inc -L/path/to/fmod/api/core/lib/x86_64 -lfmod -lpthread
 * 
 * macOS:
 *   clang++ audio_manager.cpp -o audio_manager -I/path/to/fmod/api/core/inc -L/path/to/fmod/api/core/lib -lfmod
 */
