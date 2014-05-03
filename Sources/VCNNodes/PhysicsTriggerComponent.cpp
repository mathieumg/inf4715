#include "Precompiled.h"                                     
#include "PhysicsTriggerComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
#include "NodeCore.h"
#include "VCNUtils\Constants.h"
#include "TriggerManager.h"
#include "RenderNode.h"
class VCNRenderNode;
//**EndCodeRegion
                                                               
VCNPhysicsTriggerComponent::VCNPhysicsTriggerComponent()                    
{                                                              
    // Default values                                          
    mNameOfObjectToWatch = L"";                   
    mOnEnter = LuaTrigger(L"", L"");                   
    mOnExit = LuaTrigger(L"", L"");                   
//**CodeRegion
	mTriggerNode = NULL;
//**EndCodeRegion
}                                                              
                                                               
void VCNPhysicsTriggerComponent::Initialise(const Parameters& params)
{                                                              
//**CodeRegion
	mObjectToWatchID = VCNNodeCore::GetInstance()->GetNodeByName(mNameOfObjectToWatch);
	mPreviousPosition.Set(10000000.0f, 10000000.0f, 10000000.0f);

	// Create and attach a TriggerNodeto the current node with it's aabb
	VCNNode* owner = GetOwner();
	if (owner->Is<VCNRenderNode>())
	{
		VCNRenderNode* renderNode = owner->Cast<VCNRenderNode>();
		VCNAabb aabb = renderNode->GetBoundingBox();
		mTriggerNode = TriggerManager::GetInstance().CreateTrigger(GetOwner()->GetTag() + VCNTXT("Trigger"), aabb);
		renderNode->EnableRender(false); // Hide the node
	}


//**EndCodeRegion
}                                                              
                                                               
void VCNPhysicsTriggerComponent::Update(VCNFloat dt)                 
{                                                              
//**CodeRegion

	if (mObjectToWatchID != kInvalidNodeID && mTriggerNode)
	{
		VCNNode* watchedObject = VCNNodeCore::GetInstance()->GetNode(mObjectToWatchID);
		if (watchedObject)
		{
			Vector3 watchedObjectPosition = watchedObject->GetWorldTranslation();
			TriggerNode::CollisionFlag flags;

			VCNNode* owner = GetOwner();
			if (owner->Is<VCNRenderNode>())
			{
				VCNRenderNode* renderNode = owner->Cast<VCNRenderNode>();
				VCNAabb aabb = renderNode->GetBoundingBox();
				mTriggerNode->SetBBox(aabb);

				if (mTriggerNode->Test(mPreviousPosition, watchedObjectPosition, flags))
				{
					if (flags & TriggerNode::CollisionFlag::CF_ENTER)
					{
						mOnEnter.Trigger();
					}
					else if (flags & TriggerNode::CollisionFlag::CF_LEAVE)
					{
						mOnExit.Trigger();
					}
				}
				mPreviousPosition = watchedObjectPosition;
			}
		}
	}

	
//**EndCodeRegion
}                                                              
                                                               
bool VCNPhysicsTriggerComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"NameOfObjectToWatch")                       
    {                                                                      
        mNameOfObjectToWatch = attributeValue;                           
        return true;                                                       
    }                                                                      
    else if (attributeName == L"OnEnter")                       
    {                                                                      
        std::stringstream ss;                                              
        std::string str( attributeValue.begin(), attributeValue.end() );   
        ss << str;                                                         
        ss >> mOnEnter;                                      
        return true;                                                       
    }                                                                      
    else if (attributeName == L"OnExit")                       
    {                                                                      
        std::stringstream ss;                                              
        std::string str( attributeValue.begin(), attributeValue.end() );   
        ss << str;                                                         
        ss >> mOnExit;                                      
        return true;                                                       
    }                                                                      
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNPhysicsTriggerComponent::Copy() const                          
{                                                                              
    VCNPhysicsTriggerComponent* otherComponent = new VCNPhysicsTriggerComponent();         
                                                                               
    otherComponent->mNameOfObjectToWatch = VCNString(mNameOfObjectToWatch);
    otherComponent->mOnEnter = LuaTrigger(mOnEnter);
    otherComponent->mOnExit = LuaTrigger(mOnExit);
                                                                               
    return std::shared_ptr<VCNPhysicsTriggerComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion
//**EndCodeRegion
