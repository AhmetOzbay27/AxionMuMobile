// main_spk.h — Faz 2d.1 (D7): SPK modu giriş noktası.
//
// Kullanım (GetMainInfo.exe):
//   (varsayılan)                 SPK üretimi: ConnectIP.bmd + ServerData.bmd + SPK_CRCFILE.ini
//   --mode:muig                  Eski MUIG akışı (CBGetMain/CBTextInfo/License)
//   --ini:".\GetEngine.ini"      ini yolu (varsayılan)
//   --client:"..\Client"         istemci kökü (varsayılan; Config\Info kontrolleri + CRC)
//   --data:".\Data"              GetMain veri klasörü (varsayılan)
//   --out:"..."                  çıktı klasörü (varsayılan: <client>\Data\SPK)
//   --template:"..."             ServerData şablonu (varsayılan ".\ServerData.template.bmd";
//                                yoksa mevcut <out>\ServerData.bmd kullanılır — D3 şablon stratejisi)
//   --android:"..."              Android asset kopyası klasörü (varsa yazılır)
//   --report:"..."               SPK_CRCFILE.ini yolu (varsayılan ".\SPK_CRCFILE.ini")
//   --check                      üretim yapmadan mevcut dosyaları ini ile doğrula
//
// Dönüş kodları: 0 OK, 1 ini yok, 2 şablon yok, 3 yazma hatası, 4 --check başarısız

#pragma once

int SPKMain(int argc, char** argv);
