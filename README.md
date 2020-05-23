# 4coder_jesus

4coder customization code.

Steps to add syntax highlighting for custom types, functions, and macros.

## Modify 4coder_default_include.cpp

Include **symbol_map.h** somewhere after **4coder_base_types.cpp**.

Include **symbol_map.cpp** at the end of the file.

## Modify 4coder_draw.cpp

Change the signature of the function _get_token_color_cpp_ to also accept a pointer to _Application_Links_ and a _Text_Layout_ID_. Pass the apropriate parameters from the function _draw_cpp_token_colors_.

Add the following code snippet into _get_token_color_cpp_ to handle TokenBaseKind_Identifier tokens:

```C
// NOTE(jesus): this is the added part
case TokenBaseKind_Identifier:
{
	Buffer_ID buffer = text_layout_get_buffer(app, text_layout_id);

	u8 output_string[128];
	Range_i64 range{token.pos, token.pos+token.size};
	if (buffer_read_range(app, buffer, range, output_string))
	{
		String_Const_u8 token_s = SCu8(output_string, token.size);
		int symbol_type = jesus_symbol_map_find(app, token_s);
        switch (symbol_type)
        {
            case JesusSymbol_Type:
            {
                color = defcolor_jesus_type;
            }break;
            case JesusSymbol_Macro:
            {
                color = defcolor_preproc;
            }break;
            case JesusSymbol_Function:
            {
                color = defcolor_jesus_function;
            }break;
            default:
            {
                color = defcolor_text_default;
            }break;
        }
    }
}break;
```

**NOTE:** The previous snippet is using two custom managed idenfiers for colors. I initialized them where all colors are initialized and provided values for them in the theme file.

## Modify 4coder_default_framework.cpp

Call _symbol_map_init_ at the end of _default_framework_init_.

## Modify 4coder_default_hooks.cpp

Call _symbol_map_update_ right after _code_index_update_tick(app)_.
