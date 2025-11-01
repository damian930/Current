#ifndef BASE_ARENA_H
#define BASE_ARENA_H

#include "core.h"
#include "string.h"

struct Arena {
  U8* base_p;
  
  Str8 name; 
  U64 offset_for_meta_data;
  U64 mem_used;
  U64 mem_allocated;
};
#define ArenaMetaDataSize (sizeof(Arena))
StaticAssert(ArenaMetaDataSize >= sizeof(Arena), "Arena meta data doesnt capture the size of the arena.");

Arena* arena_alloc(U64 n_bytes_to_allocate, const char* name);
void arena_clear(Arena* arena);
void arena_release(Arena* arena);

U8* arena_first_usable_byte(Arena* arena);

U8* _arena_push_no_zero(Arena* arena, U64 n_bytes_to_push);
U8* _arena_push(Arena* arena, U64 n_bytes_to_push);
#define ArenaPushNoZero(arena_p, Type)           (Type*) _arena_push_no_zero(arena_p, sizeof(Type))
#define ArenaPushArrNoZero(arena_p, Type, count) (Type*) _arena_push_no_zero(arena_p, count * sizeof(Type))
#define ArenaPush(arena_p, Type)                 (Type*) _arena_push(arena_p, sizeof(Type)) 
#define ArenaPushArr(arena_p, Type, count)       (Type*) _arena_push(arena_p, count * sizeof(Type))

void arena_pop(Arena* arena, U64 n_bytes_to_pop);

///////////////////////////////////////////////////////////
// Damian: Scratch arenas stuff
//
struct Temp_arena {
  Arena* arena;
  U64 start_pos;
};

Temp_arena temp_arena_begin(Arena* arena);
void temp_arena_end(Temp_arena* temp);

///////////////////////////////////////////////////////////
// Damian: Globals 
//
#define NumberOfScratchArenas 2
extern Arena* scratch_arenas[NumberOfScratchArenas]; // TODO: This might not be great here 
extern U32 current_scratch_index;

typedef Temp_arena Scratch;
Scratch get_scratch();
void end_scratch(Scratch* scratch);
    
#endif





