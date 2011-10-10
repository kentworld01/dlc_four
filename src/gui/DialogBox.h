#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

//===========================================================================================
#if (MODULE_GUI_SYSTEM_DIALOGBOX == 0)
#else

typedef enum
{
	MBI_NONE,
	MBI_OK,
	MBI_CANCEL,
	MBI_TIMEOUT
} DIALOGRETCODE;

//===========================================================================================
//
typedef struct
{
	FORM Form;

	WCHAR *title;
	WCHAR *message;
	DWORD remaintime;
	DWORD starttickcount;
} MSGBOX;

void MessageBox(MSGBOX *Msgbox, WCHAR *Title, WCHAR *Message, DWORD RemainTime);

//===========================================================================================
//
typedef struct
{
	FORM Form;
	DWORD exitcode;
} DIALOGBOX;

DWORD DialogBox_DoModal(
			DIALOGBOX *dlg, WCHAR *Title,
			WORD sx, WORD sy, WORD width,
			WORD height,
			DWORD style,
			DWORD RemainTime
			);

//===========================================================================================
//
typedef struct INPUTDIALOG
{
	FORM Form;
	EDITOR inputer;
	DWORD exitcode;
	bool (*dialogproc)(struct INPUTDIALOG *, void *);
} INPUTDIALOG;

typedef bool (*F_INPUT_DIALOG_PROC)(struct INPUTDIALOG *, void *);

DWORD InputDialog_DoModal(
			INPUTDIALOG *dlg, WCHAR *Title,
			WORD sx, WORD sy, WORD width, WORD height,
			DWORD style, DWORD RemainTime,
			WCHAR *inputbuffer, WORD maxinputchar, DWORD editorstyle,
			F_INPUT_DIALOG_PROC dialogproc
			);

#define DIGITALDIALOG							INPUTDIALOG
#define DigitalDialog_DoModal					InputDialog_DoModal

//===========================================================================================
//
typedef struct
{
	FORM Form;
	DWORD exitcode;
	DWORD IP;
	char EditorBuffer_1[8];
	char EditorBuffer_2[8];
	char EditorBuffer_3[6];
	char EditorBuffer_4[6];
	BYTE Col;
} IPDIALOG;

DWORD IPDialog_DoModal(
			IPDIALOG *dlg, WCHAR *Title,
			WORD sx, WORD sy, WORD width, WORD height,
			DWORD style, DWORD RemainTime,
			DWORD *IP
			);

#endif

#endif
