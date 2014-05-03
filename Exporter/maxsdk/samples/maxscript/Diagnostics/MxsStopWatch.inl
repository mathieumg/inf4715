
// Methods
def_local_generic( Start, "Start" );
def_local_generic( Pause, "Pause" );
def_local_generic( Resume, "Resume" );
def_local_generic( Stop, "Stop" );
def_local_generic( Reset, "Reset" );

// Properties all will be read only
def_property_getter( IsRunning );
def_property_getter( IsPaused );
def_property_getter( IsStopped );
def_property_getter( TimerCurrentState );
def_property_getter( ElapsedTime );
