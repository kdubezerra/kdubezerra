class CosmmusStepper : public TimerHandler
{
	public:
		CosmmusStepper(CosmmusStepperClass *a) : TimerHandler() { a_ = a; }
		virtual double expire(Event *e);
	protected:
		CosmmusStepperClass *a_;
};


double CosmmusStepper::expire(Event *e)
{
	// do the work
	// => do not reschedule timer
	// return TIMER_HANDLED;
	// => reschedule timer after delay
	// return delay;
}
