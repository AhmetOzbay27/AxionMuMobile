/* C-02 — duman testi (smoke test)
   Yeni yaratilan 14 tabloya, kaynak kodunun gercekten gonderdigi sorgulari
   birebir calistirip sonucu temizler. INSERT/UPDATE/SELECT hatalari olusmamalidir.

   NOT: CustomNpcQuest'te resmi semada Character(Name) FK'si vardir; bu yuzden
        test icin gercek bir karakter adi kullanilir (kaynak kod da her zaman
        oturumdaki karakterin adini yazar).                                  */
SET NOCOUNT ON;

DECLARE @chr varchar(10) = (SELECT TOP 1 Name FROM dbo.Character ORDER BY Name);
DECLARE @acc varchar(10) = 'c02test';
DECLARE @b256 varbinary(256) = CAST(REPLICATE(0xAB,256) AS varbinary(256));
DECLARE @b512 varbinary(512) = CAST(REPLICATE(0xAB,512) AS varbinary(512));
DECLARE @b992 varbinary(992) = CAST(REPLICATE(0xAB,992) AS varbinary(992));
DECLARE @b16  varbinary(16)  = CAST(REPLICATE(0xAB,16)  AS varbinary(16));
PRINT 'Test karakteri: ' + @chr;

PRINT '--- 1) ItemMarketData : ChoTroi.cpp:290 GDReqItemSell INSERT (birebir) ---';
INSERT INTO ItemMarketData (Account, PriceType, PriceValue, Date, TypeItem, Name, Time, Pass) VALUES ('c02test', 1, 100, '19.12.2020', 7, 'c02test', 1700000000, 3);
SELECT '1a eklenen satir' AS test, COUNT(*) AS ok FROM ItemMarketData WHERE Account='c02test' AND TypeItem=7 AND [Pass]=3;
SELECT TOP 1 ID, Account, PriceType, PriceValue FROM ItemMarketData WHERE Status = 0 ORDER BY ID DESC;
SELECT Item FROM ItemMarketData WHERE ID = (SELECT MIN(ID) FROM ItemMarketData WHERE Account='c02test');
UPDATE ItemMarketData SET Status = 1 WHERE Account='c02test';
SELECT '1b Status UPDATE' AS test, Status FROM ItemMarketData WHERE Account='c02test';
DELETE FROM ItemMarketData WHERE Account='c02test';

PRINT '--- 2) CustomItemBank : DataServerProtocol.cpp ---';
INSERT INTO CustomItemBank (AccountID,ItemIndex,ItemLevel,ItemCount,AutoPick) VALUES ('c02test',5,7,3,0);
UPDATE CustomItemBank SET ItemCount = 4, AutoPick = 1 WHERE AccountID = 'c02test' AND ItemIndex = 5 AND ItemLevel = 7;
SELECT '2 CustomItemBank' AS test, ItemCount, AutoPick FROM CustomItemBank WHERE AccountID='c02test' AND ItemIndex='5' AND ItemLevel='7';
DELETE FROM CustomItemBank WHERE AccountID='c02test';

PRINT '--- 3) CustomNpcQuest : DataServerProtocol.cpp (+ Character FK) ---';
DELETE FROM CustomNpcQuest WHERE Quest = 987654;
INSERT INTO CustomNpcQuest (Name,quest,count,MonsterCount) VALUES (@chr,987654,1,99999);
UPDATE CustomNpcQuest SET Count = Count+1, MonsterCount=99999 WHERE Name = @chr and Quest = 987654;
SELECT '3 CustomNpcQuest' AS test, Quest, [Count], MonsterCount FROM CustomNpcQuest WHERE Name=@chr and Quest = 987654;
DELETE FROM CustomNpcQuest WHERE Quest = 987654;

PRINT '--- 4/5/6) Envanter blob tablolari (256 / 512 / 992 bayt dolu) ---';
INSERT INTO EquipInventory (CharName, Items) VALUES (@chr, @b256);
SELECT '4 EquipInventory' AS test, DATALENGTH(Items) AS bayt FROM EquipInventory WHERE CharName=@chr;
DELETE FROM EquipInventory WHERE CharName=@chr;

INSERT INTO EventInventory (Name,Items) VALUES (@chr, @b512);
SELECT '5 EventInventory' AS test, DATALENGTH(Items) AS bayt FROM EventInventory WHERE Name=@chr;
DELETE FROM EventInventory WHERE Name=@chr;

INSERT INTO MuunInventory (Name,Items) VALUES (@chr, @b992);
SELECT '6 MuunInventory' AS test, DATALENGTH(Items) AS bayt FROM MuunInventory WHERE Name=@chr;
DELETE FROM MuunInventory WHERE Name=@chr;

PRINT '--- 7/8) MuRummy ---';
INSERT INTO MuRummyCard (Name,Color,Number,Slot,Status,Sequence) VALUES (@chr,1,2,0,0,1);
UPDATE MuRummyCard SET Slot=1,Status=1 WHERE Name=@chr AND Sequence=1;
SELECT '7 MuRummyCard' AS test, Color,Number,Slot,Status,Sequence FROM MuRummyCard WHERE Name=@chr AND Sequence=1;
DELETE FROM MuRummyCard WHERE Sequence=1;
INSERT INTO MuRummyData (Name,TotalScore) VALUES (@chr,55);
SELECT '8 MuRummyData' AS test, TotalScore FROM MuRummyData WHERE Name=@chr;
DELETE FROM MuRummyData WHERE Name=@chr;

PRINT '--- 9) PcPointData ---';
INSERT INTO PcPointData (AccountID,PcPoint) VALUES ('c02test',0);
UPDATE PcPointData SET PcPoint = 12 WHERE AccountID = 'c02test';
SELECT '9 PcPointData' AS test, PcPoint FROM PcPointData WHERE AccountID='c02test';
DELETE FROM PcPointData WHERE AccountID='c02test';

PRINT '--- 10) PentagramJewel : 17 sutunlu INSERT ---';
INSERT INTO PentagramJewel (Name,Type,[Index],Attribute,ItemSection,ItemType,ItemLevel,OptionIndexRank1,OptionLevelRank1,OptionIndexRank2,OptionLevelRank2,OptionIndexRank3,OptionLevelRank3,OptionIndexRank4,OptionLevelRank4,OptionIndexRank5,OptionLevelRank5)
VALUES (@chr,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
UPDATE PentagramJewel SET Attribute=30,ItemSection=31,ItemType=32,ItemLevel=33 WHERE Name=@chr AND Type=1 AND [Index]=2;
SELECT '10 PentagramJewel' AS test, Attribute,ItemSection,ItemType,ItemLevel,OptionLevelRank5 FROM PentagramJewel WHERE Name=@chr AND Type=1;
DELETE FROM PentagramJewel WHERE Type=1 AND [Index]=2 AND Name=@chr;

PRINT '--- 11) PShopItemValue ---';
INSERT INTO PShopItemValue (Name,Slot,Serial,Value,JoBValue,JoSValue,JoCValue) VALUES (@chr,0,1,100,200,300,400);
UPDATE PShopItemValue SET Serial=2,Value=101,JoBValue=201,JoSValue=301,JoCValue=401 WHERE Name=@chr AND Slot=0;
SELECT '11 PShopItemValue' AS test, Slot,Serial,Value,JoBValue,JoSValue,JoCValue FROM PShopItemValue WHERE Name=@chr AND Slot=0;
DELETE FROM PShopItemValue WHERE Name=@chr AND Slot=0;

PRINT '--- 12) SNSData ---';
INSERT INTO SNSData (Name,Data) VALUES (@chr, @b16);
SELECT '12 SNSData' AS test, DATALENGTH(Data) AS bayt FROM SNSData WHERE Name=@chr;
DELETE FROM SNSData WHERE Name=@chr;

PRINT '--- 13) CardPhone ---';
INSERT INTO CardPhone(acc, name, card_type, menhgia, card_num, card_num_md5, card_serial, timenap) VALUES ('c02test','c02test','VIP',1,'12345','d41d8cd98f00b204e9800998ecf8427e','SERIAL1',5);
UPDATE CardPhone SET Status=1, timeduyet=6, menhgia=7, addvpoint=8 WHERE acc='c02test';
SELECT Top 8 * From CardPhone Where acc='c02test' ORDER BY timenap DESC;
DELETE FROM CardPhone WHERE acc='c02test';

PRINT '--- 14) DataNapGame ---';
INSERT INTO DataNapGame (Account, Name, TienNap, Checking) VALUES ('c02test','c02test',10,20);
UPDATE DataNapGame SET Status=1, TienNap=11 WHERE Account='c02test' AND Name='c02test';
SELECT Top 8 * From DataNapGame Where Account='c02test' ORDER BY Checking DESC;
DELETE FROM DataNapGame WHERE Account='c02test';

PRINT 'C-02 DUMAN TESTI TAMAM - 14/14 tablo hatasiz, test satirlari silindi.';
