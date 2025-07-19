#include "pch.h"
#include "EmergencyAllocator.h"

DWORD WINAPI RefillThreadStatic(LPVOID param)
{
    static_cast<EmergencyAllocator*>(param)->BackgroundRefill();
    return 0;
}

EmergencyAllocator::EmergencyAllocator()
{
    for (int i = 0; i < BLOCK_COUNT; i++) {
        blocks[i] = operator new(BLOCK_SIZE, std::nothrow);
        if (blocks[i])
            printf("Slot %d initialized.\n", i);
        else
            printf("Slot %d not initialized.\n", i);
    }
    StartThread();
}

EmergencyAllocator::~EmergencyAllocator()
{
    StopThread();
    for (int i = 0; i < BLOCK_COUNT; ++i) {
        if (blocks[i]) {
            delete(blocks[i]);
            blocks[i] = nullptr;
        }
    }
    printf("EmergencyAllocator destroyed.\n");
}

void* EmergencyAllocator::Allocate(uint32_t size)
{
    void* result = operator new(size, std::nothrow);
    if (result) {
        printf("Allocated %u bytes.\n", size);
        return result;
    }

    //should never happen as we are only using it for one section of the code
    if (size != BLOCK_SIZE) {
        printf("Size %u not supported by emergency allocator.\n", size);
        return nullptr;
    }

    return AllocateFallback();
}

void* EmergencyAllocator::AllocateFallback()
{
    for (int i = 0; i < BLOCK_COUNT; ++i) {
        if (blocks[i] != nullptr) {
            void* block = blocks[i];
            blocks[i] = nullptr;
            printf("Fallback: slot %d used.\n", i);
            return block;
        }
    }

    printf("No block available!\n"); // dies in this case, RIP
    return nullptr;
}

void EmergencyAllocator::StartThread()
{
    running = true;
    threadHandle = CreateThread(nullptr, 0, RefillThreadStatic, this, 0, nullptr);
}

void EmergencyAllocator::StopThread()
{
    running = false;
    if (threadHandle) {
        WaitForSingleObject(threadHandle, INFINITE);
        CloseHandle(threadHandle);
        threadHandle = nullptr;
    }
}

void EmergencyAllocator::BackgroundRefill()
{
    while (running) {
        RefillBlocks();
        Sleep(500);
    }
}

void EmergencyAllocator::RefillBlocks()
{
    for (int i = 0; i < BLOCK_COUNT; ++i) {
        if (blocks[i] == nullptr) {
            blocks[i] = TryAllocateBlock();
            if (blocks[i])
                printf("Slot %d recreated.\n", i);
        }
    }
}

void* EmergencyAllocator::TryAllocateBlock()
{
    return operator new(BLOCK_SIZE, std::nothrow);
}