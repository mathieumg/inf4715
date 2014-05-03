--------------------------------------------------------------------------------
-- Unit tests for the loadDllsFromDir
--
-- Author:	Attila Szabo 
-- Created: Aug.23.2010
--------------------------------------------------------------------------------
fileIn "MxsUnit/MxsUnit.ms"
fileIn "DevTools/PluginDllUtilities.ms"

struct UnitTest_LoadDllsFromDir
(	
private
	originalPluginDllCount,
	
	-- loadedStateOption = true measn use a loaded plug-in for testing
	-- loadedStateOption = false means use a deferred plug-in for testing
	function doTestLoadDllsFromDir loadedStateOption forceLoadOption =
	(
		-- find first plug-in dll that has the specified loaded state (loaded or deferred)
		plugins = getPluginDllsFullPath loaded:loadedStateOption
		if (plugins.count > 0) then
		(
			with quiet true 
			(
				local pluginDllPath = getFilenamePath plugins[1]
				local pluginDllFileName = filenameFromPath plugins[1]
				loadDllsFromDir pluginDllPath pluginDllFileName forceLoad:forceLoadOption
			)
			local actualLoadedState = isPluginDllLoaded plugins[1]
			if (loadedStateOption) then
			(
				-- a loaded plug-in's state should be loaded whether it was force loaded or not
				assert_true actualLoadedState
			)
			else
			(
				if (false == forceLoadOption) then 
				(
					-- a deferred plug-in's state should be deferred if it was defer loaded
					assert_false actualLoadedState
				)
				else
				(
					-- a deferred plug-in's state should be loaded if it was force loaded
					assert_true actualLoadedState
				)
			)
		)

		assert ((countPluginDlls()) == originalPluginDllCount)
	),
	
public
	function setup =
	(
		resetMaxFile #noprompt
		originalPluginDllCount =  countPluginDlls()
	),		
	
	function testDeferLoadingLoadedPluginDlls = 
	(
		local loadedState = true
		local forceLoad = false
		doTestLoadDllsFromDir loadedState forceLoad
	),

	function testForceLoadingLoadedPluginDlls = 
	(
		local loadedState = true
		local forceLoad = true
		doTestLoadDllsFromDir loadedState forceLoad
	),

	function testDeferLoadingDeferredPluginDlls = 
	(
		local loadedState = false
		local forceLoad = false
		doTestLoadDllsFromDir loadedState forceLoad
	),

	function testForceLoadingDeferredPluginDlls = 
	(
		local loadedState = false
		local forceLoad = true
		doTestLoadDllsFromDir loadedState forceLoad
	),

	Tests = 
	#(
		testDeferLoadingLoadedPluginDlls,
		testForceLoadingLoadedPluginDlls,
		testDeferLoadingDeferredPluginDlls,
		testForceLoadingDeferredPluginDlls
	)
)

-- uncomment these lines to run tests outside of the mxs unit test framework, which "eats" exceptions making it harder to debug your tests
--test = UnitTest_LoadDllsFromDir()
--test.testDeferringLoadedPluginDlls()
run_test_fixture UnitTest_LoadDllsFromDir script:(getThisScriptFilename())	

