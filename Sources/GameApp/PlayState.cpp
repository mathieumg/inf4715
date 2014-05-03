///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Implements the play state class. 
///

#include "Precompiled.h"
#include "PlayState.h"

// Project includes
#include "CameraController.h"
#include "Config.h"
#include "DebugGraph.h"
#include "HUD.h"
#include "InputManager.h"
#include "StateMachine.h"
#include "SoundManager.h"
#include "World.h"
#include "Inventory.h"

// Engine includes
#include "VCNCore/CoreTimer.h"
#include "VCNCore/System.h"
#include "VCNNodes/Node.h"
#include "VCNNodes/NodeCore.h"
#include "VCNNodes/RenderNode.h"
#include "VCNPhysic/PhysicCore.h"
#include "VCNRenderer/EffectCore.h"
#include "VCNRenderer/GPUProfiling.h"
#include "VCNRenderer/RenderCore.h"
#include "VCNUtils/Macros.h"
#include "VCNUtils/Utilities.h"
#include <lua.hpp>
#include "VCNNodes/UsableComponent.h"
#include "VCNNodes/DoorComponent.h"
#include "VCNRenderer/ShaderCore.h"
#include "VCNNodes/NodePropertiesComponent.h"

// We need to be in C to interface with LUA.
extern "C" {
	// Expose method to add item to inventory.
	static int l_inventoryAddItem(lua_State *L) {
		VCN_ASSERT(lua_gettop(L) == 2); // check that the number of args is exactly 2 
		Inventory **ud = static_cast<Inventory **>(luaL_checkudata(L,1, "InventoryMT"));
		std::string v = luaL_checkstring(L,2); // Gather the item ID from the stack.
		(*ud)->addItem(v);
		return 0;
	}

	// Expose method to remove item from inventory.
	static int l_inventoryRemoveItem(lua_State *L) {
		VCN_ASSERT(lua_gettop(L) == 2); // check that the number of args is exactly 2 
		Inventory **ud = static_cast<Inventory **>(luaL_checkudata(L,1, "InventoryMT"));
		std::string v = luaL_checkstring(L,2); // Gather the item ID from the stack.
		(*ud)->removeItem(v);
		return 0;
	}

    // Expose method to post sound event.
    static int l_audioPostEvent(lua_State *L) {
        VCN_ASSERT(lua_gettop(L) == 2); // check that the number of args is exactly 2 
        VCNAudioCore **ud = static_cast<VCNAudioCore **>(luaL_checkudata(L, 1, "AudioMT"));
        std::string v = luaL_checkstring(L, 2); // Gather the item ID from the stack.
        (*ud)->PostEvent(VCN_A2W(v), 42);
        return 0;
    }

	// Expose method to set a sound state.
    static int l_audioSetState(lua_State *L) {
        VCN_ASSERT(lua_gettop(L) == 3); // check that the number of args is exactly 3 
        VCNAudioCore **ud = static_cast<VCNAudioCore **>(luaL_checkudata(L, 1, "AudioMT"));
        std::string stateName = luaL_checkstring(L, 2);
		std::string stateValue = luaL_checkstring(L, 3);
        (*ud)->SetState(VCN_A2W(stateName), VCN_A2W(stateValue));
        return 0;
    }

    // Expose method to set a sound switch.
    static int l_audioSetSwitch(lua_State *L) {
        VCN_ASSERT(lua_gettop(L) == 3); // check that the number of args is exactly 3 
        VCNAudioCore **ud = static_cast<VCNAudioCore **>(luaL_checkudata(L, 1, "AudioMT"));
        std::string switchName = luaL_checkstring(L, 2);
		std::string switchValue = luaL_checkstring(L, 3);
        (*ud)->SetSwitch(VCN_A2W(switchName), VCN_A2W(switchValue), 42);
        return 0;
    }

	// Expose method to display a paper note.
    static int l_displayNote(lua_State *L) {
        VCN_ASSERT(lua_gettop(L) == 2); // check that the number of args is exactly 2 
        HUD **ud = static_cast<HUD **>(luaL_checkudata(L, 1, "HUDMT"));
        std::string v = luaL_checkstring(L, 2);
        (*ud)->showTextMessage(v);
        return 0;
    }

	// Expose method to display a message at the bottom of the screen.
    static int l_displayActionDescription(lua_State *L) {
		VCNInt params = lua_gettop(L);
		VCNInt timeout = 0;
		VCN_ASSERT(params == 2 || params == 3);

        HUD **ud = static_cast<HUD **>(luaL_checkudata(L, 1, "HUDMT"));
        std::string msg = luaL_checkstring(L, 2); 
		if(params == 3)
		{
			timeout = luaL_checkint(L, 3);
		}

        (*ud)->DisplayActionDescription(msg, timeout);
        return 0;
    }

	// Display the awareness level at the top.
	static int l_displayAwarenessLevel(lua_State *L) {
        VCN_ASSERT(lua_gettop(L) == 1); // check that the number of args is exactly 1 
        HUD **ud = static_cast<HUD **>(luaL_checkudata(L, 1, "HUDMT"));
        (*ud)->displayAwarenessLevel();
        return 0;
    }

	// Hide the awareness level from the top.
	static int l_hideAwarenessLevel(lua_State *L) {
        VCN_ASSERT(lua_gettop(L) == 1); // check that the number of args is exactly 1 
        HUD **ud = static_cast<HUD **>(luaL_checkudata(L, 1, "HUDMT"));
        (*ud)->hideAwarenessLevel();
        return 0;
    }

	//Set the current awareness level at the top.
	static int l_setAwarenessLevel(lua_State *L) {
        VCN_ASSERT(lua_gettop(L) == 2); // check that the number of args is exactly 2 
        HUD **ud = static_cast<HUD **>(luaL_checkudata(L, 1, "HUDMT"));
		VCNInt awareness = luaL_checkint(L, 2);
        (*ud)->setAwarenessLevel(awareness);
        return 0;
    }

    // Teleport the player to a specific node.
    static int l_teleportPlayerTo(lua_State *L) {
        VCN_ASSERT(lua_gettop(L) == 2); // check that the number of args is exactly 2 
        CameraController **ud = static_cast<CameraController **>(luaL_checkudata(L, 1, "PlayerMT"));
        std::string nodeName = luaL_checkstring(L, 2);

        VCNNodeCore* nodeCore = VCNNodeCore::GetInstance();

        if (nodeCore == NULL)
        {
            return 0;
        }

        VCNNodeID targetNodeId = nodeCore->GetNodeByName(StringUtils::StringToWString(nodeName));

        if (targetNodeId == kInvalidNodeID) 
        {
            return 0;
        }

        VCNNode* targetNode = nodeCore->GetNode(targetNodeId);

        if (targetNode == NULL)
        {
            return 0;
        }

        Vector3 targetPosition = targetNode->GetWorldTranslation();

        (*ud)->teleport(targetPosition.x, targetPosition.y, targetPosition.z);

        return 0;
    }

    // Enable or disable player movement.
    static int l_playerCanMove(lua_State *L) {
        VCN_ASSERT(lua_gettop(L) == 2); // check that the number of args is exactly 2 
        CameraController **ud = static_cast<CameraController **>(luaL_checkudata(L, 1, "PlayerMT"));
        VCNInt canMove = luaL_checkint(L, 2);

        (*ud)->setMobility(canMove ? true : false);

        return 0;
    }

    // Expose method to add an item to the store.
    static int l_itemStoreAdd(lua_State *L) {
        VCN_ASSERT(lua_gettop(L) == 4); // check that the number of args is exactly 4 
        ItemStore **ud = static_cast<ItemStore **>(luaL_checkudata(L, 1, "ItemStoreMT"));
        std::string itemId = luaL_checkstring(L, 2);
        std::string readableName = luaL_checkstring(L, 3);
        std::string imageId = luaL_checkstring(L, 4);
        (*ud)->addItem(itemId, readableName, imageId);
        return 0;
    }

	// Utilities exposed globally to Lua:

	static int l_hideNode(lua_State *L) {
        VCN_ASSERT(lua_gettop(L) == 1); // check that the number of args is exactly 2 
        std::string nodeName = luaL_checkstring(L, 1);
        
		VCNNodeCore* nodeCore = VCNNodeCore::GetInstance();

		if (nodeCore == NULL)
		{
			return 0;
		}

		VCNNodeID nodeToHideId = nodeCore->GetNodeByName(StringUtils::StringToWString(nodeName));

		if (nodeToHideId == kInvalidNodeID) {
			return 0;
		}

		nodeCore->RemoveNode(nodeToHideId);

        return 0;
    }

    static int l_screenFade(lua_State *L) {
        VCNInt params = lua_gettop(L);
        VCN_ASSERT(params == 2 || params == 3);
        VCNInt fadeLevel = luaL_checkint(L, 1);
        VCNInt fadeTime = luaL_checkint(L, 2);
        std::string callbackName = "";

        if (params == 3)
        {
            callbackName = luaL_checkstring(L, 3);
        }

        PlayState* playState = StateMachine::GetInstance().GetState<PlayState>(GS_PLAY);
        playState->fadeScreen(fadeLevel, fadeTime, callbackName);

        return 0;
    }
}

namespace 
{

	//////////////////////////////////////////////////////////////////////////
	void PrintNodeTree(unsigned int font, unsigned int depth, VCNNode* node, int x, int& y)
	{
		VCNRenderCore* renderer = VCNRenderCore::GetInstance();

		VCNByte r = 255, g = 200, b = 200;

		if ( VCNRenderNode* renderNode = node->TryCast<VCNRenderNode>() )
		{
			if ( renderNode->IsInFrustum() )
			{
				r = 200; g = 200; b = 255;
			}
			else
			{
				r = 100; g = 100; b = 155;
			}
		}

		VCNRect textRect;
		Vector3 pos = node->GetWorldTranslation();
		renderer->RenderText(font, x, y, r, g, b, &textRect, node->GetTag().c_str());
		renderer->RenderText(font, textRect.right + 10, y, r, g, b, &textRect, VCNTXT("(%.1f,%.1f,%.1f)"), pos.x, pos.y, pos.z);

		std::vector<VCNNodeID> children;
		node->GetChildrenArray(children);

		y += 15;

		// Insert an animation node between the root and his children
		VCNNodeCore* nodeCore = VCNNodeCore::GetInstance();
		for( VCNUInt i=0; i<children.size(); i++ )
		{
			// Get the node where we wish the make the insertion
			VCNNodeID childID = children[i];
			VCNNode* childNode = nodeCore->GetNode(childID);

			if( childNode )
			{
				PrintNodeTree(font, depth + 1, childNode, x + 10, y);
			}
		}
	}

}

///////////////////////////////////////////////////////////////////////
///
/// Constructs the play state.
///
PlayState::PlayState()
	: mCamera(CST_FLOAT("Camera.PosAt.X"), CST_FLOAT("Camera.PosAt.Y"), CST_FLOAT("Camera.PosAt.Z"),
						CST_FLOAT("Camera.Focus.X"), CST_FLOAT("Camera.Focus.Y"), CST_FLOAT("Camera.Focus.Z"),
						0.0f,  1.0f, 0.0f)
	, mCameraController( new CameraController(mCamera) )
	, mHUDFont(0) 
	, mNewGame(true)
	, mHUD()
    , mSelectedNode( NULL )
    , mCurrentActionVerb("Use")
	, mShowHUD( CST_BOOL("HUD.Show") )
	, mShowDebugHud( CST_BOOL("HUD.Debug.Show") )
	, mFreeLook( true )
    , mCurrentFade( 1.0f )
	, mDebugGraph( new DebugGraph() )
{
}

PlayState::~PlayState()
{
	delete mInventory;
    delete mItemStore;
	mInventory = nullptr;
    mItemStore = nullptr;
}

//////////////////////////////////////////////////////////////////////////
VCNBool PlayState::Initialize()
{
	// Initialize sub systems
	if ( !InitializeFonts() )
		return false;
		
	// Initialize the HUD
	mHUD = HUDPointer( new HUD() );
	mHUD->Attach(this);
	
	// Initialize the audio.
    mAudio = VCNAudioCore::GetInstance();

    //FIXME: Master game object, change number later on.
    mAudio->RegisterGameObj(42, VCNTXT("Vicuna"));
    mAudio->RegisterGameObj(100, VCNTXT("test"));

    // Initialize the item store
    mItemStore = new ItemStore();
    mItemStore->Attach(this);
    
    // Initialize the inventory
	mInventory = new Inventory();
	mInventory->Attach(this);

	// Initialize the scripting.
	mScripting = VCNScriptingCore::GetInstance();
	
	//FIXME: We probably want to add a utility to do this "remotely" through VCNScripting rather than expose
	// the LUA state.
	lua_State *L = static_cast<lua_State*>( mScripting->getState() );
	
	// See: http://rubenlaguna.com/wp/2012/12/09/accessing-cpp-objects-from-lua/
	luaL_newmetatable(L, "InventoryMT");
	lua_pushvalue(L,-1);
	lua_setfield(L,-2, "__index"); // InventoryMT .__index = InventoryMT
	lua_pushcfunction(L, l_inventoryAddItem);
	lua_setfield(L,-2, "addItem"); // Expose this as the Inventory:addItem method.
		lua_pushcfunction(L, l_inventoryRemoveItem);
	lua_setfield(L,-2, "removeItem"); // Expose this as the Inventory:removeItem method.

	Inventory **ud = static_cast<Inventory **>(lua_newuserdata(L, sizeof(Inventory *)));
	*(ud) = &(*mInventory);
	luaL_setmetatable(L, "InventoryMT"); // set userdata metatable
	lua_setglobal(L, "Inventory"); // Expose mInventory as Inventory.

    luaL_newmetatable(L, "AudioMT");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index"); // AudioMT .__index = AudioMT
    lua_pushcfunction(L, l_audioPostEvent);
    lua_setfield(L, -2, "postEvent"); // Expose this as the Audio:postEvent method.
	lua_pushcfunction(L, l_audioSetState);
    lua_setfield(L, -2, "setState"); // Expose this as the Audio:setState method.
	lua_pushcfunction(L, l_audioSetSwitch);
    lua_setfield(L, -2, "setSwitch"); // Expose this as the Audio:setSwitch method.

    VCNAudioCore **ud2 = static_cast<VCNAudioCore **>(lua_newuserdata(L, sizeof(VCNAudioCore *)));
    *(ud2) = &(*mAudio);
    luaL_setmetatable(L, "AudioMT"); // set userdata metatable
    lua_setglobal(L, "Audio"); // Expose mAudio as Audio.

	luaL_newmetatable(L, "HUDMT");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index"); // HUDMT .__index = HUDMT
    lua_pushcfunction(L, l_displayNote);
    lua_setfield(L, -2, "displayNote"); // Expose this as the HUD:displayNote method.
	lua_pushcfunction(L, l_displayActionDescription);
    lua_setfield(L, -2, "displayActionDescription"); // Expose this as the HUD:displayActionDescription method.
	lua_pushcfunction(L, l_displayAwarenessLevel);
    lua_setfield(L, -2, "displayAwarenessLevel"); // Expose this as the HUD:displayAwarenessLevel method.
	lua_pushcfunction(L, l_hideAwarenessLevel);
    lua_setfield(L, -2, "hideAwarenessLevel"); // Expose this as the HUD:hideAwarenessLevel method.
	lua_pushcfunction(L, l_setAwarenessLevel);
    lua_setfield(L, -2, "setAwarenessLevel"); // Expose this as the HUD:setAwarenessLevel method.

    HUD **ud3 = static_cast<HUD **>(lua_newuserdata(L, sizeof(HUD *)));
    *(ud3) = &(*mHUD);
    luaL_setmetatable(L, "HUDMT"); // set userdata metatable
    lua_setglobal(L, "HUD"); // Expose mHUD as HUD.

    luaL_newmetatable(L, "PlayerMT");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index"); // PlayerMT .__index = PlayerMT
    lua_pushcfunction(L, l_teleportPlayerTo);
    lua_setfield(L, -2, "teleportTo"); // Expose this as the Player:canMove method.
    lua_pushcfunction(L, l_playerCanMove);
    lua_setfield(L, -2, "canMove"); // Expose this as the Player:canMove method.

    CameraController **ud4 = static_cast<CameraController **>(lua_newuserdata(L, sizeof(CameraController *)));
    *(ud4) = &(*mCameraController);
    luaL_setmetatable(L, "PlayerMT"); // set userdata metatable
    lua_setglobal(L, "Player"); // Expose mCharacterController as Player.

    luaL_newmetatable(L, "ItemStoreMT");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index"); // ItemStoreMT .__index = ItemStoreMT
    lua_pushcfunction(L, l_itemStoreAdd);
    lua_setfield(L, -2, "registerItem"); // Expose this as the ItemStore:registerItem method.

    ItemStore **ud5 = static_cast<ItemStore **>(lua_newuserdata(L, sizeof(ItemStore *)));
    *(ud5) = &(*mItemStore);
    luaL_setmetatable(L, "ItemStoreMT"); // set userdata metatable
    lua_setglobal(L, "ItemStore"); // Expose mItemStore as ItemStore.
	
	// Global functions
	lua_pushcfunction(L, l_hideNode);
	lua_setglobal(L, "hideWorldNode");

    lua_pushcfunction(L, l_screenFade);
	lua_setglobal(L, "fadeScreen");

    mCameraController->Attach(this);

	return true;
}

//////////////////////////////////////////////////////////////////////////
VCNBool PlayState::InitializeFonts()
{
	// Create a default rendering font
	if( !VCNRenderCore::GetInstance()->CreateFont( VCNTXT("Arial"), 0, false, 8, &mHUDFont) )
	{
		VCN_ASSERT_FAIL( _T("Could not create debug font.") );
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
VCNBool PlayState::Uninitialize()
{
	return true;
}

////////////////////////////////////////////////////////////////////////
void PlayState::OnEnter()
{
	//FIXME: Script test.
	if (mScripting->loadScript("level1")) {
		/*
		mScripting->zoneTriggerEnter("testzone");  // Switch not activated, sadly.
		mScripting->zoneTriggerEnter("testzone2"); // Activates switch, hurray!
		mScripting->zoneTriggerEnter("testzone");  // Switch is indeed activated.
		mScripting->zoneTriggerEnter("testzone");  // Counter does increment.
		*/
	}
	
	// Show mouse cursor
	InputManager::GetInstance().SetMouseCaptured(false);

	if ( mNewGame )
	{
		mNewGame = false;

		OnNewGame();
	}

	// Start ambient game sounds
	SoundManager::GetInstance().PlayAllAmbientTracks();
	SoundManager::GetInstance().RequestCollisionSoundStabilization();

    mAudio->SetState(VCNTXT("Jump"), VCNTXT("Metal"));

    // Post an event to the sound engine...
    //FIXME: Hardcoded for now, allow user to use any game object later on.
    mAudio->SetPosition(100, Vector3(-35.6861267, 5.81445217, 44.8532066), Vector3(0,-1,0));
    //mAudio->SetAttenuationScalingFactor(1, 0.1f);
    //mAudio->SetAttenuationScalingFactor(1, 0.0f);

    mCameraController->EnterFreeLook();
}

////////////////////////////////////////////////////////////////////////
void PlayState::OnLeave()
{
}

///////////////////////////////////////////////////////////////////////
void PlayState::OnNewGame()
{
}

void PlayState::UpdateObserver(ItemStoreData& hint)
{
    mHUD->OnItemRegistered(hint.registeredId, hint.readableName, hint.imageId);
}

void PlayState::UpdateObserver(InventoryData& hint)
{
	std::vector<std::string> myItems;
	mInventory->listItems(myItems);
	
	mHUD->OnInventoryChanged(myItems);

	if (hint.removedId == mCurrentItem)
	{
		mCurrentItem = "";
		mHUD->OnItemUnequipped(hint.removedId);
	}
}

void PlayState::UpdateObserver(HUDAction& hint)
{
	switch(hint.actionType)
	{
		// Item equipped.
        case ItemEquipped:
            setEquippedItem(hint.actionTarget);
			break;

		// Need mouse control.
		case MouseTaken:
			mFreeLook = false;
			mCameraController->LeaveFreeLook();
			break;

		// Doesn't need mouse control.
		case MouseFreed:
			mFreeLook = true;
			mCameraController->EnterFreeLook();
            break;
	}
}

////////////////////////////////////////////////////////////////////////
void PlayState::UpdateObserver( CameraData& hint )
{
    switch (hint.eventType)
    {
        case PointingObject:
            mSelectedNode = hint.selected;
            //FIXME: Everything below is temporary, we would like for this to know nothing about explicit node IDs and such.
            if (mSelectedNode != NULL)
            {
                VCNNodeID nodeId = mSelectedNode->GetNodeID();
                //TODO: Get "real life name" of an item from its tag.
                VCNString itemName = mSelectedNode->GetTag();

				VCNNodePropertiesComponent* propComponent = mSelectedNode->GetComponent<VCNNodePropertiesComponent>();
				if (propComponent)
				{
					VCNString readableName = propComponent->GetReadableName();
					if (readableName.length() > 0)
					{
                        std::transform(readableName.begin(), readableName.end(), readableName.begin(), ::tolower);
                        itemName = readableName;
					}
				}

                std::string withItem = "";

                if (mCurrentItem.length() != 0)
                {
                    withItem = " with " + mCurrentItem;
                }

                mHUD->DisplayActionDescription(mCurrentActionVerb + " " + StringUtils::WStringToString(itemName) + withItem);
            }
            else
            {
                mHUD->DisplayActionDescription("");
            }
            break;

        case WalkingStart:
            mAudio->SetState(VCNTXT("Walk_run"), VCNTXT("Walk"));
            //FIXME: Hardcoded for now, allow user to use any game object later on.
            mAudio->PostEvent(VCNTXT("Footsteps_concrete_play"), 42);
            break;

        case RunningStart:
            mAudio->SetState(VCNTXT("Walk_run"), VCNTXT("Run"));
            break;

        case Jumping:
            mAudio->PostEvent(VCNTXT("Jump_play"), 42);
            break;

        case Moving:
            mAudio->SetListenerPosition(hint.newPosition, hint.newLookAt);
            break;

        case MovingStop:
            mAudio->SetState(VCNTXT("Walk_run"), VCNTXT("None"));
            //FIXME: Hardcoded for now, allow user to use any game object later on.
            mAudio->PostEvent(VCNTXT("Footsteps_concrete_stop"), 42);
            break;
    }
}

///////////////////////////////////////////////////////////////////////
const bool PlayState::OnKeyPress(VCNUInt8 keycode)
{  
	// Create a new game.
	if ( keycode == VK_F2 )
	{
		OnNewGame();
	}
    else if (keycode == KEY_M)
    {
        mAudio->PostEvent(VCNTXT("Ambiance_music_stop"), 42);
        StateMachine::GetInstance().ChangeState(GS_MAIN_MENU);

        return true;
    }
    else if (keycode == KEY_I)
    {
        mHUD->toggleInventory();

        return true;
    }
    else if (keycode == KEY_U)
    {
        mHUD->setCurrentAction("0");
        mCurrentActionVerb = "Use";

        return true;
    }
    else if (keycode == KEY_T)
    {
        mHUD->setCurrentAction("1");
        mCurrentActionVerb = "Talk to";

        return true;
    }
    else if (keycode == KEY_K)
    {
        mHUD->setCurrentAction("2");
        mCurrentActionVerb = "Take";

        return true;
    }
    else if (keycode == KEY_G)
    {
        mHUD->setCurrentAction("3");
        mCurrentActionVerb = "Give";

        return true;
    }
	// Toggle the debugging HUD
	else if ( keycode == KEY_INSERT )
	{
		mShowDebugHud = !mShowDebugHud;
	}
	else if ( keycode == KEY_F3 )
	{
		VCNRenderCore* renderer = VCNRenderCore::GetInstance();
				 if ( renderer->GetShadeMode() == RS_SHADE_POINTS )     renderer->SetShadeMode( RS_SHADE_WIREFRAME );
		else if ( renderer->GetShadeMode() == RS_SHADE_WIREFRAME )  renderer->SetShadeMode( RS_SHADE_SOLID );
		else if ( renderer->GetShadeMode() == RS_SHADE_SOLID )      renderer->SetShadeMode( RS_SHADE_POINTS, 2.0f );

		return true;
	}
	// Toggle the HUD
	else if ( keycode == KEY_H )
	{
		mShowHUD = !mShowHUD;
	}

	return false;
}

void PlayState::OnLeftMouseButtonDown(MouseEventArgs& args)
{
    if (mFreeLook)
	{
		InputManager& inputManager = InputManager::GetInstance();
		
		if (!inputManager.IsMouseCaptured())
		{
			mCameraController->EnterFreeLook();
		}

		if(mSelectedNode != NULL)
		{
			VCNUsableComponent* usableComponent = mSelectedNode->GetComponent<VCNUsableComponent>();
			if (usableComponent != NULL)
			{
				usableComponent->UseComponent();
			}
			else
			{
				VCNString itemName = mSelectedNode->GetTag();
				if (itemName.length() != 0)
				{
					mScripting->trigger(StringUtils::WStringToString(itemName), "itemUsed");
				}
			}    
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void PlayState::NewGame()
{
	mNewGame = true;
}

//////////////////////////////////////////////////////////////////////////
void PlayState::fadeScreen(VCNInt percentage, VCNInt duration, const std::string& callbackName)
{
    const VCNInt updateRate = 100;
    
    //FIXME: VCNFloat fadeTarget = percentage / 100.0f;
    VCNFloat fadeTarget = percentage / 200.0f + 0.5f;
    VCNFloat deltaToFade = mCurrentFade - fadeTarget;

    // Do some input validation.
    if (deltaToFade == 0.0f || duration == 0) {
        return;
    }

    VCNFloat deltaFade = deltaToFade * 10 / duration;

    TimeTrigger mScriptTrigger = TimeTrigger(0.001f,
        [this, callbackName](void* data, float elapsedTime, bool* repeat) {
            mScripting->trigger(callbackName, "fadeDone");
    }, 0, false);
    
    mFadeTrigger = TimeTrigger(0.001f,
        [this, deltaFade, fadeTarget, callbackName, mScriptTrigger](void* data, float elapsedTime, bool* repeat) {
        mCurrentFade -= deltaFade;
        if (((deltaFade > 0.0f) && (mCurrentFade < fadeTarget)) ||
            ((deltaFade < 0.0f) && (mCurrentFade > fadeTarget))) {
            mCurrentFade = fadeTarget;            
            *repeat = false;

            if (callbackName.length() != 0) {
                VCNCoreTimer::GetInstance()->AddTrigger(VCNTXT("FadeScript"), mScriptTrigger, false);
            }
        }
        
        VCNShaderCore::GetInstance()->SetFadeAmount(mCurrentFade);
    }, 0, true);
    VCNCoreTimer::GetInstance()->AddTrigger(VCNTXT("Fade"), mFadeTrigger, true);
}

//////////////////////////////////////////////////////////////////////////
void PlayState::setEquippedItem(const std::string itemId)
{
    mCurrentItem = itemId;
	const char* test = itemId.c_str();
	mScripting->trigger("", "itemEquipped");
    mScripting->trigger(itemId, "itemEquipped");
	mHUD->DisplayActionDescription("Equipped item " + itemId, 2000);
}

///////////////////////////////////////////////////////////////////////
void PlayState::PreRender() const 
{
	// Setup the effect core for the incoming frame
	VCNEffectCore::GetInstance()->PrepareFrame();

	// Generate the scene shadow maps.
	VCNEffectCore::GetInstance()->GenerateShadowMap();

}

////////////////////////////////////////////////////////////////////////
void PlayState::Render() const
{
	// See if the effect module has some debug rendering to do, 
	// if so, then skip normal rendering of the scene.
	if ( !VCNEffectCore::GetInstance()->DebugRender() )
	{
		// Render the scene
		VCNNodeCore::GetInstance()->GetRootNode()->Render();    
	}

	// Render the game HUD
	RenderHUD();
}

//////////////////////////////////////////////////////////////////////////
void PlayState::Update( float elapsedTime )
{
	if ( mShowHUD && mHUD )
	{
		mHUD->Update( elapsedTime );
	}
	
	mDebugGraph->Update( elapsedTime );
}

//////////////////////////////////////////////////////////////////////////
void PlayState::RenderHUD() const
{
	GPU_PROFILE_BLOCK_NAME( L"HUD" );

	if ( mShowHUD && mHUD )
	{
		mHUD->Render();
	}

	if (mShowDebugHud)
	{
		VCNRenderCore* renderer = VCNRenderCore::GetInstance();

		const VCNUInt32 fps = VCNCoreTimer::GetInstance()->GetFPS();
		const VCNFloat32 ftps = VCNCoreTimer::GetInstance()->GetAverageFrameTimePerSecond();
		const VCNFloat32 dmps = VCNSystem::GetInstance()->GetDMPS();
		const VCNFloat32 usedMem = VCNSystem::GetInstance()->GetUsedMemoryInMB();
		const VCNFloat32 freeMem = VCNSystem::GetInstance()->GetFreeMemoryInMB();
		const VCNFloat32 usedMinMem = VCNSystem::GetInstance()->GetMinUsedMemory() / 1048576.0f;
		const VCNFloat32 usedMaxMem = VCNSystem::GetInstance()->GetMaxUsedMemory() / 1048576.0f;

		const Vector3& camPos = mCamera.GetViewerPosition();
		const Vector3& camDir = mCamera.GetDirection();

		VCNRect fontRect;
		renderer->RenderText(mHUDFont,10,15,255,255,0, 0, VCNTXT("FPS: %d (%ss)"), fps, VCN_ASCII_TO_TCHAR(VCN::Eng(ftps, 3, 0)));
		renderer->RenderText(mHUDFont,10,30,205,205,0, &fontRect,   VCNTXT("Memory: %.1f/%.1f (%.2f)"), usedMem, freeMem, usedMaxMem - usedMinMem);
		renderer->RenderText(mHUDFont,fontRect.right + 5,30,205,35,10, 0,   VCNTXT("\x0394: %.5f MB/s"), dmps);
		renderer->RenderText(mHUDFont,10,45,255,255,255, 0, VCNTXT("Camera position: (%0.2f, %0.2f, %0.2f)"), camPos.x, camPos.y, camPos.z);
		renderer->RenderText(mHUDFont,10,60,255,255,255, 0, VCNTXT("Camera direction: (%0.2f, %0.2f, %0.2f)"), camDir.x, camDir.y, camDir.z);

		if ( InputManager::GetInstance().IsKeyPressed(KEY_END) )
		{
			static float fstartx = 20.0f, fstarty = 60.0f;
			if ( InputManager::GetInstance().IsKeyPressed(KEY_PAGEDOWN) )
			{
				fstarty += 17.5f;
			}
			else if ( InputManager::GetInstance().IsKeyPressed(KEY_PAGEUP) )
			{
				fstarty -= 17.5f;
			}

			int startx = (int)fstartx, starty = (int)fstarty;
			PrintNodeTree(mHUDFont, 0, VCNNodeCore::GetInstance()->GetRootNode(), startx, starty);
		}

		mDebugGraph->Draw();

		// Draw tweak bars
		TwDraw();

	}
}
