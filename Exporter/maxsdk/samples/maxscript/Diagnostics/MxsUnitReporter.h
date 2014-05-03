#pragma once
#include <strclass.h>
#include <noncopyable.h>
#include <containers\array.h>

//! Enum to differentiate between different assertion events.
enum ReportType
{
	//! Simple logged messages
	LoggedMessage,
	//! An assertion failure
	AssertFailure,
	//! An exception
	ExceptionFailure
};

//! Type that actually holds the assert message
struct MxsUnitMessage
{
	//! The actual message
	MSTR message;
	//! Tells what type the message is
	ReportType type;
};

//! \brief Class for capturing and reporting assert errors
/*!	A singleton instance of this class is used for capturing
	maxscript assert failures and logging maxscript messages. 
	All assert failures are also written to the log messages
	buffer. So the logged messages buffer contains everything. 
	Therefore assert failures can be retrieved in the context
	of logged messages. 
	This class provides methods to  retrieve the failure messages, 
	and to clear them as well. This is used in conjunction with the
	assert methods that this DLL also exposes to maxscript. 
	
	\code
	(
		clearlistener()
		AssertReporter.Clear()
		AssertReporter.LogMessage "Start of the tests"
		assert_true ("hh" == "hi") message:"I am an assert"
		AssertReporter.LogMessage "Middle of the tests"
		assert_false true message:"This should be false"
		val = 5
		assert_equal 5 val message:"These numbers should be the same"
		assert_equal (bitmap 20 20) (bitmap 20 20) message:"These bitmaps should NOT be the same"
		h = bitmap 40 40
		assert_equal h h message:"These bitmaps should be the same"
		assert_equal "sdastas" h message:"These are not the same"
		AssertReporter.LogMessage "End of the tests"
		print (AssertReporter.GetMessages())
		print ((AssertReporter.GetAssertFailures()).count)
		ok
	)
	\endcode
	would print out the following results:
	\code
		"Start of the tests"
		"I am an assert - Expected true, got false (E:\Depot\DeveloperTools\Debugging\mxsDebugCRT\MxsUnitAssert.UnitTest.ms) at line 8"
		"Middle of the tests"
		"This should be false - Expected false, got true (E:\Depot\DeveloperTools\Debugging\mxsDebugCRT\MxsUnitAssert.UnitTest.ms) at line 10"
		"These bitmaps should NOT be the same - Expected BitMap:, got BitMap: (E:\Depot\DeveloperTools\Debugging\mxsDebugCRT\MxsUnitAssert.UnitTest.ms) at line 13"
		"These are not the same - Expected "sdastas", got BitMap: (E:\Depot\DeveloperTools\Debugging\mxsDebugCRT\MxsUnitAssert.UnitTest.ms) at line 16"
		"End of the tests"
		4
		OK
	\endcode
*/
class MxsUnitReporter : MaxSDK::Util::Noncopyable
{
private:
	//! The static instance of this singleton.
	static MxsUnitReporter* mInstance;
	//! Holds both assert messages, and other messages that the end
	//! user may want to log during test execution.
	MaxSDK::Array<MxsUnitMessage> mMessages;
protected:
	
	//! Constructor
	MxsUnitReporter();
	//! Destructor
	~MxsUnitReporter();

public:
	// statics --------------------------------------------- //
	
	//! Singleton access method
	static MxsUnitReporter* GetInstance();
	
	//! Singleton destroy method
	static void DestroyInstance();
	
	//! Formats an assert into a standard form. 
	//! Called by the assert methods themselves.
	//! \param frame_name - A string containing the name of the context or stack frame.
	//! \param expected   - A string representation of the expected value.
	//! \param message    - A user defined message that if non NULL, will be prepended to the beginning of the message.
	//! \param fileName   - The file where the assert failed.
	//! \param lineNumber - The line number where the assert failed.
	//! \return - A nicely formatted string.
	static MSTR CreateMessage(const MCHAR* frame_name, const MCHAR* expected, const MCHAR* actual, const MCHAR* message, const MCHAR* fileName, unsigned int lineNumber);

	// instance methods ------------------------------------ //
	//! For logging an assert failure
	void LogAssertFailure( MSTR );

	//! For logging an assert failure
	void LogAssertFailure( const MCHAR* );
	
	//! For logging an exception failure
	void LogExceptionFailure( MSTR );

	//! For logging an exception failure
	void LogExceptionFailure( const MCHAR* );

	//! For logging a message
	void LogMessage( const MCHAR* );
	
	//! For logging a message
	void LogMessage( MSTR );
	
	//! Gets the assert failures only.
	MaxSDK::Array<MSTR> GetAssertFailures();

	//! Gets the exceptions only.
	MaxSDK::Array<MSTR> GetExceptionFailures();
	
	//! Gets all messages and assert failures, in the order they were logged.
	MaxSDK::Array<MSTR> GetMessages();
	
	//! Gets a user defined string.
	MSTR GetUserData();

	//! Sets a user defined string, and saves it until it's cleared.
	void SetUserData(MSTR&);

	//! Erases both the logged messages and the assert failures.
	void Clear();

private:
	MaxSDK::Array<MSTR> GetFailure(ReportType type);
	MSTR mUserData;
};