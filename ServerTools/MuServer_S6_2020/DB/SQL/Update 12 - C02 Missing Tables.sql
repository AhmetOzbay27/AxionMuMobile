/* =====================================================================
   Update 12 - C02 Eksik Tablolar (Schema Patch)
   ---------------------------------------------------------------------
   Tarih      : 03.10.2026
   Kapsam     : docs/03 C-02 — DB sema uyumu denetimi
   Gerekce    : DataServer/JoinServer kaynak kodu 13 tabloya SQL sorgusu
                gonderiyor; ancak bu tablolar DB_SQL_12.bak semasinda
                YOK ve calisma aninda da olusturulmuyor. Bu ozellikler
                kullanildiginda "Invalid object name" hatasi verir.
   Kanit      : BuildLog/2e4/c02_schema_report.txt
                (743 kaynak dosyasi, 308 SQL literal, 50 tablo kullanilir)
   Not        : ItemMarketData kasitla YOK - ChoTroi.cpp:36 calisma aninda
                CREATE TABLE ile kendini olusturuyor, :92 ile Item
                sutununu ekliyor. Bu yuzden burada yer almaz.

   Tipler mevcut semadan tureti: ad/hesap = varchar(10) (Character.Name,
   MEMB_INFO.memb___id), sayaclar = int, envanter = varbinary(n)
   (Character.Inventory varbinary), tarih yerine gecen degerler kaynakta
   GetAsInteger ile okundugu icin int.

   UYGULAMA : sqlcmd -S <sunucu> -d MuOnlineS6 -E -i "bu dosya"
   Idempotent : IF OBJECT_ID(...) IS NULL korumali, tekrar calistirilabilir.
   ===================================================================== */

SET NOCOUNT ON;
GO

/* ---------------------------------------------------------------------
   1) CardPhone — CB_AutoNapGame.cpp (kartla otomatik oynama)
      timenap/timeduyet GetAsInteger ile okunuyor -> int (tarih DEGIL)
   ------------------------------------------------------------------- */
IF OBJECT_ID('dbo.CardPhone','U') IS NULL
CREATE TABLE [dbo].[CardPhone] (
    [acc]           varchar(10)  NOT NULL,
    [name]          varchar(10)  NULL,
    [card_type]     varchar(10)  NULL,
    [menhgia]       int          NULL,
    [card_num]      varchar(20)  NULL,
    [card_num_md5]  varchar(50)  NULL,
    [card_serial]   varchar(20)  NULL,
    [timenap]       int          NULL,
    [addvpoint]     int          NULL,
    [status]        int          NULL,
    [stt]           int          NULL,
    [timeduyet]     int          NULL
);
GO

/* ---------------------------------------------------------------------
   2) CustomItemBank — DataServerProtocol.cpp (hesap esyalik bankasi)
   ------------------------------------------------------------------- */
IF OBJECT_ID('dbo.CustomItemBank','U') IS NULL
CREATE TABLE [dbo].[CustomItemBank] (
    [AccountID]  varchar(10) NOT NULL,
    [ItemIndex]  int         NOT NULL,
    [ItemLevel]  int         NOT NULL,
    [ItemCount]  int         NULL,
    [AutoPick]   int         NULL,
    CONSTRAINT [PK_CustomItemBank] PRIMARY KEY ([AccountID],[ItemIndex],[ItemLevel])
);
GO

/* ---------------------------------------------------------------------
   3) CustomNpcQuest — DataServerProtocol.cpp (NPC gorev sayaci)
   ------------------------------------------------------------------- */
-- NOT: Bu tablo icin resmi betik de var -> "Update13 - CustomNpcQuest Table.sql".
--       Tanim oradan birebir alindi; boylece sema upstream ile ayni kalir
--       (varsayilan degerler + Character(Name) FK + ON DELETE CASCADE).
IF OBJECT_ID('dbo.CustomNpcQuest','U') IS NULL
CREATE TABLE [dbo].[CustomNpcQuest](
    [Name]         varchar(10) NOT NULL,
    [Quest]        int         NOT NULL,
    [Count]        int         NOT NULL,
    [MonsterCount] int         NOT NULL,
    CONSTRAINT [PK_CustomNpcQuest] PRIMARY KEY CLUSTERED ([Name] ASC,[Quest] ASC)
    ON [PRIMARY]
);
GO

IF OBJECT_ID('dbo.CustomNpcQuest','U') IS NOT NULL
   AND NOT EXISTS (SELECT 1 FROM sys.foreign_keys WHERE name = 'FK_CustomNpcQuest_Character')
    ALTER TABLE [dbo].[CustomNpcQuest] WITH CHECK
        ADD CONSTRAINT [FK_CustomNpcQuest_Character] FOREIGN KEY([Name])
        REFERENCES [dbo].[Character] ([Name]) ON UPDATE CASCADE ON DELETE CASCADE;
GO

IF OBJECT_ID('dbo.CustomNpcQuest','U') IS NOT NULL
   AND NOT EXISTS (SELECT 1 FROM sys.default_constraints WHERE name = 'DF_CustomNpcQuest_Quest')
    ALTER TABLE [dbo].[CustomNpcQuest] ADD CONSTRAINT [DF_CustomNpcQuest_Quest] DEFAULT ((0)) FOR [Quest];
GO

IF OBJECT_ID('dbo.CustomNpcQuest','U') IS NOT NULL
   AND NOT EXISTS (SELECT 1 FROM sys.default_constraints WHERE name = 'DF_CustomNpcQuest_Count')
    ALTER TABLE [dbo].[CustomNpcQuest] ADD CONSTRAINT [DF_CustomNpcQuest_Count] DEFAULT ((0)) FOR [Count];
GO

IF OBJECT_ID('dbo.CustomNpcQuest','U') IS NOT NULL
   AND NOT EXISTS (SELECT 1 FROM sys.default_constraints WHERE name = 'DF_CustomNpcQuest_MonsterQtd')
    ALTER TABLE [dbo].[CustomNpcQuest] ADD CONSTRAINT [DF_CustomNpcQuest_MonsterQtd] DEFAULT ((0)) FOR [MonsterCount];
GO

/* ---------------------------------------------------------------------
   4) DataNapGame — CB_AutoNapGame.cpp / DataServerProtocol.cpp
   ------------------------------------------------------------------- */
IF OBJECT_ID('dbo.DataNapGame','U') IS NULL
CREATE TABLE [dbo].[DataNapGame] (
    [Account]  varchar(10) NOT NULL,
    [Name]     varchar(10) NULL,
    [TienNap]  int         NULL,
    [Checking] int         NULL,
    [Status]   int         NULL
);
GO

/* ---------------------------------------------------------------------
   5) EquipInventory — NewUIMyInventory.cpp
      Items = PET_INVENTORY_SIZE(16) x 16 = 256 bayt
   ------------------------------------------------------------------- */
IF OBJECT_ID('dbo.EquipInventory','U') IS NULL
CREATE TABLE [dbo].[EquipInventory] (
    [CharName] varchar(10)    NOT NULL,
    [Items]    varbinary(256) NULL,
    CONSTRAINT [PK_EquipInventory] PRIMARY KEY ([CharName])
);
GO

/* ---------------------------------------------------------------------
   6) EventInventory — EventInventory.cpp
      Items = EVENT_INVENTORY_SIZE(32) x 16 = 512 bayt
   ------------------------------------------------------------------- */
IF OBJECT_ID('dbo.EventInventory','U') IS NULL
CREATE TABLE [dbo].[EventInventory] (
    [Name]  varchar(10)    NOT NULL,
    [Items] varbinary(512) NULL,
    CONSTRAINT [PK_EventInventory] PRIMARY KEY ([Name])
);
GO

/* ---------------------------------------------------------------------
   7) MuunInventory — MuunSystem.cpp
      Items = MUUN_INVENTORY_SIZE(62) x 16 = 992 bayt
   ------------------------------------------------------------------- */
IF OBJECT_ID('dbo.MuunInventory','U') IS NULL
CREATE TABLE [dbo].[MuunInventory] (
    [Name]  varchar(10)     NOT NULL,
    [Items] varbinary(992)  NULL,
    CONSTRAINT [PK_MuunInventory] PRIMARY KEY ([Name])
);
GO

/* ---------------------------------------------------------------------
   8) MuRummyCard — MuRummy.cpp (5 kartli Rummy)
   ------------------------------------------------------------------- */
IF OBJECT_ID('dbo.MuRummyCard','U') IS NULL
CREATE TABLE [dbo].[MuRummyCard] (
    [Name]     varchar(10) NOT NULL,
    [Color]    int         NULL,
    [Number]   int         NULL,
    [Slot]     int         NULL,
    [Status]   int         NULL,
    [Sequence] int         NULL
);
GO

/* ---------------------------------------------------------------------
   9) MuRummyData — MuRummy.cpp
   ------------------------------------------------------------------- */
IF OBJECT_ID('dbo.MuRummyData','U') IS NULL
CREATE TABLE [dbo].[MuRummyData] (
    [Name]       varchar(10) NOT NULL,
    [TotalScore] int         NULL,
    CONSTRAINT [PK_MuRummyData] PRIMARY KEY ([Name])
);
GO

/* ---------------------------------------------------------------------
   10) PcPointData — PcPoint.cpp
   ------------------------------------------------------------------- */
IF OBJECT_ID('dbo.PcPointData','U') IS NULL
CREATE TABLE [dbo].[PcPointData] (
    [AccountID] varchar(10) NOT NULL,
    [PcPoint]   int         NULL,
    CONSTRAINT [PK_PcPointData] PRIMARY KEY ([AccountID])
);
GO

/* ---------------------------------------------------------------------
   11) PentagramJewel — PentagramSystem.cpp
       Index dahil TUM alanlar GetAsInteger ile okunuyor -> int
   ------------------------------------------------------------------- */
IF OBJECT_ID('dbo.PentagramJewel','U') IS NULL
CREATE TABLE [dbo].[PentagramJewel] (
    [Name]              varchar(10) NOT NULL,
    [Type]              int         NULL,
    [Index]             int         NULL,
    [Attribute]         int         NULL,
    [ItemSection]       int         NULL,
    [ItemType]          int         NULL,
    [ItemLevel]         int         NULL,
    [OptionIndexRank1]  int         NULL,
    [OptionLevelRank1]  int         NULL,
    [OptionIndexRank2]  int         NULL,
    [OptionLevelRank2]  int         NULL,
    [OptionIndexRank3]  int         NULL,
    [OptionLevelRank3]  int         NULL,
    [OptionIndexRank4]  int         NULL,
    [OptionLevelRank4]  int         NULL,
    [OptionIndexRank5]  int         NULL,
    [OptionLevelRank5]  int         NULL
);
GO

/* ---------------------------------------------------------------------
   12) PShopItemValue — PersonalShop.cpp (kiyaslanan PStore degerleri)
   ------------------------------------------------------------------- */
IF OBJECT_ID('dbo.PShopItemValue','U') IS NULL
CREATE TABLE [dbo].[PShopItemValue] (
    [Name]      varchar(10) NOT NULL,
    [Slot]      int         NULL,
    [Serial]    int         NULL,
    [Value]     int         NULL,
    [JobValue]  int         NULL,
    [JosValue]  int         NULL,
    [JocValue]  int         NULL
);
GO

/* ---------------------------------------------------------------------
   13) SNSData — DataServerProtocol.cpp GDSNSDataRecv
       Data GetAsBinary ile okunuyor -> varbinary
   ------------------------------------------------------------------- */
IF OBJECT_ID('dbo.SNSData','U') IS NULL
CREATE TABLE [dbo].[SNSData] (
    [Name] varchar(10)    NOT NULL,
    [Data] varbinary(MAX) NULL,
    CONSTRAINT [PK_SNSData] PRIMARY KEY ([Name])
);
GO

/* ---------------------------------------------------------------------
   14) ItemMarketData — ChoTroi.cpp
       NOT: Kaynak kodda CChoTroi::CreateTable() calisma aninda tabloyu
       KENDISI olusturuyor. Ancak denetimde iki gercek hata cikti:
         - TypeItem / Time / Pass sutunlari hic olusturulmuyordu, ama
           GDReqItemSell INSERT'i (ChoTroi.cpp:290) bunlari yaziyor
           -> "Invalid column name" hatasi.
         - CREATE TABLE kosulsuz oldugu icin DataServer 2. kez
           acildiginda tum ALTER'lar basarisiz oluyordu.
       Kaynak kod (ChoTroi.cpp) duzeltildi: IF OBJECT_ID / IF COL_LENGTH
       korumalari + eksik 3 sutun eklendi. Asagidaki blok, taze DB
       kurulumunda ayni sonucu SQL tarafindan uretir.
       MARKET_NAME_DEV=1 ve MARKET_FILTER_DEV=1 (ChoTroi.h:14-15)
       -> Name ve Filter* sutunlari da olusur.
   ------------------------------------------------------------------- */
IF OBJECT_ID('dbo.ItemMarketData','U') IS NULL
CREATE TABLE [dbo].[ItemMarketData](
    [ID]         int IDENTITY(1,1) NOT NULL,
    [Account]    varchar(10)  NULL,
    [Name]       varchar(10)  NULL,
    [PriceType]  int NOT NULL DEFAULT(0),
    [PriceValue] int NOT NULL DEFAULT(0),
    [Status]     int NOT NULL DEFAULT(0),
    [FilterType] int NOT NULL DEFAULT(0),
    [FilterLevel]int NOT NULL DEFAULT(0),
    [FilterLuck] int NOT NULL DEFAULT(0),
    [FilterExl]  int NOT NULL DEFAULT(0),
    [FilterAnc]  int NOT NULL DEFAULT(0),
    [Date]       varchar(20)  NULL,
    [Item]       varbinary(16) NULL,
    [TypeItem]   int NOT NULL DEFAULT(0),
    [Time]       int NOT NULL DEFAULT(0),
    [Pass]       int NOT NULL DEFAULT(0),
    CONSTRAINT [PK_ItemMarketData] PRIMARY KEY CLUSTERED ([ID] ASC) ON [PRIMARY]
);
GO

PRINT 'C-02 sema yamasi uygulandi.';
GO