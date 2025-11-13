#ifndef BASE_H
#define BASE_H

#include <inttypes.h> // For int types
#include <stdlib.h>   // ...
#include <stdio.h>    // Printf
#include <string.h>   // For memset and stuff

// TODO: Add rjd/martins like preprocesor define for platforms, bitnes, debug/release ...

typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef float  F32;
typedef double F64;

typedef S8 B8;
typedef S16 B16;
typedef S32 B32;
typedef S64 B64;

#define global        static
#define local_persist static
#define local         static
#define file_private  static

#define Null NULL

#define DefereLoop(start_exp, end_exp) for(U64 __dli = ((start_exp), 0); __dli < 1; __dli += 1, (end_exp))
#define ForEach(it_name, arr)          for (U64 it_name = 0; it_name < ArrayCount(arr); it_name += 1)
#define ForEachEx(it_name, count, arr) for (U64 it_name = 0; it_name < count; it_name += 1)

#define ForEnum_Name(it_name, Enum_name, enum_last_value_name) for (U64 it_name = (Enum_name)0; it_name < enum_last_value_name; it_name += 1)
#define ForEachEnum(it_name, Enum_name)  ForEnum_Name(it_name, Enum_name, Enum_name##_COUNT)

#define Assert(expr, ...) do {} while(false) 
#if DEBUG_MODE 
	#undef Assert
	#define Assert(expr, ...) do { if (!(expr)) {*((int*)0) = 69;} } while(false)
#endif
#define AssertNote(expr, ...) Assert(expr);

#define StaticAssert(expr, text) static_assert((expr), text)

// Damian: (...) here are to be able to put anything it. Usually used for string notes.
#define DebugStopHere(...)  __debugbreak() // TODO: Fix this, this is msvc compiler specific
#define NotImplemented(...)  Assert(false) // TODO: This might even have to be an exit insted of assert
#define InvalidCodePath(...) Assert(false)

#define ArrayCount(arr) (sizeof(arr) / sizeof(arr[0])) 

#define SwapVaues(Type, value1, value2) do { Type temp = value1; value1 = value2; value2 = temp; } while(false);
#define SwapPointers(Type, value1, value2) do { Type temp = *value1; *value1 = *value2; *value2 = temp; } while(false);
#define ToggleBool(bool_name) do { bool_name = !bool_name; } while(false)

#define Bytes_U64(value)     ((U64)value)
#define Megabytes_U64(value) (1024 * Kilobytes_U64(value))
#define Kilobytes_U64(value) (1024 * Bytes_U64(value))
#define Gigabytes_U64(value) (1024 * Megabytes_U64(value))
#define Terabytes_U64(value) (1024 * Gigabytes_U64(value))

#define MemCopy(mem_dst_p, mem_src_p, n_bytes_u64) { memcpy(mem_dst_p, mem_src_p, n_bytes_u64); }
#define MemSet(mem_p, value_s32, n_bytes_u64)      { memset((void*)mem_p, value_s32, n_bytes_u64); } 

#define StringLine(str) str " \n"
#define StringNewL "\n"

#define MemberOffset(Type, member) ( (U64)(&(((Type*)0)->member)) - (U64)((Type*)0) )

/* NOTES:
  Stack here is a list that only has the "first" node pointer. Nodes only have the "next" pointer.
  When pushed onto the stack list, the first element is the new node, and the old first is now next for the new node.

  Queue is a list that has the "first" and "last" node pointers.
  Nodes only store the "next" node pointer.

  Dll is a list with "first" and "last" node pointers.
  Nodes store the "next" and "prev" node pointers.
*/
#define StackPush_Name(list, new_node, name_for_first_in_list, name_for_next_in_node) \
            if ((list)->name_for_first_in_list == 0) {     \
                (list)->name_for_first_in_list = new_node; \
            } else {                                     \
                (new_node)->name_for_next_in_node = (list)->name_for_first_in_list; \
                (list)->name_for_first_in_list = new_node;                        \
            }
#define StackPop_Name(list, name_for_first, name_for_next) \
				   if (list->name_for_first) { list->name_for_first = list->name_for_first->name_for_next; }

#define QueuePushFront_Name(queue, new_node, name_for_first_in_queue, name_for_last_in_queue, name_for_next_in_node) \
            if (queue->name_for_first_in_queue == 0) {     \
                queue->name_for_first_in_queue = new_node; \
                queue->name_for_last_in_queue = new_node;  \
            } else {                                       \
                new_node->name_for_next_in_node = list->name_for_first_in_queue; \
                queue->name_for_first_in_queue = new_node;                       \
            }

#define QueuePushBack_Name(queue, new_node, name_for_first_in_queue, name_for_last_in_queue, name_for_next_in_node) \
            if (queue->name_for_first_in_queue == 0) {     \
                queue->name_for_first_in_queue = new_node; \
                queue->name_for_last_in_queue = new_node;  \
            } else {                                       \
               queue->name_for_last_in_queue->name_for_next_in_node = new_node; \
               queue->name_for_last_in_queue = new_node;                        \
            }

#define DllPushBack_Name(dll_p, new_node_p, name_for_first_in_dll, name_for_last_in_dll, name_for_next_in_node, name_for_prev_in_node) \
						if (   dll_p->name_for_first_in_dll == Null                             \
								&& dll_p->name_for_last_in_dll == Null                              \
						) { 									 												                          \
							dll_p->name_for_first_in_dll = new_node_p;                            \
							dll_p->name_for_last_in_dll = new_node_p; 													  \
						} 																							 											  \
						else if (dll_p->name_for_first_in_dll == dll_p->name_for_last_in_dll) { \
							dll_p->name_for_first_in_dll->name_for_next_in_node = new_node_p;     \
							new_node_p->name_for_prev_in_node = dll_p->name_for_first_in_dll;     \
							dll_p->name_for_last_in_dll = new_node_p; 												 		\
						} 											 												 												\
						else { 									 												 												\
							dll_p->name_for_last_in_dll->name_for_next_in_node = new_node_p; 			\
							new_node_p->name_for_prev_in_node = dll_p->name_for_last_in_dll; 		  \
 							dll_p->name_for_last_in_dll = new_node_p;  											 			\
						} 

#define DllPushFront_Name(dll_p, new_node_p, name_for_first_in_dll, name_for_last_in_dll, name_for_next_in_node, name_for_prev_in_node) \
						if (   dll_p->name_for_first_in_dll == Null                             \
								&& dll_p->name_for_last_in_dll  == Null                             \
						) {                                                                     \
								dll_p->name_for_first_in_dll = new_node_p;                          \
								dll_p->name_for_last_in_dll  = new_node_p;                          \
						}                                                                       \
						else if (dll_p->name_for_first_in_dll == dll_p->name_for_last_in_dll) { \
								new_node_p->name_for_next_in_node = dll_p->name_for_last_in_dll;    \
								dll_p->name_for_last_in_dll->name_for_prev_in_node = new_node_p;    \
								dll_p->name_for_first_in_dll = new_node_p;                          \
						}                                                                       \
						else {                                                                  \
								new_node_p->name_for_next_in_node = dll_p->name_for_first_in_dll;   \
								dll_p->name_for_first_in_dll->name_for_prev_in_node = new_node_p;   \
								dll_p->name_for_first_in_dll = new_node_p;                          \
						}

#define DllPopNode_Name(dll_p, node_p, name_for_first_in_dll, name_for_last_in_dll, name_for_next_in_node, name_for_prev_in_node) \
						if (   dll_p->name_for_first_in_dll == dll_p->name_for_last_in_dll                        \
								&& dll_p->name_for_first_in_dll == node_p                                             \
						) {                                                                                       \
								dll_p->name_for_first_in_dll = Null;                                                  \
								dll_p->name_for_last_in_dll  = Null;                                                  \
						}                                                                                         \
						else if (dll_p->name_for_first_in_dll == node_p) {                                        \
								dll_p->name_for_first_in_dll = dll_p->name_for_first_in_dll->name_for_next_in_node;   \
								dll_p->name_for_first_in_dll->name_for_prev_in_node = Null;                           \
						}                                                                                         \
						else if (dll_p->name_for_last_in_dll == node_p) {                                         \
								dll_p->name_for_last_in_dll = dll_p->name_for_last_in_dll->name_for_prev_in_node;     \
								dll_p->name_for_last_in_dll->name_for_next_in_node = Null;                            \
						}                                                                                         \
						else {                                                                                    \
								node_p->name_for_prev_in_node->name_for_next_in_node = node_p->name_for_next_in_node; \
								node_p->name_for_next_in_node->name_for_prev_in_node = node_p->name_for_prev_in_node; \
						}

#define StackPush(list, new_node) StackPush_Name((list), (new_node), first, next)
#define StackPop(list)            StackPop_Name(list, first, next)

#define QueuePushFront(list, new_node) QueuePushFront_Name((list), (new_node), first, last, next)
#define QueuePushBack(list, new_node)  QueuePushBack_Name((list), (new_node), first, last, next)

#define DllPushFront(list_p, new_node_p) DllPushFront_Name((list_p), (new_node_p), first, last, next, prev)
#define DllPushBack(list_p, new_node_p)  DllPushBack_Name((list_p), (new_node_p), first, last, next, prev)
#define DllPopNode(list_p, node_p)       DllPopNode_Name((list_p), (node_p), first, last, next, prev)
// NOTE: I have not yet figured out how i want the pops to work here

// NOTE: S  -> static array
// NOTE: NS -> non static array
#define ShiftToArrEnd_Static(Type, arr_p, index_of_element_to_shift)     \
          {                                                              \
            Assert(index_of_element_to_shift < ArrayCount(hand->cards)); \
            for (int shift_index = index_of_element_to_shift;            \
                shift_index < ArrayCount(arr_p) - 1;                     \
                shift_index += 1                                         \
            ) {                                                          \
              Type* thing_to_shift = arr_p + shift_index;                \
              Type* next = arr_p + shift_index + 1;                      \
              SwapPointers(Type, thing_to_shift, next);                  \
            }                                                            \
          } 




#endif