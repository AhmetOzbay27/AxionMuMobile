#include "stdafx.h"
#include "PC_AddBuff.h"
#include "EffectManager.h"
#include "Notice.h"
#include "MemScript.h"
#include "Util.h"
#include <fstream>
#include <sstream>
#include <string>

AddBuffer gAddBuffer;

void AddBuffer::Read(char* FilePath)
{
	this->IsReadDataX.SkillCount = 0;

	std::ifstream file(FilePath);
	if (!file.is_open())
	{
		ErrorMessageBox("Không thể mở tệp", FilePath);
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty() || line[0] == '/')
		{
			continue;
		}

		std::stringstream ss(line);
		if (line == "0") {
			while (std::getline(file, line)) {
				if (line == "end") {
					break;
				}

				std::stringstream ss(line);
				BuffData buffclass;
				if (ss >> buffclass.IDBuff >> buffclass.Timer >> buffclass.Val1 >> buffclass.Val2 >> buffclass.Val3 >> buffclass.Val4)
				{
					if (this->IsReadDataX.SkillCount < 16) {
						this->IsReadDataX.Buffclass[this->IsReadDataX.SkillCount] = buffclass;
						this->IsReadDataX.SkillCount++;
					}
					else
					{
						LogAdd(LOG_RED, "Đã vượt quá giới hạn Buffclass.");
						break;
					}
				}
			}
		}
		else if (line == "1") {
			while (std::getline(file, line)) {
				if (line == "end") {
					break;
				}

				std::stringstream ss(line);
				BuffData buffshop;
				if (ss >> buffshop.IDBuff >> buffshop.Timer >> buffshop.Val1 >> buffshop.Val2 >> buffshop.Val3 >> buffshop.Val4)
				{
					if (this->IsReadDataX.SkillCount < 16) {
						this->IsReadDataX.Buffshop[this->IsReadDataX.SkillCount] = buffshop;
						this->IsReadDataX.SkillCount++;
					}
					else
					{
						LogAdd(LOG_RED, "Đã vượt quá giới hạn Buffshop.");
						break;
					}
				}
			}
		}
	}

	file.close();

	// Faz 2b.2-R: yolu sakla (reload icin) — sadece başarılı okumada
	memset(this->m_Path,0,sizeof(this->m_Path));
	strcpy_s(this->m_Path,FilePath);
}

// Faz 2b.2-R: config'i yeniden yükler (canlı SPK log deseni: '[SPK] AddBuff
// configuration saved and reloaded'). /reload addbuff komutu çağırır; hata
// durumunda mevcut veriler korunur. Read SkillCount'u sıfırladığı için
// boş-dosya koruması SkillCount ile yapılır (E-05 deseni).
void AddBuffer::Reload() // Faz 2b.2-R
{
	if(this->m_Path[0] == 0)
	{
		LogAdd(LOG_RED,"[SPK] AddBuff Reload skipped - config path not set yet");
		return;
	}

	IsReadData oldData;
	memcpy(&oldData,&this->IsReadDataX,sizeof(oldData));

	this->Read(this->m_Path);

	if(this->IsReadDataX.SkillCount == 0)
	{
		memcpy(&this->IsReadDataX,&oldData,sizeof(oldData));
		LogAdd(LOG_RED,"[SPK] AddBuff Reload failed - old data restored (%s)",this->m_Path);
		return;
	}

	LogAdd(LOG_BLUE,"[SPK] AddBuff configuration saved and reloaded");
}

bool AddBuffer::CommandAddBuff(LPOBJ lpObj)
{
	int TimeClick = 5 * 1000; // 5 giây
	if ((GetTickCount() - lpObj->ClickClientSend) < 5 * 1000)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Click ít thôi, bộ mắc click lắm hả?");
		return false;
	}
	for (int i = 0; i < this->IsReadDataX.SkillCount; i++)
	{
		BuffData& buff = this->IsReadDataX.Buffclass[i];
		gEffectManager.AddEffect(lpObj, 1, buff.IDBuff, buff.Timer, buff.Val1, buff.Val2, buff.Val3, buff.Val4);
	}
	for (int i = 0; i < this->IsReadDataX.SkillCount; i++)
	{
		BuffData& buff = this->IsReadDataX.Buffshop[i];
		gEffectManager.AddEffect(lpObj, 1, buff.IDBuff, (int)(time(0) + buff.Timer), buff.Val1, buff.Val2, buff.Val3, buff.Val4);
	}
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Bạn đã được thêm Buff");
	lpObj->ClickClientSend = GetTickCount();
	return true;
}