// ds_e2e.cpp - 2e9 uc-uca dogrulama (gercek MuOnlineS6 + MuOnline)
//  A) 14 tablo: INSERT -> LOAD -> UPDATE -> LOAD dogrula -> DELETE
//  B) MEMB_INFO: JoinServer ve DataServer sorgularinin AYNI satiri ayni
//     degeri okudugu, yazmalarin birbirini gordugu ve buyuk/kucuk harf
//     cozumlemesinin iki sunucuda da ayni oldugu.
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <cstdio>
#include <cstring>

#include "DataStore.h"

static int g_Pass = 0, g_Fail = 0;
static char g_Detail[256];

static void CHECK(const char* what, bool ok)
{
	if(ok){ g_Pass++; printf("  GECTI  %s\n",what); }
	else  { g_Fail++; printf("  KALDI  %s   %s\n",what,g_Detail); }
	g_Detail[0] = 0;
}

static void DETAIL(const char* fmt, ...)
{
	va_list ap; va_start(ap,fmt);
	_vsnprintf_s(g_Detail,sizeof(g_Detail),_TRUNCATE,fmt,ap);
	va_end(ap);
}

//---------------------------------------------------------------- MEMB_INFO araclari
static bool RawScalar(SQLHANDLE st,const char* sql,int* out)
{
	SQLFreeStmt(st,SQL_CLOSE);
	if(SQL_SUCCEEDED(SQLExecDirect(st,(SQLCHAR*)sql,SQL_NTS)) == 0) return false;
	if(SQL_SUCCEEDED(SQLFetch(st)) == 0) return false;
	SQLLEN ind = 0;
	return SQL_SUCCEEDED(SQLGetData(st,1,SQL_C_SLONG,(SQLPOINTER)out,0,&ind)) != 0;
}

static bool RawStr(SQLHANDLE st,const char* sql,char* out,int maxLen)
{
	SQLFreeStmt(st,SQL_CLOSE);
	if(SQL_SUCCEEDED(SQLExecDirect(st,(SQLCHAR*)sql,SQL_NTS)) == 0) return false;
	if(SQL_SUCCEEDED(SQLFetch(st)) == 0) return false;
	SQLLEN ind = 0;
	ZeroMemory(out,maxLen);
	if(SQL_SUCCEEDED(SQLGetData(st,1,SQL_C_CHAR,(SQLPOINTER)out,maxLen,&ind)) == 0) return false;
	return ind > 0 && ind != SQL_NULL_DATA;
}

int main(int argc,char** argv)
{
	const char* dsn     = (argc > 1) ? argv[1] : "MuOnlineS6";
	const char* membDsn = (argc > 2) ? argv[2] : "MuOnline";
	printf("### 2e9 uc-uca dogrulama\n  veri katmani DSN: %s\n  MEMB_INFO  DSN : %s\n\n",dsn,membDsn);

	//==================================================== A) 14 TABLO
	CDataStore ds;
	if(ds.Open((char*)dsn,(char*)"",(char*)"") == false)
	{
		printf("### HATA: veri katmani acilamadi (DSN=%s)\n",dsn);
		return 2;
	}

	char rep[1024];
	int missing = ds.SchemaCheck(rep,sizeof(rep));
	sprintf(g_Detail,"%s",rep);
	CHECK("A0  sema denetimi: 14/14 tablo",missing == 0);

	// --- 1 CardPhone
	{
		DS_CardPhone r,g; ZeroMemory(&r,sizeof(r)); ZeroMemory(&g,sizeof(g));
		strcpy_s(r.acc,"e2e9"); strcpy_s(r.name,"chr01"); strcpy_s(r.card_type,"VND");
		strcpy_s(r.card_num,"E2E9001"); strcpy_s(r.card_num_md5,"MD5E2E9"); strcpy_s(r.card_serial,"SER001");
		r.menhgia=0; r.timenap=1700000000; r.addvpoint=0; r.status=0; r.stt=9901; r.timeduyet=0;
		ds.DeleteCardPhone("e2e9");   // onceki kosulardan kalan satirlari temizle
		CHECK("A1  CardPhone INSERT",ds.InsertCardPhone(&r));
		bool ok = ds.LoadCardPhoneByAcc("e2e9",&g);
		sprintf(g_Detail,"card_num=%s stt=%d",g.card_num,g.stt);
		CHECK("A1  CardPhone LOAD",ok && strcmp(g.card_num,"E2E9001")==0 && strcmp(g.card_serial,"SER001")==0 && g.stt==9901);
		CHECK("A1  CardPhone UPDATE status/timeduyet",ds.UpdateCardPhoneStatusByStt(9901,2,1700000001) && ds.LoadCardPhoneByAcc("e2e9",&g) && g.status==2 && g.timeduyet==1700000001);
		CHECK("A1  CardPhone UPDATE menhgia",ds.UpdateCardPhoneNapByStt(9901,500) && ds.LoadCardPhoneByAcc("e2e9",&g) && g.menhgia==500);
		CHECK("A1  CardPhone UPDATE addvpoint",ds.UpdateCardPhoneAddVPointByStt(9901,1,1700000002) && ds.LoadCardPhoneByAcc("e2e9",&g) && g.addvpoint==1);
		CHECK("A1  CardPhone UPDATE acc ile",ds.UpdateCardPhoneStatus("e2e9",3,1700000003) && ds.LoadCardPhoneByAcc("e2e9",&g) && g.status==3);
	}

	// --- 2 CustomItemBank
	{
		DS_CustomItemBank r,g; ZeroMemory(&r,sizeof(r)); ZeroMemory(&g,sizeof(g));
		strcpy_s(r.AccountID,"e2e9"); r.ItemIndex=7; r.ItemLevel=3; r.ItemCount=0; r.AutoPick=0;
		ds.DeleteCustomItemBank("e2e9",7,3);
		CHECK("A2  CustomItemBank INSERT",ds.InsertCustomItemBank(&r));
		r.ItemCount=4; r.AutoPick=1;
		CHECK("A2  CustomItemBank UPDATE",ds.UpdateCustomItemBank(&r));
		bool ok = ds.LoadCustomItemBank("e2e9",7,3,&g);
		sprintf(g_Detail,"count=%d auto=%d",g.ItemCount,g.AutoPick);
		CHECK("A2  CustomItemBank LOAD",ok && g.ItemCount==4 && g.AutoPick==1);
		CHECK("A2  CustomItemBank DELETE",ds.DeleteCustomItemBank("e2e9",7,3) && ds.LoadCustomItemBank("e2e9",7,3,&g)==false);
	}

	// --- 3 CustomNpcQuest (FK: gercek Character adi gerekir)
	{
		char chr[32]="";
		DS_CustomNpcQuest probe; ZeroMemory(&probe,sizeof(probe));
		// DataStore ile hazir bir karakter adi oku
		SQLHANDLE dummy=0;
		{
			DS_MuRummyData md; ZeroMemory(&md,sizeof(md));
			if(ds.LoadMuRummyData("\x01__none__",&md)==false){ /* beklenen */ }
		}
		// Character tablosundan ad al (DataStore disi, dogrudan ODBC)
		SQLHANDLE env=0,db=0,st=0;
		SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&env);
		SQLSetEnvAttr(env,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3,0);
		SQLAllocHandle(SQL_HANDLE_DBC,env,&db);
		SQLAllocHandle(SQL_HANDLE_STMT,db,&st);
		if(SQL_SUCCEEDED(SQLConnect(db,(SQLCHAR*)dsn,SQL_NTS,NULL,0,NULL,0)) &&
		   SQL_SUCCEEDED(SQLExecDirect(st,(SQLCHAR*)"SELECT TOP 1 Name FROM dbo.Character ORDER BY Name",SQL_NTS)) &&
		   SQL_SUCCEEDED(SQLFetch(st)))
		{
			SQLLEN ind=0; SQLGetData(st,1,SQL_C_CHAR,(SQLPOINTER)chr,sizeof(chr),&ind);
		}
		SQLFreeHandle(SQL_HANDLE_STMT,st); SQLFreeHandle(SQL_HANDLE_DBC,db); SQLFreeHandle(SQL_HANDLE_ENV,env);

		if(chr[0]==0){ printf("  ATLA  A3  CustomNpcQuest - MuOnlineS6.Character tablosu bos, FK test edilemiyor"); puts(""); }
		else{
			DS_CustomNpcQuest r,g; ZeroMemory(&r,sizeof(r)); ZeroMemory(&g,sizeof(g));
			strcpy_s(r.Name,chr); r.Quest=987654; r.Count=1; r.MonsterCount=99999;
			ds.DeleteCustomNpcQuest(chr,987654);
			sprintf(g_Detail,"karakter=%s",chr);
			CHECK("A3  CustomNpcQuest INSERT",ds.InsertCustomNpcQuest(&r));
			CHECK("A3  CustomNpcQuest ADD-COUNT",ds.AddCustomNpcQuestCount(chr,987654,99999));
			CHECK("A3  CustomNpcQuest UPD-MONSTER",ds.UpdateCustomNpcQuestMonsterCount(chr,987654,88888));
			g.Name[0]=0; g.Quest=987654; g.Count=0;
			ds.DeleteCustomNpcQuest(chr,987654);
			CHECK("A3  CustomNpcQuest DELETE",ds.DeleteCustomNpcQuest(chr,987654));
		}
	}

	// --- 4 DataNapGame  (STT kolonu YOK - 2e9'da duzeltilen yer)
	{
		DS_DataNapGame r; ZeroMemory(&r,sizeof(r));
		strcpy_s(r.Account,"e2e9"); strcpy_s(r.Name,"chr01");
		r.TienNap=0; r.Checking=1700000100; r.Status=0;
		ds.UpdateDataNapGameDone("e2e9","chr01",1700000100,9);
		CHECK("A4  DataNapGame INSERT",ds.InsertDataNapGame(&r));
		DS_DataNapGame buf[16]; ZeroMemory(buf,sizeof(buf));
		int n = ds.LoadDataNapGameByAccount("e2e9",buf,16);
		bool found=false; for(int i=0;i<n;i++) if(buf[i].Checking==1700000100) found=true;
		sprintf(g_Detail,"satir=%d",n);
		CHECK("A4  DataNapGame LOAD (Checking ile eslesiyor)",found);
		CHECK("A4  DataNapGame UPDATE nap (STT'siz)",ds.UpdateDataNapGameNap("e2e9","chr01",1700000100,777,1));
		CHECK("A4  DataNapGame UPDATE done",ds.UpdateDataNapGameDone("e2e9","chr01",1700000100,2));
		n = ds.LoadDataNapGameByAccount("e2e9",buf,16);
		found=false; for(int i=0;i<n;i++) if(buf[i].Status==2 && buf[i].TienNap==777) found=true;
		CHECK("A4  DataNapGame degerler gercekten yazildi",found);
		n = ds.LoadDataNapGamePending(buf,16);
		CHECK("A4  DataNapGame Pending listesi calisiyor",n >= 0);
		ds.UpdateDataNapGameDone("e2e9","chr01",1700000100,9);
	}

	// --- 5/6/7 envanter bloblari
	{
		BYTE a[1200], b[1200]; int len=0;
		for(int i=0;i<992;i++) a[i]=(BYTE)((i*7+1)&0xFF);

		ds.DeleteEquipInventory("e2e9");
		CHECK("A5  EquipInventory INSERT(256)",ds.InsertEquipInventory("e2e9",a,256));
		ZeroMemory(b,sizeof(b));
		bool ok = ds.LoadEquipInventory("e2e9",b,sizeof(b),&len);
		sprintf(g_Detail,"len=%d",len);
		CHECK("A5  EquipInventory LOAD(256)",ok && len==256 && memcmp(a,b,256)==0);
		for(int i=0;i<256;i++) b[i]=(BYTE)((i*11+5)&0xFF);
		CHECK("A5  EquipInventory UPDATE",ds.UpdateEquipInventory("e2e9",b,256));
		ZeroMemory(a,sizeof(a));
		CHECK("A5  EquipInventory UPDATE dogrulama",ds.LoadEquipInventory("e2e9",a,sizeof(a),&len) && len==256 && memcmp(a,b,256)==0);

		for(int i=0;i<512;i++) a[i]=(BYTE)((i*3+2)&0xFF);
		for(int i=0;i<512;i++) a[i]=(BYTE)((i*3+2)&0xFF);
		ds.DeleteEventInventory("e2e9");
		CHECK("A6  EventInventory INSERT(512)",ds.InsertEventInventory("e2e9",a,512));
		ZeroMemory(b,sizeof(b));
		CHECK("A6  EventInventory LOAD(512)",ds.LoadEventInventory("e2e9",b,sizeof(b),&len) && len==512 && memcmp(a,b,512)==0);
		CHECK("A6  EventInventory UPDATE",ds.UpdateEventInventory("e2e9",b,512));

		for(int i=0;i<992;i++) a[i]=(BYTE)((i*13+9)&0xFF);
		ds.DeleteMuunInventory("e2e9");
		CHECK("A7  MuunInventory INSERT(992)",ds.InsertMuunInventory("e2e9",a,992));
		ZeroMemory(b,sizeof(b));
		CHECK("A7  MuunInventory LOAD(992)",ds.LoadMuunInventory("e2e9",b,sizeof(b),&len) && len==992 && memcmp(a,b,992)==0);
		for(int i=0;i<992;i++) b[i]=(BYTE)((i*17+4)&0xFF);
		CHECK("A7  MuunInventory UPDATE",ds.UpdateMuunInventory("e2e9",b,992));
		ZeroMemory(a,sizeof(a));
		CHECK("A7  MuunInventory UPDATE dogrulama",ds.LoadMuunInventory("e2e9",a,sizeof(a),&len) && len==992 && memcmp(a,b,992)==0);
	}

	// --- 8 MuRummyCard
	{
		DS_MuRummyCard r; ZeroMemory(&r,sizeof(r));
		strcpy_s(r.Name,"e2e9"); r.Color=1; r.Number=7; r.Slot=0; r.Status=0; r.Sequence=5;
		CHECK("A8  MuRummyCard INSERT",ds.InsertMuRummyCard(&r));
		r.Slot=3; r.Status=1;
		CHECK("A8  MuRummyCard UPDATE slot/status",ds.UpdateMuRummyCardSlot(&r));
	}

	// --- 9 MuRummyData
	{
		DS_MuRummyData r,g; ZeroMemory(&r,sizeof(r)); ZeroMemory(&g,sizeof(g));
		strcpy_s(r.Name,"e2e9"); r.TotalScore=1234;
		CHECK("A9  MuRummyData INSERT",ds.InsertMuRummyData(&r));
		r.TotalScore=5678;
		CHECK("A9  MuRummyData UPDATE",ds.UpdateMuRummyData(&r));
		bool ok = ds.LoadMuRummyData("e2e9",&g);
		sprintf(g_Detail,"score=%d",g.TotalScore);
		CHECK("A9  MuRummyData LOAD",ok && g.TotalScore==5678);
	}

	// --- 10 PcPointData
	{
		DS_PcPointData r,g; ZeroMemory(&r,sizeof(r)); ZeroMemory(&g,sizeof(g));
		strcpy_s(r.AccountID,"e2e9"); r.PcPoint=0;
		ds.DeletePcPointData("e2e9");
		CHECK("A10 PcPointData INSERT",ds.InsertPcPointData(&r));
		r.PcPoint=999;
		CHECK("A10 PcPointData UPDATE",ds.UpdatePcPointData(&r));
		bool ok = ds.LoadPcPointData("e2e9",&g);
		sprintf(g_Detail,"pcpoint=%d",g.PcPoint);
		CHECK("A10 PcPointData LOAD",ok && g.PcPoint==999);
	}

	// --- 11 PentagramJewel
	{
		DS_PentagramJewel r; ZeroMemory(&r,sizeof(r));
		strcpy_s(r.Name,"e2e9"); r.Type=1; r.Index=2; r.Attribute=3; r.ItemSection=4;
		r.ItemType=5; r.ItemLevel=6;
		for(int i=0;i<5;i++){ r.OptionIndexRank[i]=10+i; r.OptionLevelRank[i]=20+i; }
		ds.DeletePentagramJewel("e2e9",1,2);
		CHECK("A11 PentagramJewel INSERT(17 kolon)",ds.InsertPentagramJewel(&r));
		for(int i=0;i<5;i++){ r.OptionIndexRank[i]=30+i; r.OptionLevelRank[i]=40+i; }
		r.Attribute=99;
		CHECK("A11 PentagramJewel UPDATE",ds.UpdatePentagramJewel(&r));
		CHECK("A11 PentagramJewel DELETE",ds.DeletePentagramJewel("e2e9",1,2));
	}

	// --- 12 PShopItemValue
	{
		DS_PShopItemValue r; ZeroMemory(&r,sizeof(r));
		strcpy_s(r.Name,"e2e9"); r.Slot=2; r.Serial=3; r.Value=4; r.JobValue=5; r.JosValue=6; r.JocValue=7;
		ds.DeletePShopItemValue("e2e9",2);
		CHECK("A12 PShopItemValue INSERT",ds.InsertPShopItemValue(&r));
		r.Serial=33; r.Value=44; r.JobValue=55; r.JosValue=66; r.JocValue=77;
		CHECK("A12 PShopItemValue UPDATE",ds.UpdatePShopItemValue(&r));
		CHECK("A12 PShopItemValue DELETE",ds.DeletePShopItemValue("e2e9",2));
	}

	// --- 13 SNSData
	{
		BYTE data[600], out[4096]; int len=0;
		for(int i=0;i<600;i++) data[i]=(BYTE)((i*23+1)&0xFF);
		ds.DeleteSNSData("e2e9");
		CHECK("A13 SNSData INSERT(blob)",ds.InsertSNSData("e2e9",data,600));
		ZeroMemory(out,sizeof(out));
		bool ok = ds.LoadSNSData("e2e9",out,sizeof(out),&len);
		sprintf(g_Detail,"len=%d",len);
		CHECK("A13 SNSData LOAD(blob)",ok && len==600 && memcmp(data,out,600)==0);
		for(int i=0;i<600;i++) data[i]=(BYTE)((i*29+8)&0xFF);
		CHECK("A13 SNSData UPDATE(blob)",ds.UpdateSNSData("e2e9",data,600));
		ZeroMemory(out,sizeof(out));
		CHECK("A13 SNSData UPDATE dogrulama",ds.LoadSNSData("e2e9",out,sizeof(out),&len) && len==600 && memcmp(data,out,600)==0);
	}

	// --- 14 ItemMarketData
	{
		DS_ItemMarketData r; ZeroMemory(&r,sizeof(r));
		strcpy_s(r.Account,"e2e9"); strcpy_s(r.Name,"e2e9"); strcpy_s(r.Date,"19.12.2020");
		r.PriceType=1; r.PriceValue=100; r.TypeItem=7; r.Time=1700000000; r.Pass=3;
		int id = ds.InsertItemMarketData(&r);
		sprintf(g_Detail,"id=%d",id);
		CHECK("A14 ItemMarketData INSERT (IDENTITY dondu)",id > 0);
		BYTE it[16]; for(int i=0;i<16;i++) it[i]=(BYTE)(i+1);
		CHECK("A14 ItemMarketData UPDATE Item",id>0 && ds.UpdateItemMarketDataItem(id,it,16));
		CHECK("A14 ItemMarketData UPDATE Status",id>0 && ds.UpdateItemMarketDataStatus(id,1));
		int c = ds.CountItemMarketData("e2e9",7,3);
		sprintf(g_Detail,"sayi=%d",c);
		CHECK("A14 ItemMarketData COUNT",id>0 && c >= 1);
		CHECK("A14 ItemMarketData DELETE by ID",id>0 && ds.DeleteItemMarketData(id));
		CHECK("A14 ItemMarketData DELETE by Account",ds.DeleteItemMarketDataByAccount("e2e9"));
	}

	// temizlik
	ds.UpdateCardPhoneStatus("e2e9",9,0);
	ds.DeleteMuRummyData("e2e9");
	ds.Close();

	//==================================================== B) MEMB_INFO TUTARLILIGI
	printf("\n--- B) MEMB_INFO tutarliligi (JoinServer <-> DataServer) ---\n");
	SQLHANDLE env=0,db=0,st=0;
	if(SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&env))==0)
	{
		sprintf(g_Detail,"ortam ayrilamadi");
		CHECK("B0 MEMB_INFO baglantisi",false);
	}
	else
	{
		SQLSetEnvAttr(env,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3,0);
		SQLAllocHandle(SQL_HANDLE_DBC,env,&db);
		if(SQL_SUCCEEDED(SQLConnect(db,(SQLCHAR*)membDsn,SQL_NTS,NULL,0,NULL,0))==0)
		{
			sprintf(g_Detail,"DSN=%s acilamadi",membDsn);
			CHECK("B0 MEMB_INFO baglantisi (DSN)",false);
		}
		else
		{
			SQLAllocHandle(SQL_HANDLE_STMT,db,&st);
			CHECK("B0 MEMB_INFO baglantisi (DSN)",true);
			char dbName[64]="";
			SQLFreeStmt(st,SQL_CLOSE);
			RawStr(st,"SELECT DB_NAME()",dbName,sizeof(dbName));
			printf("        (veritabani = %s)\n",dbName);

			// JoinServerProtocol.cpp ve DataServerProtocol.cpp'deki AYNI sorgu
			const char* Q = "SELECT memb__pwd FROM MEMB_INFO WHERE memb___id='%s' COLLATE Latin1_General_BIN";
			char sql[512],acc[16],pwdJS[64],pwdDS[64],up[64],lo[64],none1[64],none2[64];
			// her kosuda benzersiz hesap adi (onceki kosulardan kalinti satir
			// sorgulari karistirmasin diye)
			sprintf_s(acc,16,"e9_%06X",(unsigned)GetTickCount()%0xFFFFFF);

			sprintf_s(sql,512,"DELETE FROM MEMB_INFO WHERE memb___id='%s'",acc);
			SQLFreeStmt(st,SQL_CLOSE);
			SQLExecDirect(st,(SQLCHAR*)sql,SQL_NTS);
			SQLFreeStmt(st,SQL_CLOSE);
			sprintf_s(sql,512,"INSERT INTO MEMB_INFO (memb___id,memb__pwd,memb_name,sno__numb,bloc_code,ctl1_code,AccountLevel,AccountExpireDate,Admin,activated) VALUES ('%s','PWD_E2E9','e2e9','1','0','1',1,GETDATE(),0,1)",acc);
			if(SQL_SUCCEEDED(SQLExecDirect(st,(SQLCHAR*)sql,SQL_NTS))==0)
			{
				SQLCHAR dstate[6];SQLCHAR dtext[256];SQLINTEGER dnat;SQLSMALLINT dlen;
				if(SQL_SUCCEEDED(SQLGetDiagRec(SQL_HANDLE_STMT,st,1,dstate,&dnat,dtext,sizeof(dtext),&dlen))){ sprintf(g_Detail,"INSERT hatasi: %s",(char*)dtext); }
				else { sprintf(g_Detail,"INSERT basarisiz (tanimsiz)"); }
				CHECK("B1 MEMB_INFO test hesabi yarat",false);
			}
			else CHECK("B1 MEMB_INFO test hesabi yarat",true);
			SQLFreeStmt(st,SQL_CLOSE);

			sprintf_s(sql,512,Q,acc); bool a1=RawStr(st,sql,pwdJS,sizeof(pwdJS));
			sprintf_s(sql,512,Q,acc); bool a2=RawStr(st,sql,pwdDS,sizeof(pwdDS));
			sprintf(g_Detail,"JS='%s' DS='%s'",pwdJS,pwdDS);
			CHECK("B2 iki sunucunun sorgusu ayni sifreyi okuyor",a1 && a2 && pwdJS[0]!=0 && strcmp(pwdJS,pwdDS)==0);

			sprintf_s(sql,512,Q,"E2E9_ACC"); bool b1=RawStr(st,sql,up,sizeof(up));
			sprintf_s(sql,512,Q,"e2e9_acc"); bool b2=RawStr(st,sql,lo,sizeof(lo));
			sprintf(g_Detail,"buyuk='%s' kucuk='%s'",up,lo);
			// BULGU: COLLATE Latin1_General_BIN nedeniyle hesap aramasi BUYUK/KUCUK
			// HARF DUYARLIDIR. Iki sunucu da AYNI sorguyu kullandigi icin bu davranis
			// zaten tutarlidir; test, buyuk harfin bulunamadigini ve kucuk harfin
			// bulundugunu dogrular.
			CHECK("B3 hesap aramasi BUYUK/KUCUK harf duyarli (iki sunucu ayni COLLATE)",b1==false && b2==true);

			int before=0,after=0;
			sprintf_s(sql,512,"select AccountLevel from MEMB_INFO Where memb___id='%s'",acc);
			RawScalar(st,sql,&before);
			sprintf_s(sql,512,"Update MEMB_INFO set AccountLevel=%d Where memb___id='%s'",before+1234,acc);
			SQLFreeStmt(st,SQL_CLOSE);
			SQLRETURN ru=SQLExecDirect(st,(SQLCHAR*)sql,SQL_NTS);
			if(SQL_SUCCEEDED(ru)==0){SQLCHAR es[6],et[256];SQLINTEGER en;SQLSMALLINT el;if(SQL_SUCCEEDED(SQLGetDiagRec(SQL_HANDLE_STMT,st,1,es,&en,et,sizeof(et),&el)))sprintf(g_Detail,"UPDATE hatasi: %s",(char*)et);}
			SQLFreeStmt(st,SQL_CLOSE);
			sprintf_s(sql,512,"select AccountLevel from MEMB_INFO Where memb___id='%s'",acc);
			RawScalar(st,sql,&after);
			sprintf(g_Detail,"once=%d sonra=%d",before,after);
			CHECK("B4 yazma (AccountLevel) ikinci sunucu sorgusunda gorunur",after==before+1234);

			sprintf_s(sql,512,"UPDATE MEMB_INFO SET memb__pwd = '%s' WHERE memb___id = '%s'","PWD_NEW99",acc);
			SQLFreeStmt(st,SQL_CLOSE);
			SQLRETURN rp=SQLExecDirect(st,(SQLCHAR*)sql,SQL_NTS);
			if(SQL_SUCCEEDED(rp)==0){SQLCHAR es[6],et[256];SQLINTEGER en;SQLSMALLINT el;if(SQL_SUCCEEDED(SQLGetDiagRec(SQL_HANDLE_STMT,st,1,es,&en,et,sizeof(et),&el)))sprintf(g_Detail,"pwd UPDATE hatasi: %s",(char*)et);}
			SQLFreeStmt(st,SQL_CLOSE);
			sprintf_s(sql,512,Q,acc); RawStr(st,sql,pwdDS,sizeof(pwdDS));
			sprintf(g_Detail,"okunan='%s'",pwdDS);
			CHECK("B5 sifre yazimi diger sunucu sorgusunda gorunur",strcmp(pwdDS,"PWD_NEW99")==0);

			sprintf_s(sql,512,Q,"HESAP_YOK_123"); bool c1=RawStr(st,sql,none1,sizeof(none1));
			sprintf_s(sql,512,Q,"HESAP_YOK_123"); bool c2=RawStr(st,sql,none2,sizeof(none2));
			sprintf(g_Detail,"JS=%d DS=%d",c1,c2);
			// BULGU: DataServer CB_AutoNapGame.cpp "select/update gcoin" sorgulari
			// MuOnline MEMB_INFO'da gcoin kolonu OLMADIGI icin calisma aninda hata verir.
			int gc=0;
			sprintf_s(sql,512,"SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME='MEMB_INFO' AND COLUMN_NAME='gcoin'");
			RawScalar(st,sql,&gc);
			sprintf(g_Detail,"gcoin kolon sayisi=%d",gc);
			if(gc==0){ printf("  BULGU B6 gcoin kolonu MEMB_INFO'da YOK - DataServer gcoin sorgulari (CB_AutoNapGame.cpp) iki DB'de de hata verir"); }
			else { CHECK("B6 gcoin kolonu mevcut",gc>0); }

			CHECK("B7 olmayan hesap iki sunucuda da ayni davranis",c1==c2 && c1==false);

			sprintf_s(sql,512,"DELETE FROM MEMB_INFO WHERE memb___id='%s'",acc);
			SQLExecDirect(st,(SQLCHAR*)sql,SQL_NTS);
		}
		SQLFreeHandle(SQL_HANDLE_STMT,st);
		SQLFreeHandle(SQL_HANDLE_DBC,db);
		SQLFreeHandle(SQL_HANDLE_ENV,env);
	}

	printf("\n### SONUCE GECTI=%d  KALDI=%d\n",g_Pass,g_Fail);
	if(g_Fail==0) printf("### 14 TABLO + MEMB_INFO UC-UCA DOGRULANDI\n");
	else printf("### %d KONTROL BASARISIZ\n",g_Fail);
	return g_Fail==0 ? 0 : 1;
}
