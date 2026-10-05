SET NOCOUNT ON;
USE MuOnlineS6;
SELECT memb___id, memb__pwd, bloc_code, ctl1_code, AccountLevel, AccountExpireDate, Lock FROM MEMB_INFO WHERE memb___id='e2etest';
DECLARE @cols1 nvarchar(max), @cols2 nvarchar(max);
SELECT @cols1 = STUFF((SELECT ',' + QUOTENAME(c.name) FROM sys.columns c WHERE c.object_id=OBJECT_ID('dbo.Character') AND c.is_identity=0 ORDER BY c.column_id FOR XML PATH(''), TYPE).value('.','nvarchar(max)'),1,1,'');
SET @cols2 = REPLACE(@cols1, QUOTENAME('AccountID'), '''e2etest''');
SET @cols2 = REPLACE(@cols2, QUOTENAME('Name'), '''H018Test''');
IF EXISTS (SELECT 1 FROM dbo.Character WHERE Name='H018Test')
  SELECT 'ZATEN VAR' AS durum;
ELSE
BEGIN
  EXEC('INSERT INTO dbo.Character (' + @cols1 + ') SELECT ' + @cols2 + ' FROM dbo.Character WHERE Name=''Party''');
  SELECT 'OLUSTURULDU' AS durum;
END
SELECT Name, AccountID, cLevel, Class, MapNumber, MapPosX, MapPosY FROM dbo.Character WHERE Name='H018Test';
