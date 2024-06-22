#pragma once

struct CCLCMsg_Move_t {
private:
	char pad[0xC];

public:
	int backup_commands;
	int new_commands;
};