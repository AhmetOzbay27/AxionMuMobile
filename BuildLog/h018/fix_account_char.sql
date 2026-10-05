-- H-018 runtime testi: karakter listesi bos donuyordu.
-- Kok neden: DataServer karakter listesini AccountCharacter.GameID1..5
-- slotlarindan okur (Source/2.DataServer/DataServer/DataServerProtocol.cpp
-- ~L998-1050); e2etest satiri vardi ama slotlar NULL'du. Karakter kaydi
-- (Character.Name='H018Test') yerinde duruyordu, slota bagli degildi.
SET NOCOUNT ON;
USE MuOnlineS6;
UPDATE dbo.AccountCharacter SET GameID1='H018Test' WHERE Id='e2etest';
SELECT Id, GameID1, GameID2, GameID3, GameID4, GameID5 FROM dbo.AccountCharacter WHERE Id='e2etest';
