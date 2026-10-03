// DataStore.cpp: MuOnlineS6 kalici veri katmani - hazirlanmis ifade tabanli.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataStore.h"
#include "Log.h"

CDataStore gDataStore;

//---------------------------------------------------------------- yardimci
bool CDataStore::Prepare(char* sql) // OK
{
	if(SQL_SUCCEEDED(SQLFreeStmt(this->m_Stmt,SQL_CLOSE)) == 0)
	{
		// SQL_CLOSE basarisizligi kritik degil; asil kontrol Prepare'da.
	}

	memset(this->m_ParamLen,0,sizeof(this->m_ParamLen));
	memset(this->m_ParamInt,0,sizeof(this->m_ParamInt));

	SQLRETURN ret = SQLPrepare(this->m_Stmt,(SQLCHAR*)sql,SQL_NTS);

	if(SQL_SUCCEEDED(ret) == 0)
	{
		this->LogError(sql);
		return false;
	}

	return true;
}

bool CDataStore::Run() // OK
{
	SQLRETURN ret = SQLExecute(this->m_Stmt);

	if(SQL_SUCCEEDED(ret) == 0 && ret != SQL_NO_DATA)
	{
		this->LogError("(SQLExecute)");
		return false;
	}

	return true;
}

void CDataStore::BindStr(int idx,void* buf,int colSize) // OK
{
	this->m_ParamLen[(idx-1)] = SQL_NTS;
	SQLBindParameter(this->m_Stmt,idx,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_VARCHAR,colSize,0,buf,0,&this->m_ParamLen[(idx-1)]);
}

void CDataStore::BindInt(int idx,int val) // OK
{
	// DIKKAT: deger buraya KOPYALANIR. Onceki surum &val (yerel kopya) adresini
	// baglardi; ODBC surucusu parametre tamponunu SQLExecute ANINDA okudugu
	// icin bu adres o ana kadar olmus oluyordu -> butun tam sayi parametreleri
	// BOZUK deger gonderiyordu. (2e9 uc-uca testi bunu yakaladi.)
	this->m_ParamInt[(idx-1)] = val;
	this->m_ParamLen[(idx-1)] = 4;
	SQLBindParameter(this->m_Stmt,idx,SQL_PARAM_INPUT,SQL_C_LONG,SQL_INTEGER,4,0,&this->m_ParamInt[(idx-1)],0,&this->m_ParamLen[(idx-1)]);
}

void CDataStore::BindBlob(int idx,void* buf,int len) // OK
{
	if(len < 0){len = 0;}
	this->m_ParamLen[(idx-1)] = len;
	SQLBindParameter(this->m_Stmt,idx,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_VARBINARY,len,0,buf,0,&this->m_ParamLen[(idx-1)]);
}

void CDataStore::LogError(char* sql) // OK
{
	SQLCHAR state[6];SQLCHAR text[256];SQLINTEGER native;
	SQLSMALLINT len;
	SQLRETURN ret = SQLGetDiagRec(SQL_HANDLE_STMT,this->m_Stmt,1,state,&native,text,sizeof(text),&len);

	if(SQL_SUCCEEDED(ret) && SQL_NTS != (int)text[0])
	{
		_snprintf_s(this->m_Error,sizeof(this->m_Error),_TRUNCATE,"[DataStore] %s | SQL=%s",(char*)text,(sql == 0 ? "?" : sql));
	}
	else
	{
		_snprintf_s(this->m_Error,sizeof(this->m_Error),_TRUNCATE,"[DataStore] SQL hatasi (tanimsiz) | SQL=%s",(sql == 0 ? "?" : sql));
	}

	gLog.Output(LOG_GENERAL,"%s",this->m_Error);
}

char* CDataStore::LastError() // OK
{
	return this->m_Error;
}

//---------------------------------------------------------------- yasam dongusu
CDataStore::CDataStore() // OK
{
	this->m_Env = 0;
	this->m_Dbc = 0;
	this->m_Stmt = 0;
	this->m_Error[0] = 0;
	memset(this->m_ParamLen,0,sizeof(this->m_ParamLen));
}

CDataStore::~CDataStore() // OK
{
	this->Close();
}

bool CDataStore::Open(char* odbc,char* user,char* pass) // OK
{
	this->Close();

	if(SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&this->m_Env)) == 0)
	{
		return false;
	}

	if(SQL_SUCCEEDED(SQLSetEnvAttr(this->m_Env,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3,0)) == 0)
	{
		this->Close();
		return false;
	}

	if(SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_DBC,this->m_Env,&this->m_Dbc)) == 0)
	{
		this->Close();
		return false;
	}


	if(SQL_SUCCEEDED(SQLConnect(this->m_Dbc,(SQLCHAR*)odbc,SQL_NTS,(SQLCHAR*)user,SQL_NTS,(SQLCHAR*)pass,SQL_NTS)) == 0)
	{
		SQLCHAR state[6];SQLCHAR text[256];SQLINTEGER native;SQLSMALLINT len;
		if(SQL_SUCCEEDED(SQLGetDiagRec(SQL_HANDLE_DBC,this->m_Dbc,1,state,&native,text,sizeof(text),&len)))
		{
			gLog.Output(LOG_GENERAL,"[DataStore] SQLConnect(%s) basarisiz: %s",odbc,(char*)text);
		}
		else
		{
			gLog.Output(LOG_GENERAL,"[DataStore] SQLConnect(%s) basarisiz",odbc);
		}
		this->Close();
		return false;
	}

	if(SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT,this->m_Dbc,&this->m_Stmt)) == 0)
	{
		this->Close();
		return false;
	}

	SQLSetStmtAttr(this->m_Stmt,SQL_ATTR_QUERY_TIMEOUT,(SQLPOINTER)15,0);

	gLog.Output(LOG_GENERAL,"[DataStore] MuOnlineS6 kalici veri katmani acildi (ODBC=%s)",odbc);

	return true;
}

void CDataStore::Close() // OK
{
	if(this->m_Stmt)
	{
		SQLFreeHandle(SQL_HANDLE_STMT,this->m_Stmt);
		this->m_Stmt = 0;
	}

	if(this->m_Dbc)
	{
		SQLDisconnect(this->m_Dbc);
		SQLFreeHandle(SQL_HANDLE_DBC,this->m_Dbc);
		this->m_Dbc = 0;
	}

	if(this->m_Env)
	{
		SQLFreeHandle(SQL_HANDLE_ENV,this->m_Env);
		this->m_Env = 0;
	}
}

bool CDataStore::IsOpen() // OK
{
	return (this->m_Stmt != 0) ? true : false;
}

//---------------------------------------------------------------- sema denetimi
int CDataStore::SchemaCheck(char* report,int reportSize) // OK
{
	static char* TABLES[14] = {
		"CardPhone","CustomItemBank","CustomNpcQuest","DataNapGame",
		"EquipInventory","EventInventory","MuunInventory","MuRummyCard",
		"MuRummyData","PcPointData","PentagramJewel","PShopItemValue",
		"SNSData","ItemMarketData"
	};

	int missing = 0;
	int used = 0;

	if(report != 0 && reportSize > 0){report[0] = 0;}

	if(this->Prepare("SELECT COUNT(*) FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_TYPE='BASE TABLE' AND TABLE_NAME = ?") == false)
	{
		return 14;
	}

	for(int n=0;n < 14;n++)
	{
		char name[32];
		ZeroMemory(name,sizeof(name));
		strcpy_s(name,TABLES[n]);

		SQLFreeStmt(this->m_Stmt,SQL_CLOSE);   // onceki sonuc kumesini kapat
		this->BindStr(1,name,31);

		if(this->Run() == false){missing++;continue;}

		if(SQL_SUCCEEDED(SQLFetch(this->m_Stmt)) == 0)
		{
			missing++;
			if(report != 0 && used < reportSize - 64)
			{
				used += _snprintf_s(report+used,reportSize-used,_TRUNCATE,"EKSIK: %s\n",TABLES[n]);
			}
			continue;
		}

		int cnt = 0;
		SQLLEN ind = 0;
		if(SQL_SUCCEEDED(SQLGetData(this->m_Stmt,1,SQL_C_SLONG,(SQLPOINTER)&cnt,0,&ind)) == 0 || cnt == 0)
		{
			missing++;
			if(report != 0 && used < reportSize - 64)
			{
				used += _snprintf_s(report+used,reportSize-used,_TRUNCATE,"EKSIK: %s\n",TABLES[n]);
			}
		}
	}

	if(missing == 0)
	{
		gLog.Output(LOG_GENERAL,"[DataStore] Sema denetimi: 14/14 tablo yerinde");
	}
	else
	{
		gLog.Output(LOG_GENERAL,"[DataStore] SEMA HATASI: %d/14 tablo eksik\n%s",missing,(report == 0 ? "" : report));
	}

	return missing;
}

//---------------------------------------------------------------- yardimcilar
static bool DS_ReadStr(SQLHANDLE stmt,int col,char* out,int maxLen)
{
	SQLLEN ind = 0;
	ZeroMemory(out,maxLen);
	if(SQL_SUCCEEDED(SQLGetData(stmt,col,SQL_C_CHAR,(SQLPOINTER)out,maxLen-1,&ind)) == 0 && ind == SQL_NULL_DATA){return false;}
	return true;
}

static bool DS_ReadInt(SQLHANDLE stmt,int col,int* out)
{
	SQLLEN ind = 0;
	*out = 0;
	if(SQL_SUCCEEDED(SQLGetData(stmt,col,SQL_C_SLONG,(SQLPOINTER)out,0,&ind)) == 0){return false;}
	return true;
}

static bool DS_ReadBlob(SQLHANDLE stmt,int col,BYTE* out,int maxLen,int* outLen)
{
	SQLLEN ind = 0;
	SQLRETURN ret = SQLGetData(stmt,col,SQL_C_BINARY,(SQLPOINTER)out,maxLen,&ind);
	if(SQL_SUCCEEDED(ret) == 0 && ret != SQL_NO_DATA){return false;}
	if(ind == SQL_NULL_DATA){*outLen = 0;return false;}
	*outLen = (ind == SQL_NTS) ? 0 : (int)ind;
	return true;
}

static void DS_FillNapRow(SQLHANDLE stmt,DS_DataNapGame* r)
{
	DS_ReadStr(stmt,1,r->Account,11); DS_ReadStr(stmt,2,r->Name,11);
	DS_ReadInt(stmt,3,&r->TienNap); DS_ReadInt(stmt,4,&r->Checking); DS_ReadInt(stmt,5,&r->Status);
}

//---------------------------------------------------------------- 1 CardPhone
bool CDataStore::InsertCardPhone(DS_CardPhone* r) // OK
{
	if(this->Prepare("INSERT INTO CardPhone (acc,name,card_type,menhgia,card_num,card_num_md5,card_serial,timenap,addvpoint,status,stt,timeduyet) VALUES (?,?,?,?,?,?,?,?,?,?,?,?)") == false) return false;
	this->BindStr(1,r->acc,10); this->BindStr(2,r->name,10); this->BindStr(3,r->card_type,10);
	this->BindInt(4,r->menhgia); this->BindStr(5,r->card_num,20); this->BindStr(6,r->card_num_md5,50);
	this->BindStr(7,r->card_serial,20); this->BindInt(8,r->timenap); this->BindInt(9,r->addvpoint);
	this->BindInt(10,r->status); this->BindInt(11,r->stt); this->BindInt(12,r->timeduyet);
	return this->Run();
}

bool CDataStore::LoadCardPhoneByAcc(char* acc,DS_CardPhone* out) // OK
{
	if(this->Prepare("SELECT acc,name,card_type,menhgia,card_num,card_num_md5,card_serial,timenap,addvpoint,status,stt,timeduyet FROM CardPhone WHERE acc = ?") == false) return false;
	this->BindStr(1,acc,10);
	if(this->Run() == false) return false;
	if(SQL_SUCCEEDED(SQLFetch(this->m_Stmt)) == 0) return false;
	DS_ReadStr(this->m_Stmt,1,out->acc,11); DS_ReadStr(this->m_Stmt,2,out->name,11); DS_ReadStr(this->m_Stmt,3,out->card_type,11);
	DS_ReadInt(this->m_Stmt,4,&out->menhgia); DS_ReadStr(this->m_Stmt,5,out->card_num,21); DS_ReadStr(this->m_Stmt,6,out->card_num_md5,51);
	DS_ReadStr(this->m_Stmt,7,out->card_serial,21); DS_ReadInt(this->m_Stmt,8,&out->timenap); DS_ReadInt(this->m_Stmt,9,&out->addvpoint);
	DS_ReadInt(this->m_Stmt,10,&out->status); DS_ReadInt(this->m_Stmt,11,&out->stt); DS_ReadInt(this->m_Stmt,12,&out->timeduyet);
	return true;
}

bool CDataStore::UpdateCardPhoneStatus(char* acc,int status,int timeduyet) // OK
{
	if(this->Prepare("UPDATE CardPhone SET status = ?, timeduyet = ? WHERE acc = ?") == false) return false;
	this->BindInt(1,status); this->BindInt(2,timeduyet); this->BindStr(3,acc,10);
	return this->Run();
}

bool CDataStore::UpdateCardPhoneStatusByStt(int stt,int status,int timeduyet) // OK
{
	if(this->Prepare("UPDATE CardPhone SET status = ?, timeduyet = ? WHERE stt = ?") == false) return false;
	this->BindInt(1,status); this->BindInt(2,timeduyet); this->BindInt(3,stt);
	return this->Run();
}

bool CDataStore::UpdateCardPhoneNapByStt(int stt,int menhgia) // OK
{
	if(this->Prepare("UPDATE CardPhone SET status = 2, menhgia = ? WHERE stt = ?") == false) return false;
	this->BindInt(1,menhgia); this->BindInt(2,stt);
	return this->Run();
}

bool CDataStore::UpdateCardPhoneAddVPointByStt(int stt,int addvpoint,int timeduyet) // OK
{
	if(this->Prepare("UPDATE CardPhone SET addvpoint = ?, timeduyet = ? WHERE stt = ?") == false) return false;
	this->BindInt(1,addvpoint); this->BindInt(2,timeduyet); this->BindInt(3,stt);
	return this->Run();
}

bool CDataStore::DeleteCardPhone(char* acc) // OK
{
	if(this->Prepare("DELETE FROM CardPhone WHERE acc = ?") == false) return false;
	this->BindStr(1,acc,10);
	return this->Run();
}

//---------------------------------------------------------------- 2 CustomItemBank
bool CDataStore::InsertCustomItemBank(DS_CustomItemBank* r) // OK
{
	if(this->Prepare("INSERT INTO CustomItemBank (AccountID,ItemIndex,ItemLevel,ItemCount,AutoPick) VALUES (?,?,?,?,?)") == false) return false;
	this->BindStr(1,r->AccountID,10); this->BindInt(2,r->ItemIndex); this->BindInt(3,r->ItemLevel);
	this->BindInt(4,r->ItemCount); this->BindInt(5,r->AutoPick);
	return this->Run();
}

bool CDataStore::UpdateCustomItemBank(DS_CustomItemBank* r) // OK
{
	if(this->Prepare("UPDATE CustomItemBank SET ItemCount = ?, AutoPick = ? WHERE AccountID = ? AND ItemIndex = ? AND ItemLevel = ?") == false) return false;
	this->BindInt(1,r->ItemCount); this->BindInt(2,r->AutoPick);
	this->BindStr(3,r->AccountID,10); this->BindInt(4,r->ItemIndex); this->BindInt(5,r->ItemLevel);
	return this->Run();
}

bool CDataStore::LoadCustomItemBank(char* acc,int itemIndex,int itemLevel,DS_CustomItemBank* out) // OK
{
	if(this->Prepare("SELECT AccountID,ItemIndex,ItemLevel,ItemCount,AutoPick FROM CustomItemBank WHERE AccountID = ? AND ItemIndex = ? AND ItemLevel = ?") == false) return false;
	this->BindStr(1,acc,10); this->BindInt(2,itemIndex); this->BindInt(3,itemLevel);
	if(this->Run() == false) return false;
	if(SQL_SUCCEEDED(SQLFetch(this->m_Stmt)) == 0) return false;
	DS_ReadStr(this->m_Stmt,1,out->AccountID,11); DS_ReadInt(this->m_Stmt,2,&out->ItemIndex);
	DS_ReadInt(this->m_Stmt,3,&out->ItemLevel); DS_ReadInt(this->m_Stmt,4,&out->ItemCount); DS_ReadInt(this->m_Stmt,5,&out->AutoPick);
	return true;
}

bool CDataStore::DeleteCustomItemBank(char* acc,int itemIndex,int itemLevel) // OK
{
	if(this->Prepare("DELETE FROM CustomItemBank WHERE AccountID = ? AND ItemIndex = ? AND ItemLevel = ?") == false) return false;
	this->BindStr(1,acc,10); this->BindInt(2,itemIndex); this->BindInt(3,itemLevel);
	return this->Run();
}

//---------------------------------------------------------------- 3 CustomNpcQuest
// 'Count' SQL anahtar kelimesidir; koseli parantezle verilir.
bool CDataStore::InsertCustomNpcQuest(DS_CustomNpcQuest* r) // OK
{
	if(this->Prepare("INSERT INTO CustomNpcQuest (Name,Quest,[Count],MonsterCount) VALUES (?,?,?,?)") == false) return false;
	this->BindStr(1,r->Name,10); this->BindInt(2,r->Quest); this->BindInt(3,r->Count); this->BindInt(4,r->MonsterCount);
	return this->Run();
}

bool CDataStore::AddCustomNpcQuestCount(char* name,int quest,int monsterCount) // OK
{
	if(this->Prepare("UPDATE CustomNpcQuest SET [Count] = [Count] + 1, MonsterCount = ? WHERE Name = ? AND Quest = ?") == false) return false;
	this->BindInt(1,monsterCount); this->BindStr(2,name,10); this->BindInt(3,quest);
	return this->Run();
}

bool CDataStore::UpdateCustomNpcQuestMonsterCount(char* name,int quest,int monsterCount) // OK
{
	if(this->Prepare("UPDATE CustomNpcQuest SET MonsterCount = ? WHERE Name = ? AND Quest = ?") == false) return false;
	this->BindInt(1,monsterCount); this->BindStr(2,name,10); this->BindInt(3,quest);
	return this->Run();
}

bool CDataStore::DeleteCustomNpcQuest(char* name,int quest) // OK
{
	if(this->Prepare("DELETE FROM CustomNpcQuest WHERE Name = ? AND Quest = ?") == false) return false;
	this->BindStr(1,name,10); this->BindInt(2,quest);
	return this->Run();
}

//---------------------------------------------------------------- 4 DataNapGame
// DIKKAT: DataNapGame tablosunda STT kolonu YOK. Kaynak koddaki eski
// "... and STT=%d" kosulu calisma aninda Msg 207 (Invalid column name)
// veriyordu; burada satir (Account,Name,Checking) ile tanimlanir.
bool CDataStore::InsertDataNapGame(DS_DataNapGame* r) // OK
{
	if(this->Prepare("INSERT INTO DataNapGame (Account,Name,TienNap,Checking) VALUES (?,?,?,?)") == false) return false;
	this->BindStr(1,r->Account,10); this->BindStr(2,r->Name,10); this->BindInt(3,r->TienNap); this->BindInt(4,r->Checking);
	return this->Run();
}

int CDataStore::LoadDataNapGamePending(DS_DataNapGame* out,int maxCount) // OK
{
	if(this->Prepare("SELECT TOP (?) Account,Name,TienNap,Checking,Status FROM DataNapGame WHERE Status = 0") == false) return -1;
	this->BindInt(1,maxCount);
	if(this->Run() == false) return -1;
	int count = 0;
	while(count < maxCount && SQL_SUCCEEDED(SQLFetch(this->m_Stmt))){ DS_FillNapRow(this->m_Stmt,&out[count]); count++; }
	return count;
}

int CDataStore::LoadDataNapGameByAccount(char* acc,DS_DataNapGame* out,int maxCount) // OK
{
	if(this->Prepare("SELECT TOP (?) Account,Name,TienNap,Checking,Status FROM DataNapGame WHERE Account = ? ORDER BY Checking DESC") == false) return -1;
	this->BindInt(1,maxCount); this->BindStr(2,acc,10);
	if(this->Run() == false) return -1;
	int count = 0;
	while(count < maxCount && SQL_SUCCEEDED(SQLFetch(this->m_Stmt))){ DS_FillNapRow(this->m_Stmt,&out[count]); count++; }
	return count;
}

bool CDataStore::UpdateDataNapGameDone(char* acc,char* name,int checking,int status) // OK
{
	if(this->Prepare("UPDATE DataNapGame SET Status = ? WHERE Account = ? AND Name = ? AND Checking = ?") == false) return false;
	this->BindInt(1,status); this->BindStr(2,acc,10); this->BindStr(3,name,10); this->BindInt(4,checking);
	return this->Run();
}

bool CDataStore::UpdateDataNapGameNap(char* acc,char* name,int checking,int tienNap,int status) // OK
{
	if(this->Prepare("UPDATE DataNapGame SET TienNap = ?, Status = ? WHERE Account = ? AND Name = ? AND Checking = ?") == false) return false;
	this->BindInt(1,tienNap); this->BindInt(2,status); this->BindStr(3,acc,10); this->BindStr(4,name,10); this->BindInt(5,checking);
	return this->Run();
}

//---------------------------------------------------------------- 5/6/7 envanter blob
bool CDataStore::InsertEquipInventory(char* charName,BYTE* items,int len) // OK
{
	if(len < 0 || len > 256){len = 0;}
	if(this->Prepare("INSERT INTO EquipInventory (CharName,Items) VALUES (?,?)") == false) return false;
	this->BindStr(1,charName,10); this->BindBlob(2,items,len);
	return this->Run();
}

bool CDataStore::LoadEquipInventory(char* charName,BYTE* items,int maxLen,int* outLen) // OK
{
	if(this->Prepare("SELECT Items FROM EquipInventory WHERE CharName = ?") == false) return false;
	this->BindStr(1,charName,10);
	if(this->Run() == false) return false;
	if(SQL_SUCCEEDED(SQLFetch(this->m_Stmt)) == 0){*outLen = 0;return false;}
	return DS_ReadBlob(this->m_Stmt,1,items,maxLen,outLen);
}

bool CDataStore::UpdateEquipInventory(char* charName,BYTE* items,int len) // OK
{
	if(len < 0 || len > 256){len = 0;}
	if(this->Prepare("UPDATE EquipInventory SET Items = ? WHERE CharName = ?") == false) return false;
	this->BindBlob(1,items,len); this->BindStr(2,charName,10);
	return this->Run();
}

bool CDataStore::InsertEventInventory(char* name,BYTE* items,int len) // OK
{
	if(len < 0 || len > 512){len = 0;}
	if(this->Prepare("INSERT INTO EventInventory (Name,Items) VALUES (?,?)") == false) return false;
	this->BindStr(1,name,10); this->BindBlob(2,items,len);
	return this->Run();
}

bool CDataStore::LoadEventInventory(char* name,BYTE* items,int maxLen,int* outLen) // OK
{
	if(this->Prepare("SELECT Items FROM EventInventory WHERE Name = ?") == false) return false;
	this->BindStr(1,name,10);
	if(this->Run() == false) return false;
	if(SQL_SUCCEEDED(SQLFetch(this->m_Stmt)) == 0){*outLen = 0;return false;}
	return DS_ReadBlob(this->m_Stmt,1,items,maxLen,outLen);
}

bool CDataStore::UpdateEventInventory(char* name,BYTE* items,int len) // OK
{
	if(len < 0 || len > 512){len = 0;}
	if(this->Prepare("UPDATE EventInventory SET Items = ? WHERE Name = ?") == false) return false;
	this->BindBlob(1,items,len); this->BindStr(2,name,10);
	return this->Run();
}

bool CDataStore::InsertMuunInventory(char* name,BYTE* items,int len) // OK
{
	if(len < 0 || len > 992){len = 0;}
	if(this->Prepare("INSERT INTO MuunInventory (Name,Items) VALUES (?,?)") == false) return false;
	this->BindStr(1,name,10); this->BindBlob(2,items,len);
	return this->Run();
}

bool CDataStore::LoadMuunInventory(char* name,BYTE* items,int maxLen,int* outLen) // OK
{
	if(this->Prepare("SELECT Items FROM MuunInventory WHERE Name = ?") == false) return false;
	this->BindStr(1,name,10);
	if(this->Run() == false) return false;
	if(SQL_SUCCEEDED(SQLFetch(this->m_Stmt)) == 0){*outLen = 0;return false;}
	return DS_ReadBlob(this->m_Stmt,1,items,maxLen,outLen);
}

bool CDataStore::UpdateMuunInventory(char* name,BYTE* items,int len) // OK
{
	if(len < 0 || len > 992){len = 0;}
	if(this->Prepare("UPDATE MuunInventory SET Items = ? WHERE Name = ?") == false) return false;
	this->BindBlob(1,items,len); this->BindStr(2,name,10);
	return this->Run();
}

bool CDataStore::DeleteEquipInventory(char* charName) // OK
{
	if(this->Prepare("DELETE FROM EquipInventory WHERE CharName = ?") == false) return false;
	this->BindStr(1,charName,10);
	return this->Run();
}

bool CDataStore::DeleteEventInventory(char* name) // OK
{
	if(this->Prepare("DELETE FROM EventInventory WHERE Name = ?") == false) return false;
	this->BindStr(1,name,10);
	return this->Run();
}

bool CDataStore::DeleteMuunInventory(char* name) // OK
{
	if(this->Prepare("DELETE FROM MuunInventory WHERE Name = ?") == false) return false;
	this->BindStr(1,name,10);
	return this->Run();
}

//---------------------------------------------------------------- 8 MuRummyCard
bool CDataStore::InsertMuRummyCard(DS_MuRummyCard* r) // OK
{
	if(this->Prepare("INSERT INTO MuRummyCard (Name,Color,Number,Slot,Status,Sequence) VALUES (?,?,?,?,?,?)") == false) return false;
	this->BindStr(1,r->Name,10); this->BindInt(2,r->Color); this->BindInt(3,r->Number);
	this->BindInt(4,r->Slot); this->BindInt(5,r->Status); this->BindInt(6,r->Sequence);
	return this->Run();
}

bool CDataStore::UpdateMuRummyCardSlot(DS_MuRummyCard* r) // OK
{
	if(this->Prepare("UPDATE MuRummyCard SET Slot = ?, Status = ? WHERE Name = ? AND Sequence = ?") == false) return false;
	this->BindInt(1,r->Slot); this->BindInt(2,r->Status); this->BindStr(3,r->Name,10); this->BindInt(4,r->Sequence);
	return this->Run();
}

//---------------------------------------------------------------- 9 MuRummyData
bool CDataStore::InsertMuRummyData(DS_MuRummyData* r) // OK
{
	if(this->Prepare("INSERT INTO MuRummyData (Name,TotalScore) VALUES (?,?)") == false) return false;
	this->BindStr(1,r->Name,10); this->BindInt(2,r->TotalScore);
	return this->Run();
}

bool CDataStore::UpdateMuRummyData(DS_MuRummyData* r) // OK
{
	if(this->Prepare("UPDATE MuRummyData SET TotalScore = ? WHERE Name = ?") == false) return false;
	this->BindInt(1,r->TotalScore); this->BindStr(2,r->Name,10);
	return this->Run();
}

bool CDataStore::LoadMuRummyData(char* name,DS_MuRummyData* out) // OK
{
	if(this->Prepare("SELECT Name,TotalScore FROM MuRummyData WHERE Name = ?") == false) return false;
	this->BindStr(1,name,10);
	if(this->Run() == false) return false;
	if(SQL_SUCCEEDED(SQLFetch(this->m_Stmt)) == 0) return false;
	DS_ReadStr(this->m_Stmt,1,out->Name,11); DS_ReadInt(this->m_Stmt,2,&out->TotalScore);
	return true;
}

//---------------------------------------------------------------- 10 PcPointData
bool CDataStore::InsertPcPointData(DS_PcPointData* r) // OK
{
	if(this->Prepare("INSERT INTO PcPointData (AccountID,PcPoint) VALUES (?,?)") == false) return false;
	this->BindStr(1,r->AccountID,10); this->BindInt(2,r->PcPoint);
	return this->Run();
}

bool CDataStore::UpdatePcPointData(DS_PcPointData* r) // OK
{
	if(this->Prepare("UPDATE PcPointData SET PcPoint = ? WHERE AccountID = ?") == false) return false;
	this->BindInt(1,r->PcPoint); this->BindStr(2,r->AccountID,10);
	return this->Run();
}

bool CDataStore::LoadPcPointData(char* acc,DS_PcPointData* out) // OK
{
	if(this->Prepare("SELECT AccountID,PcPoint FROM PcPointData WHERE AccountID = ?") == false) return false;
	this->BindStr(1,acc,10);
	if(this->Run() == false) return false;
	if(SQL_SUCCEEDED(SQLFetch(this->m_Stmt)) == 0) return false;
	DS_ReadStr(this->m_Stmt,1,out->AccountID,11); DS_ReadInt(this->m_Stmt,2,&out->PcPoint);
	return true;
}

bool CDataStore::DeleteMuRummyData(char* name) // OK
{
	if(this->Prepare("DELETE FROM MuRummyData WHERE Name = ?") == false) return false;
	this->BindStr(1,name,10);
	return this->Run();
}

bool CDataStore::DeletePcPointData(char* acc) // OK
{
	if(this->Prepare("DELETE FROM PcPointData WHERE AccountID = ?") == false) return false;
	this->BindStr(1,acc,10);
	return this->Run();
}

//---------------------------------------------------------------- 11 PentagramJewel
bool CDataStore::InsertPentagramJewel(DS_PentagramJewel* r) // OK
{
	if(this->Prepare("INSERT INTO PentagramJewel (Name,Type,[Index],Attribute,ItemSection,ItemType,ItemLevel,OptionIndexRank1,OptionLevelRank1,OptionIndexRank2,OptionLevelRank2,OptionIndexRank3,OptionLevelRank3,OptionIndexRank4,OptionLevelRank4,OptionIndexRank5,OptionLevelRank5) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)") == false) return false;
	this->BindStr(1,r->Name,10); this->BindInt(2,r->Type); this->BindInt(3,r->Index);
	this->BindInt(4,r->Attribute); this->BindInt(5,r->ItemSection); this->BindInt(6,r->ItemType); this->BindInt(7,r->ItemLevel);
	this->BindInt(8,r->OptionIndexRank[0]); this->BindInt(9,r->OptionLevelRank[0]);
	this->BindInt(10,r->OptionIndexRank[1]); this->BindInt(11,r->OptionLevelRank[1]);
	this->BindInt(12,r->OptionIndexRank[2]); this->BindInt(13,r->OptionLevelRank[2]);
	this->BindInt(14,r->OptionIndexRank[3]); this->BindInt(15,r->OptionLevelRank[3]);
	this->BindInt(16,r->OptionIndexRank[4]); this->BindInt(17,r->OptionLevelRank[4]);
	return this->Run();
}

bool CDataStore::UpdatePentagramJewel(DS_PentagramJewel* r) // OK
{
	if(this->Prepare("UPDATE PentagramJewel SET Attribute=?,ItemSection=?,ItemType=?,ItemLevel=?,OptionIndexRank1=?,OptionLevelRank1=?,OptionIndexRank2=?,OptionLevelRank2=?,OptionIndexRank3=?,OptionLevelRank3=?,OptionIndexRank4=?,OptionLevelRank4=?,OptionIndexRank5=?,OptionLevelRank5=? WHERE Name=? AND Type=? AND [Index]=?") == false) return false;
	this->BindInt(1,r->Attribute); this->BindInt(2,r->ItemSection); this->BindInt(3,r->ItemType); this->BindInt(4,r->ItemLevel);
	this->BindInt(5,r->OptionIndexRank[0]); this->BindInt(6,r->OptionLevelRank[0]);
	this->BindInt(7,r->OptionIndexRank[1]); this->BindInt(8,r->OptionLevelRank[1]);
	this->BindInt(9,r->OptionIndexRank[2]); this->BindInt(10,r->OptionLevelRank[2]);
	this->BindInt(11,r->OptionIndexRank[3]); this->BindInt(12,r->OptionLevelRank[3]);
	this->BindInt(13,r->OptionIndexRank[4]); this->BindInt(14,r->OptionLevelRank[4]);
	this->BindStr(15,r->Name,10); this->BindInt(16,r->Type); this->BindInt(17,r->Index);
	return this->Run();
}

bool CDataStore::DeletePentagramJewel(char* name,int type,int index) // OK
{
	if(this->Prepare("DELETE FROM PentagramJewel WHERE Name = ? AND Type = ? AND [Index] = ?") == false) return false;
	this->BindStr(1,name,10); this->BindInt(2,type); this->BindInt(3,index);
	return this->Run();
}

//---------------------------------------------------------------- 12 PShopItemValue
bool CDataStore::InsertPShopItemValue(DS_PShopItemValue* r) // OK
{
	if(this->Prepare("INSERT INTO PShopItemValue (Name,Slot,Serial,Value,JobValue,JosValue,JocValue) VALUES (?,?,?,?,?,?,?)") == false) return false;
	this->BindStr(1,r->Name,10); this->BindInt(2,r->Slot); this->BindInt(3,r->Serial); this->BindInt(4,r->Value);
	this->BindInt(5,r->JobValue); this->BindInt(6,r->JosValue); this->BindInt(7,r->JocValue);
	return this->Run();
}

bool CDataStore::UpdatePShopItemValue(DS_PShopItemValue* r) // OK
{
	if(this->Prepare("UPDATE PShopItemValue SET Serial=?,Value=?,JobValue=?,JosValue=?,JocValue=? WHERE Name = ? AND Slot = ?") == false) return false;
	this->BindInt(1,r->Serial); this->BindInt(2,r->Value); this->BindInt(3,r->JobValue);
	this->BindInt(4,r->JosValue); this->BindInt(5,r->JocValue);
	this->BindStr(6,r->Name,10); this->BindInt(7,r->Slot);
	return this->Run();
}

bool CDataStore::DeletePShopItemValue(char* name,int slot) // OK
{
	if(this->Prepare("DELETE FROM PShopItemValue WHERE Name = ? AND Slot = ?") == false) return false;
	this->BindStr(1,name,10); this->BindInt(2,slot);
	return this->Run();
}

//---------------------------------------------------------------- 13 SNSData
bool CDataStore::InsertSNSData(char* name,BYTE* data,int len) // OK
{
	if(len < 0 || len > 4096){len = 0;}
	if(this->Prepare("INSERT INTO SNSData (Name,Data) VALUES (?,?)") == false) return false;
	this->BindStr(1,name,10); this->BindBlob(2,data,len);
	return this->Run();
}

bool CDataStore::LoadSNSData(char* name,BYTE* data,int maxLen,int* outLen) // OK
{
	if(this->Prepare("SELECT Data FROM SNSData WHERE Name = ?") == false) return false;
	this->BindStr(1,name,10);
	if(this->Run() == false) return false;
	if(SQL_SUCCEEDED(SQLFetch(this->m_Stmt)) == 0){*outLen = 0;return false;}
	return DS_ReadBlob(this->m_Stmt,1,data,maxLen,outLen);
}

bool CDataStore::UpdateSNSData(char* name,BYTE* data,int len) // OK
{
	if(len < 0 || len > 4096){len = 0;}
	if(this->Prepare("UPDATE SNSData SET Data = ? WHERE Name = ?") == false) return false;
	this->BindBlob(1,data,len); this->BindStr(2,name,10);
	return this->Run();
}

bool CDataStore::DeleteSNSData(char* name) // OK
{
	if(this->Prepare("DELETE FROM SNSData WHERE Name = ?") == false) return false;
	this->BindStr(1,name,10);
	return this->Run();
}

//---------------------------------------------------------------- 14 ItemMarketData
// ID kolonu IDENTITY; OUTPUT ile gercek ID geri alinir (sirada tutulan kayit
// kazanilabilir).
int CDataStore::InsertItemMarketData(DS_ItemMarketData* r) // OK
{
	if(this->Prepare("INSERT INTO ItemMarketData (Account,PriceType,PriceValue,Date,TypeItem,Name,Time,Pass) OUTPUT INSERTED.ID VALUES (?,?,?,?,?,?,?,?)") == false) return -1;
	this->BindStr(1,r->Account,10); this->BindInt(2,r->PriceType); this->BindInt(3,r->PriceValue);
	this->BindStr(4,r->Date,20); this->BindInt(5,r->TypeItem); this->BindStr(6,r->Name,10);
	this->BindInt(7,r->Time); this->BindInt(8,r->Pass);
	if(this->Run() == false) return -1;
	if(SQL_SUCCEEDED(SQLFetch(this->m_Stmt)) == 0) return -1;
	int id = 0;
	if(DS_ReadInt(this->m_Stmt,1,&id) == false) return -1;
	r->ID = id;
	return id;
}

bool CDataStore::UpdateItemMarketDataItem(int id,BYTE* item,int len) // OK
{
	if(len < 0 || len > 16){len = 0;}
	if(this->Prepare("UPDATE ItemMarketData SET Item = ? WHERE ID = ?") == false) return false;
	this->BindBlob(1,item,len); this->BindInt(2,id);
	return this->Run();
}

bool CDataStore::UpdateItemMarketDataStatus(int id,int status) // OK
{
	if(this->Prepare("UPDATE ItemMarketData SET Status = ? WHERE ID = ?") == false) return false;
	this->BindInt(1,status); this->BindInt(2,id);
	return this->Run();
}

bool CDataStore::DeleteItemMarketData(int id) // OK
{
	if(this->Prepare("DELETE FROM ItemMarketData WHERE ID = ?") == false) return false;
	this->BindInt(1,id);
	return this->Run();
}

bool CDataStore::DeleteItemMarketDataByAccount(char* acc) // OK
{
	if(this->Prepare("DELETE FROM ItemMarketData WHERE Account = ? AND Status = 1") == false) return false;
	this->BindStr(1,acc,10);
	return this->Run();
}

int CDataStore::CountItemMarketData(char* acc,int typeItem,int pass) // OK
{
	if(this->Prepare("SELECT COUNT(*) FROM ItemMarketData WHERE Account = ? AND TypeItem = ? AND Pass = ?") == false) return -1;
	this->BindStr(1,acc,10); this->BindInt(2,typeItem); this->BindInt(3,pass);
	if(this->Run() == false) return -1;
	if(SQL_SUCCEEDED(SQLFetch(this->m_Stmt)) == 0) return -1;
	int cnt = 0;
	DS_ReadInt(this->m_Stmt,1,&cnt);
	return cnt;
}
