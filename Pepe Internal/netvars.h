#pragma once
#include <Windows.h>
#include "Recv.h"

intptr_t GetOffset(RecvTable* table, const char* tablename, const char* netvarname) {
	for (int i = 0; i < table->m_nProps; i++) {
		RecvProp prop = table->m_pProps[i];

		if (!_stricmp(prop.m_pVarName, netvarname)) {
			return prop.m_Offset;
		}

		if (prop.m_pDataTable) {
			intptr_t offset = GetOffset(prop.m_pDataTable, tablename, netvarname);
			if (offset) {
				return offset + prop.m_Offset;
			}
		}
	}
	return 0;
}

intptr_t GetNetvarOffset(const char* tablename, const char* netvarname, ClientClass* clientclass) {
	ClientClass* currnode = clientclass;

	for (auto currnode = clientclass; currnode; currnode = currnode->m_pNext) {
		if (!_stricmp(tablename, currnode->m_pRecvTable->m_pNetTableName)) {
			return GetOffset(currnode->m_pRecvTable, tablename, netvarname);
		}
	}
	return 0;
}