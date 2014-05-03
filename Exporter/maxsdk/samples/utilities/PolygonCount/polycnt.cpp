/**********************************************************************
 *<
	FILE: polycnt.cpp

	DESCRIPTION:  A polygon counter utility plugin

	CREATED BY: Scott Morrison

	HISTORY: created May 15, 1996

 *>	Copyright (c) 1996, All Rights Reserved.
 **********************************************************************/

#include "polycnt.h"
#include <maxheapdirect.h>

//-----------------------------------------------------------------------------
// atoi() function - String to Integer
//return value is true (success) or false (error)
bool atoi( const TCHAR* value, int& i ) {
	if( _stscanf( value, _T("%i"), &i ) == 1 )
		return true;
	//Log(1,"Don't understand \"%s\", must be an integer\n", value); //FIXME: how to do this?
	return false;
}

// The polygon counter utility plugin displays a modeless dialog
// with a count of the current number of polygons in the scene
// and the current selection set.  You can assign a budget to each of
// these, and a bar graph turns red as you approach the budget.
class PolygonCounter : public UtilityObj {
	public:
    PolygonCounter() {
        hDlg            = NULL;
        ip              = NULL;
        iu              = NULL;
        maxPolys        = NULL;
        maxSelected     = NULL;
    }
    // From UtilityObj
    void BeginEditParams(Interface *ip,IUtil *iu);
    void EndEditParams(Interface *ip,IUtil *iu);
    void SelectionSetChanged(Interface *ip,IUtil *iu) {}
    void DeleteThis() {}

    void Init();
    void End();
    void DrawBars();
    void DrawBar(HWND hWnd, int faces, int maxFaces);
    void DrawBar();

    Interface*        ip;
    IUtil*            iu;
    HWND              hDlg;
    ISpinnerControl*  maxPolys;
    ISpinnerControl*  maxSelected;
};

static PolygonCounter thePolyCounter;

class PolygonCounterClassDesc:public ClassDesc {
public:
    int 			IsPublic() {return 1;}
    void *			Create(BOOL loading = FALSE)
        {return &thePolyCounter;}
    const TCHAR * ClassName() {return GetString(IDS_POLYGON_COUNTER_CLASS);}
    SClass_ID	  SuperClassID() {return UTILITY_CLASS_ID;}
    Class_ID	  ClassID() {return POLYGON_COUNTER_CLASS_ID;}
    const TCHAR*  Category() {return _T("");}
};

static PolygonCounterClassDesc classDesc;
ClassDesc* GetPolyCounterDesc() {return &classDesc;}

static int faceCount = 0;
static int selFaceCount  = 0;
static BOOL countTriangles = TRUE;

static void
InitFaceCount()
{
    faceCount = selFaceCount = 0;
}

// Traverse the node counting polygons.  
static void
TraverseNode(INode* node, TimeValue t)
{
    const ObjectState& os = node->EvalWorldState(t);
    Object* ob = os.obj;
    if (ob!=NULL) {
        int numFaces, numVerts;
		if (countTriangles)
			GetTriMeshFaceCount(t, ob, numFaces, numVerts);
		else
			GetPolygonCount(t, ob, numFaces, numVerts);
        faceCount += numFaces;
        if (node->Selected())
            selFaceCount += numFaces;
    }

    int i, numChildren = node->NumberOfChildren();
    for(i=0; i<numChildren; i++)
        TraverseNode(node->GetChildNode(i), t);
}

// Traverse all the nodes in the scene graph.
static void
CountFaces(Interface* ip)
{
    TraverseNode(ip->GetRootNode(), ip->GetTime());
}

//
// The following GetAppData/WriteAppData code was taken from the vrml exporter as is ...
//

#define MAX_POLYS_ID          7
#define MAX_SELECTED_ID       8

// Get a chunk of app data off the sound object
void
GetAppData(Interface * ip, int id, TCHAR* def, TSTR &val)
{
    SoundObj *node = ip->GetSoundObject();
	if (node == NULL)
	{
		val = def;
		return;
	}
    AppDataChunk *ad = node->GetAppDataChunk(POLYGON_COUNTER_CLASS_ID, UTILITY_CLASS_ID, id);
	if (ad == NULL || ad->data == NULL)
        val = def;
    else
        val = TSTR::FromACP((char*) ad->data);
}

// Write a chunk of app data on the sound object
void
WriteAppData(Interface* ip, int id, const TCHAR* val)
{
	SoundObj *node = ip->GetSoundObject();
	node->RemoveAppDataChunk(POLYGON_COUNTER_CLASS_ID, UTILITY_CLASS_ID, id);

	if (val == NULL)
		return;

	CStr the_val = CStr::FromMCHAR(val);
	int len = the_val.Length()+1; 
	char* buf = (char*)MAX_malloc(len);
	strcpy(buf, the_val);
	node->AddAppDataChunk(POLYGON_COUNTER_CLASS_ID, UTILITY_CLASS_ID, id, (DWORD)len, buf);
	SetSaveRequiredFlag(TRUE);
}

// Handler for the modeless polygon counter dialog.
static INT_PTR CALLBACK
PolyCountDlgProc(HWND hDlg, UINT msg, WPARAM wParam,
                              LPARAM lParam) 
{
    TSTR buf;
    int val;

    switch (msg) {
    case WM_INITDIALOG:
        thePolyCounter.hDlg = hDlg;
        CenterWindow(hDlg, GetParent(hDlg));
        thePolyCounter.maxPolys =
            GetISpinner(GetDlgItem(hDlg, IDC_MAX_POLY_SPIN));
        thePolyCounter.maxPolys->SetScale(10.0f);
        thePolyCounter.maxPolys->SetLimits(0, 1000000);
        thePolyCounter.maxPolys->LinkToEdit(GetDlgItem(hDlg,
                                                       IDC_MAX_POLY_EDIT),
                                            EDITTYPE_INT);
        thePolyCounter.maxPolys->SetResetValue(10000);
        GetAppData(thePolyCounter.ip, MAX_POLYS_ID, _T("10000"), buf);
        if (atoi(buf, val))
            thePolyCounter.maxPolys->SetValue(val, FALSE);

        thePolyCounter.maxSelected =
            GetISpinner(GetDlgItem(hDlg, IDC_MAX_SELECTED_SPIN));
        thePolyCounter.maxSelected->SetScale(10.0f);
        thePolyCounter.maxSelected->SetLimits(0, 1000000);
        thePolyCounter.maxSelected->LinkToEdit(GetDlgItem(hDlg,
                                                    IDC_MAX_SELECTED_EDIT),
                                               EDITTYPE_INT);
        thePolyCounter.maxSelected->SetResetValue(1000);
        GetAppData(thePolyCounter.ip, MAX_SELECTED_ID, _T("1000"), buf);
		if (atoi(buf, val))
			thePolyCounter.maxSelected->SetValue(val, FALSE);

		CheckRadioButton(hDlg, IDC_TRIANGLE, IDC_POLY, IDC_TRIANGLE);
		
		countTriangles = IsDlgButtonChecked(hDlg, IDC_TRIANGLE);

        InitFaceCount();
        CountFaces(thePolyCounter.ip);
        break;
    case CC_SPINNER_CHANGE:
    case WM_CUSTEDIT_ENTER:
        val = thePolyCounter.maxPolys->GetIVal();
        buf.printf(_T("%d"), val);
        WriteAppData(thePolyCounter.ip, MAX_POLYS_ID, buf);
        val = thePolyCounter.maxSelected->GetIVal();
        buf.printf(_T("%d"), val);
        WriteAppData(thePolyCounter.ip, MAX_SELECTED_ID, buf);
		// fall through
    case WM_PAINT:
        thePolyCounter.DrawBars();
        break;
    case WM_COMMAND:
        switch(LOWORD(wParam)) {
		case IDC_TRIANGLE:
			countTriangles = IsDlgButtonChecked(hDlg, IDC_TRIANGLE);
			InitFaceCount();
			CountFaces(thePolyCounter.ip);
			thePolyCounter.DrawBars();
			break;
		case IDC_POLY:
			countTriangles = !(IsDlgButtonChecked(hDlg, IDC_POLY));
			InitFaceCount();
			CountFaces(thePolyCounter.ip);
			thePolyCounter.DrawBars();
			break;
        case IDCANCEL:
            EndDialog(hDlg, FALSE);
            ReleaseISpinner(thePolyCounter.maxPolys);
            ReleaseISpinner(thePolyCounter.maxSelected);
            if (thePolyCounter.iu)
                thePolyCounter.iu->CloseUtility();
            thePolyCounter.hDlg = NULL;
            thePolyCounter.End();
            break;
        }
    }
    return FALSE;
        
}

extern HINSTANCE hInstance;

void 
PolygonCounter::EndEditParams(Interface* ip, IUtil* u) {
	this->iu = u;
}

// Bring the dialog up
void 
PolygonCounter::BeginEditParams(Interface* ip, IUtil* u)
{
    this->iu = u;
    this->ip = ip;
    if (!hDlg) {
        this->Init();
        hDlg = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_POLYCOUNT), 
                          GetActiveWindow(), PolyCountDlgProc,
                          (LPARAM) this);
    } else
        SetActiveWindow(hDlg);

    u->CloseUtility();
}

// Get redraw views callbacks
class PolyCountCallback: public RedrawViewsCallback
{
    void proc(Interface *ip);
};

// Count the polygons and dislpay the bar graph after every redraw views
void
PolyCountCallback::proc(Interface* ip)
{
    InitFaceCount();
    CountFaces(ip);
    thePolyCounter.DrawBars();
}

PolyCountCallback pccb;

void 
PolygonCounter::Init()
{
    ip->RegisterRedrawViewsCallback(&pccb);
}

void
PolygonCounter::End()
{
    ip->UnRegisterRedrawViewsCallback(&pccb);
}

// Draw the bar graph for a polygon counter
void
PolygonCounter::DrawBar(HWND hWnd, int faces, int maxFaces)
{
    HDC hdc = GetDC(hWnd);
    RECT r;
    GetClientRect(hWnd, &r);
    int numBlocks = 50;
    float facesPerBlock = maxFaces / ((float) numBlocks - 1);
    int blockBorder = 1;
    int blockWidth = (r.right - r.left - blockBorder * (numBlocks + 1)) / numBlocks;
    int blockHeight = r.bottom - r.top - 2 * blockBorder;
    int yellowBlock = numBlocks - 15;
    int redBlock =    numBlocks - 5;
    int i;
    float face;

 // Clear it out
    HBRUSH hb = CreateSolidBrush(RGB(0,0,0));
    r.bottom--; r.top++; r.left++; r.right--;
    FillRect(hdc, &r, hb);
    DeleteObject(hb);
    hb = CreateSolidBrush(RGB(0, 255, 0));
    int left = r.left;
    r.top = r.top + blockBorder;
    r.bottom = r.bottom - blockBorder;
    for (i = 0, face = 0.0f; i < numBlocks + 1 && face <= (float) faces; i++, face += facesPerBlock) {
     // draw a block
        r.left = left + (i) * (blockWidth + blockBorder) + blockBorder;
        r.right = r.left + blockWidth;
        if (i == yellowBlock) {
            DeleteObject(hb);
            hb = CreateSolidBrush(RGB(255, 255, 0)); 
        } else if (i == redBlock) {
            DeleteObject(hb);
            hb = CreateSolidBrush(RGB(255, 0, 0));
        }
        FillRect(hdc, &r, hb);
    }

    DeleteObject(hb);
    DeleteDC(hdc);
}

// Fill in all the data in the polygon counter dialog.
void
PolygonCounter::DrawBars()
{
    TSTR buf;
    int val;

    GetAppData(thePolyCounter.ip, MAX_POLYS_ID, _T("10000"), buf);
	if (atoi(buf, val))
		thePolyCounter.maxPolys->SetValue(val, FALSE);
    GetAppData(thePolyCounter.ip, MAX_SELECTED_ID, _T("1000"), buf);
	if (atoi(buf, val))
		thePolyCounter.maxSelected->SetValue(val, FALSE);

    HWND hName = GetDlgItem(thePolyCounter.hDlg, IDC_POLY_COUNT);
    TCHAR str[256];
    _stprintf(str, _T("%d"), faceCount);
    Static_SetText(hName, str);
    
    hName = GetDlgItem(thePolyCounter.hDlg, IDC_SEL_COUNT);
    _stprintf(str, _T("%d"), selFaceCount);
    Static_SetText(hName, str);

    HWND hPolyBar = GetDlgItem(hDlg, IDC_POLY_BAR);
    int maxFaces = maxPolys->GetIVal();
    DrawBar(hPolyBar, faceCount, maxFaces);
    HWND hSelBar = GetDlgItem(hDlg, IDC_SELECTED_BAR);
    int maxSel = maxSelected->GetIVal();
    DrawBar(hSelBar,  selFaceCount, maxSel);
}

