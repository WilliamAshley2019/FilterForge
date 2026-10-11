#include "PresetManager.h"
#include "PluginProcessor.h"

//==============================================================================
// Factory preset bank
//
// Categories: Clean, KR-106, Ladder, HPF/Shelf, Comb, Creative
//==============================================================================
namespace
{
    using P = std::pair<const char*, float>;

    PresetManager::FactoryPreset makePreset(const char* name,
        const char* category,
        std::initializer_list<P> pairs)
    {
        PresetManager::FactoryPreset fp;
        fp.name = name;
        fp.category = category;
        fp.author = "Factory";
        for (auto& p : pairs)
            fp.params.push_back({ juce::String(p.first), p.second });
        return fp;
    }
}

const std::vector<PresetManager::FactoryPreset>&
PresetManager::getFactoryPresets()
{
    using namespace FilterParams;

    static const std::vector<FactoryPreset> presets = []()
        {
            std::vector<FactoryPreset> v;

            // =========================================================
            // CLEAN / UTILITY
            // =========================================================
            v.push_back(makePreset("Init (Clean)", "Clean", {
                { F1_MODEL, 0.f },{ F1_TYPE, 1.f },{ F1_CUTOFF, 1000.f },
                { F1_RESO, 0.3f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 5.f },{ F2_TYPE, 0.f },{ F2_CUTOFF, 5000.f },
                { F2_RESO, 0.f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 0.f },
                { ROUTING, 0.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 1.f },{ OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Transparent LP", "Clean", {
                { F1_MODEL, 5.f },{ F1_TYPE, 0.f },{ F1_CUTOFF, 12000.f },
                { F1_RESO, 0.f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 0.f },{ F2_TYPE, 1.f },{ F2_CUTOFF, 1000.f },
                { F2_RESO, 0.3f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 0.f },
                { ROUTING, 2.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 1.f },{ OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Gentle LP24", "Clean", {
                { F1_MODEL, 0.f },{ F1_TYPE, 1.f },{ F1_CUTOFF, 8000.f },
                { F1_RESO, 0.f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 5.f },{ F2_TYPE, 0.f },{ F2_CUTOFF, 5000.f },
                { F2_RESO, 0.f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 0.f },
                { ROUTING, 2.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 1.f },{ OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("High Pass Cleanup", "Clean", {
                { F1_MODEL, 5.f },{ F1_TYPE, 2.f },{ F1_CUTOFF, 120.f },
                { F1_RESO, 0.f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 0.f },{ F2_TYPE, 1.f },{ F2_CUTOFF, 1000.f },
                { F2_RESO, 0.3f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 0.f },
                { ROUTING, 2.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 1.f },{ OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            // =========================================================
            // KR-106 / JUNO
            // =========================================================
            v.push_back(makePreset("Juno 106 LP", "KR-106", {
                { F1_MODEL, 0.f },{ F1_TYPE, 1.f },{ F1_CUTOFF, 2000.f },
                { F1_RESO, 0.2f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 0.f },{ F2_TYPE, 1.f },{ F2_CUTOFF, 1000.f },
                { F2_RESO, 0.3f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 0.f },
                { ROUTING, 2.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 1.f },{ OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Juno 106 Resonant", "KR-106", {
                { F1_MODEL, 0.f },{ F1_TYPE, 1.f },{ F1_CUTOFF, 800.f },
                { F1_RESO, 0.7f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 0.f },{ F2_TYPE, 1.f },{ F2_CUTOFF, 1000.f },
                { F2_RESO, 0.3f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 0.f },
                { ROUTING, 2.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 1.f },{ OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Juno Pad Sweep", "KR-106", {
                { F1_MODEL, 0.f },{ F1_TYPE, 1.f },{ F1_CUTOFF, 400.f },
                { F1_RESO, 0.5f },{ F1_DRIVE, 0.1f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 3.f },{ F2_TYPE, 0.f },{ F2_CUTOFF, 1000.f },
                { F2_RESO, 0.5f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 1.f },
                { ROUTING, 0.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 1.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 0.3f },
                { LFO_AMOUNT, 0.4f },{ LFO_TARGET, 0.f },{ LFO_DELAY, 0.5f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 1.f },{ OUTPUT, -2.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Juno Bass Boost", "KR-106", {
                { F1_MODEL, 4.f },{ F1_TYPE, 0.f },{ F1_CUTOFF, 1000.f },
                { F1_RESO, 0.f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 0.f },{ F2_TYPE, 1.f },{ F2_CUTOFF, 3000.f },
                { F2_RESO, 0.3f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 0.f },
                { ROUTING, 2.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 1.f },{ OUTPUT, -3.f },{ BYPASS, 0.f }
                }));

            // =========================================================
            // WASP LADDER
            // =========================================================
            v.push_back(makePreset("Wasp Ladder Clean", "Ladder", {
                { F1_MODEL, 1.f },{ F1_TYPE, 1.f },{ F1_CUTOFF, 5000.f },
                { F1_RESO, 0.f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 0.f },{ F2_TYPE, 1.f },{ F2_CUTOFF, 1000.f },
                { F2_RESO, 0.3f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 0.f },
                { ROUTING, 2.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 1.f },{ OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Wasp Ladder Screamer", "Ladder", {
                { F1_MODEL, 1.f },{ F1_TYPE, 1.f },{ F1_CUTOFF, 1500.f },
                { F1_RESO, 0.85f },{ F1_DRIVE, 1.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 0.f },{ F2_TYPE, 1.f },{ F2_CUTOFF, 1000.f },
                { F2_RESO, 0.3f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 0.f },
                { ROUTING, 2.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 1.f },{ OUTPUT, -6.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Wasp CMOS Crunch", "Ladder", {
                { F1_MODEL, 1.f },{ F1_TYPE, 1.f },{ F1_CUTOFF, 2500.f },
                { F1_RESO, 0.6f },{ F1_DRIVE, 1.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 0.f },{ F2_TYPE, 1.f },{ F2_CUTOFF, 1000.f },
                { F2_RESO, 0.3f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 0.f },
                { ROUTING, 2.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 1.f },{ OUTPUT, -3.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Wasp Bandpass Lead", "Ladder", {
                { F1_MODEL, 1.f },{ F1_TYPE, 4.f },{ F1_CUTOFF, 1200.f },
                { F1_RESO, 0.4f },{ F1_DRIVE, 0.3f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 0.f },{ F2_TYPE, 1.f },{ F2_CUTOFF, 1000.f },
                { F2_RESO, 0.3f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 0.f },
                { ROUTING, 2.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 1.f },{ OUTPUT, -3.f },{ BYPASS, 0.f }
                }));

            // =========================================================
            // HPF / SHELF
            // =========================================================
            v.push_back(makePreset("Juno HPF 236 Hz", "HPF/Shelf", {
                { F1_MODEL, 3.f },{ F1_TYPE, 0.f },{ F1_CUTOFF, 1000.f },
                { F1_RESO, 0.67f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 0.f },{ F2_TYPE, 1.f },{ F2_CUTOFF, 1000.f },
                { F2_RESO, 0.3f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 0.f },
                { ROUTING, 2.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 1.f },{ OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Juno HPF 754 Hz", "HPF/Shelf", {
                { F1_MODEL, 3.f },{ F1_TYPE, 0.f },{ F1_CUTOFF, 1000.f },
                { F1_RESO, 1.f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 0.f },{ F2_TYPE, 1.f },{ F2_CUTOFF, 1000.f },
                { F2_RESO, 0.3f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 0.f },
                { ROUTING, 2.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 1.f },{ OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Sub Bass Boost", "HPF/Shelf", {
                { F1_MODEL, 4.f },{ F1_TYPE, 0.f },{ F1_CUTOFF, 1000.f },
                { F1_RESO, 0.f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 5.f },{ F2_TYPE, 0.f },{ F2_CUTOFF, 12000.f },
                { F2_RESO, 0.f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 1.f },
                { ROUTING, 0.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 1.f },{ OUTPUT, -2.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Air Boost HP", "HPF/Shelf", {
                { F1_MODEL, 3.f },{ F1_TYPE, 0.f },{ F1_CUTOFF, 1000.f },
                { F1_RESO, 0.f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 5.f },{ F2_TYPE, 2.f },{ F2_CUTOFF, 6000.f },
                { F2_RESO, 0.2f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 1.f },
                { ROUTING, 0.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 0.5f },{ OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            // =========================================================
            // COMB
            // =========================================================
            v.push_back(makePreset("Comb Resonance 440", "Comb", {
                { F1_MODEL, 6.f },{ F1_TYPE, 0.f },{ F1_CUTOFF, 440.f },
                { F1_RESO, 0.5f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 0.f },{ F2_TYPE, 1.f },{ F2_CUTOFF, 1000.f },
                { F2_RESO, 0.3f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 0.f },
                { ROUTING, 2.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 0.6f },{ OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Comb Tuned Low", "Comb", {
                { F1_MODEL, 6.f },{ F1_TYPE, 0.f },{ F1_CUTOFF, 110.f },
                { F1_RESO, 0.7f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 5.f },{ F2_TYPE, 0.f },{ F2_CUTOFF, 8000.f },
                { F2_RESO, 0.f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 1.f },
                { ROUTING, 0.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 0.5f },{ OUTPUT, 0.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Comb Sweep Pad", "Comb", {
                { F1_MODEL, 6.f },{ F1_TYPE, 0.f },{ F1_CUTOFF, 220.f },
                { F1_RESO, 0.85f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 0.f },{ F2_TYPE, 1.f },{ F2_CUTOFF, 1000.f },
                { F2_RESO, 0.3f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 0.f },
                { ROUTING, 2.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 1.f },{ LFO_SHAPE, 1.f },{ LFO_RATE, 0.2f },
                { LFO_AMOUNT, 0.6f },{ LFO_TARGET, 0.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 0.7f },{ OUTPUT, -3.f },{ BYPASS, 0.f }
                }));

            // =========================================================
            // CREATIVE COMBOS
            // =========================================================
            v.push_back(makePreset("Wah LFO Lead", "Creative", {
                { F1_MODEL, 2.f },{ F1_TYPE, 4.f },{ F1_CUTOFF, 800.f },
                { F1_RESO, 0.6f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 5.f },{ F2_TYPE, 0.f },{ F2_CUTOFF, 8000.f },
                { F2_RESO, 0.f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 0.f },
                { ROUTING, 2.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 1.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 2.5f },
                { LFO_AMOUNT, 0.65f },{ LFO_TARGET, 0.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 1.f },{ OUTPUT, -3.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Envelope Auto-Wah", "Creative", {
                { F1_MODEL, 2.f },{ F1_TYPE, 4.f },{ F1_CUTOFF, 400.f },
                { F1_RESO, 0.6f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 5.f },{ F2_TYPE, 0.f },{ F2_CUTOFF, 8000.f },
                { F2_RESO, 0.f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 0.f },
                { ROUTING, 2.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 1.f },{ ENV_AMOUNT, 0.7f },{ ENV_ATTACK, 0.05f },
                { ENV_RELEASE, 0.5f },{ ENV_TARGET, 0.f },
                { MIX, 1.f },{ OUTPUT, -2.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Parallel Dirty+Clean", "Creative", {
                { F1_MODEL, 1.f },{ F1_TYPE, 1.f },{ F1_CUTOFF, 1200.f },
                { F1_RESO, 0.7f },{ F1_DRIVE, 1.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 5.f },{ F2_TYPE, 0.f },{ F2_CUTOFF, 10000.f },
                { F2_RESO, 0.f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 1.f },
                { ROUTING, 1.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 1.f },{ OUTPUT, -4.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Filter Chaos", "Creative", {
                { F1_MODEL, 2.f },{ F1_TYPE, 4.f },{ F1_CUTOFF, 600.f },
                { F1_RESO, 0.9f },{ F1_DRIVE, 0.5f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 6.f },{ F2_TYPE, 0.f },{ F2_CUTOFF, 300.f },
                { F2_RESO, 0.85f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 1.f },
                { ROUTING, 1.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 1.f },{ LFO_SHAPE, 5.f },{ LFO_RATE, 4.f },
                { LFO_AMOUNT, 0.5f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 1.f },{ ENV_AMOUNT, 0.4f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 0.6f },{ OUTPUT, -6.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Dubstep Wobble", "Creative", {
                { F1_MODEL, 1.f },{ F1_TYPE, 1.f },{ F1_CUTOFF, 300.f },
                { F1_RESO, 0.85f },{ F1_DRIVE, 0.7f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 5.f },{ F2_TYPE, 0.f },{ F2_CUTOFF, 12000.f },
                { F2_RESO, 0.f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 1.f },
                { ROUTING, 0.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 1.f },{ LFO_SHAPE, 1.f },{ LFO_RATE, 6.f },
                { LFO_AMOUNT, 0.85f },{ LFO_TARGET, 0.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 1.f },{ OUTPUT, -5.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Vocal Formant", "Creative", {
                { F1_MODEL, 5.f },{ F1_TYPE, 4.f },{ F1_CUTOFF, 800.f },
                { F1_RESO, 0.85f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 5.f },{ F2_TYPE, 4.f },{ F2_CUTOFF, 2400.f },
                { F2_RESO, 0.85f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 1.f },
                { ROUTING, 1.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 0.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 1.f },
                { LFO_AMOUNT, 0.f },{ LFO_TARGET, 2.f },{ LFO_DELAY, 0.f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 0.8f },{ OUTPUT, -3.f },{ BYPASS, 0.f }
                }));

            v.push_back(makePreset("Echo Band Selector", "Creative", {
                { F1_MODEL, 5.f },{ F1_TYPE, 4.f },{ F1_CUTOFF, 1500.f },
                { F1_RESO, 0.75f },{ F1_DRIVE, 0.f },{ F1_KEYTRACK, 0.f },
                { F1_ON, 1.f },
                { F2_MODEL, 0.f },{ F2_TYPE, 1.f },{ F2_CUTOFF, 1000.f },
                { F2_RESO, 0.3f },{ F2_DRIVE, 0.f },{ F2_KEYTRACK, 0.f },
                { F2_ON, 0.f },
                { ROUTING, 2.f },{ PARALLEL_MIX, 0.5f },
                { LFO_ON, 1.f },{ LFO_SHAPE, 0.f },{ LFO_RATE, 0.15f },
                { LFO_AMOUNT, 0.9f },{ LFO_TARGET, 0.f },{ LFO_DELAY, 1.0f },
                { ENV_ON, 0.f },{ ENV_AMOUNT, 0.f },{ ENV_ATTACK, 0.1f },
                { ENV_RELEASE, 0.3f },{ ENV_TARGET, 2.f },
                { MIX, 0.85f },{ OUTPUT, -3.f },{ BYPASS, 0.f }
                }));

            return v;
        }();

    return presets;
}

//==============================================================================
// Constructor
//==============================================================================
PresetManager::PresetManager(juce::AudioProcessorValueTreeState& a)
    : apvts(a)
{
    refresh();
}

//==============================================================================
// refresh — rebuild the internal preset list
//==============================================================================
void PresetManager::refresh()
{
    presets.clear();

    // Factory presets first
    const auto& factory = getFactoryPresets();
    for (int i = 0; i < (int)factory.size(); ++i)
    {
        PresetInfo info;
        info.name = factory[(size_t)i].name;
        info.category = factory[(size_t)i].category;
        info.author = factory[(size_t)i].author;
        info.isFactory = true;
        info.factoryIndex = i;
        presets.push_back(info);
    }

    // Then user presets from disk
    scanUserPresets();
}

//==============================================================================
// scanUserPresets
//==============================================================================
void PresetManager::scanUserPresets()
{
    auto dir = getUserPresetDirectory();
    if (!dir.isDirectory()) return;

    juce::Array<juce::File> files;
    dir.findChildFiles(files, juce::File::findFiles, false, "*.filterforge");

    // Sort alphabetically
    std::sort(files.begin(), files.end(),
        [](const juce::File& a, const juce::File& b)
        {
            return a.getFileNameWithoutExtension().compareNatural(
                b.getFileNameWithoutExtension()) < 0;
        });

    for (auto& f : files)
    {
        juce::String name, category, author;
        if (readUserPresetMetadata(f, name, category, author))
        {
            PresetInfo info;
            info.name = name;
            info.category = category;
            info.author = author;
            info.isFactory = false;
            info.factoryIndex = -1;
            info.file = f;
            presets.push_back(info);
        }
    }
}

//==============================================================================
// Query helpers
//==============================================================================
int PresetManager::getNumPresets() const
{
    return (int)presets.size();
}

const PresetManager::PresetInfo& PresetManager::getPresetInfo(int index) const
{
    jassert(index >= 0 && index < (int)presets.size());
    return presets[(size_t)index];
}

//==============================================================================
// getCurrentPresetIndex — best-effort match against current APVTS state
//==============================================================================
int PresetManager::getCurrentPresetIndex() const
{
    const auto& factory = getFactoryPresets();
    for (int i = 0; i < (int)factory.size(); ++i)
    {
        bool match = true;
        for (auto& kv : factory[(size_t)i].params)
        {
            auto* param = apvts.getParameter(kv.first);
            if (!param) { match = false; break; }
            const float natural = param->getNormalisableRange().convertFrom0to1(
                param->getValue());
            if (std::abs(natural - kv.second) > 1e-3f)
            {
                match = false;
                break;
            }
        }
        if (match) return i;
    }
    return -1;
}

//==============================================================================
// loadPreset
//==============================================================================
bool PresetManager::loadPreset(int index)
{
    if (index < 0 || index >= (int)presets.size()) return false;
    const auto& info = presets[(size_t)index];

    if (info.isFactory)
    {
        const auto& factory = getFactoryPresets();
        if (info.factoryIndex < 0 || info.factoryIndex >= (int)factory.size())
            return false;

        for (auto& kv : factory[(size_t)info.factoryIndex].params)
        {
            if (auto* p = apvts.getParameter(kv.first))
                p->setValueNotifyingHost(
                    p->getNormalisableRange().convertTo0to1(kv.second));
        }
        return true;
    }
    else
    {
        return loadPresetFromFile(info.file);
    }
}

//==============================================================================
// loadPresetFromFile
//==============================================================================
bool PresetManager::loadPresetFromFile(const juce::File& file)
{
    if (!file.existsAsFile()) return false;
    auto xml = juce::XmlDocument::parse(file);
    if (xml == nullptr) return false;
    return applyPresetXml(*xml);
}

//==============================================================================
// saveUserPreset
//==============================================================================
juce::File PresetManager::saveUserPreset(const juce::String& name,
    const juce::String& category,
    const juce::String& author,
    bool allowOverwrite)
{
    auto dir = getUserPresetDirectory();
    if (!dir.isDirectory())
        dir.createDirectory();

    auto safeName = juce::File::createLegalFileName(name);
    auto target = dir.getChildFile(safeName + ".filterforge");

    if (target.existsAsFile() && !allowOverwrite)
        return {};

    if (auto xml = writePresetXml(name, category, author))
        xml->writeTo(target);

    refresh();
    return target;
}

//==============================================================================
// exportPresetToFile
//==============================================================================
bool PresetManager::exportPresetToFile(const juce::File& destination,
    const juce::String& name,
    const juce::String& category,
    const juce::String& author)
{
    if (auto xml = writePresetXml(name, category, author))
        return xml->writeTo(destination);
    return false;
}

//==============================================================================
// deletePreset
//==============================================================================
bool PresetManager::deletePreset(int index)
{
    if (index < 0 || index >= (int)presets.size()) return false;
    const auto& info = presets[(size_t)index];
    if (info.isFactory) return false;

    if (info.file.existsAsFile())
    {
        info.file.deleteFile();
        refresh();
        return true;
    }
    return false;
}

//==============================================================================
// getUserPresetDirectory
//==============================================================================
juce::File PresetManager::getUserPresetDirectory() const
{
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("FilterForge")
        .getChildFile("Presets");
}

//==============================================================================
// XML serialization
//==============================================================================
void PresetManager::writeParametersToXml(juce::XmlElement& parent) const
{
    auto* paramsXml = parent.createNewChildElement("PARAMETERS");
    for (auto* param : apvts.processor.getParameters())
    {
        if (auto* p = dynamic_cast<juce::RangedAudioParameter*>(param))
        {
            auto* pXml = paramsXml->createNewChildElement("PARAM");
            pXml->setAttribute("id", p->getParameterID());
            pXml->setAttribute("value", (double)p->getValue());
        }
    }
}

void PresetManager::applyParametersFromXml(const juce::XmlElement& parent)
{
    if (auto* paramsXml = parent.getChildByName("PARAMETERS"))
    {
        for (auto* pXml : paramsXml->getChildWithTagNameIterator("PARAM"))
        {
            auto id = pXml->getStringAttribute("id");
            auto val = (float)pXml->getDoubleAttribute("value");
            if (auto* p = apvts.getParameter(id))
                p->setValueNotifyingHost(val);
        }
    }
}

//==============================================================================
// XML write / read
//==============================================================================
std::unique_ptr<juce::XmlElement> PresetManager::writePresetXml(
    const juce::String& name,
    const juce::String& category,
    const juce::String& author) const
{
    auto xml = std::make_unique<juce::XmlElement>("FILTERFORGE_PRESET");
    xml->setAttribute("version", 1);
    xml->setAttribute("name", name);
    xml->setAttribute("category", category);
    xml->setAttribute("author", author);
    writeParametersToXml(*xml);
    return xml;
}

bool PresetManager::applyPresetXml(const juce::XmlElement& xml)
{
    if (!xml.hasTagName("FILTERFORGE_PRESET")) return false;
    applyParametersFromXml(xml);
    return true;
}

bool PresetManager::readUserPresetMetadata(const juce::File& file,
    juce::String& name,
    juce::String& category,
    juce::String& author) const
{
    auto xml = juce::XmlDocument::parse(file);
    if (xml == nullptr) return false;
    if (!xml->hasTagName("FILTERFORGE_PRESET")) return false;

    name = xml->getStringAttribute("name", file.getFileNameWithoutExtension());
    category = xml->getStringAttribute("category", "User");
    author = xml->getStringAttribute("author", "User");
    return true;
}