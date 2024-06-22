#include "CUserCmd.h"

#include "../../Utils/Utils.h"

void WriteUserCmd(void* buf, const CUserCmd* to, const CUserCmd* from) {
	static void* write_user_cmd = Utils::PatternScan("client.dll", "55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D");
	__asm
	{
		mov     ecx, buf
		mov     edx, to
		push    from
		call    write_user_cmd
		add     esp, 4
	}
}