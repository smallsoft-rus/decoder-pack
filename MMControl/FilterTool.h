/* SmallSoft Multimedia Control Panel
 * Copyright (c) 2025, SmallSoft (https://github.com/smallsoft-rus/) 
 * License: BSD 2.0 */
#include "fil_data.h"

extern HWND hFilterList;
extern HWND hCategoryList;
void FillCategoryList();
void Command_GetFilters();
void Command_FilterInfo();
void Command_FilterProperties();
BOOL ParseBinaryData(BYTE* p,UINT c,REGFILTER2* data);
