#include "CosmmusSmoothAvatar.h"
#include "pp-avatar.h"
#include "pp-simulator.h"

void PPAvatar::step(unsigned long delay)  {
  CosmmusSmoothAvatar::step(delay);
	
  if (mp_simulator->isClientServer()) return;

  m_elapsed_time += delay;

  if (m_elapsed_time >= m_state_time) {
    
    if (m_state == social) {
      mp_simulator->spaceChangeCB(social, action, player_id);
      m_state = action;
      m_state_time = rand() % MAX_ACTION_TIME;
    }
    else {
      if (rand() % 100 < PROB_TO_SOCIAL) {
        mp_simulator->spaceChangeCB(action, social, player_id);
        m_state = social;
        m_state_time = rand() % MAX_SOCIAL_TIME;
      }
      else {
        mp_simulator->spaceChangeCB(action, action, player_id);
        m_state_time = rand() % MAX_ACTION_TIME;
      }
    }
					
    m_elapsed_time = 0;
					
  }
}


pp_state PPAvatar::getStatus() {
  return m_state;
}