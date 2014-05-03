///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Game generic core module implementation
///

#include "Precompiled.h"
#include "GameBase.h"

// Engine includes
#include "VCNNodes/Node.h"
#include "VCNNodes/NodeCore.h"
#include "VCNRenderer/RenderCore.h"
#include "VCNUtils/Chrono.h"
#include "VCNUtils/StringUtils.h"

VCN_TYPE( VCNGameBase, VCNAtom );

///////////////////////////////////////////////////////////////////////
///
/// Construct the game base module.
///
VCNGameBase::VCNGameBase()
: mRequestQuit(false)
{
}



///////////////////////////////////////////////////////////////////////
///
/// Destructs the game module.
///
VCNGameBase::~VCNGameBase()
{
}



///////////////////////////////////////////////////////////////////////
///
/// Called at startup, init all the modules of the game.
///
/// @return true if everything went right, otherwise false is returned.
///
VCNBool VCNGameBase::Initialize()
{
  // Create the modules
  if( !CreateModules() )
  {
    VCN_ASSERT_FAIL( "Could not create modules!" );
    return false;
  }

  // Init the modules
  if( !InitModules() )
  {
    VCN_ASSERT_FAIL( "Could not init modules!" );
    return false;
  }

  return true;
}



///////////////////////////////////////////////////////////////////////
///
/// Once all modules have been created and registered, we initialized 
/// them in the order they have been created.
///
/// @return success state.
///
const VCNBool VCNGameBase::InitModules()
{
  // Init the other modules
  for (CoreList::iterator iter = mCoreModules.begin(), end = mCoreModules.end(); iter != end; ++iter)
  {
    IVCNCore* core = (*iter);

    if( !core->Initialize() )
    {
      VCNAtom* coreAtom = safe_pointer_cast<VCNAtom*>( core );
      if ( coreAtom )
      {
        VCN_ASSERT_FAIL( VCNTXT("Could not initialize module %s!"), VCN_ASCII_TO_TCHAR(coreAtom->GetTypeName()) );
      }
      else
      {
        VCN_ASSERT_FAIL( VCNTXT("Could not initialize module!") );
      }
      return false;
    }
  }

  return true;
}



///////////////////////////////////////////////////////////////////////
///
/// Updates the game modules every frame.
///
/// @return true if all module have been ticked normally.
///
VCNBool VCNGameBase::Process(const float elapsedTime)
{
  // Start by processing the modules
  bool success = ProcessModules( elapsedTime );
  VCN_ASSERT_MSG( success, "Error processing modules! Your game is flawed!" );

  return success;
}



///////////////////////////////////////////////////////////////////////
///
/// Loops all module and tick them.
///
VCNBool VCNGameBase::ProcessModules(const float elapsedTime)
{
  // Process the other modules
  CoreList::iterator iter = mCoreModules.begin();
  CoreList::iterator iter_end = mCoreModules.end();
  while( iter != iter_end )
  {
    if( !(*iter)->Process( elapsedTime ) )
    {
      VCN_ASSERT( false && "One of the core modules could not process." );
      return false;
    }
    ++iter;
  }
  return true;
}


///////////////////////////////////////////////////////////////////////
///
/// Release all module in reverse order they were created.
/// 
/// @return true if all module were successfully released.
///
const VCNBool VCNGameBase::ReleaseModules()
{
  // Delete the other modules in reverse other they were created.
  CoreList::reverse_iterator it = mCoreModules.rbegin();
  CoreList::reverse_iterator end = mCoreModules.rend();

  while( it != end )
  {
    IVCNCore* core = *it;
    if( !core->Uninitialize() )
    {
      VCNAtom* coreAtom = dynamic_cast<VCNAtom*>( core );
      VCN_ASSERT_FAIL( "%s module failed to uninitialize.", coreAtom->GetTypeName() );
      return false;
    }
    ++it;
  }

  // Delete cores.
  it = mCoreModules.rbegin();
  while( it != end )
  {
    delete (*it);
    ++it;
  }

  mCoreModules.clear();

  return true;
}


//-------------------------------------------------------------
/// Call to render graphics to the screen
//-------------------------------------------------------------
///////////////////////////////////////////////////////////////////////
///
/// Renders the game using the rendering engine.
///
/// @return true if the rendering was executed correctly.
///
const VCNBool VCNGameBase::Render() const
{
  VCNRenderCore* renderer = VCNRenderCore::GetInstance();

  PrepareFrame();

  renderer->SetClearColor( 0.0f, 0.0f, 0.0f );

  if( !renderer->BeginRendering( true, true, true ) )
  {
    VCN_ASSERT_FAIL( "Could not begin scene rendering." );
    return false;
  }

  RenderFrame();

  // We're done rendering!
  renderer->EndRendering();

  return true;
}


///////////////////////////////////////////////////////////////////////
///
/// Uninitialize the game module.
///
VCNBool VCNGameBase::Uninitialize()
{
  return ReleaseModules();
}


///////////////////////////////////////////////////////////////////////
///
/// Prepare the game to quit. The game should quit after processing the
/// last frame.
///
void VCNGameBase::Quit()
{
  PostQuitMessage(0);
  mRequestQuit = true;
}
