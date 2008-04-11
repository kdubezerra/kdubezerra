#pragma once

#include "state_enum.h"
#include "RoundSmoothAOIAvatar.h"

#define PROB_TO_SOCIAL 80

class PPSim;

class PPAvatar : public RoundSmoothAOIAvatar {

	public:

		PPAvatar() {
			RoundSmoothAOIAvatar();
			m_state = social;			
			m_state_time = rand() % MAX_SOCIAL_TIME;
			m_elapsed_time = 0;			
		}
		
		void setSimulator (PPSim* ps) {
			mp_simulator = ps;
		}

		void step(unsigned long delay);
						
		pp_state getStatus();
		
	protected:
		
		pp_state m_state;
		unsigned long m_state_time, m_elapsed_time;
		PPSim* mp_simulator;
};

