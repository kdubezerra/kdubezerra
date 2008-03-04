/*
		ZIG game-oriented networking engine
		Project Home: http://zige.sourceforge.net

		Copyright (c) 2002-2007, Fábio Reis Cecin.
		All rights reserved.
    
		This free software is licensed under a BSD-style license. Read the 
		LICENSE.TXT file for the full license.
*/

#ifndef _ZIG_HEADER_POLICY_H_
#define _ZIG_HEADER_POLICY_H_

#define STATION_DEFAULT_SEND_INTERVAL (0.0)       // this is more important to STREAM_INDIVIDUAL_ACK
#define STATION_DEFAULT_RESEND_INTERVAL (0.5)     // only affects STREAM_INDIVIDUAL_ACK
#define STATION_DEFAULT_MAX_OUTGOING_FLOW (10240) // way too high

/*! \brief A collection of parameters for streams.

		When a new stream is created on an existing ZIG connection it takes a collection of parameters which determine 
		some of the details of how the stream will work. The parameters are as follows:

		- Send interval: The minimum amount of time, in seconds, that should elapse before sending the stream payload 
		on a new outgoing packet. Example: 
			- Suppose the send interval for a given stream S is configured to 0.1s;
			- A ZIG UDP packet is sent with the payload of stream S (for instance, a collection of messages being sent 
			or re-sent through stream S);
			- 0.05s of real time elapses (less than 0.1s);
			- A new ZIG UDP packet is sent. Since the last packet was sent less than 0.1s ago, this packet won't contain 
			message retransmissons for stream S.

		- Resend interval: The minimum amount of time, in seconds, that should elapse before re-sending a message 
		through a stream of type STREAM_INDIVIDUAL_ACK (streams of type STREAM_CUMULATIVE_ACK always resend messages 
		on every outgoing packet). Example:
			- Suppose the resend interval for a given stream S is configured to 0.4s;
			- The local application requests that message M is sent through stream S;
			- A ZIG UDP packet is sent with the payload of stream S, which contains message M;
			- 0.2s of real time elapses (less than 0.4s);
			- A new ZIG UDP packet is sent with the payload of stream S. The receipt of message M is still not acknowledged 
			by the remote host, but since the resend interval has not elapsed yet (0.4s), message M is not re-sent in this 
			outgoing packet.

		- Max outgoing flow: The maximum amount of bytes that the stream may insert on the outgoing UDP packet.

		- Discard unsent unreliables: A boolean flag for the stream. Default is "false", which keeps this new 
		tool disabled. When this parameter set to "true" on a stream of type STREAM_NO_ACK, it causes ALL of the 
		pending outgoing messages list to be WIPED OUT CLEAN, EVERY TIME A PACKET IS SENT. When this is set to 
		"true" on a stream of type STREAM_CUMULATIVE_ACK, it causes all pending outgoing messages that are marked 
		with "unreliable" delivery to be WIPED OUT, EVERY TIME A PACKET IS SENT (messages marked as "reliable" 
		delivery are not affected). NOTE: A message's "unreliable" or "reliable" delivery status is controlled 
		by the bool argument of either zigclient_c::send_message() or zigserver_c::send_message()). If you don't 
		know what you are doing, leave this flag set to false. This is a helper for some applications so they can 
		better implement their flow/bandwidth control schemes.

		\see zigclient_c and zigserver_c for an explanation of streams and connections
*/
class policy_c {

	// public methods:
	public:

		/*! \brief Constructor: can set the value of the parameters, or leave at defaults (see documentation 
				of class policy_c for details).

				\param send_interval The minimum interval between sending the stream payload on outgoing UDP 
				packets. Default is ZERO (0.0) seconds (STATION_DEFAULT_SEND_INTERVAL).
				\param resend_interval The minimum interval between resending a message through this stream. 
				This works only for streams of type STREAM_INDIVIDUAL_ACK. Default is 0.5 seconds 
				(STATION_DEFAULT_RESEND_INTERVAL).
        \param max_outgoing_flow The maximum amount of bytes that the stream may insert on the 
				outgoing UDP packet. Default value is 10240 (STATION_DEFAULT_MAX_OUTGOING_FLOW).
				\param discard_unsent_unreliables If set to \c true, then all pending outgoing messages on the 
				stream that are marked for 'unreliable' delivery will be deleted after any packet is sent. This 
				works only for streams of type STREAM_NO_ACK and STREAM_CUMULATIVE_ACK.
		*/
		policy_c(double send_interval = STATION_DEFAULT_SEND_INTERVAL,
		         double resend_interval = STATION_DEFAULT_RESEND_INTERVAL,
		         int max_outgoing_flow = STATION_DEFAULT_MAX_OUTGOING_FLOW,
		         bool discard_unsent_unreliables = false)
		{
			set_send_interval(send_interval);
			set_resend_interval(resend_interval);
			set_max_outgoing_flow(max_outgoing_flow);
			set_discard_unsent_unreliables(discard_unsent_unreliables);
		}

		/*! \brief Gets the send interval of this stream policy record (see documentation of class policy_c for details).

				\return The current send interval of this stream policy record, in seconds.
		*/
		double get_send_interval()     { return send_interval; }

		/*! \brief Gets the resend interval of this stream policy record (see documentation of class policy_c for details).

				\return The current resend interval of this stream policy record, in seconds.
		*/
		double get_resend_interval()   { return resend_interval; }

		/*! \brief Gets the max outgoing flow of this stream policy record (see documentation of class policy_c for details).

				\return The current max outgoing flow of this stream policy record, in bytes.
		*/
		int    get_max_outgoing_flow() { return max_outgoing_flow; }

		/*! \brief Gets the unsent unreliable data behavior of this stream policy record, after the outgoing packet (see documentation of class policy_c for details).

				\return The current discard unsent unreliable policy of this stream policy record.
		*/
		bool   get_discard_unsent_unreliables() { return discard_unsent_unreliables; }

		/*! \brief Sets the send interval of this stream policy record (see documentation of class policy_c for details).

				\param send_interval The new send interval for this stream policy record, in seconds.
		*/
		void   set_send_interval(double send_interval)      { this->send_interval = send_interval; }

		/*! \brief Sets the resend interval of this stream policy record (see documentation of class policy_c for details).

				\param resend_interval The new resend interval for this stream policy record, in seconds.
		*/
		void   set_resend_interval(double resend_interval)  { this->resend_interval = resend_interval; }

		/*! \brief Sets the max outgoing flow of this stream policy record (see documentation of class policy_c for details).

				\param max_outgoing_flow The new max outgoing flow for this stream policy record, in bytes.
		*/
		void   set_max_outgoing_flow(int max_outgoing_flow) { this->max_outgoing_flow = max_outgoing_flow; }

		/*! \brief Gets the unsent unreliable data behavior of this stream policy record, after the outgoing packet (see documentation of class policy_c for details).

				\return The current discard unsent unreliable policy of this stream policy record.
		*/
		void   set_discard_unsent_unreliables(bool discard_unsent_unreliables) {
			this->discard_unsent_unreliables = discard_unsent_unreliables;
		}

	// won't show up on doxygen
	private:

		// minimum interval between two sends
		double send_interval;

		// minimum interval between two resends (timeout)
		double resend_interval;

		// maximum size of a stream (inside a packet)
		int max_outgoing_flow;

		// discard unsent unreliables messages policy
		bool discard_unsent_unreliables;
};

#endif

