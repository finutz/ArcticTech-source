#include "NetVars.h"
#include "../SDK/Globals.h"

LuaProp_t NetVars::FindProp(RecvTable* table, std::string prop_name) {
	for (int i = 0; i < table->m_nProps; i++) {
		RecvProp* prop = &table->m_pProps[i];

		if (prop->m_pVarName == prop_name)
			return LuaProp_t(prop, prop->m_Offset);

		if (prop->m_pDataTable) {
			int offset = prop->m_Offset;
			RecvTable* recvp = prop->m_pDataTable;

			auto tmp = FindProp(recvp, prop_name);

			if (tmp.offset)
				return LuaProp_t(tmp.prop, offset + tmp.offset);
		}
	}

	return LuaProp_t(nullptr, 0);
}

int NetVars::GetNetVar(const char* table, const char* netvar) {
	ClientClass* start = Client->GetAllClasses();
	ClientClass* current = start;

	for (ClientClass* tab = start; tab; tab = tab->m_pNext) {
		if (!strcmp(table, tab->m_pRecvTable->m_pNetTableName))
			return GetNetVar(tab->m_pRecvTable, netvar);
	}

	return 0;
}

int NetVars::GetNetVar(RecvTable* table, const char* netvar) {
	for (int i = 0; i < table->m_nProps; i++) {
		RecvProp prop = table->m_pProps[i];

		if (!strcmp(prop.m_pVarName, netvar))
			return prop.m_Offset;

		if (prop.m_pDataTable) {
			int offset = prop.m_Offset;
			RecvTable* recvp = prop.m_pDataTable;

			int tmp = GetNetVar(recvp, netvar);

			if (tmp)
				return offset + tmp;
		}
	}

	return 0;
}

int NetVars::FindInDataMap(datamap_t* map, const char* netvar) {
	while (map != nullptr)
	{
		for (int i = 0; i < map->m_data_num_fields; i++)
		{
			if (map->m_data_desc[i].fieldName == nullptr)
				continue;

			if (!strcmp(map->m_data_desc[i].fieldName, netvar))
				return map->m_data_desc[i].fieldOffset[TD_OFFSET_NORMAL];

			if (map->m_data_desc[i].fieldType == FIELD_EMBEDDED)
			{
				if (map->m_data_desc[i].td != nullptr)
				{
					if (const auto uOffset = FindInDataMap(map->m_data_desc[i].td, netvar); uOffset != 0U)
						return uOffset;
				}
			}
		}

		map = map->m_base_map;
	}

	return 0U;
}