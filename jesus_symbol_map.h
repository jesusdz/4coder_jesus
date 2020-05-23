/* date = May 23rd 2020 5:50 pm */

#ifndef JESUS_SYMBOL_MAP_H
#define JESUS_SYMBOL_MAP_H

enum {
    JesusSymbol_None,
    JesusSymbol_Function,
    JesusSymbol_Macro,
    JesusSymbol_Type,
    JesusSymbol_Count
};

void jesus_symbol_map_init(Application_Links *app);
void jesus_symbol_map_update(Application_Links *app);
int  jesus_symbol_map_find(Application_Links *app, String_Const_u8 token_str);

#endif //JESUS_SYMBOL_MAP_H
