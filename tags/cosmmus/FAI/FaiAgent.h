class ECHO_Timer;

class ECHO_Agent : public Agent {
	public:
		ECHO_Agent();
		int command(int argc, const char*const* argv);
	protected:
		void timeout(int);
		void sendit();
		double interval_;
		ECHO_Timer echo_timer_;
};

class ECHO_Timer : public TimerHandler {
	public:
		ECHO_Timer(ECHO_Agent *a) : TimerHandler() { a_ = a; }
	protected:
		virtual void expire(Event *e);
		ECHO_Agent *a_;
};


void ECHO_Agent::timeout(int)
{
	sendit();
	echo_timer_.resched(interval_);
}

void ECHO_Agent::sendit()
{
	Packet* p = allocpkt();
	ECHOHeader *eh = ECHOHeader::access(p-\>bits());
	eh-\>timestamp() = Scheduler::instance().clock();
	send(p, 0);     // Connector::send()
}

void ECHO_Timer::expire(Event *e)
{
	a_-\>timeout(0);
}