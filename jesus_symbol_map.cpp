
struct Symbol
{
    String_Const_u8 str;
    int type;
};

struct Symbol_Chunk
{
    Symbol symbols[16];
    Symbol_Chunk *next = nullptr;
    u8 num_symbols = 0;
};

struct Symbol_Map
{
    // NOTE(jesus): hash table
    Symbol_Chunk chunks[1024];
};


global Arena jesus_arena;
global Symbol_Map jesus_symbol_map = {};


#define push_struct(a,T) ((T*)linalloc_wrap_unintialized(linalloc_push((a), sizeof(T), file_name_line_number_lit_u8)))
#define push_struct_zero(a,T) ((T*)linalloc_wrap_zero(linalloc_push((a), sizeof(T), file_name_line_number_lit_u8)))
#define push_struct_write(a,T,s) ((T*)linalloc_wrap_write(linalloc_push((a), sizeof(T), file_name_line_number_lit_u8), sizeof(T), s))


internal void jesus_symbol_map_clear(Application_Links *app)
{
    linalloc_clear(&jesus_arena);
    jesus_symbol_map = {};
}

internal void jesus_symbol_map_insert(String_Const_u8 symbol_str, int symbol_type)
{
    u64 chunk_index = table_hash_u8(symbol_str.str, symbol_str.size) % ArrayCount(jesus_symbol_map.chunks);
    
    Symbol_Chunk *first = &jesus_symbol_map.chunks[chunk_index];
    
    Symbol_Chunk *last = nullptr;
    for (Symbol_Chunk *chunk = first;
         chunk != nullptr;
         chunk = chunk->next)
    {
        for (int i = 0; i < chunk->num_symbols; ++i)
        {
            if (string_match(symbol_str, chunk->symbols[i].str))
            {
                // NOTE(jesus): The symbol already exists, we can return
                return;
            }
        }
        last = chunk;
    }
    
    if (last->num_symbols == ArrayCount(last->symbols))
    {
        last->next = push_struct_zero(&jesus_arena, Symbol_Chunk);
        last = last->next;
    }
    
    Symbol sym = {symbol_str, symbol_type};
    last->symbols[last->num_symbols] = sym;
    last->num_symbols++;
}

void jesus_symbol_map_init(Application_Links *app)
{
    jesus_arena = make_arena_system();
}

void jesus_symbol_map_update(Application_Links *app)
{
    jesus_symbol_map_clear(app);
    
    code_index_lock();
    
    for (Buffer_ID buffer_id = get_buffer_next(app, 0, Access_Always);
         buffer_id != 0;
         buffer_id = get_buffer_next(app, buffer_id, Access_Always))
    {
        Code_Index_File *file = code_index_get_file(buffer_id);
        if (file != 0)
        {
            for (i32 i = 0; i < file->note_array.count; ++i)
            {
                Code_Index_Note *note = file->note_array.ptrs[i];
                String_Const_u8 symbol_str = note->text;
                int symbol_type = JesusSymbol_None;
                switch(note->note_kind)
                {
                    case CodeIndexNote_Function: symbol_type = JesusSymbol_Function; break;
                    case CodeIndexNote_Macro: symbol_type = JesusSymbol_Macro; break;
                    case CodeIndexNote_Type: symbol_type = JesusSymbol_Type; break;
                }
                jesus_symbol_map_insert(symbol_str, symbol_type);
            }
        }
    }
    
    code_index_unlock();
}

int jesus_symbol_map_find(Application_Links *app, String_Const_u8 token_s)
{
    int symbol_type = JesusSymbol_None;
    
    u64 chunk_index = table_hash_u8(token_s.str, token_s.size) % ArrayCount(jesus_symbol_map.chunks);
    
    Symbol_Chunk *chunk = &jesus_symbol_map.chunks[chunk_index];
    while (chunk)
    {
        for (int i = 0; i < chunk->num_symbols; ++i)
        {
            if (string_match(token_s, chunk->symbols[i].str))
            {
                return chunk->symbols[i].type;
            }
        }
        chunk = chunk->next;
    }
    
    return symbol_type;
}
