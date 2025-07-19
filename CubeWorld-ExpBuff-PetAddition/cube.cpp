#include "cube.h"
#include "pch.h"

Color ASMPrintMessage_defaultColor = Color(1.0, 1.0, 1.0, 1.0);
DWORD ASMPrintMessage_defaultColorPtr = (DWORD)&ASMPrintMessage_defaultColor;

wchar_t ASMPrintMessage_defaultMessage[1024];
DWORD ASMPrintMessage_defaultMessagePtr = (DWORD)&ASMPrintMessage_defaultMessage;

char ASMPrintMessage_msgObject[255];
DWORD ASMPrintMessage_msgObjectPtr = (DWORD)&ASMPrintMessage_msgObject;

unsigned int imageBase = 0x400000;

using namespace cube;

void cube::SetBase(unsigned int base) {
    imageBase = base;
}
unsigned int cube::Offset(unsigned int offset) {
    return imageBase + offset;
}

Color::Color(float r, float g, float b, float a)
    : red(r), green(g), blue(b), alpha(a) {
}

Block::Block(unsigned int _x, unsigned int _y, int _z,
    unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _t)
    : x(_x), y(_y), z(_z), color(_r, _g, _b, _t) {
}

Vector3_Int64::Vector3_Int64() : x(0), y(0), z(0) {}

Vector3_Int64::Vector3_Int64(long long _x, long long _y, long long _z)
    : x(_x), y(_y), z(_z) {
}

Vector3_Float::Vector3_Float() : x(0.f), y(0.f), z(0.f) {}

Vector3_Float::Vector3_Float(float _x, float _y, float _z)
    : x(_x), y(_y), z(_z) {
}

BlockColor::BlockColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _t)
    : r(_r), g(_g), b(_b), type(_t) {
}

BlockColor* cube::World::GetBlock(unsigned int x, unsigned int y, int z, Zone* zone) {
    typedef BlockColor* (__thiscall* cube_World_GetBlock_t)(cube::World*, unsigned int, unsigned int, int, cube::Zone*);
    auto cube_World_GetBlock = (cube_World_GetBlock_t)(imageBase + 0x2F7E0);
    BlockColor* color = cube_World_GetBlock(this, x, y, z, zone);
    return color;
}

cube::GameController* cube::GetGameController() {
    return (GameController*)*(DWORD*)(imageBase + 0x36B1C8);
}

noinl void GameController::ASMPrintMessage() {
    DWORD msgPtr = ASMPrintMessage_defaultMessagePtr;
    DWORD objPtr = ASMPrintMessage_msgObjectPtr;
    DWORD colPtr = ASMPrintMessage_defaultColorPtr;
    DWORD base = imageBase;

    __asm {
        push msgPtr
        mov ecx, objPtr

        mov eax, base
        add eax, 0x0EB60
        call eax // call some message constructing function

        mov ecx, base
        add ecx, 0x36B1C8
        mov ecx, dword ptr[ecx] // ecx points to GameController
        mov ecx, dword ptr[ecx + 0x800A14] // ecx points to ChatWidget

        push colPtr
        push objPtr
        mov edx, base
        add edx, 0x3AB30
        call edx // prints message

        mov ecx, objPtr
        mov eax, base
        add eax, 0x193E50
        call eax // destructor for the message object
    }
}

noinl void GameController::PrintMessage(wchar_t message[]) {
    wcsncpy_s(ASMPrintMessage_defaultMessage, message, 255);
    ASMPrintMessage_defaultColor.red = 1.0f;
    ASMPrintMessage_defaultColor.blue = 1.0f;
    ASMPrintMessage_defaultColor.green = 1.0f;
    ASMPrintMessage_defaultColor.alpha = 1.0f;
    this->ASMPrintMessage();
}

noinl void GameController::PrintMessage(wchar_t message[], int r, int g, int b) {
    wcsncpy_s(ASMPrintMessage_defaultMessage, message, 255);
    ASMPrintMessage_defaultColor.red = r / 255.0f;
    ASMPrintMessage_defaultColor.green = g / 255.0f;
    ASMPrintMessage_defaultColor.blue = b / 255.0f;
    ASMPrintMessage_defaultColor.alpha = 1.0f;
    this->ASMPrintMessage();
}

noinl Creature* GameController::GetLocalPlayer() {
    DWORD entityaddr = (DWORD)this;
    entityaddr += 0x39C;
    entityaddr = *(DWORD*)entityaddr;
    Creature* player = (Creature*)entityaddr;
    return player;
}

noinl Block* GameController::GetBlockAtCrosshair(float reach_in_blocks_from_camera, bool want_face_block) {
    Creature* player = this->GetLocalPlayer();
    // Calculate the direction the camera is in
    float yaw = degrees_to_radians(this->cameraYaw + 90.0f);
    float pitch = degrees_to_radians(this->cameraPitch);

    float camera_x_direction = cosf(yaw) * sinf(pitch);
    float camera_y_direction = sinf(yaw) * sinf(pitch);
    float camera_z_direction = -cosf(pitch);

    // Calculate where the camera is, relative to the player
    float blocks_away_from_player_x = camera_x_direction * this->cameraZoom;
    float blocks_away_from_player_y = camera_y_direction * this->cameraZoom;
    float blocks_away_from_player_z = camera_z_direction * this->cameraZoom;

    // Get the absolute location of the camera, in world units (not blocks)
    float VERTICAL_OFFSET = player->physical_size / 2.0f + 0.5f; // Calculation is ~1 block too low otherwise.
    const float SIZE_OF_BLOCK_IN_WORLD_UNITS = 65536.0f;

    auto camera_x = player->x + (long long int)(blocks_away_from_player_x * SIZE_OF_BLOCK_IN_WORLD_UNITS);
    auto camera_y = player->y + (long long int)(blocks_away_from_player_y * SIZE_OF_BLOCK_IN_WORLD_UNITS);
    auto camera_z = player->z + (long long int)((blocks_away_from_player_z + VERTICAL_OFFSET) * SIZE_OF_BLOCK_IN_WORLD_UNITS);

    unsigned int blockx = camera_x / 0x10000;
    unsigned int blocky = camera_y / 0x10000;
    unsigned int blockz = camera_z / 0x10000;

    unsigned int lastblockx = blockx;
    unsigned int lastblocky = blocky;
    unsigned int lastblockz = blockz;

    float reach_limit = SIZE_OF_BLOCK_IN_WORLD_UNITS * reach_in_blocks_from_camera;
    float raycast_precision = 1000.0f;
    bool withinReach = false;

    for (float world_units_traveled = 0.0f; world_units_traveled < reach_limit; world_units_traveled += raycast_precision) {
        lastblockx = blockx;
        lastblocky = blocky;
        lastblockz = blockz;

        blockx = camera_x / 0x10000;
        blocky = camera_y / 0x10000;
        blockz = camera_z / 0x10000;

        BlockColor* color = this->world.GetBlock(blockx, blocky, blockz, nullptr);
        if ((color->type & 0b00111111) != 0) {
            withinReach = true;
            if (want_face_block) {
                blockx = lastblockx;
                blocky = lastblocky;
                blockz = lastblockz;
            }
            break;
        }

        camera_x -= (long long int)(camera_x_direction * raycast_precision);
        camera_y -= (long long int)(camera_y_direction * raycast_precision);
        camera_z -= (long long int)(camera_z_direction * raycast_precision);
    }

    if (withinReach) {
        BlockColor* color = this->world.GetBlock(blockx, blocky, blockz, nullptr);
        Block* block = new Block(blockx, blocky, blockz, color->r, color->g, color->b, color->type);
        return block;
    }
    else {
        return nullptr;
    }
}

noinl std::vector<Creature*>* GameController::GetCreatures() {
    std::vector<Creature*>* creatures = new std::vector<Creature*>;
    unsigned int map_ptr = this->world.map_ptr;
    unsigned int node_ptr = *(unsigned int*)map_ptr;

    typedef void(__thiscall* mapnext_t)(unsigned int* node_ptr);
    auto mapnext = (mapnext_t)(imageBase + 0x1C3EA0);

    while (node_ptr != map_ptr) {
        unsigned int creature_ptr_ptr = node_ptr + 0x18;
        unsigned int creature_ptr = *(unsigned int*)creature_ptr_ptr;
        cube::Creature* creature = (cube::Creature*)creature_ptr;

        creatures->push_back(creature);
        mapnext(&node_ptr);
    }
    return creatures;
}
