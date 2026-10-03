// pdbtype.cpp - canli GameServer.pdb icinden UDT (struct) yerlesimini DIA ile okur.
// Kullanim: pdbtype.exe <pdb-yolu> <isim-alt-dizisi> [max-derinlik]
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <dia2.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#pragma comment(lib, "ole32.lib")

typedef HRESULT (WINAPI *DllGetClassObjectProc)(REFCLSID, REFIID, LPVOID *);

static void toUtf8(BSTR b, char *out, size_t n)
{
    out[0] = 0;
    if (!b) return;
    WideCharToMultiByte(CP_UTF8, 0, b, -1, out, (int)n, NULL, NULL);
    // SysFreeString birakildi (ole32 bagimliligi)
}

static void symName(IDiaSymbol *s, char *out, size_t n)
{
    out[0] = 0;
    if (!s) return;
    BSTR b = NULL;
    if (SUCCEEDED(s->get_name(&b))) toUtf8(b, out, n);
}

static const char *locName(DWORD lt)
{
    switch (lt) {
    case LocIsNull:          return "null";
    case LocIsStatic:        return "static";
    case LocIsThisRel:       return "this+";
    case LocIsBitField:      return "bitfield";
    case LocIsConstant:      return "const";
    default:                 return "loc?";
    }
}

// Tip adini + (dizi ise) eleman bilgisini yazar.
static void typeText(IDiaSymbol *owner, char *out, size_t n)
{
    out[0] = 0;
    IDiaSymbol *t = NULL;
    if (!owner || FAILED(owner->get_type(&t)) || !t) return;
    char tb[400]; symName(t, tb, sizeof(tb));
    DWORD tag = SymTagNull; t->get_symTag(&tag);
    if (tag == SymTagArrayType) {
        ULONGLONG bytes = 0; t->get_length(&bytes);
        IDiaSymbol *el = NULL; t->get_type(&el);
        char eb[300] = "?"; if (el) symName(el, eb, sizeof(eb));
        ULONGLONG esz = 0; if (el) el->get_length(&esz);
        if (esz > 0)
            sprintf_s(out, n, "%s <%s[%llu]>", tb, eb, (unsigned long long)(bytes / esz));
        else
            sprintf_s(out, n, "%s <%s bytes=%llu>", tb, eb, (unsigned long long)bytes);
        if (el) el->Release();
    } else {
        strcpy_s(out, n, tb);
    }
    t->Release();
}

static void dumpChildren(IDiaSymbol *parent, int indent, int maxDepth);

static void dumpOne(IDiaSymbol *sym, int indent, int maxDepth)
{
    char nm[512], tn[400];
    DWORD tag = SymTagNull; sym->get_symTag(&tag);
    ULONGLONG size = 0; sym->get_length(&size);
    symName(sym, nm, sizeof(nm));
    typeText(sym, tn, sizeof(tn));
    printf("%*s%s %s  size=%llu\n", indent * 2, "", nm[0] ? nm : "(anon)", tn, (unsigned long long)size);
    if (indent < maxDepth) dumpChildren(sym, indent + 1, maxDepth);
    // sym cagiran tarafindan release edilir
}

static void dumpChildren(IDiaSymbol *parent, int indent, int maxDepth)
{
    IDiaEnumSymbols *en = NULL;
    if (FAILED(parent->findChildren(SymTagNull, NULL, 0, &en)) || !en) return;
    LONG cnt = 0; en->get_Count(&cnt);
    for (LONG i = 0; i < cnt; i++) {
        IDiaSymbol *c = NULL;
        if (FAILED(en->Item((DWORD)i, &c)) || !c) break;
        DWORD tag = SymTagNull; c->get_symTag(&tag);
        if (tag == SymTagData) {
            char nm[512], tn[400];
            symName(c, nm, sizeof(nm));
            typeText(c, tn, sizeof(tn));
            DWORD lt = LocIsNull; c->get_locationType(&lt);
            LONG off = 0; c->get_offset(&off);
            DWORD bp = 0; c->get_bitPosition(&bp);
            ULONGLONG bl = 0; c->get_length(&bl);
            if (bl > 0)
                printf("%*s+%-5ld %-22s %-34s %s  (bit %u:%u)\n", indent * 2, "", (long)off, nm, tn, locName(lt), bp, bl);
            else
                printf("%*s+%-5ld %-22s %-34s %s\n", indent * 2, "", (long)off, nm, tn, locName(lt));
        } else if (tag == SymTagBaseClass) {
            char nm[512], tn[400];
            symName(c, nm, sizeof(nm));
            LONG off = 0; c->get_offset(&off);
            typeText(c, tn, sizeof(tn));
            printf("%*s[base +%-4ld %-22s %s]\n", indent * 2, "", (long)off, nm, tn);
            IDiaSymbol *t = NULL;
            if (SUCCEEDED(c->get_type(&t)) && t) {
                if (indent < maxDepth) dumpChildren(t, indent + 1, maxDepth);
                t->Release();
            }
        } else if (tag == SymTagUDT) {
            dumpOne(c, indent, maxDepth);
        }
        c->Release();
    }
    en->Release();
}

int wmain(int argc, wchar_t **argv)
{
    if (argc < 3) {
        printf("kullanim: pdbtype.exe <pdb> <isim-alt-dizisi> [max-derinlik]\n");
        return 2;
    }
    const wchar_t *pdb = argv[1];
    const wchar_t *filter = argv[2];
    int maxDepth = (argc > 3) ? _wtoi(argv[3]) : 1;

    HMODULE h = LoadLibraryW(L"msdia140.dll");
    if (!h) { printf("msdia140.dll yuklenemedi: %lu\n", GetLastError()); return 3; }
    DllGetClassObjectProc gco = (DllGetClassObjectProc)GetProcAddress(h, "DllGetClassObject");
    if (!gco) { printf("DllGetClassObject yok\n"); return 3; }
    IClassFactory *cf = NULL;
    HRESULT hr = gco(__uuidof(DiaSource), __uuidof(IClassFactory), (void **)&cf);
    if (FAILED(hr) || !cf) { printf("CoCreate DIA hatasi 0x%08lX\n", (unsigned long)hr); return 3; }
    IDiaDataSource *ds = NULL;
    hr = cf->CreateInstance(NULL, __uuidof(IDiaDataSource), (void **)&ds);
    cf->Release();
    if (FAILED(hr) || !ds) { printf("CreateInstance hatasi 0x%08lX\n", (unsigned long)hr); return 3; }

    hr = ds->loadDataFromPdb(pdb);
    if (FAILED(hr)) { printf("PDB acilamadi: 0x%08lX\n", (unsigned long)hr); return 4; }
    IDiaSession *sess = NULL;
    hr = ds->openSession(&sess);
    if (FAILED(hr)) { printf("session hatasi\n"); return 4; }
    IDiaSymbol *gs = NULL;
    hr = sess->get_globalScope(&gs);
    if (FAILED(hr)) { printf("globalscope hatasi\n"); return 4; }

    IDiaEnumSymbols *udts = NULL;
    if (FAILED(gs->findChildren(SymTagUDT, NULL, 0, &udts)) || !udts) {
        printf("global UDT listesi alinamadi\n");
        return 4;
    }
    LONG total = 0; udts->get_Count(&total);
    printf("### PDB=%S globalUDT=%ld\n", (const char *)pdb, (long)total);
    fflush(stdout);

    int found = 0;
    for (LONG i = 0; i < total; i++) {
        IDiaSymbol *s = NULL;
        if (FAILED(udts->Item((DWORD)i, &s)) || !s) continue;
        char nm[512]; symName(s, nm, sizeof(nm));
        
        char f[512];
        WideCharToMultiByte(CP_UTF8, 0, filter, -1, f, (int)sizeof(f), NULL, NULL);
        if (strstr(nm, f) != NULL) { dumpOne(s, 0, maxDepth); found++; }
        s->Release();
    }
    printf("### eslesen=%d\n", found);
    udts->Release(); gs->Release(); sess->Release(); ds->Release();
    return found ? 0 : 1;
}