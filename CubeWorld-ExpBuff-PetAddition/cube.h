#ifndef CUBE_H
#define CUBE_H

#define noinl __declspec(noinline)
#define no_shenanigans noinl __declspec(dllexport)

#include <math.h>
#include <vector>
#include <iostream>
#include <windows.h>

// code from core_maxe adapted for MCVS

class Color {
public:
    float red, green, blue, alpha;
    Color(float r = 0.f, float g = 0.f, float b = 0.f, float a = 0.f);
};

extern "C" {
    extern Color ASMPrintMessage_defaultColor;
    extern DWORD ASMPrintMessage_defaultColorPtr;

    extern wchar_t ASMPrintMessage_defaultMessage[1024];
    extern DWORD ASMPrintMessage_defaultMessagePtr;

    extern char ASMPrintMessage_msgObject[255];
    extern DWORD ASMPrintMessage_msgObjectPtr;

    extern unsigned int imageBase;
}

inline float degrees_to_radians(float degrees) {
    return (degrees * 3.1415926535f) / 180.0f;
}

inline float radians_to_degrees(float radians) {
    return (radians * 180.0f) / 3.1415926535f;
}

// Vector and color classes
class Vector3_Int64 {
public:
    long long x, y, z;
    Vector3_Int64();
    Vector3_Int64(long long _x, long long _y, long long _z);
};

class Vector3_Float {
public:
    float x, y, z;
    Vector3_Float();
    Vector3_Float(float _x, float _y, float _z);
};

class BlockColor {
public:
    unsigned char r, g, b, type;
    BlockColor(unsigned char _r = 0, unsigned char _g = 0, unsigned char _b = 0, unsigned char _t = 0);
};

class Block {
public:
    unsigned int x, y;
    int z;
    BlockColor color;

    Block(unsigned int _x, unsigned int _y, int _z,
        unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _t);
};

// External variables from ASM or game memory
extern "C" {
    extern Color ASMPrintMessage_defaultColor;
    extern DWORD ASMPrintMessage_defaultColorPtr;

    extern wchar_t ASMPrintMessage_defaultMessage[1024];
    extern DWORD ASMPrintMessage_defaultMessagePtr;

    extern char ASMPrintMessage_msgObject[255];
    extern DWORD ASMPrintMessage_msgObjectPtr;

    extern unsigned int imageBase;
}

namespace cube {

    void SetBase(unsigned int base);
    unsigned int Offset(unsigned int offset);

    class Zone;

    class Chunk {
    public:
        char padding0[0x18];
        unsigned int x;
        unsigned int y;
        char padding2[0x54];
        uint8_t needs_update;
        char padding1[0x1F3];
    };

    class Creature {
    public:
        unsigned int field_0;
        unsigned int field_4;
        long long GUID;
        unsigned long long x, y, z;
        char padding2[0x60];
        float physical_size;
        char padding1[0xD4];
        Vector3_Float camera_offset;
        float HP, MP, block_power;
        float HP_multiplier, attack_speed_multiplier, damage_multiplier;
        float armor_multiplier, resistance_multiplier;
        char field_18C, field_18D, field_18E, field_18F;
        int level;
        int XP;
        long long parent_GUID;
    };

    class World {
    public:
        unsigned int field_0;
        unsigned int map_ptr;
        char padding0[0x8C];
        char worldName[0x10];
        char padding1[0x80001C];
        unsigned int critical_section;

        void SetBlock(unsigned int x, unsigned int y, int z, BlockColor* color, Zone* zone);
        void SetBlock(unsigned int x, unsigned int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char type);
        BlockColor* GetBlock(unsigned int x, unsigned int y, int z, Zone* zone);

        void Lock();
        void Unlock();
    };

    class GameController {
    public:
        unsigned int field_0;
        uint8_t M1, M2, key_1, key_2, key_3, key_4, M3;
        uint8_t key_W, key_S, key_A, key_D, key_R, key_T, key_E;
        uint8_t key_Space, key_Ctrl, key_LeftShift, key_LeftShift2;
        uint8_t field_16, key_Q, field_18, field_19;
        char padding0[0x156];
        unsigned int isFullscreen;
        unsigned int width, height;
        unsigned int antialiasingLevel, renderDistance;
        unsigned int soundFXVolume, musicVolume;
        float cameraSpeed, cameraSmoothness;
        unsigned int invertYAxis, language;
        unsigned int miliseconds_per_frame, shutdown;
        float cameraPitch, cameraRoll, cameraYaw;
        float destinationCameraPitch, destinationCameraRoll, destinationCameraYaw;
        float cameraZoom, destinationCameraZoom;
        char padding1[0x114];
        unsigned int field_2D8;
        unsigned int chunk_array_dimensions;
        Chunk* chunks;
        World world;

        void UpdateChunk(unsigned int chunk_x, unsigned int chunk_y);

        void ASMPrintMessage();
        void PrintMessage(wchar_t message[]);
        void PrintMessage(wchar_t message[], int r, int g, int b);

        Creature* GetLocalPlayer();
        Block* GetBlockAtCrosshair(float reach_in_blocks_from_camera, bool want_face_block);

        std::vector<Creature*>* GetCreatures();
    };

    GameController* GetGameController();

    class Zone {
    public:
        char padding0[0x60];
        unsigned int x;
        unsigned int y;
    };

} // namespace cube

#endif