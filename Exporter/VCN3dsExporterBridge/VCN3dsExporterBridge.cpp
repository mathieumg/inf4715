// This is the main DLL file.

#include "stdafx.h"

#include "VCN3dsExporterBridge.h"
#include <msclr/marshal_cppstd.h>
#include "VCN3dsExporterBridgeHelpers.h"
#include "VCN3dsExporterManagedConverterVisitors.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Reflection;
using namespace VCN3dsExporterPropertyEditor;
using namespace VCN3dsExporterLog;
using namespace VCNNodesCSharp;
using namespace System::Runtime::InteropServices;


static void UnhandledExceptionFunction(Object^ sender, UnhandledExceptionEventArgs^ args) 
{
	Exception^ e = (Exception^)args->ExceptionObject;
	std::string mess = VCN3dsExporterBridge::VCN3dsExporterBridgeHelpers::ConvertManagedString((System::String^)e->Message);
	std::wstring wmess;
	wmess.assign(mess.begin(), mess.end());
	VCN3dsExporterBridge::VCN3dsExporterBridgeManager::ShowErrorMessage(wmess.c_str());
}

static String^ BuildAssemblyFilePath(String^ assemblyName)
{
	String^ finalPath = "";
	String^ baseLoc = Assembly::GetExecutingAssembly()->Location;
	String^ assDirPath = System::IO::Path::GetDirectoryName(baseLoc);
	if (assemblyName == "VCN3dsExporter")
	{
		finalPath = assDirPath + System::IO::Path::DirectorySeparatorChar + assemblyName + ".dlu";
	}
	else
	{
		finalPath = assDirPath + System::IO::Path::DirectorySeparatorChar + assemblyName + ".dll";
	}
	return finalPath;
}

static Assembly^ AssemblyResolveFunction(Object^ sender, ResolveEventArgs^ args)
{
	sender;

	// If this is an mscorlib, do a bare load
	if (args->Name->Length >= 8 && args->Name->Substring(0, 8) == L"mscorlib")
	{
		return Assembly::Load(args->Name->Substring(0, args->Name->IndexOf(L",")) + L".dll");
	}

	// Load the assembly from the specified path
	String^ finalPath = nullptr;
	try
	{
		String^ assemblyName = args->Name->Substring(0, args->Name->IndexOf(","));
		finalPath = BuildAssemblyFilePath(assemblyName);
		if (assemblyName == "VCNNodesCSharp")
		{
			String^ customAssemblyPath = BuildAssemblyFilePath("VCNNodesCSharpCustom");
			Assembly^ retvalCustom = Assembly::LoadFrom(customAssemblyPath);
		}
		Assembly^ retval = Assembly::LoadFrom(finalPath);
		return retval;
	}
	catch (...)
	{
	}

	return nullptr;
}



namespace VCN3dsExporterBridge
{


	// Need MarshalByRefObject to be able to communicate between AppDomains
	public ref class RemoteEditorManager : MarshalByRefObject, VCN3dsExporterPropertyEditor::PropertyEditorManager
	{
	public:
		RemoteEditorManager()
		{
			mProgressWindow = nullptr;
			mPropertyWindow = nullptr;
			mIsPropertyWindowValid = false;
		}

		// This is neede to make sure that the remote object does not get released by the GC
		// See http://social.msdn.microsoft.com/Forums/en-US/3ab17b40-546f-4373-8c08-f0f072d818c9/remotingexception-when-raising-events-across-appdomains?forum=netfxremoting
		virtual Object^ InitializeLifetimeService() override
		{
			return nullptr;
		}

		void SetParentAppDomain(AppDomain^ parentAppDomain)
		{
			mParentAppDomain = parentAppDomain;
		}

		


		// Progress Window methods
		ProgressWindow^ GetProgressWindow()
		{
			if (mProgressWindow == nullptr)
			{
				mProgressWindow = gcnew ProgressWindow();
			}
			return mProgressWindow;
		}
		void ShowProgressWindow()
		{
			ProgressWindow^ progWindow = GetProgressWindow();
			progWindow->SetProgress(0);
			progWindow->SetName("");
			progWindow->Show();
		}
		void HideProgressWindow()
		{
			ProgressWindow^ progWindow = GetProgressWindow();
			progWindow->Hide();
		}
		void SetProgressWindowPercentage(int per)
		{
			ProgressWindow^ progWindow = GetProgressWindow();
			progWindow->SetProgress(per);
		}
		void SetProgressWindowName(String^ name)
		{
			ProgressWindow^ progWindow = GetProgressWindow();
			progWindow->SetName(name);
		}


		// Editor Window Methods
		NewPropertyEditor^ GetPropertyEditorWindow()
		{
			if (!mIsPropertyWindowValid || mPropertyWindow == nullptr)
			{
				List<Type^>^ componentTypes = VCNNodesCSharp::VCNNodesCSharpHelper::GetListOfComponents(true);
				mPropertyWindow = gcnew NewPropertyEditor(this, gcnew List<UIComponent^>(), componentTypes);
				mIsPropertyWindowValid = true;
			}
			return mPropertyWindow;
		}

		// Returns the HWND of the Editor Window
		HWND ShowEditorWindow(HWND parentWindowHandle)
		{
			NewPropertyEditor^ propWindow = GetPropertyEditorWindow();
			System::Windows::Interop::WindowInteropHelper^ helper = gcnew System::Windows::Interop::WindowInteropHelper(propWindow);
			helper->Owner = (System::IntPtr)parentWindowHandle;
			propWindow->Show();

			return (HWND)helper->Handle.ToPointer();
		}

		void HideEditorWindow()
		{
			if (mPropertyWindow != nullptr)
			{
				mPropertyWindow->Close();
				mPropertyWindow = nullptr;
			}
		}

		void SetPropertyEditorCallbacks(CrossAppDomainDelegate^ closedCallback, CrossAppDomainDelegate^ modificationCallback, CrossAppDomainDelegate^ activateStateCallback)
		{
			mClosedCallback = closedCallback;
			mModificationCallback = modificationCallback;
			mActivateStateCallback = activateStateCallback;
		}

		void SetEditorWindowData(List<UIComponent^>^ components)
		{
			NewPropertyEditor^ propWindow = GetPropertyEditorWindow();
			propWindow->UpdateComponents(components);
		}

		void SetEditorWindowSelectionState(bool validSelection)
		{
			NewPropertyEditor^ propWindow = GetPropertyEditorWindow();
			propWindow->SetValidSelection(validSelection);
		}

		// These callbacks cant have parameters. Once notified, the other AppDomain must call this one to get the necessary information
		virtual void CallbackFromUIClosed() 
		{
			if (mParentAppDomain == nullptr || mClosedCallback == nullptr)
			{
				return;
			}
			mIsPropertyWindowValid = false;
			mParentAppDomain->DoCallBack(mClosedCallback);
		}
		virtual void CallbackFromUIActiveState( bool activeState ) 
		{
			if (mParentAppDomain == nullptr || mActivateStateCallback == nullptr)
			{
				return;
			}
			mParentAppDomain->DoCallBack(mActivateStateCallback);
		}
		virtual void CallbackFromUIModification( ) 
		{
			if (mParentAppDomain == nullptr || mModificationCallback == nullptr)
			{
				return;
			}
			mParentAppDomain->DoCallBack(mModificationCallback);
		}

		List<UIComponentSerializable^>^ GetCurrentUIComponentsSerializable() 
		{
			List<UIComponent^>^ components = mPropertyWindow->GetCurrentUIComponents();
			return UIComponent::BuildSerializableCopy(components);
		}


		bool GetCurrentEditorState() 
		{
			return mPropertyWindow->IsActiveState;
		}

		void CleanForAppDomainUnload() 
		{
			// Delete everything that could prevent the AppDomain::Unload to work
			if (mPropertyWindow != nullptr)
			{
				mPropertyWindow->Close();
				mPropertyWindow = nullptr;
			}
			if (mProgressWindow != nullptr)
			{
				mProgressWindow->Close();
				mProgressWindow = nullptr;
			}
			
			mClosedCallback = nullptr;
			mModificationCallback = nullptr;
			mActivateStateCallback = nullptr;
			mParentAppDomain = nullptr;

			mIsPropertyWindowValid = false;

			VCN3dsExporterBridgeManager::AddLogEntry("App domain cleaned", LogTypeCPP_MESSAGE);
		}

		

	private:
		ProgressWindow^ mProgressWindow;
		NewPropertyEditor^ mPropertyWindow;

		CrossAppDomainDelegate^ mClosedCallback;
		CrossAppDomainDelegate^ mModificationCallback;
		CrossAppDomainDelegate^ mActivateStateCallback;

		AppDomain^ mParentAppDomain;

		bool mIsPropertyWindowValid;
	};

	

	// Need MarshalByRefObject to be able to communicate between AppDomains
	public ref class RemoteEditorInitializer : MarshalByRefObject
	{
	public:
		RemoteEditorInitializer()
		{
			mLoadedAssemblies = gcnew List<String^>();
		}

		// This is neede to make sure that the remote object does not get released by the GC
		// See http://social.msdn.microsoft.com/Forums/en-US/3ab17b40-546f-4373-8c08-f0f072d818c9/remotingexception-when-raising-events-across-appdomains?forum=netfxremoting
		virtual Object^ InitializeLifetimeService() override
		{
			return nullptr;
		}

		bool LoadAssembly(String^ assemblyPath)
		{
			//VCN3dsExporterBridgeManager::AddLogEntry("Loading Assembly \"" + VCN3dsExporterBridgeHelpers::ConvertManagedString(assemblyPath) + "\" in the remote App Domain", LogTypeCPP_MESSAGE);
			try
			{
				if (Assembly::LoadFrom(assemblyPath) != nullptr)
				{
					mLoadedAssemblies->Add(assemblyPath);
					return true;
				}
				return false;
			}
			catch (Exception^)
			{
				return false;
			}
		}

		void BindAssemblyResolveEvent()
		{
			mAssemblyResolverReference = gcnew ResolveEventHandler(AssemblyResolveFunctionProxy);
			AppDomain::CurrentDomain->AssemblyResolve += mAssemblyResolverReference;
		}

		static Assembly^ AssemblyResolveFunctionProxy(Object^ sender, ResolveEventArgs^ args)
		{
			sender;

			// If this is an mscorlib, do a bare load
			if (args->Name->Length >= 8 && args->Name->Substring(0, 8) == L"mscorlib")
			{
				return Assembly::Load(args->Name->Substring(0, args->Name->IndexOf(L",")) + L".dll");
			}

			// Load the assembly from the specified path
			String^ finalPath = nullptr;
			try
			{
				String^ assemblyName = args->Name->Substring(0, args->Name->IndexOf(","));
				finalPath = BuildAssemblyFilePath(assemblyName);
				if (assemblyName == "VCNNodesCSharp")
				{
					String^ customAssemblyPath = BuildAssemblyFilePath("VCNNodesCSharpCustom");
					Assembly^ retvalCustom = Assembly::LoadFrom(customAssemblyPath);
				}
				Assembly^ retval = Assembly::LoadFrom(finalPath);
				return retval;
			}
			catch (...)
			{
			}

			return nullptr;
		}

		void CleanForAppDomainUnload() 
		{
			AppDomain::CurrentDomain->AssemblyResolve -= mAssemblyResolverReference;
			mLoadedAssemblies = nullptr;
			mAssemblyResolverReference = nullptr;
			VCN3dsExporterBridgeManager::AddLogEntry("App domain cleaned for initializer", LogTypeCPP_MESSAGE);
		}

	private:
		List<String^>^ mLoadedAssemblies;
		ResolveEventHandler^ mAssemblyResolverReference;
	};



	public ref class VCN3dsExporterBridgeManagerDataContainer
	{
	public:

		static void SetupDataVisitors()
		{
			DataVisitors = gcnew Dictionary<Type^, DataVisitor^>();
			DataVisitors[System::Int32::typeid]				    = gcnew DataVisitorInt();
			DataVisitors[System::UInt32::typeid]			    = gcnew DataVisitorUInt();
			DataVisitors[System::Boolean::typeid]			    = gcnew DataVisitorBool();
			DataVisitors[System::Single::typeid]			    = gcnew DataVisitorFloat();
			DataVisitors[System::Double::typeid]			    = gcnew DataVisitorDouble();
			DataVisitors[System::Char::typeid]				    = gcnew DataVisitorChar();
			DataVisitors[System::String::typeid]			    = gcnew DataVisitorString();
			DataVisitors[VCNNodesCSharp::Vector2::typeid]	    = gcnew DataVisitorVector2();
			DataVisitors[VCNNodesCSharp::Vector3::typeid]	    = gcnew DataVisitorVector3();
			DataVisitors[VCNNodesCSharp::Vector4::typeid]	    = gcnew DataVisitorVector4();
			DataVisitors[VCNNodesCSharp::LuaTrigger::typeid]	= gcnew DataVisitorLuaTrigger();

			EnumDict = gcnew Dictionary<Type^, System::Int32>();
			EnumDict[System::Int32::typeid]				    = ATTR_INTEGER;
			EnumDict[System::UInt32::typeid]			    = ATTR_UNSIGNED_INTEGER;
			EnumDict[System::Boolean::typeid]			    = ATTR_BOOL;
			EnumDict[System::Single::typeid]			    = ATTR_FLOAT;
			EnumDict[System::Double::typeid]			    = ATTR_DOUBLE;
			EnumDict[System::Char::typeid]				    = ATTR_CHAR;
			EnumDict[System::String::typeid]			    = ATTR_STRING;
			EnumDict[VCNNodesCSharp::Vector2::typeid]	    = ATTR_VECTOR2;
			EnumDict[VCNNodesCSharp::Vector3::typeid]	    = ATTR_VECTOR3;
			EnumDict[VCNNodesCSharp::Vector4::typeid]	    = ATTR_VECTOR4;
			EnumDict[VCNNodesCSharp::LuaTrigger::typeid]	= ATTR_LUA_TRIGGER;
		}

		static void DialogClosedCallback()
		{
			if (VCN3dsExporterBridgeManager::CppCloseCallback)
			{
				VCN3dsExporterBridgeManager::CppCloseCallback();
			}
			VCN3dsExporterBridgeManager::AddLogEntry("Editor dialog closed callback received", LogTypeCPP_MESSAGE);
		}
		static void ModificationCallback(List<UIComponentSerializable^>^ newData)
		{
			VCNINodeWrapperData convertedData = ConvertCSharpDataToCPP(newData);
			if (VCN3dsExporterBridgeManager::EditorModificationCallback)
			{
				VCN3dsExporterBridgeManager::EditorModificationCallback(convertedData);
			}
			// Overflows the log
			//VCN3dsExporterBridgeManager::AddLogEntry("Editor dialog modification callback received", LogTypeCPP_MESSAGE);
		}
		static void ActivateStateCallback(bool activated)
		{
			if (VCN3dsExporterBridgeManager::EditorActivateStateCallback)
			{
				VCN3dsExporterBridgeManager::EditorActivateStateCallback(activated);
			}
		}

		static VCNAttributeType ConvertTypeToEnum(Type^ t)
		{
			System::Int32 value;
			if (EnumDict->TryGetValue(t, value))
			{
				return (VCNAttributeType)value;
			}
			else
			{
				return ATTR_UNDEFINED;
			}
		}
		static VCNINodeWrapperData ConvertCSharpDataToCPP(List<UIComponentSerializable^>^ data)
		{
			VCNINodeWrapperData cpp;

			for each (UIComponentSerializable^ bc in data)
			{
				VCNINodeComponent component;
				component.SetName(VCN3dsExporterBridgeHelpers::ConvertManagedString(bc->Name));
				for each (UIAttributeSerializable^ uiAttr in bc->Attributes)
				{
					VCNINodeAttributeKey key;
					key.name = VCN3dsExporterBridgeHelpers::ConvertManagedString(uiAttr->Name);
					key.type = ConvertTypeToEnum(uiAttr->Value->GetType());

					VCNINodeAttributeValue val;
					try 
					{
						DataVisitor^ visitor = DataVisitors[uiAttr->Value->GetType()];
						visitor->Visit(val, uiAttr->Value);
						component.AddOrEditAttribute(key.type, key.name, val);
					}
					catch(KeyNotFoundException^)
					{
					}
				}

				cpp.push_back(component);
			}
			// TODO: Implement this

			return cpp;
		}

		// Callbacks coming from the remote instance in the other AppDomain (they need to be dispatched to the local callbacks)
		static void RemoteDialogClosedCallback()
		{
			DialogClosedCallback();
		}
		static void RemoteModificationCallback()
		{
			RemoteEditorManager^ remoteManager = GetRemoteEditorManager();
			try
			{
				List<UIComponentSerializable^>^ curComponents = remoteManager->GetCurrentUIComponentsSerializable();
				ModificationCallback(curComponents);
			}
			catch (Exception^ e)
			{
				int i=0; 
			}
			
		}
		static void RemoteActivateStateCallback()
		{
			RemoteEditorManager^ remoteManager = GetRemoteEditorManager();
			ActivateStateCallback(remoteManager->GetCurrentEditorState());
		}

		static void ShutDownDispatcher()
		{
			System::Windows::Application^ application = System::Windows::Application::Current;
			if (application != nullptr)
			{
				System::Windows::Threading::Dispatcher^ dispatcher = application->Dispatcher;
				if (dispatcher != nullptr)
				{
					dispatcher->InvokeShutdown();
				}
			}
		}

		static void SetEditorAppDomain(AppDomain^ appDomain)
		{
			EditorAppDomain = appDomain;
		}
		static AppDomain^ GetEditorAppDomain()
		{
			if (EditorAppDomain == nullptr)
			{
				VCN3dsExporterBridgeManager::InitEditorAppDomain();
			}
			return EditorAppDomain;
		}

		static void SetRemoteEditorManager(RemoteEditorManager^ editorManager)
		{
			RemoteManager = editorManager;
		}
		static RemoteEditorManager^ GetRemoteEditorManager()
		{
			if (RemoteManager == nullptr)
			{
				VCN3dsExporterBridgeManager::InitEditorManager();
			}
			return RemoteManager;
		}

		static void SetRemoteEditorInitializer(RemoteEditorInitializer^ editorInit)
		{
			RemoteInitializer = editorInit;
		}
		static RemoteEditorInitializer^ GetRemoteEditorInitializer()
		{
			if (RemoteInitializer == nullptr)
			{
				VCN3dsExporterBridgeManager::InitEditorInitializer();
			}
			return RemoteInitializer;
		}

		static bool IsEditorAppDomainInit()
		{
			return EditorAppDomain != nullptr;
		}
		static bool IsRemoteInitializerInit()
		{
			return RemoteInitializer !=  nullptr;
		}
		static bool IsRemoteEditorManagerInit()
		{
			return RemoteManager != nullptr;
		}

		// Log Window methods
		static VCN3dsExporterLogWindow^ GetLogWindow()
		{
			return LogWindow;
		}

		static void InitLog(HWND parentWindowHandle)
		{
			VCN3dsExporterLogWindow^ logWindow = gcnew VCN3dsExporterLogWindow();
			System::Windows::Interop::WindowInteropHelper^ helper = gcnew System::Windows::Interop::WindowInteropHelper(logWindow);
			helper->Owner = (System::IntPtr)parentWindowHandle;
			logWindow->Show();
			LogWindow = logWindow;
		}

	public:
		//static List<BaseComponent^>^ CurrentComponents = nullptr;
		static VCN3dsExporterLogWindow^ LogWindow = nullptr;
	private:
		static AppDomain^ EditorAppDomain = nullptr;
		static RemoteEditorInitializer^ RemoteInitializer = nullptr;
		static RemoteEditorManager^ RemoteManager = nullptr;
		static Dictionary<Type^, DataVisitor^>^ DataVisitors = nullptr;
		static Dictionary<Type^, System::Int32>^ EnumDict = nullptr;
		
	};



	

// 
// 	public ref class VCN3dsExporterBridgeDataContainerAppDomain
// 	{
// 	public:
// 
// 		static void UnloadAppDomain()
// 		{
// 			if (CSPluginAppDomain == nullptr)
// 			{
// 				return;
// 			}
// 			AppDomain::Unload(CSPluginAppDomain);
// 			CSPluginAppDomain = nullptr;
// 		}
// 
// 		static AppDomain^ GetCSPluginAppDomain()
// 		{
// 			if (CSPluginAppDomain == nullptr)
// 			{
// 				System::Security::Policy::Evidence^ evidence = gcnew System::Security::Policy::Evidence(AppDomain::CurrentDomain->Evidence);
// 				AppDomainSetup^ setup = AppDomain::CurrentDomain->SetupInformation;
// 				CSPluginAppDomain = AppDomain::CreateDomain("CSPlugin", evidence, setup);
// 
// 				Type^ pdType = RemoteEditorInitializer::typeid;
// 				auto pd = static_cast<RemoteEditorInitializer^>( CSPluginAppDomain->CreateInstanceFrom(pdType->Assembly->Location, pdType->FullName)->Unwrap() );
// 
// 
// 				String^ baseLoc = Assembly::GetExecutingAssembly()->Location;
// 				String^ assDirPath = System::IO::Path::GetDirectoryName(baseLoc);
// 				String^ nodesCSharpPath = assDirPath + System::IO::Path::DirectorySeparatorChar + "VCNNodesCSharp.dll";
// 				pd->LoadAssembly(nodesCSharpPath);
// 
// 				String^ propertyEditorPath = assDirPath + System::IO::Path::DirectorySeparatorChar + "VCN3dsExporterPropertyEditor.dll";
// 				pd->LoadAssembly(propertyEditorPath);
// 
// 				String^ nodesCSharpCustomPath = assDirPath + System::IO::Path::DirectorySeparatorChar + "VCNNodesCSharpCustom.dll";
// 				pd->LoadAssembly(nodesCSharpCustomPath);
// 
// 				pd->BindAssemblyResolveEvent();
// 
// 
// 
// 				Type^ pdType2 = RemoteEditorManager::typeid;
// 				auto pd2 = static_cast<RemoteEditorManager^>( CSPluginAppDomain->CreateInstanceFrom(pdType2->Assembly->Location, pdType2->FullName)->Unwrap() );
// 
// 
// 			}
// 			return CSPluginAppDomain;
// 		}
// 
// 		
// 
// 	private:
// 		static AppDomain^ CSPluginAppDomain = nullptr;
// 
// 	};

	

	///////////////////////////////////////////


	bool VCN3dsExporterBridgeManager::Init(HWND parentWindowHandle)
	{ 
		// Bind the assembly resolver function for the current AppDomain (just in case we need it)
		AppDomain::CurrentDomain->AssemblyResolve += gcnew ResolveEventHandler(AssemblyResolveFunction);
		AppDomain::CurrentDomain->UnhandledException += gcnew UnhandledExceptionEventHandler(UnhandledExceptionFunction);

		VCN3dsExporterBridgeManagerDataContainer::SetupDataVisitors();

		VCN3dsExporterBridgeManagerDataContainer::InitLog(parentWindowHandle);

		// Init the Editor's AppDomain
		bool retVal = InitEditorAppDomain();

		if (retVal)
		{
			VCN3dsExporterBridgeManager::AddLogEntry("App Domain loaded successfully", LogTypeCPP_MESSAGE);
		}
		else
		{
			VCN3dsExporterBridgeManager::AddLogEntry("App Domain failed to load", LogTypeCPP_ERROR);
		}

		return retVal;
	}

	bool VCN3dsExporterBridgeManager::InitEditorAppDomain()
	{
		try
		{
			// Only run this if the AppDomain is nto initialized
			if (!VCN3dsExporterBridgeManagerDataContainer::IsEditorAppDomainInit())
			{
				System::Security::Policy::Evidence^ evidence = gcnew System::Security::Policy::Evidence(AppDomain::CurrentDomain->Evidence);
				AppDomainSetup^ setup = AppDomain::CurrentDomain->SetupInformation;
				AppDomain^ newAppDomain = AppDomain::CreateDomain("EditorAppDomain", evidence, setup);
				newAppDomain->InitializeLifetimeService();
				VCN3dsExporterBridgeManagerDataContainer::SetEditorAppDomain(newAppDomain);

				auto remoteInitializer = VCN3dsExporterBridgeManagerDataContainer::GetRemoteEditorInitializer();

				String^ baseLoc = Assembly::GetExecutingAssembly()->Location;
				String^ assDirPath = System::IO::Path::GetDirectoryName(baseLoc);
				String^ nodesCSharpPath = assDirPath + System::IO::Path::DirectorySeparatorChar + "VCNNodesCSharp.dll";
				if (!remoteInitializer->LoadAssembly(nodesCSharpPath))
				{
					//VCN3dsExporterBridgeManager::AddLogEntry("Failed to load the VCNNodesCSharp.dll assembly", LogTypeCPP_ERROR);
					ShowErrorMessage(L"Could not load VCNNodesCSharp.dll assembly. The application may be unstable.\n Make sure that the file exists in the plugin directory.");
					return false;
				}

				String^ propertyEditorPath = assDirPath + System::IO::Path::DirectorySeparatorChar + "VCN3dsExporterPropertyEditor.dll";
				if (!remoteInitializer->LoadAssembly(propertyEditorPath))
				{
					//VCN3dsExporterBridgeManager::AddLogEntry("Failed to load the VCN3dsExporterPropertyEditor.dll assembly", LogTypeCPP_ERROR);
					ShowErrorMessage(L"Could not load VCN3dsExporterPropertyEditor.dll assembly. The application may be unstable.\n Make sure that the file exists in the plugin directory.");
					return false;
				}

				String^ nodesCSharpCustomPath = assDirPath + System::IO::Path::DirectorySeparatorChar + "VCNNodesCSharpCustom.dll";
				if (!remoteInitializer->LoadAssembly(nodesCSharpCustomPath))
				{
					//VCN3dsExporterBridgeManager::AddLogEntry("Failed to load the VCNNodesCSharpCustom.dll assembly", LogTypeCPP_ERROR);
					ShowErrorMessage(L"Could not load VCNNodesCSharpCustom.dll assembly. The application may be unstable.\n Make sure that the file exists in the plugin directory.");
					return false;
				}

				remoteInitializer->BindAssemblyResolveEvent();
			}
		}
		catch (Exception^)
		{
			ShowErrorMessage(L"An error has occurred while loading the plugin. The application may be unstable.\n Make sure that the file exists in the plugin directory.");
			return false;
		}
		return true;
	}

	void VCN3dsExporterBridgeManager::InitEditorManager()
	{
		if (!VCN3dsExporterBridgeManagerDataContainer::IsRemoteEditorManagerInit())
		{
			AppDomain^ appDomain = VCN3dsExporterBridgeManagerDataContainer::GetEditorAppDomain();

			Type^ pdType = RemoteEditorManager::typeid;
			auto remoteManager = static_cast<RemoteEditorManager^>( appDomain->CreateInstanceFrom(pdType->Assembly->Location, pdType->FullName)->Unwrap() );
			remoteManager->SetParentAppDomain(AppDomain::CurrentDomain);
			VCN3dsExporterBridgeManagerDataContainer::SetRemoteEditorManager(remoteManager);
		}
	}


	void VCN3dsExporterBridgeManager::InitEditorInitializer()
	{
		if (!VCN3dsExporterBridgeManagerDataContainer::IsRemoteInitializerInit())
		{
			AppDomain^ appDomain = VCN3dsExporterBridgeManagerDataContainer::GetEditorAppDomain();

			Type^ pdType = RemoteEditorInitializer::typeid;
			auto remoteInitializer = static_cast<RemoteEditorInitializer^>( appDomain->CreateInstanceFrom(pdType->Assembly->Location, pdType->FullName)->Unwrap() );
			VCN3dsExporterBridgeManagerDataContainer::SetRemoteEditorInitializer(remoteInitializer);
		}
	}


	HWND VCN3dsExporterBridgeManager::OpenPropertyEditor(HWND parentWindowHandle, PropertyEditorCloseCallback closeCallback, PropertyEditorModificationCallback editorModificationCallback, PropertyEditorActivateStateCallback activateStateCallback)
	{
		VCN3dsExporterBridgeManager::AddLogEntry("Opening the Editor window", LogTypeCPP_MESSAGE);
		CppCloseCallback = closeCallback;
		EditorModificationCallback = editorModificationCallback;
		EditorActivateStateCallback = activateStateCallback;

		RemoteEditorManager^ remoteEditorManager = VCN3dsExporterBridgeManagerDataContainer::GetRemoteEditorManager();
		remoteEditorManager->SetPropertyEditorCallbacks(
			gcnew CrossAppDomainDelegate(VCN3dsExporterBridgeManagerDataContainer::RemoteDialogClosedCallback),
			gcnew CrossAppDomainDelegate(VCN3dsExporterBridgeManagerDataContainer::RemoteModificationCallback),
			gcnew CrossAppDomainDelegate(VCN3dsExporterBridgeManagerDataContainer::RemoteActivateStateCallback)
			);
		return remoteEditorManager->ShowEditorWindow(parentWindowHandle);
	}

	void VCN3dsExporterBridgeManager::ClosePropertyEditor() 
	{
		VCN3dsExporterBridgeManager::AddLogEntry("Closing the Editor window", LogTypeCPP_MESSAGE);
		RemoteEditorManager^ remoteEditorManager = VCN3dsExporterBridgeManagerDataContainer::GetRemoteEditorManager();
		remoteEditorManager->HideEditorWindow();
	}

	void VCN3dsExporterBridgeManager::EnableProgressWindow(const std::string& name)
	{
		RemoteEditorManager^ remoteEditorManager = VCN3dsExporterBridgeManagerDataContainer::GetRemoteEditorManager();
		remoteEditorManager->ShowProgressWindow();
		remoteEditorManager->SetProgressWindowPercentage(0);
		remoteEditorManager->SetProgressWindowName(gcnew System::String(name.c_str()));
	}

	void VCN3dsExporterBridgeManager::DisableProgressWindow()
	{
		RemoteEditorManager^ remoteEditorManager = VCN3dsExporterBridgeManagerDataContainer::GetRemoteEditorManager();
		remoteEditorManager->HideProgressWindow();
	}

	void VCN3dsExporterBridgeManager::SetProgressWindowValue( int newValue )
	{
		RemoteEditorManager^ remoteEditorManager = VCN3dsExporterBridgeManagerDataContainer::GetRemoteEditorManager();
		remoteEditorManager->SetProgressWindowPercentage(newValue);
	}

	void VCN3dsExporterBridgeManager::UpdatePropertyEditor( const VCNINodeWrapperData& data )
	{
		// Overflows the log
		//VCN3dsExporterBridgeManager::AddLogEntry("Updating the Editor window", LogTypeCPP_MESSAGE);
		List<BaseComponent^>^ components = gcnew List<BaseComponent^>();

		for each (VCNINodeComponent c in data)
		{

			String^ componentName = gcnew String(c.GetName().c_str());
			BaseComponent^ bc = VCNNodesCSharpHelper::BuildComponentFromName(componentName);
			if (bc == nullptr)
			{
				bc = gcnew BaseComponent();
			}
			for (std::map<VCNINodeAttributeKey, VCNINodeAttributeValue>::iterator it = c.GetAttributesBegin(); it != c.GetAttributesEnd(); it++)
			{
				String^ attrName = gcnew String(it->first.name.c_str());

				String^ buf = nullptr;
				std::string base;
				::Vector2 baseV2;
				::Vector3 baseV3;
				::Vector4 baseV4;
				::LuaTrigger baseLuaTrigger;
				VCNNodesCSharp::Vector2^ v2;
				VCNNodesCSharp::Vector3^ v3;
				VCNNodesCSharp::Vector4^ v4;
				VCNNodesCSharp::LuaTrigger^ luaTrigger;

				// Convert attribute
				switch (it->first.type)
				{
				case ATTR_UNDEFINED	         :
					break;
				case ATTR_ID				 :	
					bc->SetAttribute(attrName, *it->second.GetValueAsInt());
					break;
				case ATTR_INTEGER			 :
					bc->SetAttribute(attrName, *it->second.GetValueAsInt());
					break;
				case ATTR_UNSIGNED_INTEGER	 :
					bc->SetAttribute(attrName, *it->second.GetValueAsUnsignedInt());
					break;
				case ATTR_FLOAT              :
					bc->SetAttribute(attrName, *it->second.GetValueAsFloat());
					break;
				case ATTR_DOUBLE             :
					bc->SetAttribute(attrName, *it->second.GetValueAsDouble());
					break;
				case ATTR_STRING             :
					base = *it->second.GetValueAsString();
					buf = gcnew String(base.c_str());
					bc->SetAttribute(attrName, buf);
					break;
				case ATTR_CHAR               :
					bc->SetAttribute(attrName, *it->second.GetValueAsChar());
					break;
				case ATTR_VECTOR2            :
					v2 = gcnew VCNNodesCSharp::Vector2();
					baseV2 = *it->second.GetValueAsVector2();
					v2->x = baseV2.x;
					v2->y = baseV2.y;
					bc->SetAttribute(attrName, v2);
					break;
				case ATTR_VECTOR3            :
					v3 = gcnew VCNNodesCSharp::Vector3();
					baseV3 = *it->second.GetValueAsVector3();
					v3->x = baseV3.x;
					v3->y = baseV3.y;
					v3->z = baseV3.z;
					bc->SetAttribute(attrName, v3);
					break;
				case ATTR_VECTOR4            :
					v4 = gcnew VCNNodesCSharp::Vector4();
					baseV4 = *it->second.GetValueAsVector4();
					v4->x = baseV4.x;
					v4->y = baseV4.y;
					v4->z = baseV4.z;
					v4->w = baseV4.w;
					bc->SetAttribute(attrName, v4);
					break;
				case ATTR_BOOL               :
					bc->SetAttribute(attrName, *it->second.GetValueAsBool());
					break;
				case ATTR_LUA_TRIGGER:
					luaTrigger = gcnew VCNNodesCSharp::LuaTrigger();
					baseLuaTrigger = *it->second.GetValueAsLuaTrigger();
					luaTrigger->TriggerName = gcnew String(baseLuaTrigger.triggerName.c_str());
					luaTrigger->TriggerType = gcnew String(baseLuaTrigger.triggerType.c_str());
					bc->SetAttribute(attrName, luaTrigger);
					break;
				default:
					break;
				}
			}
			components->Add(bc);
		}


		RemoteEditorManager^ remoteEditorManager = VCN3dsExporterBridgeManagerDataContainer::GetRemoteEditorManager();
		remoteEditorManager->SetEditorWindowData(BaseComponent::BuildUIComponentList(components));
	}

	bool VCN3dsExporterBridgeManager::ReloadDll()
	{
// 		VCN3dsExporterBridgeManager::AddLogEntry("Reloading the DLLs", LogTypeCPP_MESSAGE);
// 
// 		AppDomain^ currentAppDomain = VCN3dsExporterBridgeManagerDataContainer::GetEditorAppDomain();
// 		currentAppDomain->DoCallBack(gcnew CrossAppDomainDelegate(VCN3dsExporterBridgeManagerDataContainer::ShutDownDispatcher));
// 
// 		auto initializer = VCN3dsExporterBridgeManagerDataContainer::GetRemoteEditorInitializer();
// 		initializer->CleanForAppDomainUnload();
// 
// 		auto editorManager = VCN3dsExporterBridgeManagerDataContainer::GetRemoteEditorManager();
// 		editorManager->CleanForAppDomainUnload();
// 
// 		
// 		for (int i=0; i<2; i++)
// 		{
// 			try
// 			{
// 				System::GC::Collect();
// 				// FIXME: Fix the bug where the Unload is failing because there are references/threads still active in the WPF Windows
// 				AppDomain::Unload(currentAppDomain);
// 				break;
// 			}
// 			catch (Exception^ e)
// 			{
// 				VCN3dsExporterBridgeManager::AddLogEntry(VCN3dsExporterBridgeHelpers::ConvertManagedString("Could not delete the old AppDomain. You can continue working but it will stay in memory (Try " + (i + 1) + ")"), LogTypeCPP_WARNING);
// 			}
// 			
// 		}
// 		
// 		VCN3dsExporterBridgeManagerDataContainer::SetEditorAppDomain(nullptr);
// 		VCN3dsExporterBridgeManagerDataContainer::SetRemoteEditorInitializer(nullptr);
// 		VCN3dsExporterBridgeManagerDataContainer::SetRemoteEditorManager(nullptr);
// 
// 		if (!InitEditorAppDomain())
// 		{
// 			ShowErrorMessage(L"Could not reload the plugin's DLL.");
// 			return false;
// 		}
// 		return true;

		VCN3dsExporterBridgeManager::AddLogEntry("The Reload of the DLLs is not supported yet", LogTypeCPP_WARNING);
		return false;
	}



	void VCN3dsExporterBridgeManager::ShowErrorMessage( const wchar_t* message )
	{
		MessageBox(nullptr, message, L"An error has occurred", MB_OK);
	}

	void VCN3dsExporterBridgeManager::AddLogEntry( const std::string& message, LogTypeCPP logType )
	{
		if (logType == LogTypeCPP_ERROR || logType == LogTypeCPP_FATAL)
		{
			if (System::Diagnostics::Debugger::IsAttached)
			{
				System::Diagnostics::Debugger::Break();
			}
		}

		String^ messageManaged = gcnew String(message.c_str());
		if (VCN3dsExporterBridgeManagerDataContainer::LogWindow != nullptr)
		{
			VCN3dsExporterBridgeManagerDataContainer::LogWindow->AddLogEntry(messageManaged, (LogType) logType);
		}
	}



	bool VCN3dsExporterBridgeManager::TryToAddToSVN( const std::string& filepathToAdd )
	{
		int exitCode = 1;
		try
		{
			System::Diagnostics::Process^ proc = gcnew System::Diagnostics::Process();
			proc->StartInfo->WorkingDirectory = "C:\\Program Files\\TortoiseSVN\\bin";
			proc->StartInfo->FileName = "C:\\Program Files\\TortoiseSVN\\bin\\TortoiseProc.exe";
			proc->StartInfo->Arguments = "/command:add /path:\"" + gcnew String(filepathToAdd.c_str()) + "\" /closeonend:1";
			proc->StartInfo->UseShellExecute = false;
			proc->StartInfo->RedirectStandardError = false;
			proc->Start();
			proc->WaitForExit();
			int exitCode = proc->ExitCode;
			proc->Close();
		}
		catch (Exception^)
		{
		}
		return exitCode == 0;
	}

	void VCN3dsExporterBridgeManager::SetValidSelectionState( bool validSelection )
	{
		RemoteEditorManager^ remoteEditorManager = VCN3dsExporterBridgeManagerDataContainer::GetRemoteEditorManager();
		remoteEditorManager->SetEditorWindowSelectionState(validSelection);
	}


	PropertyEditorActivateStateCallback VCN3dsExporterBridgeManager::EditorActivateStateCallback = nullptr;
	PropertyEditorModificationCallback VCN3dsExporterBridgeManager::EditorModificationCallback = nullptr;
	PropertyEditorCloseCallback VCN3dsExporterBridgeManager::CppCloseCallback = nullptr;

}




