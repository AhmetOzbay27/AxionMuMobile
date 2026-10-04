const fs = require('fs');
const f = 'Source/5.Main/source/PacketManager.h';
const raw = fs.readFileSync(f, 'latin1');
const NL = raw.includes('\r\n') ? '\r\n' : '\n';
let s = raw;
const rep = (a, b) => { if (s.split(a).length !== 2) throw new Error('cipa yok: ' + a.slice(0, 30)); s = s.replace(a, b); };
rep(['using namespace CryptoPP;', '', '#endif'].join(NL),
    ['// 04.10.2026 (docs/34): "using namespace CryptoPP;" KALDIRILDI — global', '// "Singleton" sembolu CryptoPP::Singleton ile belirsizlesip istemcinin', '// GAMESERVER_UPDATE>=701 derlemesini C2872 ile kiriyordu (WSclient.cpp).', '// CryptoPP tipleri asagida acikca nitelendirilir.', '#endif'].join(NL));
rep(['\tECB_Mode<DES_XEX3>::Encryption m_Encryption;', '\tECB_Mode<DES_XEX3>::Decryption m_Decryption;'].join(NL),
    ['\tCryptoPP::ECB_Mode<CryptoPP::DES_XEX3>::Encryption m_Encryption;', '\tCryptoPP::ECB_Mode<CryptoPP::DES_XEX3>::Decryption m_Decryption;'].join(NL));
fs.writeFileSync(f, s, 'latin1');
console.log('PacketManager.h duzeltildi NL=' + JSON.stringify(NL));
