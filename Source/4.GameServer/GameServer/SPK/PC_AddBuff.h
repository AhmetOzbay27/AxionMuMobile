#pragma once
#include "User.h"

struct BuffData {
	int IDBuff;
	int Timer;
	int Val1;
	int Val2;
	int Val3;
	int Val4;
};

struct IsReadData {
	BuffData Buffclass[16]; // Giới hạn 32 buff
	BuffData Buffshop[16]; // Giới hạn 32 buff
	int SkillCount; // Đếm số buff đã đọc
};




class AddBuffer {
public:
	IsReadData IsReadDataX;
	void AddBuffer::Read(char* FilePath);
	void Reload();	// Faz 2b.2-R: canlı SPK log deseni '[SPK] AddBuff configuration saved and reloaded' — /reload addbuff
	bool CommandAddBuff(LPOBJ lpObj);
private:
	char m_Path[256];	// Faz 2b.2-R: Read'te saklanan config yolu (reload için)
};
extern AddBuffer gAddBuffer;