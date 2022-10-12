#include "global.h"
#include "day_night.h"
#include "decompress.h"
#include "event_data.h"
#include "field_tasks.h"
#include "field_weather.h"
#include "overworld.h"
#include "palette.h"
#include "rtc.h"
#include "strings.h"
#include "string_util.h"
#include "constants/day_night.h"
#include "constants/region_map_sections.h"
#include "constants/rgb.h"

#define TINT_MORNING Q_8_8(0.7), Q_8_8(0.7), Q_8_8(0.9)
#define TINT_DAY Q_8_8(1.0), Q_8_8(1.0), Q_8_8(1.0)
#define TINT_NIGHT Q_8_8(0.6), Q_8_8(0.6), Q_8_8(0.92)

#define TINT_MORNING_CAVE Q_8_8(0.60), Q_8_8(0.66), Q_8_8(0.83)
#define TINT_DAY_CAVE Q_8_8(0.62), Q_8_8(0.62), Q_8_8(0.72)
#define TINT_NIGHT_CAVE Q_8_8(0.55), Q_8_8(0.59), Q_8_8(0.62)
#define TINT_DARK_CAVE Q_8_8(0.15), Q_8_8(0.16), Q_8_8(0.24)

EWRAM_DATA u16 gPlttBufferPreDN[PLTT_BUFFER_SIZE] = {0};
EWRAM_DATA struct PaletteOverride *gPaletteOverrides[4] = {NULL};

static EWRAM_DATA struct {
    bool8 initialized:1;
    bool8 retintPhase:1;
    u8 timeOfDay;
    u16 prevTintPeriod; // tint period associated with currently drawn palettes
    u16 currTintPeriod; // tint period associated with currRGBTint
    u16 currRGBTint[3];
} sDNSystemControl = {0};

#if DEBUG
EWRAM_DATA bool8 gPaletteOverrideDisabled = 0;
EWRAM_DATA s16 gDNPeriodOverride = 0;
EWRAM_DATA u16 gDNTintOverride[3] = {0};
#endif

static const u16 sTimeOfDayTints[][3] = {
    [0] =   {TINT_NIGHT},
    [1] =   {TINT_NIGHT},
    [2] =   {TINT_NIGHT},
    [3] =   {TINT_NIGHT},
    [4] =   {Q_8_8(0.6), Q_8_8(0.65), Q_8_8(1.0)},
    [5] =   {TINT_MORNING},
    [6] =   {TINT_MORNING},
    [7] =   {TINT_MORNING},
    [8] =   {Q_8_8(0.9), Q_8_8(0.85), Q_8_8(1.0)},
    [9] =   {Q_8_8(1.0), Q_8_8(0.9), Q_8_8(1.0)},
    [10] =  {TINT_DAY},
    [11] =  {TINT_DAY},
    [12] =  {Q_8_8(1.2), Q_8_8(1.2), Q_8_8(1.2)},
    [13] =  {Q_8_8(1.1), Q_8_8(1.1), Q_8_8(1.1)},
    [14] =  {TINT_DAY},
    [15] =  {TINT_DAY},
    [16] =  {TINT_DAY},
    [17] =  {Q_8_8(1.0), Q_8_8(0.98), Q_8_8(0.9)},
    [18] =  {Q_8_8(0.9), Q_8_8(0.7), Q_8_8(0.67)},
    [19] =  {Q_8_8(0.75), Q_8_8(0.66), Q_8_8(0.77)},
    [20] =  {Q_8_8(0.7), Q_8_8(0.63), Q_8_8(0.82)},
    [21] =  {TINT_NIGHT},
    [22] =  {TINT_NIGHT},
    [23] =  {TINT_NIGHT},
};

static const u16 sTimeOfDayTintsCave[][3] = {
    [0] =   {TINT_NIGHT_CAVE},
    [1] =   {TINT_NIGHT_CAVE},
    [2] =   {TINT_NIGHT_CAVE},
    [3] =   {TINT_NIGHT_CAVE},
    [4] =   {TINT_NIGHT_CAVE},
    [5] =   {TINT_MORNING_CAVE},
    [6] =   {TINT_MORNING_CAVE},
    [7] =   {TINT_MORNING_CAVE},
    [8] =   {TINT_MORNING_CAVE},
    [9] =   {TINT_DAY_CAVE},
    [10] =  {TINT_DAY_CAVE},
    [11] =  {TINT_DAY_CAVE},
    [12] =  {TINT_DAY_CAVE},
    [13] =  {TINT_DAY_CAVE},
    [14] =  {TINT_DAY_CAVE},
    [15] =  {TINT_DAY_CAVE},
    [16] =  {TINT_DAY_CAVE},
    [17] =  {TINT_DAY_CAVE},
    [18] =  {Q_8_8(0.75), Q_8_8(0.62), Q_8_8(0.68)},
    [19] =  {Q_8_8(0.75), Q_8_8(0.62), Q_8_8(0.68)},
    [20] =  {TINT_NIGHT_CAVE},
    [21] =  {TINT_NIGHT_CAVE},
    [22] =  {TINT_NIGHT_CAVE},
    [23] =  {TINT_NIGHT_CAVE},
};

u8 GetCurrentTimeOfDay(void)
{
    return GetTimeOfDay(gLocalTime.hours);
}

u8 GetTimeOfDay(s8 hours)
{
    if (hours < HOUR_MORNING)
        return TIME_NIGHT;
    else if (hours < HOUR_DAY)
        return TIME_MORNING;
    else if (hours < HOUR_NIGHT)
        return TIME_DAY;
    else
        return TIME_NIGHT;
}

static void LoadPaletteOverrides(void)
{
    u8 i, j;
    const u16* src;
    u16* dest;
    s8 hour;

#if DEBUG
    if (gPaletteOverrideDisabled)
        return;
#endif

    hour = gLocalTime.hours;

#if DEBUG
    if (gDNPeriodOverride > 0)
        hour = (gDNPeriodOverride - 1) / TINT_PERIODS_PER_HOUR;
#endif

    for (i = 0; i < ARRAY_COUNT(gPaletteOverrides); i++)
    {
        const struct PaletteOverride *curr = gPaletteOverrides[i];
        if (curr != NULL)
        {
            while (curr->slot != PALOVER_LIST_TERM && curr->palette != NULL)
            {
                if ((curr->startHour < curr->endHour && hour >= curr->startHour && hour < curr->endHour) ||
                    (curr->startHour > curr->endHour && (hour >= curr->startHour || hour < curr->endHour)))
                {
                    for (j = 0, src = curr->palette, dest = gPlttBufferUnfaded + (curr->slot * 16); j < 16; j++, src++, dest++)
                    {
                        if (*src != RGB_BLACK)
                            *dest = *src;
                    }
                }
                curr++;
            }
        }
    }
}

static bool8 ShouldTintOverworld(void)
{
    if (FlagGet(FLAG_SYS_USE_FLASH))
      return FALSE;
    if (IsMapTypeOutdoors(gMapHeader.mapType))
      return TRUE;
    else if (IsMapTypeUnderground(gMapHeader.mapType))
      return TRUE;
    // more conditions?
    return FALSE;
}

bool32 LerpColors(u16 *rgbDest, const u16 *rgb1, const u16 *rgb2, u8 coeff)
{
    bool32 ret = FALSE;
    u16 rgbTemp[3];

    memcpy(rgbTemp, rgb1, sizeof(rgbTemp));

    if (rgb1[0] != rgb2[0] ||
        rgb1[1] != rgb2[1] ||
        rgb1[2] != rgb2[2])
    {
        rgbTemp[0] = (((rgb2[0] - rgb1[0]) * coeff) / TINT_PERIODS_PER_HOUR) + rgb1[0];
        rgbTemp[1] = (((rgb2[1] - rgb1[1]) * coeff) / TINT_PERIODS_PER_HOUR) + rgb1[1];
        rgbTemp[2] = (((rgb2[2] - rgb1[2]) * coeff) / TINT_PERIODS_PER_HOUR) + rgb1[2];
    }

    if (rgbTemp[0] != rgbDest[0] ||
        rgbTemp[1] != rgbDest[1] ||
        rgbTemp[2] != rgbDest[2])
    {
        ret = TRUE;
        memcpy(rgbDest, rgbTemp, sizeof(rgbTemp));
    }

    return ret;
}

static void TintPaletteForDayNight(u16 offset, u16 size)
{
    s8 hour, nextHour;
    u8 hourPhase;
    u16 period;

    if (ShouldTintOverworld())
    {
        RtcCalcLocalTimeFast();

        hour = gLocalTime.hours;
        hourPhase = gLocalTime.minutes / MINUTES_PER_TINT_PERIOD;

#if DEBUG
        if (gDNPeriodOverride > 0)
        {
            hour = (gDNPeriodOverride - 1) / TINT_PERIODS_PER_HOUR;
            hourPhase = (gDNPeriodOverride - 1) % TINT_PERIODS_PER_HOUR;
        }
#endif

        period = (hour * TINT_PERIODS_PER_HOUR) + hourPhase;
        if (sDNSystemControl.prevTintPeriod == 0xFFFF)
        {
          CpuCopy16(gPlttBufferUnfaded, gPlttBufferFaded, PLTT_SIZE);
        }
        if (!sDNSystemControl.initialized || sDNSystemControl.currTintPeriod != period)
        {
          sDNSystemControl.initialized = TRUE;
          sDNSystemControl.currTintPeriod = period;
          nextHour = (hour + 1) % 24;
          if (IsMapTypeOutdoors(gMapHeader.mapType))
            {
              LerpColors(sDNSystemControl.currRGBTint, sTimeOfDayTints[hour], sTimeOfDayTints[nextHour], hourPhase);
            }
          if (IsMapTypeUnderground(gMapHeader.mapType) && !FlagGet(FLAG_SYS_USE_FLASH))
            {
              LerpColors(sDNSystemControl.currRGBTint, sTimeOfDayTintsCave[hour], sTimeOfDayTintsCave[nextHour], hourPhase);
            }
        }
        TintPalette_CustomToneWithCopy(gPlttBufferPreDN + offset, gPlttBufferUnfaded + offset, size / 2, sDNSystemControl.currRGBTint[0], sDNSystemControl.currRGBTint[1], sDNSystemControl.currRGBTint[2], FALSE);
    }
    else
    {
        CpuCopy16(gPlttBufferPreDN + offset, gPlttBufferUnfaded + offset, size);
    }
    LoadPaletteOverrides();
}

void LoadCompressedPaletteDayNight(const void *src, u16 offset, u16 size)
{
    LZDecompressWram(src, gPaletteDecompressionBuffer);
    CpuCopy16(gPaletteDecompressionBuffer, gPlttBufferPreDN + offset, size);
    TintPaletteForDayNight(offset, size);
    CpuCopy16(gPlttBufferUnfaded + offset, gPlttBufferFaded + offset, size);
}

void LoadPaletteDayNight(const void *src, u16 offset, u16 size)
{
    CpuCopy16(src, gPlttBufferPreDN + offset, size);
    TintPaletteForDayNight(offset, size);
    CpuCopy16(gPlttBufferUnfaded + offset, gPlttBufferFaded + offset, size);
}

void CheckClockForImmediateTimeEvents(void)
{
    if (!sDNSystemControl.retintPhase && ShouldTintOverworld())
        RtcCalcLocalTimeFast();
}

void InvalidateCurrentTint(void)
{
  sDNSystemControl.prevTintPeriod = 0xFFFF;
  ProcessImmediateTimeEvents();
}

void ProcessImmediateTimeEvents(void)
{
    s8 hour, nextHour;
    u8 hourPhase;
    u16 period;
    u8 timeOfDay = GetCurrentTimeOfDay();

    if (ShouldTintOverworld())
    {
        if (sDNSystemControl.retintPhase == 0)
        {
            hour = gLocalTime.hours;
            hourPhase = gLocalTime.minutes / MINUTES_PER_TINT_PERIOD;

#if DEBUG
            if (gDNPeriodOverride > 0)
            {
                hour = (gDNPeriodOverride - 1) / TINT_PERIODS_PER_HOUR;
                hourPhase = (gDNPeriodOverride - 1) % TINT_PERIODS_PER_HOUR;
            }
            else if (gDNTintOverride[0] > 0 ||
                     gDNTintOverride[1] > 0 ||
                     gDNTintOverride[2] > 0)
            {
                sDNSystemControl.prevTintPeriod = 0xFFFF; // invalidate current tint

                if (gDNTintOverride[0] == 0xFFFF) // signal to invalidate when turning off override
                {
                    gDNTintOverride[0] = 0;
                }
            }
#endif

            period = (hour * TINT_PERIODS_PER_HOUR) + hourPhase;

            if (!sDNSystemControl.initialized || sDNSystemControl.prevTintPeriod != period)
            {
                sDNSystemControl.initialized = TRUE;
                sDNSystemControl.prevTintPeriod = sDNSystemControl.currTintPeriod = period;
#if DEBUG
                if (gDNTintOverride[0] > 0 ||
                    gDNTintOverride[1] > 0 ||
                    gDNTintOverride[2] > 0)
                {
                    sDNSystemControl.currRGBTint[0] = gDNTintOverride[0];
                    sDNSystemControl.currRGBTint[1] = gDNTintOverride[1];
                    sDNSystemControl.currRGBTint[2] = gDNTintOverride[2];
                }
                else
#endif
                {
                  nextHour = (hour + 1) % 24;
                  if (IsMapTypeOutdoors(gMapHeader.mapType))
                    {
                      LerpColors(sDNSystemControl.currRGBTint, sTimeOfDayTints[hour], sTimeOfDayTints[nextHour], hourPhase);
                    }
                  if (IsMapTypeUnderground(gMapHeader.mapType) && !FlagGet(FLAG_SYS_USE_FLASH))
                    {
                      LerpColors(sDNSystemControl.currRGBTint, sTimeOfDayTintsCave[hour], sTimeOfDayTintsCave[nextHour], hourPhase);
                    }
                }

                TintPalette_CustomToneWithCopy(gPlttBufferPreDN, gPlttBufferUnfaded, BG_PLTT_SIZE / 2, sDNSystemControl.currRGBTint[0], sDNSystemControl.currRGBTint[1], sDNSystemControl.currRGBTint[2], TRUE);
                sDNSystemControl.retintPhase = 1;
            }
        }
        else
        {
            sDNSystemControl.retintPhase = 0;
            TintPalette_CustomToneWithCopy(gPlttBufferPreDN + (BG_PLTT_SIZE / 2), gPlttBufferUnfaded + (BG_PLTT_SIZE / 2), OBJ_PLTT_SIZE / 2, sDNSystemControl.currRGBTint[0], sDNSystemControl.currRGBTint[1], sDNSystemControl.currRGBTint[2], TRUE);
            LoadPaletteOverrides();

            if (gWeatherPtr->palProcessingState != WEATHER_PAL_STATE_SCREEN_FADING_IN &&
                gWeatherPtr->palProcessingState != WEATHER_PAL_STATE_SCREEN_FADING_OUT)
                CpuCopy16(gPlttBufferUnfaded, gPlttBufferFaded, PLTT_SIZE);
        }
    }

    if (sDNSystemControl.timeOfDay != timeOfDay)
    {
        sDNSystemControl.timeOfDay = timeOfDay;
        ChooseAmbientCrySpecies();  // so a time-of-day appropriate mon is chosen
        ForceTimeBasedEvents();     // for misc events that should run on time of day boundaries
    }
}

void DoLoadSpritePaletteDayNight(const u16 *src, u16 paletteOffset)
{
    LoadPaletteDayNight(src, paletteOffset + 0x100, 32);
}