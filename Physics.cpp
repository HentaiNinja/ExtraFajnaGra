#include "Physics.h"



Physics::Physics(pos_t pos /*0, 0*/, pos_t vel /*0, 0*/, pos_t acc /*0, 0*/)
{
	position = pos;
	velocity = vel;
	acceleration = acc;
}

void Physics::move(int dt, bool resistance /*false*/)
{
	double acc_sd = 0.930001;
	double vel_sd = 0.92043;
	if (resistance) acceleration = acceleration * acc_sd;
	velocity = (velocity + acceleration * dt);
	if (resistance) velocity = velocity * vel_sd;
	position = position + velocity * dt + acceleration * dt * dt * 0.5;
}

Physics::~Physics()
{
}
