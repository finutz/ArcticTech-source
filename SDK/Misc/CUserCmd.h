#pragma once

#include "QAngle.h"
#include "Vector.h"
#include "checksum_crc.h"

enum ECommandButtons {
	IN_ATTACK	= (1 << 0),
	IN_JUMP		= (1 << 1),
	IN_DUCK		= (1 << 2),
	IN_FORWARD	= (1 << 3),
	IN_BACK		= (1 << 4),
	IN_USE		= (1 << 5),
	IN_CANCEL	= (1 << 6),
	IN_LEFT		= (1 << 7),
	IN_RIGHT	= (1 << 8),
	IN_MOVELEFT = (1 << 9),
	IN_MOVERIGHT= (1 << 10),
	IN_ATTACK2	= (1 << 11),
	IN_RUN		= (1 << 12),
	IN_RELOAD	= (1 << 13),
	IN_ALT1		= (1 << 14),
	IN_ALT2		= (1 << 15),
	IN_SCORE	= (1 << 16),
	IN_SPEED	= (1 << 17),
	IN_WALK		= (1 << 18),
	IN_BULLRUSH = (1 << 22)
};

struct CUserCmd {
public:
	virtual			~CUserCmd() { };

	CRC32_t GetChecksum(void) const
	{
		CRC32_t crc;
		CRC32_Init(&crc);

		CRC32_ProcessBuffer(&crc, &command_number, sizeof(command_number));
		CRC32_ProcessBuffer(&crc, &tick_count, sizeof(tick_count));
		CRC32_ProcessBuffer(&crc, &viewangles, sizeof(viewangles));
		CRC32_ProcessBuffer(&crc, &aimdirection, sizeof(aimdirection));
		CRC32_ProcessBuffer(&crc, &forwardmove, sizeof(forwardmove));
		CRC32_ProcessBuffer(&crc, &sidemove, sizeof(sidemove));
		CRC32_ProcessBuffer(&crc, &upmove, sizeof(upmove));
		CRC32_ProcessBuffer(&crc, &buttons, sizeof(buttons));
		CRC32_ProcessBuffer(&crc, &impulse, sizeof(impulse));
		CRC32_ProcessBuffer(&crc, &weaponselect, sizeof(weaponselect));
		CRC32_ProcessBuffer(&crc, &weaponsubtype, sizeof(weaponsubtype));
		CRC32_ProcessBuffer(&crc, &random_seed, sizeof(random_seed));
		CRC32_ProcessBuffer(&crc, &mousedx, sizeof(mousedx));
		CRC32_ProcessBuffer(&crc, &mousedy, sizeof(mousedy));

		CRC32_Final(&crc);
		return crc;
	}

	int		command_number;
	int		tick_count;
	QAngle	viewangles;
	Vector	aimdirection;
	float	forwardmove;
	float	sidemove;
	float	upmove;
	int		buttons;
	char	impulse;
	int		weaponselect;
	int		weaponsubtype;
	int		random_seed;
	short	mousedx;
	short	mousedy;
	bool	hasbeenpredicted;
private:
	char	pad[0x18];
};

struct C_CommandContext {
	bool			needsprocessing;
	CUserCmd		cmd;
	int				command_number;
};

void WriteUserCmd(void* buf, const CUserCmd* to, const CUserCmd* from);