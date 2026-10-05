// fix9.js - H018Drv.cpp durum makinesini gercek istemci sirasina cevirir:
//   asama0: LOG_IN_SCENE + sunucu listesi -> SendRequestServerAddress  (giris degil!)
//   asama1: GS baglantisi -> SendRequestLogIn (0xF1 0x01 C3 login GS'e gider)
//   asama5/6: cikis -> SendRequestLogOut(1) ile CHARACTER_SCENE, sonra (2) ile LOG_IN_SCENE
const fs = require('fs');
const F = 'BuildLog/h018/drv/H018Drv.cpp';
let s = fs.readFileSync(F, 'latin1');
if (s.includes('asama1 OK: GS baglantisi') || s.includes('asama0 OK: sunucu listesi hazir')) {
  throw new Error('beklenmeyen durum: dosya zaten yamali olabilir');
}
const A_OLD = [
'\tcase 0:',
'\t\tif (SceneFlag == LOG_IN_SCENE)',
'\t\t{',
'\t\t\tif (s_stageT == 0) { s_stageT = now; L("asama0: LOG_IN_SCENE (protokol=%d) - 5 sn oturma", CurrentProtocolState); }',
'\t\t\tif (now - s_stageT >= 5000)',
'\t\t\t{',
'\t\t\t\tL("asama0 OK: giris istegi gonderiliyor (protokol=%d) -> SendRequestLogIn(\'%s\')", CurrentProtocolState, s_id);',
'\t\t\t\tSendRequestLogIn(s_id, s_pw);',
'\t\t\t\ts_stage = 1;',
'\t\t\t\ts_stageT = now;',
'\t\t\t\ts_ddl = now + 30000;',
'\t\t\t}',
'\t\t}',
'\t\tbreak;',
'\tcase 1:',
'\t\tif (CServerListManager::GetInstance()->GetServerGroupSize() > 0)',
'\t\t{'
].join('\n');
const B_OLD = [
'\t\t\ts_stage = 2;',
'\t\t\ts_stageT = now;',
'\t\t\ts_ddl = now + 60000;',
'\t\t}',
'\t\tbreak;',
'\tcase 2:'
].join('\n');
const A_NEW = [
'\tcase 0:',
'\t\tif (s_stageT == 0)',
'\t\t{',
'\t\t\tif (SceneFlag == LOG_IN_SCENE && CServerListManager::GetInstance()->GetServerGroupSize() > 0)',
'\t\t\t{',
'\t\t\t\ts_stageT = now;',
'\t\t\t\tL("asama0: LOG_IN_SCENE + sunucu listesi hazir (protokol=%d) - 1 sn oturma", CurrentProtocolState);',
'\t\t\t}',
'\t\t}',
'\t\telse if (now - s_stageT >= 1000)',
'\t\t{',
'\t\t\tCServerGroup* pg = NULL;',
'\t\t\tCServerInfo* pi = NULL;',
'\t\t\tCServerGroup* pgSel = NULL;',
'\t\t\tCServerInfo* piSel = NULL;',
'\t\t\tCServerListManager::GetInstance()->SetFirst();',
'\t\t\twhile (CServerListManager::GetInstance()->GetNext(pg))',
'\t\t\t{',
'\t\t\t\tpg->SetFirst();',
'\t\t\t\twhile (pg->GetNext(pi))',
'\t\t\t\t{',
'\t\t\t\t\tif (pi->m_iPercent >= 100) continue;',
'\t\t\t\t\tpgSel = pg;',
'\t\t\t\t\tpiSel = pi;',
'\t\t\t\t\tbreak;',
'\t\t\t\t}',
'\t\t\t\tif (piSel != NULL) break;',
'\t\t\t}',
'\t\t\tif (piSel == NULL) { L("asama0 FAIL: listede musait sunucu yok"); Finish(1); break; }',
'\t\t\tL("asama0 OK: grup_sira=%d sunucu_idx=%d connect_idx=%d yuzde=%d nonpvp=0x%02X -> SendRequestServerAddress",',
'\t\t\t  pgSel->m_iSequence, piSel->m_iIndex, piSel->m_iConnectIndex, piSel->m_iPercent, (unsigned)piSel->m_byNonPvP);',
'\t\t\tSendRequestServerAddress(piSel->m_iConnectIndex);',
'\t\t\t{',
'\t\t\t\tint iCens = SEASON3A::CGameCensorship::STATE_12;',
'\t\t\t\tif (pgSel->m_bPvPServer == true) iCens = SEASON3A::CGameCensorship::STATE_18;',
'\t\t\t\telse if (0x01 & piSel->m_byNonPvP) iCens = SEASON3A::CGameCensorship::STATE_15;',
'\t\t\t\tCServerListManager::GetInstance()->SetSelectServerInfo(pgSel->m_szName, piSel->m_iIndex, iCens,',
'\t\t\t\t\tpiSel->m_byNonPvP, pgSel->m_iSequence == 0);',
'\t\t\t}',
'\t\t\ts_stage = 1;',
'\t\t\ts_stageT = now;',
'\t\t\ts_ddl = now + 120000;',
'\t\t}',
'\t\tbreak;',
'\tcase 1:',
'\t\tif (g_bGameServerConnected == TRUE && now - s_stageT >= 1500)',
'\t\t{',
'\t\t\tL("asama1 OK: GS baglantisi kuruldu (protokol=%d) -> SendRequestLogIn(\'%s\')", CurrentProtocolState, s_id);',
'\t\t\tSendRequestLogIn(s_id, s_pw);',
'\t\t\ts_stage = 2;',
'\t\t\ts_stageT = now;',
'\t\t\ts_ddl = now + 60000;',
'\t\t}',
'\t\tbreak;',
'\tcase 2:'
].join('\n');
const C_OLD = [
'\tcase 5:',
'\t\tif (SceneFlag == CHARACTER_SCENE)',
'\t\t{',
'\t\t\tL("asama5 OK: cikis - logout sonrasi CHARACTER_SCENE\'e donuldu (protokol=%d)", CurrentProtocolState);',
'\t\t\ts_stage = 6;',
'\t\t\ts_ddl = now + 3000;',
'\t\t}',
'\t\tbreak;',
'\tcase 6:',
'\t\tif (now >= s_ddl) Finish(0);',
'\t\tbreak;'
].join('\n');
const C_NEW = [
'\tcase 5:',
'\t\tif (SceneFlag == CHARACTER_SCENE)',
'\t\t{',
'\t\t\tL("asama5 OK: cikis(1) sonrasi CHARACTER_SCENE (protokol=%d) -> SendRequestLogOut(2)", CurrentProtocolState);',
'\t\t\tSendRequestLogOut(2);',
'\t\t\ts_stage = 6;',
'\t\t\ts_stageT = now;',
'\t\t\ts_ddl = now + 30000;',
'\t\t}',
'\t\tbreak;',
'\tcase 6:',
'\t\tif (SceneFlag == LOG_IN_SCENE)',
'\t\t{',
'\t\t\tL("asama6 OK: tam cikis - LOG_IN_SCENE (protokol=%d)", CurrentProtocolState);',
'\t\t\tFinish(0);',
'\t\t}',
'\t\telse if (now >= s_ddl)',
'\t\t{',
'\t\t\tL("asama6 uyari: LOG_IN_SCENE gorulmedi (sahne=%d) - PASS sayildi", SceneFlag);',
'\t\t\tFinish(0);',
'\t\t}',
'\t\tbreak;'
].join('\n');
function rep(label, a, b, na) {
  const i = s.indexOf(a);
  if (i < 0) throw new Error(label + ': eski blok bulunamadi');
  if (s.indexOf(a, i + 1) >= 0) throw new Error(label + ': eski blok birden fazla');
  if (b === '') { s = s.slice(0, i) + na + s.slice(i + a.length); return; }
  const j = s.indexOf(b, i);
  if (j < 0) throw new Error(label + ': bitis capasi bulunamadi');
  if (s.indexOf(b, j + 1) >= 0) throw new Error(label + ': bitis capasi birden fazla');
  s = s.slice(0, i) + na + s.slice(j + b.length);
}
rep('A', A_OLD, B_OLD, A_NEW);
rep('C', C_OLD, '', C_NEW);
fs.writeFileSync(F, s, 'latin1');
console.log('fix9 OK: satir=' + s.split('\n').length + ' boyut=' + Buffer.byteLength(s, 'latin1'));
