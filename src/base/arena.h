#ifndef BASE_ARENA_H
#define BASE_ARENA_H

#include "core.h"
#include "string.h"

// TODO: Would it be a good idea to add some extra space to the arena just in case it faild to then be able to recover, cause we might need some mem still 
struct Arena {
  U8* base_p;
  
  Str8 name; 
  U64 offset_for_meta_data;

  U64 mem_reserved;
  U64 mem_commited;
  U64 mem_used;
};
#define ArenaStaticMetaDataSize (sizeof(Arena))
StaticAssert(ArenaStaticMetaDataSize >= sizeof(Arena), "Arena meta data doesnt capture the size of the arena.");

#define ArenaDefaultCommitSize Kilobytes_U64(64)

// TODO: Need a way to initialise this with a specific set of parameter for commit, reserve ...
Arena* arena_alloc(U64 n_bytes_to_allocate, const char* name);
void arena_clear(Arena* arena);
void arena_release(Arena* arena);

// TODO: Use asan poison/unpoison markup for arena allocations
U8* _arena_push_no_zero(Arena* arena, U64 n_bytes_to_push);
U8* _arena_push(Arena* arena, U64 n_bytes_to_push);
#define ArenaPushNoZero(arena_p, Type)           (Type*) _arena_push_no_zero(arena_p, sizeof(Type))
#define ArenaPushArrNoZero(arena_p, Type, count) (Type*) _arena_push_no_zero(arena_p, count * sizeof(Type))
#define ArenaPush(arena_p, Type)                 (Type*) _arena_push(arena_p, sizeof(Type)) 
#define ArenaPushArr(arena_p, Type, count)       (Type*) _arena_push(arena_p, count * sizeof(Type))

void arena_pop(Arena* arena, U64 n_bytes_to_pop);
B32 arena_is_clear(Arena* arena);

U8* arena_first_usable_byte(Arena* arena);

///////////////////////////////////////////////////////////
// Damian: Scratch arenas stuff
//
struct Temp_arena {
  Arena* arena;
  U64 start_pos;
};

Temp_arena temp_arena_begin(Arena* arena);
void temp_arena_end(Temp_arena* temp);






#endif