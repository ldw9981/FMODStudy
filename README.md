# FMODStudy - FMOD SoundSystem Tutorial

This repository contains examples and documentation for using the FMOD SoundSystem API.

## What is FMOD?

FMOD is a powerful audio middleware solution used in games, applications, and interactive media. FMOD Studio provides a comprehensive audio engine with features like:

- 3D sound positioning
- Real-time audio mixing
- Sound effects and filters
- Streaming audio
- Low-level audio control

## Prerequisites

Before using FMOD, you need to:

1. Download FMOD Engine from [FMOD.com](https://www.fmod.com/)
2. Register for a free license (indie/educational use)
3. Install the FMOD API for your platform

## Getting Started

### Basic FMOD System Setup

The core of FMOD is the `FMOD::System` object. Here's the basic initialization flow:

```cpp
#include <fmod.hpp>
#include <fmod_errors.h>

// 1. Create the system object
FMOD::System* system = nullptr;
FMOD::System_Create(&system);

// 2. Initialize the system
system->init(512, FMOD_INIT_NORMAL, nullptr);

// 3. Use the system (load sounds, play audio, etc.)

// 4. Clean up
system->release();
```

### Key Concepts

#### 1. **System Object**
The main interface to FMOD. Handles audio device management, channel allocation, and overall audio system control.

#### 2. **Sound Object**
Represents an audio file loaded into memory or set up for streaming. Sounds can be:
- **Sample-based**: Fully loaded into memory (good for short sounds)
- **Streaming**: Streamed from disk (good for music/long audio)

#### 3. **Channel Object**
Represents a playing sound instance. Controls playback, volume, pan, effects, etc.

#### 4. **Channel Groups**
Organize channels together for collective control (useful for volume categories like SFX, Music, Voice).

## Common Use Cases

### 1. Playing a Simple Sound Effect

```cpp
FMOD::Sound* sound = nullptr;
FMOD::Channel* channel = nullptr;

// Load sound
system->createSound("explosion.wav", FMOD_DEFAULT, nullptr, &sound);

// Play sound
system->playSound(sound, nullptr, false, &channel);

// Cleanup when done
sound->release();
```

### 2. Streaming Background Music

```cpp
FMOD::Sound* music = nullptr;
FMOD::Channel* musicChannel = nullptr;

// Create stream
system->createStream("background_music.mp3", FMOD_LOOP_NORMAL, nullptr, &music);

// Play music
system->playSound(music, nullptr, false, &musicChannel);

// Cleanup when done
music->release();
```

### 3. Adjusting Volume

```cpp
// Set volume (0.0 = silent, 1.0 = full volume)
channel->setVolume(0.5f);

// Get current volume
float volume;
channel->getVolume(&volume);
```

### 4. 3D Positional Audio

```cpp
// Set 3D attributes for listener (camera/player)
FMOD_VECTOR listenerPos = {0, 0, 0};
FMOD_VECTOR listenerVel = {0, 0, 0};
FMOD_VECTOR listenerForward = {0, 0, 1};
FMOD_VECTOR listenerUp = {0, 1, 0};

system->set3DListenerAttributes(0, &listenerPos, &listenerVel, &listenerForward, &listenerUp);

// Create 3D sound
FMOD::Sound* sound3D = nullptr;
system->createSound("footstep.wav", FMOD_3D, nullptr, &sound3D);

// Set 3D position for sound
FMOD_VECTOR soundPos = {10, 0, 0};
FMOD_VECTOR soundVel = {0, 0, 0};
channel->set3DAttributes(&soundPos, &soundVel);
```

## Project Structure

```
FMODStudy/
├── README.md               # This file
├── examples/              # Example code
│   ├── basic_playback.cpp
│   ├── streaming.cpp
│   └── 3d_audio.cpp
├── docs/                  # Additional documentation
│   └── FMOD_QuickStart.md
└── assets/               # Sample audio files (not included)
```

## Important Functions Reference

### System Functions
- `FMOD::System_Create()` - Create system object
- `system->init()` - Initialize audio system
- `system->update()` - Update system (call every frame)
- `system->createSound()` - Load sound into memory
- `system->createStream()` - Create streaming sound
- `system->playSound()` - Play a sound
- `system->release()` - Shutdown system

### Channel Functions
- `channel->setPaused()` - Pause/unpause
- `channel->setVolume()` - Set volume
- `channel->setPitch()` - Adjust pitch
- `channel->isPlaying()` - Check if playing
- `channel->stop()` - Stop playback

### Sound Functions
- `sound->getLength()` - Get sound duration
- `sound->setLoopCount()` - Set loop count
- `sound->release()` - Free sound resource

## Best Practices

1. **Call `system->update()` regularly** - Typically once per frame in your main loop
2. **Check return codes** - Always check FMOD function return values for errors
3. **Release resources** - Call `release()` on sounds and system when done
4. **Use streaming for long audio** - Music and ambient sounds should be streamed
5. **Use channel groups** - Organize audio into categories for easier management

## Error Handling

```cpp
FMOD_RESULT result = system->init(512, FMOD_INIT_NORMAL, nullptr);
if (result != FMOD_OK) {
    printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
    // Handle error
}
```

## Building with FMOD

### Windows (Visual Studio)
1. Add FMOD include directory to project settings
2. Add FMOD lib directory to linker settings
3. Link against `fmod_vc.lib`
4. Copy `fmod.dll` to output directory

### Linux
1. Install FMOD API
2. Add include path: `-I/path/to/fmod/api/core/inc`
3. Add library path: `-L/path/to/fmod/api/core/lib/x86_64`
4. Link: `-lfmod`

### macOS
Similar to Linux, adjust paths for macOS SDK location.

## Resources

- [FMOD Official Documentation](https://www.fmod.com/docs)
- [FMOD API Reference](https://www.fmod.com/docs/2.02/api/core-api.html)
- [FMOD Examples](https://www.fmod.com/download#fmodengine)
- [FMOD Community Forum](https://qa.fmod.com/)

## License

FMOD is free for non-commercial use. For commercial projects, check [FMOD Licensing](https://www.fmod.com/licensing).

## Contributing

Feel free to add more examples and documentation to help others learn FMOD!
