
///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief GameEvent
///

#ifndef GAME_EVENT_H
#define	GAME_EVENT_H

///
/// Class encapsulating the game's events.
///
class GameEvent
{
public:
  GameEvent();
  virtual ~GameEvent();

  virtual void CallActions();

  void SetActivation(const bool val) { mActivated = val; }

protected:
  bool mRepeatable;
  bool mActivated;
};

#endif // GAME_EVENT_H
