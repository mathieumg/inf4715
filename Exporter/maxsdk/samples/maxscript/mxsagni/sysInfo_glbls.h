define_struct_global (_T("processAffinity"),	_T("sysInfo"),		getProcessAffinity,		setProcessAffinity);
define_struct_global (_T("systemAffinity"),		_T("sysInfo"),		getSystemAffinity,		setSystemAffinity);

define_struct_global (_T("DesktopSize"),		_T("sysInfo"),		getDesktopSize,			NULL);
define_struct_global (_T("DesktopBPP"),			_T("sysInfo"),		getDesktopBPP,			NULL);
define_struct_global (_T("MAXPriority"),		_T("sysInfo"),		getMAXPriority,			setMAXPriority);
define_struct_global (_T("username"),			_T("sysInfo"),		getusername,			NULL);
define_struct_global (_T("computername"),		_T("sysInfo"),		getcomputername,		NULL);
define_struct_global (_T("windowsdir"),			_T("sysInfo"),		getWinDirectory,		NULL);
define_struct_global (_T("systemdir"),			_T("sysInfo"),		getSystemDirectory,		NULL);
define_struct_global (_T("tempdir"),			_T("sysInfo"),		getTempDirectory,		NULL);
define_struct_global (_T("currentdir"),			_T("sysInfo"),		getCurrentDirectory,	setCurrentDirectory);
define_struct_global (_T("cpucount"),			_T("sysInfo"),		getCPUcount,			NULL);
