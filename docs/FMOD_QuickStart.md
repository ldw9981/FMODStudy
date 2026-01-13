# FMOD Quick Start Guide

This guide will help you get started with FMOD SoundSystem quickly.

## Table of Contents
1. [Installation](#installation)
2. [First Steps](#first-steps)
3. [Common Patterns](#common-patterns)
4. [Troubleshooting](#troubleshooting)

## Installation

### 1. Download FMOD
Visit [FMOD.com](https://www.fmod.com/download) and download the FMOD Engine for your platform.

### 2. Register (Free for Indie/Education)
Create a free account to access the download. FMOD is free for:
- Non-commercial projects
- Educational use
- Commercial projects under a certain revenue threshold (check licensing)

### 3. Install FMOD API
- **Windows**: Run the installer, note the installation path (e.g., `C:\FMOD\`)
- **Linux**: Extract to a location like `/opt/fmod/` or `~/fmod/`
- **macOS**: Extract to a location like `/Library/FMOD/` or `~/fmod/`

## First Steps

### Minimal Working Example

```cpp
#include <fmod.hpp>
#include <iostream>

int main() {
    FMOD::System* system;
    FMOD::Sound* sound;
    FMOD::Channel* channel;
    
    // Initialize
    FMOD::System_Create(&system);
    system->init(512, FMOD_INIT_NORMAL, nullptr);
    
    // Load and play
    system->createSound("test.wav", FMOD_DEFAULT, nullptr, &sound);
    system->playSound(sound, nullptr, false, &channel);
    
    // Wait for playback
    bool playing = true;
    while (playing) {
        system->update();
        channel->isPlaying(&playing);
    }
    
    // Cleanup
    sound->release();
    system->release();
    
    return 0;
}
```

### The Update Loop

**CRITICAL**: You must call `system->update()` regularly (typically every frame) for FMOD to work properly.

```cpp
// In your game/application loop
while (running) {
    // Your application logic
    
    // Update FMOD - do this every frame!
    system->update();
    
    // Render, sleep, etc.
}
```

## Common Patterns

### Pattern 1: Playing a Sound Effect

```cpp
FMOD::Sound* sfx;
FMOD::Channel* channel;

// Load once (during initialization)
system->createSound("explosion.wav", FMOD_DEFAULT, nullptr, &sfx);

// Play multiple times (when needed)
system->playSound(sfx, nullptr, false, &channel);
```

### Pattern 2: Background Music

```cpp
FMOD::Sound* music;
FMOD::Channel* musicChannel;

// Stream the music (saves memory)
system->createStream("bgm.mp3", FMOD_LOOP_NORMAL, nullptr, &music);

// Play
system->playSound(music, nullptr, false, &musicChannel);

// Control
musicChannel->setVolume(0.5f);  // 50% volume
musicChannel->setPaused(true);   // Pause
musicChannel->setPaused(false);  // Resume
```

### Pattern 3: Managing Multiple Sounds

```cpp
class AudioManager {
    FMOD::System* system;
    std::map<std::string, FMOD::Sound*> sounds;
    
public:
    void init() {
        FMOD::System_Create(&system);
        system->init(512, FMOD_INIT_NORMAL, nullptr);
    }
    
    void loadSound(const std::string& name, const std::string& path) {
        FMOD::Sound* sound;
        system->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &sound);
        sounds[name] = sound;
    }
    
    void playSound(const std::string& name) {
        if (sounds.find(name) != sounds.end()) {
            system->playSound(sounds[name], nullptr, false, nullptr);
        }
    }
    
    void update() {
        system->update();
    }
    
    void cleanup() {
        for (auto& pair : sounds) {
            pair.second->release();
        }
        system->release();
    }
};
```

### Pattern 4: Volume Categories (Channel Groups)

```cpp
FMOD::ChannelGroup* sfxGroup;
FMOD::ChannelGroup* musicGroup;

// Create groups
system->createChannelGroup("SFX", &sfxGroup);
system->createChannelGroup("Music", &musicGroup);

// Play sounds into specific groups
system->playSound(explosionSound, sfxGroup, false, nullptr);
system->playSound(bgMusic, musicGroup, false, nullptr);

// Control entire categories
sfxGroup->setVolume(0.8f);      // All SFX at 80%
musicGroup->setVolume(0.3f);    // All music at 30%
```

## Troubleshooting

### Problem: "FMOD error! (37) ERR_FILE_NOTFOUND"
**Solution**: 
- Check the file path is correct
- Use absolute paths if relative paths don't work
- Ensure the audio file exists and is readable

### Problem: "FMOD error! (20) ERR_OUTPUT_INIT"
**Solution**:
- Check audio drivers are working
- Try different initialization flags
- On Linux, ensure ALSA/PulseAudio is configured

### Problem: No sound output
**Check**:
1. Is `system->update()` being called?
2. Is the volume set correctly? (check channel volume, channel group volume)
3. Is the channel paused? (`channel->getPaused()`)
4. Is audio file format supported? (WAV, MP3, OGG are common)

### Problem: Crackling or distortion
**Solution**:
- Increase buffer size in `system->init()`
- Increase DSP buffer size with `system->setDSPBufferSize()`
- Check CPU usage

### Problem: Memory leaks
**Solution**:
- Always call `sound->release()` for each sound created
- Always call `system->release()` when done
- Don't create sounds in a loop without releasing them

## Best Practices Summary

1. **Initialize once, cleanup once**: Create the system at app start, release at app end
2. **Update every frame**: Call `system->update()` in your main loop
3. **Load once, play many**: Load sounds during initialization, play them multiple times
4. **Stream long audio**: Use `createStream()` for music and long audio
5. **Check return values**: Always check FMOD_RESULT to catch errors early
6. **Release resources**: Call `release()` on sounds when no longer needed
7. **Use channel groups**: Organize audio into categories for easier management
8. **Test on target platform**: Audio behavior can vary across platforms

## Next Steps

- Explore the example code in the `examples/` directory
- Read the [FMOD Core API documentation](https://www.fmod.com/docs/2.02/api/core-api.html)
- Experiment with effects, filters, and DSP
- Try 3D audio positioning for spatial sound
- Look into FMOD Studio for advanced audio design

## Useful FMOD Functions Reference

### System Management
```cpp
FMOD::System_Create(&system)           // Create system
system->init(512, flags, extradata)    // Initialize
system->update()                        // Update (call every frame!)
system->release()                       // Shutdown
```

### Sound Loading
```cpp
system->createSound(path, mode, info, &sound)   // Load into memory
system->createStream(path, mode, info, &sound)  // Stream from disk
sound->release()                                 // Free sound
```

### Playback
```cpp
system->playSound(sound, group, paused, &channel)  // Play
channel->setPaused(true/false)                     // Pause/unpause
channel->stop()                                    // Stop
channel->isPlaying(&playing)                       // Check if playing
```

### Volume Control
```cpp
channel->setVolume(0.0f to 1.0f)    // Set volume
channel->getVolume(&volume)          // Get volume
```

### Position
```cpp
channel->setPosition(ms, FMOD_TIMEUNIT_MS)  // Seek
channel->getPosition(&ms, FMOD_TIMEUNIT_MS) // Get position
```

Happy coding with FMOD! 🎵
