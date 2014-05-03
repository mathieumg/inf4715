///
/// @brief Defines an abstract physic controller
///

#ifndef VCNPHYSICCONTROLLER_H
#define VCNPHYSICCONTROLLER_H

#include "VCNCore/Atom.h"
#include "VCNUtils/Matrix.h"

class VCNPhysicController : public VCNAtom
{

	VCN_CLASS;

public:

	// Destructor
	virtual ~VCNPhysicController();

	/// Returns the actor transformation
	virtual Matrix4 GetTransform() const = 0;

	/// Adds a impulsion to the actor
	//virtual void AddForce(const Vector3& force) = 0;

protected:

	// Default constructor
	VCNPhysicController();

};

#endif // VCNPHYSICCONTROLLER_H
