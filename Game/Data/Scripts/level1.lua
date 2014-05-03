--[[
    INF4715: Level 1 scripts

    This file will contains all business logic pertaining to the interactivity of the level 1.
]]

-- Examples

--hideWorldNode("FIT_Toilet_Toilet")


-- Track puzzle elements.
wentToBed = false
retrievedDirtyKey = false
bridgeRolled = false
knifeTaken = false
clothTaken = false
fuseTaken = false
fuseUsed = false
glueTaken = false
keyUsed = false
lockGlued = false
breakerActive = false
fibonacciSolved = false
mazeSwitch1Activated = false
mazeSwitch2Activated = false
tetrisSwitch1Activated = false
tetrisSwitch2Activated = false
inMaze = false

awarenessLevel = 0
fibonacciDoor = -1

-- Currently equipped item.
currentlyEquipped = nil

-- All item definitions.
ItemStore:registerItem("knife", "Knife", "knife_small")
ItemStore:registerItem("glueknife", "Glued Knife", "knife_small")
ItemStore:registerItem("dirtykey", "Dirty Key", "key_small")
ItemStore:registerItem("glue", "Glue", "glue_small")
ItemStore:registerItem("oil", "Oil", "oil_small")
ItemStore:registerItem("fuse", "Fuse", "fuse_small")

-- Initial inventory.
-- Nothing for now.

-- Ambient sound.
Audio:setState("Levels_music", "Puzzle1")
Audio:postEvent("Ambiance_music_play")

-- Start message.
HUD:displayNote("Welcome to Prison Escape! You can use the '<strong>I</strong>' key to toggle the display of your item inventory " ..
                "at any point. It is where you can see which items you carry and decide to equip one. You can use the " ..
                "'<strong>U</strong>', '<strong>T</strong>', '<strong>K</strong>' and '<strong>G</strong>' keys to select your " ..
                "current action, the one that will be executed contextually with a left click. At any point in time you can use the " ..
                "'<strong>M</strong>' key to go back to the main menu. Click OK to close messages like these. Good luck!")

--DEBUG AREA. OVERRIDE GLOBALS HERE ONLY.

godMode = false

if (godMode) then
    keyUsed = true
end

--END DEBUG AREA.


-- Called when any item is equipped.
function on_itemEquipped() 
  Audio:postEvent("Grab_objects_play")
  currentlyEquipped = nil
  return ""
end

-- Called when entering the maze section.
function onMAZE_enter()
    inMaze = true
    HUD:displayAwarenessLevel()
	Audio:setState("Levels_music", "Puzzle2")
	Audio:postEvent("Randombots_play")
	return ""
end

-- Called when leaving the maze section.
function onMAZE_exit()
	inMaze = false
    HUD:hideAwarenessLevel()
	Audio:setState("Levels_music", "Puzzle1")
	Audio:postEvent("Randombots_stop")
	return ""
end

-- Called at the very end of the game.
function onGAME_end()
	inMaze = false
    HUD:hideAwarenessLevel()
	HUD:displayActionDescription("Congratulations! You Won!")
    HUD:displayNote("Thanks for playing Prison Escape. You can use '<strong>M</strong>' to return to the main menu as usual...")
    Player:canMove(0)
	--fadeScreen(20000, 500)
	fadeScreen(0, 200)
	Player:teleportTo("teleportNoMansLand")
	return ""
end

-- Called when item "dirtykey" is equipped.
function onDIRTYKEY_itemEquipped() 
  currentlyEquipped = "dirtykey"
  return ""
end

-- Called when the door of the first cell is opened.
function onDOOR1_open() 
  Audio:postEvent("Metal_door_play")
  return ""
end

-- Called when the door of the second cell is opened.
function onDOOR2_open() 
  Audio:postEvent("Metal_door_play")
  return ""
end

-- Called when the door of the third cell is opened.
function onDOOR003_open() 
  Audio:postEvent("Metal_door_play")
  return ""
end

-- Called when the door of the maintenance room is opened.
function onDOOR004_open() 
  Audio:postEvent("Metal_door_play")
  return ""
end

-- Indicates whether the door of the first cell can be opened.
function onDOOR1_canOpen() 
  -- Once the door has been opened once, no need to go back.
  if (keyUsed) then
    return "1"
  end
  
  -- If the dirty key is equipped, allow to open the door of the first cell.
  if (currentlyEquipped == "dirtykey") then
	Audio:postEvent("Keyring_play")
    Inventory:removeItem("dirtykey")
    keyUsed = true;
    HUD:displayActionDescription("At long last, the door opens!", 2000)
	Audio:setState("Metal_door", "Opened")
	return "1"
  -- Otherwise door is "locked".
  else
    HUD:displayActionDescription("Door doesn't seem to bulge...", 2000)
	Audio:setState("Metal_door", "Locked_not_openable")
	Audio:postEvent("Metal_door_play")
	return "0"
  end
end

-- Called when the node "texte1" is used.
function onTEXTE1_itemUsed()
    HUD:displayActionDescription("I wonder what it says.", 2000) 
	HUD:displayNote("The jailer was back yestarday.  Was it yestarday?  I've been stuck in this godforsaken hole for so long now," ..
					"i'm afraid  i've lost any sense of time.  Gave me some food.  Told me to ration it, said he'd be gone for a while." ..
					"Honestly, i'm not even sure why they are trying to keep me alive.  At first, i thought they'd just leave me here to " ..
					"rot or they'd try torture to get some intel out of me, but aside from the guy bringing me food, no one's bothered to " ..
					"visit.  Russia said this would probably happen, told me they knew the prison they'd send me to if i was captured. " ..
					"I can only hope they will send someone...")
    return ""
end

-- Called when the node "texte2" is used.
function onTEXTE2_itemUsed()
    HUD:displayActionDescription("I wonder what it says.", 2000) 
	HUD:displayNote("They were trying to silence me.  Trying... well they goddamned succeeded.  No one is leaving this prison alive. " ..
					"I saw them take down that german fellow,  right after he managed to somehow get passed the guards and into the " ..
					"room with the three doors.   ' Twice through the left one' he kept yelling as they were beating him to a pulp. " ..
					"Poor fucker.")
    return ""
end

-- Called when the node "texte3" is used.
function onTEXTE3_itemUsed()
    HUD:displayActionDescription("I wonder what it says.", 2000) 
	HUD:displayNote("I think i've finally figured it out.  It needs to be done 4 times. Damned these bastards and their fucked up " ..
					"passage way. I'm getting out of here.")
    return ""
end

-- Called when the node "texte4" is used.
function onTEXTE4_itemUsed()
    HUD:displayActionDescription("I wonder what it says.", 2000) 
	HUD:displayNote("I saw them take him in last week.  More like drag him in.  Some people call his leaks heroism... said he " ..
					"should be given a Nobel Prize.  Bugger that.  The traitor got what he deserved. It was a joint operation " ..
					"between the NSA and CSEC, the Canadians promised him asylum after his position in Russia was compromised. " ..
					"They said they were bringing him to a safe house in the wilderness and he bought it. We need to get some in" ..
					"here who can fix the damned door.   Someone's broken a key in it.  I gotta find something to take it out or " ..
					"at least to twist it.")
    return ""
end

-- Called when the node "texte5" is used.
function onTEXTE5_itemUsed()
    HUD:displayActionDescription("I wonder what it says.", 2000) 
	HUD:displayNote("Prisoner Snowdon just arrived today.  The timing is perfect as the sentries are finally operational and you " ..
					"would still need to activate two switches in order to leave the maze.  Anyone is welcome to give escaping a " ..
					"try, but I can't guarantee they'll make it out in one piece.")
    return ""
end

-- Called when the node "mur15" is used.
function onMUR15_itemUsed()
    HUD:displayActionDescription("I wonder what it says.", 2000)
	HUD:displayNote("I think i've finally figured it out.  It needs to be done 4 times. \nDamned these bastards and their fucked " ..
					"up passage way.  I'm getting out of here.")	
    return ""
end

-- Called when the node "fuse" is used.
function onFUSE_itemUsed()
   if (not fuseTaken) then
      Inventory:addItem("fuse")
      fuseTaken = true
      HUD:displayActionDescription("This should be useful.", 2000)
	  hideWorldNode("Group01")
   else
      HUD:displayActionDescription("One is enough.", 2000)
   end
   return ""
end

-- Called when the item "fuse" is equipped.
function onFUSE_itemEquipped() 
  currentlyEquipped = "fuse"
  -- Audio events HERE
  HUD:displayActionDescription("This must go somewhere.")
  return ""
end

-- Indicates whether the node "fusedoor" can be opened.
function onFUSEDOOR_canOpen() 
  if (currentlyEquipped == "fuse") then
	Inventory:removeItem("fuse")
	fuseUsed = true
    Audio:setState("Metal_door", "Opened") --FIXME: This is not enough, we need to post an event to play a sound.
    HUD:displayActionDescription("I'm getting out of here!", 2000) 
    Audio:setSwitch("Breaker", "Power_on")
    Audio:postEvent("Breaker_play")
    Audio:setSwitch("Breaker", "Activated")
    return "1"
  else
    HUD:displayActionDescription("There seems to be something missing.", 2000)
    Audio:postEvent("Breaker_play")
    return "0"
  end
end

-- Called when the node "lit1" is used.
function onLIT1_itemUsed()   
    if (wentToBed) then
        HUD:displayActionDescription("Nah, I feel in shape now.", 2000) 
    else
        wentToBed = true
        Audio:postEvent("Bed_grit_play")
        Player:canMove(0)
        HUD:displayActionDescription("ZzZzZz") 
        fadeScreen(0, 500, "wakeup")
    end
    return ""
end

-- Called after having slept in the bed.
function onWAKEUP_fadeDone()
    fadeScreen(100, 250)
    Player:canMove(1)
    HUD:displayActionDescription("Feel much better now!", 2000) 
    return ""
end

-- Called when the node "breaker" is used.
function onBREAKER_itemUsed()   
  --currentlyEquipped = "breaker"
  if (currentlyEquipped == "fuse" and not breakerActive) then
      breakerActive = true
      Audio:postEvent("Breaker_play")
      HUD:displayNote("Now that's something!")
      return ""
  else
    HUD:displayNote("Nothing seems to change...")
    -- Audio events HERE
    return ""
  end  
end

-- Bring player back to the beginning of the Fibonacci puzzle and reset the tracking.
function fibonacciReset()
	fibonacciDoor = -1
	Audio:setState("Metal_door", "Locked_not_openable")
    Audio:postEvent("Metal_door_play")
	Audio:setSwitch("Fibonacci", "Wrong")
	Audio:postEvent("Fibonacci_play")
	HUD:displayActionDescription("I think I screwed it...", 2000) 
    Player:canMove(0)
	fadeScreen(0, 500, "fibonacci")
end

-- Make the player progress in the Fibonacci puzzle but bring him back to the beginning to simulate multiple rooms.
function fibonacciProgress(level)
	fibonacciDoor = level
	Audio:setSwitch("Fibonacci", "Right")
	Audio:postEvent("Fibonacci_play")
	HUD:displayActionDescription("Oh, this seems to be working!", 2000) 
    Player:canMove(0)
	fadeScreen(0, 500, "fibonacci")
end

-- Once the player was moved to the beginning of the Fibonacci puzzle, reshow the level.
function onFIBONACCI_fadeDone()
	Player:teleportTo("teleport")
    Player:canMove(1)
	fadeScreen(100, 500)
	return ""
end

-- Called when the node "portefib1" is used.
function onPORTEFIB1_itemUsed()
    if (fibonacciSolved) then
        HUD:displayActionDescription("Enough with this trickery...", 2000) 
        return ""
    else
        HUD:displayActionDescription("What da...?", 2000) 
        if (fibonacciDoor == -1) then
        fibonacciProgress(0)
        return ""
        elseif (fibonacciDoor == 0) then
        fibonacciProgress(1)
        return ""
        else
        fibonacciReset()
        return ""
        end
    end
end

-- Called when the node "portefib2" is used.
function onPORTEFIB2_itemUsed()
    if (fibonacciSolved) then
        HUD:displayActionDescription("Enough with this trickery...", 2000) 
        return ""
    else
        HUD:displayActionDescription("What kind of prison is this?", 2000) 
        if (fibonacciDoor == 1) then
            fibonacciProgress(2)
            return ""
        else
            fibonacciReset()
            return ""
        end
    end
end

-- Called when the node "portefib3" is used.
function onPORTEFIB3_canOpen() 
  if (fibonacciSolved) then
    Audio:postEvent("Metal_door_play")
    return "1"
  end
  
  if (fibonacciDoor == 2) then
    Audio:setState("Metal_door", "Opened")
    fibonacciDoor = 3
    fibonacciSolved = true
    HUD:displayActionDescription("Finally!", 2000) 
    return "1"
  else
    fibonacciReset()
    return "0"
  end
end

-- Called when the node "mazeswitch1" is pressed.
function onMAZESWITCH1_pressed()
	mazeSwitch1Activated = true
	Audio:postEvent("Switch1_play")
    HUD:displayActionDescription("Activated!", 2000)
	if (mazeSwitch2Activated) then
	  Audio:setState("Levels_music", "Ending")
	  return "1"
	else
	  return "0"
	end
end

-- Called when the node "mazeswitch2" is pressed. 
function onMAZESWITCH2_pressed()
	mazeSwitch2Activated = true
	Audio:postEvent("Switch2_play")
    HUD:displayActionDescription("Activated!", 2000)
	if (mazeSwitch1Activated) then
		Audio:setState("Levels_music", "Ending")
		return "1"
	else
		return "0"
	end
end

-- Called when the node "tetrisswitch1" is pressed. 
function onTETRISSWITCH1_pressed()
	tetrisSwitch1Activated = true
	Audio:postEvent("Switch1_play")
    HUD:displayActionDescription("Activated!", 2000)
	if (tetrisSwitch2Activated) then
		hideWorldNode("murInvisible001")
		return "1"
	else
		return "0"
	end
end

-- Called when the node "tetrisswitch2" is pressed. 
function onTETRISSWITCH2_pressed()
	tetrisSwitch2Activated = true
	Audio:postEvent("Switch2_play")
    HUD:displayActionDescription("Activated!", 2000)
	if (tetrisSwitch1Activated) then
		hideWorldNode("murInvisible001")
		return "1"
	else
		return "0"
	end
end

-- Called when the bridge is slided. 
function onBRIDGE_slided()
    if (not bridgeRolled) then
        bridgeRolled = true
        Audio:postEvent("lateral_door_play")
    end
    
    return ""
end

-- Called when the node "robotai" has its awareness change.
function onROBOTAI_awarenessChange(newAwareness)
    -- Ignore false positives prior.
    if (inMaze) then
        if(newAwareness >= 100) then
            switch2Activated = false
            switch1Activated = false
            Player:canMove(0)
            HUD:displayActionDescription("You got caught!", 2000)
            fadeScreen(0, 200, "mazecaught")
            return
        end
        
        HUD:setAwarenessLevel(newAwareness)
    end
end

-- Called after the fade when caught in the maze.
function onMAZECAUGHT_fadeDone()
    Player:teleportTo("teleportMaze")
    HUD:setAwarenessLevel(0)
    Player:canMove(1)
    fadeScreen(100, 200)

    return ""
end

-- Called when the node "toilet" is used.
function onTOILET_itemUsed() 
	if (wentToBed) then
		if (not retrievedDirtyKey) then
			Inventory:addItem("dirtykey")
			retrievedDirtyKey = true
			Audio:postEvent("Flush_play")
			HUD:displayActionDescription("Cool! There was a... dirty key inside!", 2000)
			return ""
		else
			HUD:displayActionDescription("There's nothing left but sewage.", 2000)
			return ""
		end
	else
		HUD:displayActionDescription("Do not really feel like it, too tired.", 2000)
		return ""
	end
end

-- Called when the node "knife" is used.
function onKNIFE_itemUsed()
    if (not knifeTaken) then 
        Inventory:addItem("knife")
        knifeTaken = true
		hideWorldNode("knife08")
        HUD:displayActionDescription("Just in case", 2000)
    else
        HUD:displayActionDescription("One is enough for me", 2000) 
    end   
    return ""
end

-- Called when the node "knife" is used.
function onKNIFE_itemEquipped()
    currentlyEquipped = "knife"
    return ""
end

--[[Called when the node "glue" is used. not used anymore
function onGLUE_itemUsed()
    if (not glueTaken) then 
		Inventory:addItem("glue")
		glueTaken = true
		HUD:displayActionDescription("Wonder how it smells", 2000)
    end    
    return ""
end
]]

-- Called when the node "glue" is equipped.
function onGLUE_itemUsed()
    if (currentlyEquipped == "knife") then
        Inventory:removeItem("knife")
        --Inventory:removeItem("glue")
        Inventory:addItem("glueknife")
        currentlyEquipped = nil
        HUD:displayActionDescription("I Know right where to use this", 2000)
        hideWorldNode("Glue_Bottle_Part_5")
    else
        HUD:displayActionDescription("Not much happening...", 2000)
	end    
    return ""
end

-- Called when the node "glueknife" is used.
function onGLUEKNIFE_itemEquipped()
    currentlyEquipped = "glueknife"
    return ""
end


-- Indicates whether the door to go to last maze can be opened.
function onDOOR7_canOpen() 
  if (currentlyEquipped == "glueknife") then
	Inventory:removeItem("glueknife")	
    	HUD:displayActionDescription("I can almost smell freedom", 2000)
	Audio:setState("Metal_door", "Opened")
	return "1"
  -- Otherwise door is "locked".
  else
    	HUD:displayActionDescription("I think I gotta stick something in there", 2000)
	Audio:setState("Metal_door", "Locked_not_openable")
	Audio:postEvent("Metal_door_play")
	return "0"
  end
end


-- Indicates whether the door to go to last maze can be opened.
function onENDGRID_canOpen() 
  if (mazeSwitch1Activated and mazeSwitch2Activated) then
    HUD:displayActionDescription("Yes!!!", 2000)
	return "1"
  -- Otherwise door is "locked".
  else
    HUD:displayActionDescription("Door seems locked, perhaps I need to do something...", 2000)
	return "0"
  end
end


--FIXME: Cloth still in the game?
function onCLOTH_itemUsed()
    if (not clothTaken) then 
      	clothTaken = true
	--Inventory:addItem("cloth")
      --hideWorldNode("cloth")
     	HUD:displayActionDescription("Wish I had that a few crap load before.", 2000)
    else
        HUD:displayActionDescription("Okay, that's enough.", 2000)
    end
    return ""
end

-- Called when the node "oilcan" is used.
function onOILCAN_itemUsed()
    --[[
    if (currentlyEquipped == "cloth") then
        --Inventory:removeItem("cloth")
        Inventory:addItem("oily cloth")
        return ""
    end
    ]]
    
    -- Infinite oil!
    Inventory:addItem("oil")
    return ""
end

-- Called when the node "brokenlock" is used.
function onBROKENLOCK_itemUsed()
    if (currentlyEquipped == "glue") then
        if (not lockGlued) then
            lockGlued = true
            HUD:displayActionDescription("It worked!", 2000)
            return ""
        end

        return ""
    else 
        HUD:displayActionDescription("Didn't work so well...", 2000)
        
        return ""
    end
end
