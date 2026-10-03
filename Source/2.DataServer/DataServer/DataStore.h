// DataStore.h: MuOnlineS6 kalici veri katmani.
//
// 2e4'te (C-02) kaynak kodun gonderdigi ancak semada bulunmayan 14 tablo
// yaratildi. Bu sinif o 14 tablonun SEMAYA UYGUN, PARAMETRELI ve
// HAZIRLANMIS IFADE (prepared statement) tabanli erisimini tek yerde
// toplar.
//
// Neden ayri katman:
//  1) Semada olmayan kolon (DataNapGame.STT) bir kez yakalandi; string
//     birlestirmeyle yazilan sorgu hatasi derlemede yakalanmiyor.
//  2) Hesap/karakter adlari SQL'e dogrudan gomuluyordu ("WHERE acc='%s'").
//     Burada degerler SQLCEVT_DECIMAL/ SQL_C_CHAR parametre olarak gider.
//  3) Ondort tablo icin tek baglanti + tek hazirli ifade tamponu.
//////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#include <sql.h>
#include <sqlext.h>

#pragma comment(lib,"odbc32.lib")

#define DS_MAX_PARAM 32

//---------------------------------------------------------------- kayitlar
// Alan tipleri ve boyutlari MuOnlineS6 semasiyla birebir ayni.
// (sys.columns'tan cikarildi: BuildLog/2e9/schema_14.txt)

struct DS_CardPhone // acc PK (10) | name card_type card_num card_num_md5 card_serial | menhgia timenap addvpoint status stt timeduyet
{
	char acc[11];
	char name[11];
	char card_type[11];
	int menhgia;
	char card_num[21];
	char card_num_md5[51];
	char card_serial[21];
	int timenap;
	int addvpoint;
	int status;
	int stt;
	int timeduyet;
};

struct DS_CustomItemBank // PK(AccountID,ItemIndex,ItemLevel)
{
	char AccountID[11];
	int ItemIndex;
	int ItemLevel;
	int ItemCount;
	int AutoPick;
};

struct DS_CustomNpcQuest // PK(Name,Quest)
{
	char Name[11];
	int Quest;
	int Count;
	int MonsterCount;
};

struct DS_DataNapGame // Account PK(10) | Name TienNap Checking Status  -- STT YOK
{
	char Account[11];
	char Name[11];
	int TienNap;
	int Checking;
	int Status;
};

struct DS_EquipInventory // PK(CharName) + Items varbinary(256)
{
	char CharName[11];
	BYTE Items[256];
	int ItemsLen;
};

struct DS_EventInventory // PK(Name) + Items varbinary(512)
{
	char Name[11];
	BYTE Items[512];
	int ItemsLen;
};

struct DS_MuunInventory // PK(Name) + Items varbinary(992)
{
	char Name[11];
	BYTE Items[992];
	int ItemsLen;
};

struct DS_MuRummyCard // PK(Name,Color,Number,Slot,Status,Sequence)
{
	char Name[11];
	int Color;
	int Number;
	int Slot;
	int Status;
	int Sequence;
};

struct DS_MuRummyData // PK(Name) + TotalScore
{
	char Name[11];
	int TotalScore;
};

struct DS_PcPointData // PK(AccountID) + PcPoint
{
	char AccountID[11];
	int PcPoint;
};

struct DS_PentagramJewel // PK(Name,Type,Index) + ...
{
	char Name[11];
	int Type;
	int Index;
	int Attribute;
	int ItemSection;
	int ItemType;
	int ItemLevel;
	int OptionIndexRank[5];
	int OptionLevelRank[5];
};

struct DS_PShopItemValue // PK(Name,Slot) + Serial Value JobValue JosValue JocValue
{
	char Name[11];
	int Slot;
	int Serial;
	int Value;
	int JobValue;
	int JosValue;
	int JocValue;
};

struct DS_SNSData // PK(Name) + Data varbinary(MAX)
{
	char Name[11];
	BYTE Data[4096];
	int DataLen;
};

struct DS_ItemMarketData // PK(ID) identity + ...
{
	int ID;
	char Account[11];
	char Name[11];
	int PriceType;
	int PriceValue;
	int Status;
	int FilterType;
	int FilterLevel;
	int FilterLuck;
	int FilterExl;
	int FilterAnc;
	char Date[21];
	BYTE Item[16];
	int ItemLen;
	int TypeItem;
	int Time;
	int Pass;
};

//---------------------------------------------------------------- katman
class CDataStore
{
public:
	CDataStore();
	virtual ~CDataStore();

	bool Open(char* odbc,char* user,char* pass); // OK
	void Close(); // OK
	bool IsOpen(); // OK

	// 14 tablonun tamami semada var mi? -> eksik sayisini doner, raporu doldurur.
	//   (0 = hepsi yerinde; acilista cagrilir, sunucu bozuk semada calismaz)
	int SchemaCheck(char* report,int reportSize); // OK

	// --- 1 CardPhone
	bool InsertCardPhone(DS_CardPhone* r); // OK
	bool LoadCardPhoneByAcc(char* acc,DS_CardPhone* out); // OK
	bool UpdateCardPhoneStatus(char* acc,int status,int timeduyet); // OK
	bool UpdateCardPhoneStatusByStt(int stt,int status,int timeduyet); // OK
	bool UpdateCardPhoneNapByStt(int stt,int menhgia); // OK
	bool UpdateCardPhoneAddVPointByStt(int stt,int addvpoint,int timeduyet); // OK
	bool DeleteCardPhone(char* acc); // OK

	// --- 2 CustomItemBank
	bool InsertCustomItemBank(DS_CustomItemBank* r); // OK
	bool UpdateCustomItemBank(DS_CustomItemBank* r); // OK
	bool LoadCustomItemBank(char* acc,int itemIndex,int itemLevel,DS_CustomItemBank* out); // OK
	bool DeleteCustomItemBank(char* acc,int itemIndex,int itemLevel); // OK

	// --- 3 CustomNpcQuest
	bool InsertCustomNpcQuest(DS_CustomNpcQuest* r); // OK
	bool AddCustomNpcQuestCount(char* name,int quest,int monsterCount); // OK
	bool UpdateCustomNpcQuestMonsterCount(char* name,int quest,int monsterCount); // OK
	bool DeleteCustomNpcQuest(char* name,int quest); // OK

	// --- 4 DataNapGame  (STT kolonu YOK - bkz. SchemaCheck)
	bool InsertDataNapGame(DS_DataNapGame* r); // OK
	int  LoadDataNapGamePending(DS_DataNapGame* out,int maxCount); // OK -> satir sayisi
	int  LoadDataNapGameByAccount(char* acc,DS_DataNapGame* out,int maxCount); // OK
	bool UpdateDataNapGameDone(char* acc,char* name,int checking,int status); // OK
	bool UpdateDataNapGameNap(char* acc,char* name,int checking,int tienNap,int status); // OK

	// --- 5/6/7 envanter bloblari
	bool InsertEquipInventory(char* charName,BYTE* items,int len); // OK
	bool LoadEquipInventory(char* charName,BYTE* items,int maxLen,int* outLen); // OK
	bool UpdateEquipInventory(char* charName,BYTE* items,int len); // OK
	bool DeleteEquipInventory(char* charName); // OK
	bool InsertEventInventory(char* name,BYTE* items,int len); // OK
	bool LoadEventInventory(char* name,BYTE* items,int maxLen,int* outLen); // OK
	bool UpdateEventInventory(char* name,BYTE* items,int len); // OK
	bool DeleteEventInventory(char* name); // OK
	bool InsertMuunInventory(char* name,BYTE* items,int len); // OK
	bool LoadMuunInventory(char* name,BYTE* items,int maxLen,int* outLen); // OK
	bool UpdateMuunInventory(char* name,BYTE* items,int len); // OK
	bool DeleteMuunInventory(char* name); // OK

	// --- 8 MuRummyCard
	bool InsertMuRummyCard(DS_MuRummyCard* r); // OK
	bool UpdateMuRummyCardSlot(DS_MuRummyCard* r); // OK

	// --- 9 MuRummyData
	bool InsertMuRummyData(DS_MuRummyData* r); // OK
	bool UpdateMuRummyData(DS_MuRummyData* r); // OK
	bool LoadMuRummyData(char* name,DS_MuRummyData* out); // OK
	bool DeleteMuRummyData(char* name); // OK

	// --- 10 PcPointData
	bool InsertPcPointData(DS_PcPointData* r); // OK
	bool UpdatePcPointData(DS_PcPointData* r); // OK
	bool LoadPcPointData(char* acc,DS_PcPointData* out); // OK
	bool DeletePcPointData(char* acc); // OK

	// --- 11 PentagramJewel
	bool InsertPentagramJewel(DS_PentagramJewel* r); // OK
	bool UpdatePentagramJewel(DS_PentagramJewel* r); // OK
	bool DeletePentagramJewel(char* name,int type,int index); // OK

	// --- 12 PShopItemValue
	bool InsertPShopItemValue(DS_PShopItemValue* r); // OK
	bool UpdatePShopItemValue(DS_PShopItemValue* r); // OK
	bool DeletePShopItemValue(char* name,int slot); // OK

	// --- 13 SNSData
	bool InsertSNSData(char* name,BYTE* data,int len); // OK
	bool LoadSNSData(char* name,BYTE* data,int maxLen,int* outLen); // OK
	bool UpdateSNSData(char* name,BYTE* data,int len); // OK
	bool DeleteSNSData(char* name); // OK

	// --- 14 ItemMarketData
	int  InsertItemMarketData(DS_ItemMarketData* r); // OK -> IDENTITY ID doner, <0 hata
	bool UpdateItemMarketDataItem(int id,BYTE* item,int len); // OK
	bool UpdateItemMarketDataStatus(int id,int status); // OK
	bool DeleteItemMarketData(int id); // OK
	bool DeleteItemMarketDataByAccount(char* acc); // OK -> silinen satir
	int  CountItemMarketData(char* acc,int typeItem,int pass); // OK

	// son ODBC hatasi (loglanabilir)
	char* LastError(); // OK

private:
	bool Prepare(char* sql); // OK
	bool Run(); // OK
	void BindStr(int idx,void* buf,int colSize); // OK
	void BindInt(int idx,int val); // OK
	void BindBlob(int idx,void* buf,int len); // OK
	void LogError(char* sql); // OK

	SQLHANDLE m_Env;
	SQLHANDLE m_Dbc;
	SQLHANDLE m_Stmt;
	SQLINTEGER m_ParamLen[DS_MAX_PARAM];
	int m_ParamInt[DS_MAX_PARAM]; // BindInt degerleri yasam alani (asagida)
	SQLLEN m_ParamBlobLen[DS_MAX_PARAM];
	char m_Error[512];
};

extern CDataStore gDataStore;
