#pragma once

#if defined(__ANDROID__) || defined(MU_IOS)

#include <cstdint>

void MU_MobileTimeInit();
uint32_t MU_MobileGetTicks();
uint64_t MU_MobilePerfNow();
uint64_t MU_MobilePerfFrequency();
double MU_MobilePerfToSeconds(uint64_t ticks);
double MU_MobilePerfToMilliseconds(uint64_t ticks);
void MU_MobileSleep(uint32_t ms);

#else // masaustu (Win32): sokol_time yerine QueryPerformanceCounter

// Paylasimli dosyalar (ZzzObject/ZzzCharacter/ZzzScene/ZzzLodTerrain)
// MU_MobilePerfNow() ile frame sureleri olcer; Win32'de QPC birebir karsiligi
// saglar ve inline tanimlanir (link birimi gerektirmez).

inline void MU_MobileTimeInit() {}

inline unsigned int MU_MobileGetTicks()
{
    return static_cast<unsigned int>(::GetTickCount());
}

inline unsigned long long MU_MobilePerfNow()
{
    LARGE_INTEGER li;
    ::QueryPerformanceCounter(&li);
    return static_cast<unsigned long long>(li.QuadPart);
}

inline unsigned long long MU_MobilePerfFrequency()
{
    LARGE_INTEGER li;
    ::QueryPerformanceFrequency(&li);
    return static_cast<unsigned long long>(li.QuadPart);
}

inline double MU_MobilePerfToSeconds(unsigned long long ticks)
{
    return static_cast<double>(ticks) / static_cast<double>(MU_MobilePerfFrequency());
}

inline double MU_MobilePerfToMilliseconds(unsigned long long ticks)
{
    return (static_cast<double>(ticks) * 1000.0) / static_cast<double>(MU_MobilePerfFrequency());
}

inline void MU_MobileSleep(unsigned int ms)
{
    ::Sleep(ms);
}

#endif // defined(__ANDROID__) || defined(MU_IOS)
