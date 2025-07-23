#include "uni_util.h"
#ifndef MEMORY_MODULE
#define MEMORY_MODULE
#define MODULE_NAME "MEMORY"

#define mem_unit uint8_t 
#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define ALIGN_TO(size, allign) (((size) + (allign - 1)) & ~(allign - 1))
#define ALIGN_DOWN(size) ((size) & ~(ALIGNMENT - 1))
#define FLOOR_TO(value, chunk) ((value) & ~(chunk - 1))
#define ptr_get_arena(ptr, arena)
#define MemoryCanAlloc(arena, byte_size)(bool_s8)((arena.size + (byte_size)) < arena.alloc_size)

#define ptr_in_buffer(address, arena) (InRange(address, arena.buffer, arena.buffer + arena.alloc_size))
#define MB(_mb) (ALIGN((size_t)((float)(_mb) * (size_t)1250000)))
#define GB(_gb) (ALIGN((size_t)((float)(_gb) * 1e+9)))
#define BYTE_TO_MB(_byte)((double)((double)(_byte) / (double)1250000))
typedef struct {
  mem_unit* buffer;
  size_t size;
  size_t alloc_size;
  size_t occupaid_size;
}MemoryFragment;
typedef struct {
  mem_unit* buffer;
  size_t size;
  size_t alloc_size;

  MemoryFragment* mem_frag;
  size_t frag_size;
}InternalMemoryArena;
typedef InternalMemoryArena ArenaC;
#ifndef HEAP_ARENA_SIZE
#define HEAP_ARENA_SIZE MB(0.2)
#endif
typedef struct {
  mem_unit buffer[HEAP_ARENA_SIZE];
  size_t size;
  const size_t alloc_size;
}StackMemoryArena;
#define STACK_MEMORY_INIT(name)StackMemoryArena name = {.buffer = {0}, .size = 0, .alloc_size = HEAP_ARENA_SIZE };
static void* MemoryRequest(size_t byte_size, InternalMemoryArena* arena) {
  assertf(arena->size + byte_size < arena->alloc_size, "memory arena doesnt have enought for this request, need to add handle\n");

  byte_size = ALIGN(byte_size + 1);
  mem_unit* _ret = (mem_unit*)(arena->buffer + arena->size);
  arena->size += (size_t)(byte_size + (size_t)1);
  arena->buffer[byte_size] = '\0';
  return (void*)_ret;
}
static void* MemoryReserve(size_t byte_size,InternalMemoryArena* arena) {
  mem_unit* _ret = NULL;
  if(arena->size+ byte_size < arena->alloc_size)
    _ret = (mem_unit*)(arena->buffer + arena->size);
  return (void*)_ret;
}
static void MemoryAddSize(size_t byte_size, InternalMemoryArena* arena) {
  arena->size += byte_size;
}
typedef size_t mem_id ;
typedef mem_id aptr;
static aptr MemoryRequestID(size_t byte_size, InternalMemoryArena* arena) {
  assertf(arena->size + byte_size < arena->alloc_size, "memory arena doesnt have enought for this request, need to add handle\n");

  byte_size = ALIGN(byte_size + 1);
  //mem_unit* _ret = (mem_unit*)(arena->buffer + arena->size);
  aptr _ret = (aptr)arena->size;
  arena->size += (size_t)(byte_size + (size_t)1);
  arena->buffer[byte_size] = '\0';
  return _ret;
}
#define IndexArena(arena, id, type) (*((type *)(arena.buffer + ((aptr)id))))

void* Memory_GetFragment(MemoryFragment* sub, size_t byte_size) {
  byte_size = ALIGN(byte_size);
  mem_unit* _ret = (mem_unit*)(sub->buffer + sub->size);
  memset(sub->buffer + sub->size, 0, byte_size);
  sub->size += (size_t)(byte_size);
  sub->occupaid_size += (size_t)(byte_size);
  return (void*)_ret;
}
static void* MemoryRequestFragment(size_t byte_size, InternalMemoryArena* arena) {
  void* nptr = NULL;
  loop(i, arena->frag_size) {
    if (MemoryCanAlloc(arena->mem_frag[i], byte_size))
      return Memory_GetFragment(&arena->mem_frag[i], byte_size);
  }
  return nptr;
}
static void* MemoryRequestNA(size_t byte_size, InternalMemoryArena* arena) {
  mem_unit* _ret = (mem_unit*)(arena->buffer + arena->size);
  arena->size += (size_t)(byte_size + (size_t)1);
  arena->buffer[byte_size] = '\0';
  return (void*)_ret;
}
static MemoryFragment Memory_SubSplit(InternalMemoryArena arena, size_t allocated_size, size_t offset) {
  MemoryFragment sub_arena = {
    .buffer = (mem_unit*)((uintptr_t)arena.buffer + offset),
    .size = 0,
    .alloc_size = allocated_size ,
    .occupaid_size = 0
  };
  return sub_arena;
}
static void MemoryArenaFragment(InternalMemoryArena* arena, size_t frag_size) {
  //InternalMemoryArena arena = { 0 };
  if (arena != NULL && arena->alloc_size > ALIGNMENT) {
    arena->frag_size = frag_size;
    u32 fragments = arena->alloc_size / frag_size;
    arena->mem_frag = (MemoryFragment*)calloc(fragments, sizeof(MemoryFragment));
    loop(i, fragments) {
      arena->mem_frag[i] = Memory_SubSplit(*arena, frag_size, frag_size * i);
    }
    ShowMessageModule(MODULE_NAME, ML_INFO, TextFormat("Memory Arena Fragmented into %d fragments of size : %llu ", fragments, frag_size));
  }
}
static InternalMemoryArena MemoryArenaInit(size_t allocated_size) {
  InternalMemoryArena arena = { 0 };
  arena.alloc_size = allocated_size;
  arena.buffer = (mem_unit*)calloc(allocated_size, sizeof(mem_unit));
  //arena.buffer_end = (mem_unit*)((arena.buffer + allocated_size) - 1);
  ShowMessageModule(MODULE_NAME, ML_INFO, TextFormat("Memory Arena created with size %lf mb ", BYTE_TO_MB(allocated_size * sizeof(mem_unit))));

  return arena;
}
static void MemoryArenaClean(InternalMemoryArena* arena) {
  ZeroMemory_Util(arena->buffer, arena->size);
  arena->size = 0;
}
static void MemoryArenaCleanFragments(InternalMemoryArena* arena) {
  if (arena->mem_frag == NULL) {
    MemoryArenaClean(arena);
    return;
  }
  loop(i, arena->frag_size) {
    if (arena->mem_frag[i].occupaid_size <= 0) {
      arena->mem_frag[i].size = 0;
      ZeroMemory_Util(arena->mem_frag[i].buffer, arena->mem_frag[i].alloc_size);
    }
  }
}

static InternalMemoryArena Memory_Split(InternalMemoryArena arena, size_t allocated_size) {
  InternalMemoryArena sub_arena = {
    .buffer = (mem_unit*)MemoryRequest(allocated_size,&arena),
    .size = 0,
    .alloc_size = allocated_size
  };
  return sub_arena;
}
//STACK_MEMORY_INIT(memory_ephemeral);
static InternalMemoryArena memory_ephemeral = {0};
static void MemoryInitEphemeral(size_t allocated_size) {
  memory_ephemeral.alloc_size = allocated_size;
  memory_ephemeral.buffer = (mem_unit*)calloc(allocated_size, sizeof(mem_unit));
  ShowMessageModule(MODULE_NAME, ML_INFO, TextFormat("%lf mb allocated to Ephemeral Memory", BYTE_TO_MB(memory_ephemeral.alloc_size * sizeof(mem_unit))));
}
#define REQUEST_THE_END 0
static void* MemoryRequestEphemeral(size_t byte_size) {
  //byte_size = ALIGN(byte_size);
  mem_unit* _ret = NULL;
  assertf(memory_ephemeral.size + byte_size < memory_ephemeral.alloc_size, "Ephemeral memory doesnt have enought for this request, need to add handle\n");

  _ret = (mem_unit*)(memory_ephemeral.buffer + memory_ephemeral.size);
  memory_ephemeral.size += (size_t)(byte_size + (size_t)1);
  memory_ephemeral.buffer[memory_ephemeral.size] = '\0';
#if REQUEST_THE_END
  memory_ephemeral.buffer[memory_ephemeral.size++] = 'e';
  memory_ephemeral.buffer[memory_ephemeral.size++] = 'n';
  memory_ephemeral.buffer[memory_ephemeral.size++] = 'd';
  memory_ephemeral.buffer[memory_ephemeral.size++] = '\0';

#endif

  return (void*)_ret;
}
static void* MemoryReserveEphemeral(size_t byte_size) {
  mem_unit* _ret = NULL;
  assertf(memory_ephemeral.size + byte_size < memory_ephemeral.alloc_size, "Ephemeral memory doesnt have enought for this reserve, need to add handle\n");
 // if (memory_ephemeral.size + byte_size < memory_ephemeral.alloc_size)
  _ret = (mem_unit*)(memory_ephemeral.buffer + memory_ephemeral.size);
  return (void*)_ret;
}
static void MemoryEndReserveEphemeral(size_t byte_size) {
  memory_ephemeral.size += byte_size;
}
static void MemoryCleanEphemeral() {
  ZeroMemory_Util(memory_ephemeral.buffer, memory_ephemeral.size);
  memory_ephemeral.size = 0;
}
static size_t MemoryGetSizeEphemeral() {
  return memory_ephemeral.size;
}
void Memory_SubFree(MemoryFragment* sub, size_t byte_size) {
  sub->occupaid_size -= byte_size;
  if (sub->occupaid_size <= 0) {
    sub->size = 0;
    memset(sub->buffer, 0, sub->alloc_size);
  }
}


#undef MODULE_NAME
#endif 