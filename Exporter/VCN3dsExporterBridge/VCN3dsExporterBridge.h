// VCN3dsExporterBridge.h

#pragma once
#include <windows.h>
#include "..\VCNINodeWrapperData.h"


typedef void (*PropertyEditorCloseCallback)(void);
typedef void (*PropertyEditorModificationCallback)(const VCNINodeWrapperData& data);
typedef void (*PropertyEditorActivateStateCallback)(bool activated);

namespace VCN3dsExporterBridge {

	enum LogTypeCPP
	{
		LogTypeCPP_MESSAGE = 0,
		LogTypeCPP_WARNING = 1,
		LogTypeCPP_ERROR = 2,
		LogTypeCPP_FATAL = 3
	};


	class VCN3dsExporterBridgeManager
	{
	public:
		static bool Init(HWND parentWindowHandle);
		static bool InitEditorAppDomain();
		static void InitEditorManager();
		static void InitEditorInitializer();
		static HWND OpenPropertyEditor(HWND parentWindowHandle, PropertyEditorCloseCallback closeCallback, PropertyEditorModificationCallback editorModificationCallback, PropertyEditorActivateStateCallback activateStateCallback);
		static void ClosePropertyEditor();
		static void EnableProgressWindow(const std::string& name);
		static void DisableProgressWindow();
		static void SetProgressWindowValue(int newValue);
		static void UpdatePropertyEditor(const VCNINodeWrapperData& data);
		static void SetValidSelectionState(bool validSelection);
		static bool ReloadDll();

		static void ShowErrorMessage(const wchar_t* message);
		static void AddLogEntry(const std::string& message, LogTypeCPP logType);

		static bool TryToAddToSVN(const std::string& filepathToAdd);

		//static HWND InitPropertyEditor(HWND parentWindowHandle);
		//static void ActivateStateCallback(bool activated);


		static PropertyEditorCloseCallback CppCloseCallback;
		static PropertyEditorModificationCallback EditorModificationCallback;
		static PropertyEditorActivateStateCallback EditorActivateStateCallback;
	};

}
