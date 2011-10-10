#ifndef FORM_H
#define FORM_H

//===========================================================================================
#if (MODULE_GUI_SYSTEM_FORM == 0)

#define Form_Init()					
#define Form_Handler(x)				(0)

#else

//===========================================================================================
typedef bool (*F_FORMPROC)(FORM *, void *);

//===========================================================================================
void Form_Init(void);

bool Form_Create(
			FORM *form,
			WCHAR *caption,
			WORD sx, WORD sy,
			WORD width, WORD height,
			DWORD style,
			F_FORMPROC formproc,
			DWORD IdleExitTime
			);

void Form_Close(FORM *form);

// 在窗口事件函数内调用，仅对发生改变的显示区域进行更新
void Form_Update(FORM *form);

// 令整个窗口进行重新绘制，可在窗口事件函数外部进行调用
void Form_ReDraw(FORM *form);

bool Form_Handler(void *Event);

void Form_AddObject(FORM *form, VCOBJECT *vcobject);
void Form_DelObject(FORM *form, VCOBJECT *vcobject);
FORM *Object_GetParentForm(VCOBJECT *vcobject);

bool Form_IsAliveForm(FORM *form);

DWORD Form_GetFormCountOfPanel(BYTE panel);

void Form_GetClientRect(FORM *form, RECT *clientrect);

void Form_SetIdleExitTime(FORM *form, DWORD Timeout);
void Form_ResetIdleExitTime(FORM *form);

void Form_DoModal(FORM *form);

#endif


#endif
