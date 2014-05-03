--------------------------------------------------------------------------------
-- Unit tests for the Morpher modifier
--
-- Author:	Attila Szabo                     
-- Created: Sep.09.2010
--------------------------------------------------------------------------------
fileIn "MxsUnit/MxsUnit.ms"

struct UnitTest_Morpher
(
private
	NUM_MAX_MORPH_CHANNELS = 100,
	NUM_MAX_PROGRESSIVE_TARGETS = 25,
	
	mMorphHostObject = undefined,
	mMorphTargets = #(), -- array of NUM_MORPH_CHANNELS objects
	mMorphProgressiveTargets = #(), -- array [NUM_MORPH_CHANNELS, NUM_PROGRESSIVE_TARGETS] objects
	
	EMTPTY_CHANNEL_NAME="- empty -",
	
	function doSetup numChannels numTargetsPerChannel =
	(
		assert_true (1 <= numChannels)
		assert_true (numChannels <= NUM_MAX_MORPH_CHANNELS)
		assert_true (0 <= numTargetsPerChannel)
		assert_true (numTargetsPerChannel <= NUM_MAX_PROGRESSIVE_TARGETS)
		
		-- Create the set of objects used as morph targets and object to be morphed
		mMorphHostObject = Plane length:10 width:10 name:"MorphHost"
		convertToMesh mMorphHostObject
		for i=1 to numChannels do 
		(
			maxOps.CloneNodes mMorphHostObject offset:[i*(10+2), 0, 0] cloneType:#copy newNodes:&newMorphTarget
			mMorphTargets[i] = newMorphTarget[1]
			mMorphTargets[i].name="MorphTarget"+(i as string)
			local progressiveTargets=#()
			for j = 1 to numTargetsPerChannel do
			(
				maxOps.CloneNodes newMorphTarget[1] offset:[0, j*(10+2), 0] cloneType:#copy newNodes:&newMorphProgressiveTraget
				progressiveTargets[j] = newMorphProgressiveTraget[1]
				progressiveTargets[j].name=mMorphTargets[i].name+"." +(j as string)

			)		
			mMorphProgressiveTargets[i] = progressiveTargets 
		)
		
		-- setup a morpher with targets
		local morpherMod = Morpher()
		addModifier mMorphHostObject morpherMod
		WM3_RebuildInternalCache morpherMod
		for i=1 to numChannels do 
		(
			WM3_MC_SetTarget morpherMod i mMorphTargets[i]
			WM3_MC_SetValue morpherMod i (i as float)
			for j = 1 to numTargetsPerChannel do
			(
				WM3_AddProgressiveMorphNode morpherMod i mMorphProgressiveTargets[i][j]
				assert_true (undefined != (WM3_GetProgressiveMorphNode morpherMod i j))
			)		
		)		
	),
	
	function doTestChannelSwap morphId1 morphId2 = 
	(
		local morphTarget1 = WM3_MC_GetTarget mMorphHostObject.morpher morphId1
		local morphTarget2 = WM3_MC_GetTarget mMorphHostObject.morpher morphId2
		assert_true (morphTarget1 != morphTarget2)
		with undo on
		(
			WM3_SwapMorph mMorphHostObject.morpher morphId1  morphId2
			assert_true (WM3_MC_GetTarget mMorphHostObject.morpher morphId1 == morphTarget2)
		)
		max undo
		assert_true (WM3_MC_GetTarget mMorphHostObject.morpher morphId1 == morphTarget1)
		max redo
		assert_true (WM3_MC_GetTarget mMorphHostObject.morpher morphId1 == morphTarget2)
		max undo
		assert_true (WM3_MC_GetTarget mMorphHostObject.morpher morphId1 == morphTarget1)
	),

	function doTestChannelMove morphId1 morphId2 = 
	(
		local morphTarget1 = WM3_MC_GetTarget mMorphHostObject.morpher morphId1
		local morphTarget2 = WM3_MC_GetTarget mMorphHostObject.morpher morphId2
		assert_true (morphTarget1 != morphTarget2)
		with undo on
		(
			WM3_MoveMorph mMorphHostObject.morpher morphId1  morphId2
			assert_true (WM3_MC_GetTarget mMorphHostObject.morpher morphId1 == undefined)
			assert_true (WM3_MC_GetTarget mMorphHostObject.morpher morphId2 == morphTarget1)
		)
		max undo
		assert_true (WM3_MC_GetTarget mMorphHostObject.morpher morphId1 == morphTarget1)
		assert_true (WM3_MC_GetTarget mMorphHostObject.morpher morphId2 == morphTarget2)
		max redo
		assert_true (WM3_MC_GetTarget mMorphHostObject.morpher morphId2 == morphTarget1)
		max undo
		assert_true (WM3_MC_GetTarget mMorphHostObject.morpher morphId1 == morphTarget1)
		assert_true (WM3_MC_GetTarget mMorphHostObject.morpher morphId2 == morphTarget2)
	),

public
	function Setup =
	(
		resetMaxFile #noprompt
	),
	
	function teardown =
	(
	
	),
	
	function testMorpherModifierValidity =
	(
		assert_true (IsValidMorpherMod (Morpher()))
		assert_false (IsValidMorpherMod (Bend()))
	),
	
	function testNumChannels = 
	(
		assert_equal NUM_MAX_MORPH_CHANNELS (WM3_NumberOfChannels (Morpher()))
	),
	
	function testChannelName = 
	(
		local morpherMod = Morpher()
		-- getting name 
		for i = 1 to NUM_MAX_MORPH_CHANNELS do 
		(
			assert_equal EMTPTY_CHANNEL_NAME (WM3_MC_GetName morpherMod i)
		)
		-- accessing out of range channels 
		assert_equal EMTPTY_CHANNEL_NAME (WM3_MC_GetName morpherMod -1)
		assert_equal EMTPTY_CHANNEL_NAME (WM3_MC_GetName morpherMod 101) -- This was buggy
		assert_equal EMTPTY_CHANNEL_NAME (WM3_MC_GetName morpherMod 10000)
		
		-- setting names
		local newName = "testChannelName"
		for i = 1 to NUM_MAX_MORPH_CHANNELS do 
		(
			WM3_MC_SetName morpherMod i newName
			assert_equal newName (WM3_MC_GetName morpherMod i)
		)
	),
	
	function testChannelActiveState = 
	(
		local morphChannelId = 1
		doSetup morphChannelId 0
		assert_true (WM3_MC_IsActive mMorphHostObject.morpher morphChannelId)
		WM3_MC_SetActive mMorphHostObject.morpher morphChannelId false
		assert_false (WM3_MC_IsActive mMorphHostObject.morpher morphChannelId)
	),
	
	function testChannelHasDataState = 
	(
		local morpherMod = Morpher()
		assert_false (WM3_MC_HasData morpherMod 1)
		
		local morphChannelId = 1
		doSetup morphChannelId 0
		assert_true (WM3_MC_HasData mMorphHostObject.morpher morphChannelId)
		local morphTarget = WM3_MC_GetTarget mMorphHostObject.morpher morphChannelId
		assert_true (undefined != morphTarget)
		with undo on 
		(
			delete morphTarget
		)
		assert_true (WM3_MC_HasData mMorphHostObject.morpher morphChannelId)
		max undo
		assert_true (WM3_MC_HasData mMorphHostObject.morpher morphChannelId)
		max redo 
		assert_true (WM3_MC_HasData mMorphHostObject.morpher morphChannelId)
		max undo
		assert_true (undefined != morphTarget)
	),

	function testChannelIsValidState = 
	(
		local morpherMod = Morpher()
		assert_true (WM3_MC_IsValid morpherMod 1)
		
		local morphChannelId = 1
		doSetup morphChannelId 0
		assert_true (WM3_MC_IsValid mMorphHostObject.morpher morphChannelId)
		local morphTarget = WM3_MC_GetTarget mMorphHostObject.morpher morphChannelId
		assert_true (undefined != morphTarget)
		with undo on 
		(
			delete morphTarget
		)
		assert_true (WM3_MC_IsValid mMorphHostObject.morpher morphChannelId)
		max undo
		assert_true (WM3_MC_IsValid mMorphHostObject.morpher morphChannelId)
		assert_true (undefined != morphTarget)
	),

	function testChannelMorphWeight = 
	(
		local morphChannelId = 1
		doSetup morphChannelId 0
		local startWeight = 30.0
		local endWeight = 60.0
		local startTime = 0
		local endTime = 100
		with animate on
		(
			at time startTime WM3_MC_SetValue mMorphHostObject.morpher morphChannelId startWeight
			at time endTime WM3_MC_SetValue mMorphHostObject.morpher morphChannelId endWeight
		)
		assert_true (at time startTime (WM3_MC_GetValue mMorphHostObject.morpher morphChannelId) == startWeight)
		assert_true (at time endTime (WM3_MC_GetValue mMorphHostObject.morpher morphChannelId) == endWeight)
	),

	function testChannelMorphTarget = 
	(
		local morphChannelId = 1
		doSetup morphChannelId 0
		assert_true (WM3_MC_HasTarget mMorphHostObject.morpher morphChannelId)
		assert_false (WM3_MC_HasTarget mMorphHostObject.morpher (morphChannelId+1))
		-- can't set  morph target for channel that already has one
		maxOps.CloneNodes mMorphHostObject cloneType:#copy newNodes:&newMorphTarget
		assert_true (undefined != newMorphTarget[1])
		assert_false (WM3_MC_SetTarget mMorphHostObject.morpher morphChannelId newMorphTarget[1])
			
		local morphTarget = WM3_MC_GetTarget mMorphHostObject.morpher morphChannelId
		with undo on
		(
			delete morphTarget
		)
		assert_false (WM3_MC_HasTarget mMorphHostObject.morpher morphChannelId)
		max undo
		assert_true (WM3_MC_HasTarget mMorphHostObject.morpher morphChannelId)
		max redo
		assert_false (WM3_MC_HasTarget mMorphHostObject.morpher morphChannelId)
		max undo
		assert_true (WM3_MC_HasTarget mMorphHostObject.morpher morphChannelId)
		
		local boxObject = Box()
		WM3_MC_SetTarget mMorphHostObject.morpher (morphChannelId+1) boxObject
		assert_undefined (WM3_MC_GetTarget mMorphHostObject.morpher (morphChannelId+1))
	),
	
	function testProgressiveMorphTension = 
	(
		local numMorphChannels = 1
		local numProgMorphs = NUM_MAX_PROGRESSIVE_TARGETS-1
		doSetup numMorphChannels numProgMorphs
		local tension = 0.7
		assert_true (WM3_SetProgressiveMorphTension mMorphHostObject.morpher numMorphChannels tension)
		assert_true (tension == (WM3_GetProgressiveMorphTension mMorphHostObject.morpher numMorphChannels))
		assert_true (WM3_SetProgressiveMorphTension mMorphHostObject.morpher numMorphChannels (2*tension))
		assert_true (1.0 == (WM3_GetProgressiveMorphTension mMorphHostObject.morpher numMorphChannels))
	),
	
	function testProgressiveMorphWeight = 
	(
		local numMorphChannels = 1
		local numProgMorphs = NUM_MAX_PROGRESSIVE_TARGETS
		doSetup numMorphChannels numProgMorphs
		local weight = 70.0
		local progMT = WM3_GetProgressiveMorphNode mMorphHostObject.morpher numMorphChannels numProgMorphs
		assert_true (undefined != progMT)
		assert_true (WM3_SetProgressiveMorphWeight mMorphHostObject.morpher numMorphChannels progMT weight)
		assert_true (weight == (WM3_GetProgressiveMorphWeight mMorphHostObject.morpher numMorphChannels progMT))
		assert_true (WM3_SetProgressiveMorphWeight mMorphHostObject.morpher numMorphChannels progMT (2*weight))
		assert_true (100.0 == (WM3_GetProgressiveMorphWeight mMorphHostObject.morpher numMorphChannels progMT))
	),

	function testGetProgressiveMorphTarget = 
	(
		local numMorphChannels = 1
		local numProgMorphs = NUM_MAX_PROGRESSIVE_TARGETS
		doSetup numMorphChannels numProgMorphs
		assert_true ((numProgMorphs+1) == (WM3_NumberOfProgressiveMorphs mMorphHostObject.morpher numMorphChannels))

		local progMT = WM3_GetProgressiveMorphNode mMorphHostObject.morpher numMorphChannels 0
		assert_true (undefined == progMT)
		for i=1 to (WM3_NumberOfProgressiveMorphs mMorphHostObject.morpher numMorphChannels) do
		(
			progMT = WM3_GetProgressiveMorphNode mMorphHostObject.morpher numMorphChannels i
			assert_true (undefined != progMT)
		)
	),
	
	function testAddProgressiveMorphTarget =
	(
		local numMorphChannels = 1
		local numProgMorphs = NUM_MAX_PROGRESSIVE_TARGETS-1
		doSetup numMorphChannels numProgMorphs
		assert_true ((numProgMorphs+1) == (WM3_NumberOfProgressiveMorphs mMorphHostObject.morpher numMorphChannels))

		assert_true (undefined != mMorphTargets[1])
		maxOps.CloneNodes mMorphTargets[1] cloneType:#copy newNodes:&newMorphTarget

		with undo on
		(
			assert_true (WM3_AddProgressiveMorphNode mMorphHostObject.morpher numMorphChannels newMorphTarget[1])
			assert_true ((numProgMorphs+2) == (WM3_NumberOfProgressiveMorphs mMorphHostObject.morpher numMorphChannels))
		)
		
		max undo
		assert_true ((numProgMorphs+1) == (WM3_NumberOfProgressiveMorphs mMorphHostObject.morpher numMorphChannels))
		max redo
		assert_true ((numProgMorphs+2) == (WM3_NumberOfProgressiveMorphs mMorphHostObject.morpher numMorphChannels))
		
		assert_false (WM3_AddProgressiveMorphNode mMorphHostObject.morpher (numMorphChannels+1) newMorphTarget[1])
	),
	
	function testDeleteProgressiveMorphTarget = 
	(
		local numMorphChannels = 1
		local numProgMorphs = NUM_MAX_PROGRESSIVE_TARGETS
		doSetup numMorphChannels numProgMorphs
		local numProgressiveMorphs = WM3_NumberOfProgressiveMorphs mMorphHostObject.morpher numMorphChannels
		assert_true ((numProgMorphs+1) == numProgressiveMorphs)

		with undo on
		(
			assert_true (WM3_DeleteProgressiveMorphNode mMorphHostObject.morpher numMorphChannels 1)
			assert_true (WM3_DeleteProgressiveMorphNode mMorphHostObject.morpher numMorphChannels numProgMorphs)
			assert_true ( 2 == numProgressiveMorphs - (WM3_NumberOfProgressiveMorphs mMorphHostObject.morpher numMorphChannels))
		)
		
		max undo
		assert_true ( 0 == numProgressiveMorphs - (WM3_NumberOfProgressiveMorphs mMorphHostObject.morpher numMorphChannels))
		max redo
		assert_true ( 2 == numProgressiveMorphs - (WM3_NumberOfProgressiveMorphs mMorphHostObject.morpher numMorphChannels))
	),

	function testChannelSwap = 
	(
		local numMorphChannels = NUM_MAX_MORPH_CHANNELS-1
		doSetup numMorphChannels 0
		doTestChannelSwap 1 2
		doTestChannelSwap 1 NUM_MAX_MORPH_CHANNELS
	),
	
	function testChannelMove = 
	(
		local numMorphChannels = NUM_MAX_MORPH_CHANNELS-1
		doSetup numMorphChannels 0
		doTestChannelMove 1 2
		doTestChannelMove 1 NUM_MAX_MORPH_CHANNELS
	),
	
	function testChannelDelete = 
	(
		local numMorphChannels = 1
		local numProgMorphs = NUM_MAX_PROGRESSIVE_TARGETS
		doSetup numMorphChannels numProgMorphs
		
		with undo on
		(
			WM3_MC_Delete mMorphHostObject.morpher numMorphChannels
			assert_false (WM3_MC_HasData mMorphHostObject.morpher numMorphChannels)
			assert_false (WM3_MC_HasTarget mMorphHostObject.morpher numMorphChannels)
			assert_true (0 == WM3_NumberOfProgressiveMorphs mMorphHostObject.morpher numMorphChannels)
		)
		max undo
		assert_true (WM3_MC_HasData mMorphHostObject.morpher numMorphChannels)
		assert_true (WM3_MC_HasTarget mMorphHostObject.morpher numMorphChannels)
		assert_true ((numProgMorphs +1) == WM3_NumberOfProgressiveMorphs mMorphHostObject.morpher numMorphChannels)
		max redo
		assert_false (WM3_MC_HasData mMorphHostObject.morpher numMorphChannels)
		assert_false (WM3_MC_HasTarget mMorphHostObject.morpher numMorphChannels)
		assert_true (0 == WM3_NumberOfProgressiveMorphs mMorphHostObject.morpher numMorphChannels)
		max undo
		assert_true (WM3_MC_HasData mMorphHostObject.morpher numMorphChannels)
		assert_true (WM3_MC_HasTarget mMorphHostObject.morpher numMorphChannels)
		assert_true ((numProgMorphs +1) == WM3_NumberOfProgressiveMorphs mMorphHostObject.morpher numMorphChannels)
		
		assert_true (WM3_MC_Delete mMorphHostObject.morpher (numMorphChannels+1))
	),
	
	Tests = 
	#(
		testMorpherModifierValidity,
		testNumChannels,
		testChannelName,
		testChannelActiveState,
		testChannelHasDataState,
		testChannelIsValidState,
		testChannelMorphWeight,
		testChannelMorphTarget,
		testProgressiveMorphTension,
		testProgressiveMorphWeight,
		testGetProgressiveMorphTarget,
		testAddProgressiveMorphTarget,
		testDeleteProgressiveMorphTarget,
		testChannelSwap,
		testChannelMove,
		 testChannelDelete
	)
)

run_test_fixture UnitTest_Morpher script:(getThisScriptFilename())
