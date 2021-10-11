#pragma once
#include "QAngle.h"
#include "_vector.h"

class CUserCmd {
public:
	virtual ~CUserCmd() {};
	int		command_number;
	int		tick_count;
	QAngle	viewangles;
	vec3	aimdirection;
	float	forwardmove;
	float	sidemove;
	float	upmove;
	int		buttons;
	char    impulse;
	int		weaponselect;
	int		weaponsubtype;
	int		random_seed;
	short	mousedx;
	short	mousedy;
	bool	hasbeenpredicted;
	char	pad_0x4C[0x18];
};

class CVerifiedUserCmd
{
public:
	CUserCmd m_cmd;
	unsigned long m_crc;
};