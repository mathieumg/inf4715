/**********************************************************************
 *<
	FILE: rendvue.cpp

	DESCRIPTION:

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "rvuepch.h"
#include "resource.h"
#include "rendvue.h"


extern BOOL FileExists(const TCHAR *filename);

static void catflt(TCHAR *s, int maxlen, FLOAT f) {
	int i;
	TCHAR buf[20];
	_sntprintf(buf,20,_T(" %.4f\0"),f);

	// Keith Trummel November 27, 1996
	// Change comma decimal separator to a period
	for (i=0; i<static_cast<int>(_tcslen(buf)); i++)	// SR DCAST64: Downcast to 2G limit.
		if (buf[i] == _T(','))
			buf[i] = _T('.');

	for (i=static_cast<int>(_tcslen(buf)-1); i>0; i--) {	// SR DCAST64: Downcast to 2G limit.
		switch(buf[i]) {
			case _T('0'): if (i==1) goto finish; else buf[i] = 0; break;
			case _T('.'): buf[i] =  0; goto finish; 
			default: goto finish; 
			}
		}
   finish:
    if ( (_tcslen(buf)==3) && (buf[1]==_T('-')) && (buf[2]==_T('0'))) _tcscpy(buf,_T(" 0"));
	_tcsncat(s,buf,maxlen);
	}

static void catpnt(TCHAR *s, int maxlen, Point3 p) {
	catflt(s,maxlen,p.x);
	catflt(s,maxlen,p.y);
	catflt(s,maxlen,p.z);
	}



class RendVueClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading) { return new VueRenderer; }
	const TCHAR *	ClassName() { return GetString(IDS_VRENDTITLE); }
	SClass_ID		SuperClassID() { return RENDERER_CLASS_ID; }
	Class_ID 		ClassID() { return VREND_CLASS_ID; }
	const TCHAR* 	Category() { return _T("");  }
	void			ResetClassParams(BOOL fileReset) {}
	};

static RendVueClassDesc srendCD;

ClassDesc* GetRendVueDesc() { 
	return &srendCD;  
	}

class INodeEnum {
	public:
	virtual void proc(INode *node)=0;
	};

void INodeEnumTree(INode *node, INodeEnum& inodeEnum) {
	inodeEnum.proc(node);
	for (int i=0; i<node->NumberOfChildren(); i++) 
		INodeEnumTree(node->GetChildNode(i), inodeEnum);
	}

class RendINodeEnum: public INodeEnum {
	BOOL rendSelection;
	VueRenderer *sr;
	TimeValue t;
	public:
		RendINodeEnum(VueRenderer* scanrend, BOOL sel, TimeValue time) { 
			sr = scanrend; rendSelection = sel; t = time; }
		void proc(INode *node) {
			BOOL nodeHidden = node->IsNodeHidden();
			ObjectState os = node->EvalWorldState(t);
			if (os.obj==NULL) 
				return;
			switch (os.obj->SuperClassID()) {
				case LIGHT_CLASS_ID: 
					{
					LightObject *lo = (LightObject*)os.obj;
					sr->anyLights = TRUE;
					if (lo->GetUseLight()) {
		 				//sr->AddLight(node,(LightObject*)os.obj);
						sr->nlts++;
						}
					}
					break;
				case GEOMOBJECT_CLASS_ID: {
					if ((!sr->RP.rendHidden)&&nodeHidden) return;
					if (!os.obj->IsRenderable()) return;
					if (rendSelection) {
						if (!node->Selected()) 
							return;
						}
					sr->nobs++ ;
//					if (node->MotBlur()&&sr->RP.motBlur) {
//						for (int i=sr->RP.nBlurFrames-1; i>=0; i--)   // DS 8-24-96
//							sr->AddInstance(node,i);
//						}
//					else 
//						sr->AddInstance(node);
					break;
				}
			}
		}
	};

class RendINodeEnumFrame: public INodeEnum {
	BOOL rendSelection;
	VueRenderer *sr;
	TimeValue t;
	public:
		RendINodeEnumFrame(VueRenderer* scanrend, BOOL sel, TimeValue time) { 
			sr = scanrend; rendSelection = sel; t = time; 
			}
		void proc(INode *node);
	};


void RendINodeEnumFrame::proc(INode *node) {
	BOOL nodeHidden = node->IsNodeHidden();
	ObjectState os = node->EvalWorldState(t);
	if (os.obj==NULL) 
		return;
	Matrix3 tm = node->GetObjTMAfterWSM(t,NULL);
	Point3 p = tm.GetTrans();
	switch (os.obj->SuperClassID()) {
		case LIGHT_CLASS_ID: {
			LightObject *lo = (LightObject*)os.obj;
			GenLight *gl = (GenLight *)lo;
			Point3 col = gl->GetRGBColor(t);
			if (gl->GetUseLight()) {
				if (os.obj->ClassID()==Class_ID(OMNI_LIGHT_CLASS_ID,0))	{
					// OMNI Light --------------
					_sntprintf(sr->buffer,256,_T("light \"%s\""), node->GetName());
					catpnt(sr->buffer,256,p); 
					catpnt(sr->buffer,256,col);	
					if (gl->GetShadow()) _tcsncat(sr->buffer,_T(" 1"), 256);
					_tcsncat(sr->buffer,_T("\n"),256);
					sr->vuefile->Write(sr->buffer);
					}		 
				else 
				if (os.obj->ClassID()==Class_ID(SPOT_LIGHT_CLASS_ID,0)) {
					// SPOT light ----------
					_sntprintf(sr->buffer,256,_T("spotlight \"%s\""),node->GetName());
					catpnt(sr->buffer,256,p);
					// target is in the negative Z direction.
					Point3 tp = p - 100.0f*tm.GetRow(2);
					catpnt(sr->buffer,256,tp); 
					catpnt(sr->buffer,256,col);	
					catflt(sr->buffer,256,gl->GetHotspot(t));
					catflt(sr->buffer,256,gl->GetFallsize(t));
					if (gl->GetShadow()) _tcsncat(sr->buffer,_T(" 1"), 256);
					//if (gl->IsDir()) _tcsncat(sr->buffer,_T(" D"), 256);
					_tcsncat(sr->buffer,_T("\n"),256);
					sr->vuefile->Write(sr->buffer);
					}
				}
			 }
			break;

		case GEOMOBJECT_CLASS_ID: {
			if ((!sr->RP.rendHidden)&&nodeHidden) 	return;
			if (!os.obj->IsRenderable()) 	return;
			if (rendSelection&&!node->Selected()) 	return;
			_sntprintf(sr->buffer,256,_T("transform \"%s\"\0"), node->GetName());
			for (int j=0;j<4; j++) 
				catpnt(sr->buffer,256,tm.GetRow(j));
			_tcsncat(sr->buffer,_T("\n"),256);
			sr->vuefile->Write(sr->buffer);
			}
			break;
		}
	}

void RemoveScaling(Matrix3 &m) {
	for (int i=0; i<3; i++) 
		m.SetRow(i,Normalize(m.GetRow(i)));
	}


// ---------- ExtractRollAngle ----------------
// Returns roll angle in degrees, given a transform matrix.
//
// The matrix z axis is the negative view direction
// First rotate about world Z so this axis in the world y/z plane.
// Then rotate about world Z so the matrix z axis lies along the y axis (0,1,0)
// THen the position of the matrix X axis in the world ZX plane can be used
// to get the Roll angle.
float ExtractRollAngle(Matrix3 &m) {
	float roll;
	Point3 axis = m.GetRow(2);
	float bearing = (float)atan2(axis.x,axis.y);
	Matrix3 m2 = m;
	m2.RotateZ(bearing);
	axis = m2.GetRow(2);
	float azimuth = (float)atan2(axis.z,axis.y);
	m2.RotateX(-azimuth);
	Point3 v = m2.GetRow(1);
	roll = -RadToDeg(atan2(v.x,v.z));
	return roll;
	}


void VueRenderer::WriteViewNode(INode* vnode, TimeValue t) {
	Interval iv;
	iv.SetInfinite();
	const ObjectState& os = vnode->EvalWorldState(t);
	switch (os.obj->SuperClassID()){
		case CAMERA_CLASS_ID: {
			// compute camera transform
			CameraObject *cam = (CameraObject *)os.obj;
			Matrix3 camtm = vnode->GetObjTMAfterWSM(t,&iv);
			RemoveScaling(camtm);
			float fov = cam->GetFOV(t);
			Point3 p = camtm.GetTrans();
			Point3 t = p + camtm.GetRow(2)*(-100.0f);  // target is (somewhere) in negative Z direction
			_tcsncpy(buffer,_T("camera"),256);
			catpnt(buffer,256,p);
			catpnt(buffer,256,t);
			catflt(buffer, 256, ExtractRollAngle(camtm));
			catflt(buffer, 256, 2400.0f/RadToDeg(fov));
			_tcsncat(buffer,_T("\n"),256);
			vuefile->Write(buffer);
			}
			break;
		default:
			break;

		}	
	}

void VueRenderer::WriteViewParams(ViewParams *v) {
	// Not implemented
	}

RefResult VueRenderer::NotifyRefChanged (
		Interval			 changeInt, 
		RefTargetHandle		 hTarget, 
		PartID				&partID,  
		RefMessage			 message
	) 
{
	return REF_SUCCEED;
}

int VueRenderer::Open(
	INode *scene,     	// root node of scene to render
	INode *vnode,     	// view node (camera or light), or NULL
	ViewParams *viewPar,// view params for rendering ortho or user viewport
	RendParams &rp,  	// common renderer parameters
	HWND hwnd, 				// owner window, for messages
	DefaultLight* defaultLights, // Array of default lights if none in scene
	int numDefLights,	// number of lights in defaultLights array
	RendProgressCallback* prog
	) 
	{
	sceneNode = scene;
	viewNode = vnode;
	viewParams = *viewPar;
	RP = rp;	
	vuefile = NULL;
	if (_tcslen(vueFileName)==0) return 0;
	if (FileExists(vueFileName)) {
		TSTR buf;
		buf.printf(GetString(IDS_FILE_ALREADY_EXISTS),vueFileName);
		int ret = MessageBox(NULL, buf, GetString(IDS_VRENDTITLE), MB_YESNO|MB_ICONEXCLAMATION|MB_TASKMODAL);
		if (ret!=IDYES)  return 0;  
		}
	vuefile = new MaxSDK::Util::TextFile::Writer();
	Interface14 *iface = GetCOREInterface14();
	UINT codepage  = iface-> DefaultTextSaveCodePage(true);
	vuefile->Open(vueFileName,false, MaxSDK::Util::TextFile::Writer::WRITE_BOM | codepage );
	if (!vuefile->CanWrite()) {
		TSTR buf;
		buf.printf(GetString(IDS_CANT_OPEN),vueFileName);
		MessageBox(NULL, buf, GetString(IDS_RENDER_ERR), MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
		delete vuefile;
		vuefile = NULL;
		return(0);
		}

	vuefile->Printf(_T("VERSION 202\n"));

	RendINodeEnum nodeEnum(this, RP.rendType==RENDTYPE_SELECT, 0);
	
	//  Initialize instance list and Light list.
    INodeEnumTree(sceneNode, nodeEnum);

	return 1;
	}


int VueRenderer::Render(
	TimeValue t,   			// frame to render.
	Bitmap *tobm, 			// optional target bitmap
	FrameRendParams &frp,	// Time dependent parameters
	HWND hwnd, 				// owner window
	RendProgressCallback *prog,
	ViewParams *vp
	) 
	{

	if (vuefile==NULL || !vuefile->CanWrite()) 
		return 0;

	int frameNum = t/GetTicksPerFrame();

	vuefile->Printf(_T("\nframe %d\n"), frameNum);

	RendINodeEnumFrame nodeEnum(this, RP.rendType==RENDTYPE_SELECT, t);

	// write out the entries to the VUE file
    INodeEnumTree(sceneNode, nodeEnum);		 

	// write the camera entry to the VUE file
	if (viewNode) 
		WriteViewNode(viewNode, t);
	else 
		WriteViewParams(&viewParams);
	return 1;
	}

void VueRenderer::Close( HWND hwnd, RendProgressCallback* prog ) { 
	if (vuefile)
		delete vuefile;
	vuefile = NULL;
	}		

RefTargetHandle VueRenderer::Clone(RemapDir &remap) {
	VueRenderer *newRend = new VueRenderer;
	newRend->vueFileName = vueFileName;
	BaseClone(this, newRend, remap);
	return newRend;
	}

void VueRenderer::ResetParams(){
	vueFileName.Resize(0);
	}

#define VUE_FILENAME_CHUNKID 001

IOResult VueRenderer::Save(ISave *isave) {
    if (_tcslen(vueFileName)>0) {	
	     isave->BeginChunk(VUE_FILENAME_CHUNKID);
	     isave->WriteWString(vueFileName);
	     isave->EndChunk();	
		 }
	return IO_OK;
	}

IOResult VueRenderer::Load(ILoad *iload) {
	int id;
	TCHAR *buf;
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(id = iload->CurChunkID())  {
			case VUE_FILENAME_CHUNKID:	
				if (IO_OK==iload->ReadWStringChunk(&buf)) 
					vueFileName = buf;
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}


