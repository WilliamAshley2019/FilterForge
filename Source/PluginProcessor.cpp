#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PresetManager.h"

//==============================================================================
// Parameter layout
//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
FilterForgeProcessor::createParameterLayout()
{
    using namespace FilterParams;
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // --- Filter 1 ---
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ F1_MODEL, 1 }, "F1 Model",
        juce::StringArray{ "KR-106 VCF", "Wasp Ladder", "Chamberlin",
                           "KR-106 HPF", "Bass Boost", "SVF 12dB",
                           "Comb" }, 0));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ F1_TYPE, 1 }, "F1 Type",
        juce::StringArray{ "LP12", "LP24", "HP12", "HP24",
                           "BP12", "BP24", "Notch" }, 1));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ F1_CUTOFF, 1 }, "F1 Cutoff",
        juce::NormalisableRange<float>(20.0f, 18000.0f, 0.1f, 0.3f),
        1000.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ F1_RESO, 1 }, "F1 Resonance",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.3f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ F1_DRIVE, 1 }, "F1 Drive",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ F1_KEYTRACK, 1 }, "F1 Keytrack",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ F1_ON, 1 }, "F1 On", true));

    // --- Filter 2 ---
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ F2_MODEL, 1 }, "F2 Model",
        juce::StringArray{ "KR-106 VCF", "Wasp Ladder", "Chamberlin",
                           "KR-106 HPF", "Bass Boost", "SVF 12dB",
                           "Comb" }, 5));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ F2_TYPE, 1 }, "F2 Type",
        juce::StringArray{ "LP12", "LP24", "HP12", "HP24",
                           "BP12", "BP24", "Notch" }, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ F2_CUTOFF, 1 }, "F2 Cutoff",
        juce::NormalisableRange<float>(20.0f, 18000.0f, 0.1f, 0.3f),
        5000.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ F2_RESO, 1 }, "F2 Resonance",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ F2_DRIVE, 1 }, "F2 Drive",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ F2_KEYTRACK, 1 }, "F2 Keytrack",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ F2_ON, 1 }, "F2 On", false));

    // --- Routing ---
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ ROUTING, 1 }, "Routing",
        juce::StringArray{ "Series (1->2)", "Parallel", "F1 Only",
                           "F2 Only" }, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ PARALLEL_MIX, 1 }, "Parallel Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    // --- LFO ---
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ LFO_ON, 1 }, "LFO On", false));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ LFO_SHAPE, 1 }, "LFO Shape",
        juce::StringArray{ "Sine", "Triangle", "Saw", "RevSaw",
                           "Square", "S&H" }, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ LFO_RATE, 1 }, "LFO Rate",
        juce::NormalisableRange<float>(0.01f, 20.0f, 0.001f, 0.4f), 1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ LFO_AMOUNT, 1 }, "LFO Amount",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ LFO_TARGET, 1 }, "LFO Target",
        juce::StringArray{ "F1", "F2", "Both" }, 2));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ LFO_DELAY, 1 }, "LFO Delay",
        juce::NormalisableRange<float>(0.0f, 4.0f, 0.001f, 0.5f), 0.0f));

    // --- Envelope Follower ---
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ ENV_ON, 1 }, "Env On", false));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ENV_AMOUNT, 1 }, "Env Amount",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.001f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ENV_ATTACK, 1 }, "Env Attack",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.1f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ENV_RELEASE, 1 }, "Env Release",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.3f));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ ENV_TARGET, 1 }, "Env Target",
        juce::StringArray{ "F1", "F2", "Both" }, 2));

    // --- Output ---
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ MIX, 1 }, "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ OUTPUT, 1 }, "Output",
        juce::NormalisableRange<float>(-24.0f, 12.0f, 0.01f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ BYPASS, 1 }, "Bypass", false));

    return layout;
}

//==============================================================================
// Constructor / destructor
//==============================================================================
FilterForgeProcessor::FilterForgeProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    apvts(*this, nullptr, "FilterForgeState", createParameterLayout())
{
    for (auto* param : apvts.processor.getParameters())
    {
        if (auto* p = dynamic_cast<juce::RangedAudioParameter*>(param))
            apvts.addParameterListener(p->getParameterID(), this);
    }
    presetManager = std::make_unique<PresetManager>(apvts);
}

FilterForgeProcessor::~FilterForgeProcessor()
{
    presetManager.reset();
    onPresetChanged = nullptr;

    for (auto* param : apvts.processor.getParameters())
    {
        if (auto* p = dynamic_cast<juce::RangedAudioParameter*>(param))
            apvts.removeParameterListener(p->getParameterID(), this);
    }
}

//==============================================================================
// prepareToPlay / releaseResources
//==============================================================================
void FilterForgeProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = juce::jlimit(8000.0, 384000.0, sampleRate);
    currentBlockSize = juce::jmax(1, samplesPerBlock);

    // Prepare all filter models for both channels
    auto prepAll = [&](auto&... filters) {
        (filters.prepare(currentSampleRate), ...);
        };

    prepAll(kr106F1L, kr106F1R, kr106F2L, kr106F2R);
    prepAll(waspF1L, waspF1R, waspF2L, waspF2R);
    prepAll(chamF1L, chamF1R, chamF2L, chamF2R);
    prepAll(hpfF1L, hpfF1R, hpfF2L, hpfF2R);
    prepAll(bassF1L, bassF1R, bassF2L, bassF2R);
    prepAll(svfF1L, svfF1R, svfF2L, svfF2R);
    prepAll(combF1L, combF1R, combF2L, combF2R);

    lfoPhase = 0.f;
    lfoDelayTimer = 0.f;
    envFollower = 0.f;

    smoothedOutput.reset(currentSampleRate, 0.02);
    smoothedMix.reset(currentSampleRate, 0.02);

    updateSnapshot();

    levelPeakL.store(0.0f);
    levelPeakR.store(0.0f);

    isPrepared = true;
}

void FilterForgeProcessor::releaseResources() { isPrepared = false; }

bool FilterForgeProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& out = layouts.getMainOutputChannelSet();
    const auto& in = layouts.getMainInputChannelSet();
    if (out != juce::AudioChannelSet::stereo() && out != juce::AudioChannelSet::mono())
        return false;
    if (in != juce::AudioChannelSet::stereo() && in != juce::AudioChannelSet::mono())
        return false;
    return true;
}

//==============================================================================
// parameterChanged
//==============================================================================
void FilterForgeProcessor::parameterChanged(const juce::String&, float)
{
    updateSnapshot();
}

//==============================================================================
// updateSnapshot — push current params into the atomics for the display
//==============================================================================
void FilterForgeProcessor::updateSnapshot()
{
    using namespace FilterParams;

    snapshot.f1Model.store((int)apvts.getRawParameterValue(F1_MODEL)->load());
    snapshot.f1Type.store((int)apvts.getRawParameterValue(F1_TYPE)->load());
    snapshot.f1Cutoff.store(apvts.getRawParameterValue(F1_CUTOFF)->load());
    snapshot.f1Reso.store(apvts.getRawParameterValue(F1_RESO)->load());
    snapshot.f1On.store(apvts.getRawParameterValue(F1_ON)->load() > 0.5f);

    snapshot.f2Model.store((int)apvts.getRawParameterValue(F2_MODEL)->load());
    snapshot.f2Type.store((int)apvts.getRawParameterValue(F2_TYPE)->load());
    snapshot.f2Cutoff.store(apvts.getRawParameterValue(F2_CUTOFF)->load());
    snapshot.f2Reso.store(apvts.getRawParameterValue(F2_RESO)->load());
    snapshot.f2On.store(apvts.getRawParameterValue(F2_ON)->load() > 0.5f);

    snapshot.routing.store((int)apvts.getRawParameterValue(ROUTING)->load());
    snapshot.parallelMix.store(apvts.getRawParameterValue(PARALLEL_MIX)->load());

    snapshot.lfoAmount.store(apvts.getRawParameterValue(LFO_AMOUNT)->load());
    snapshot.envAmount.store(apvts.getRawParameterValue(ENV_AMOUNT)->load());

    snapshot.sampleRate.store((float)currentSampleRate);
}

//==============================================================================
// evaluateResponse — analytic filter magnitude at a given frequency, for the
// display. This does NOT use the running filter state — it computes the
// theoretical response for the current parameter values.
//==============================================================================
float FilterForgeProcessor::evaluateResponse(float freqHz) const
{
    using namespace FilterDSP;

    const float sr = snapshot.sampleRate.load();
    const float nyq = sr * 0.5f;
    const float w = juce::jlimit(20.f, nyq * 0.95f, freqHz) / nyq;  // 0..1
    const float wn = w * w;

    auto respFor = [&](int model, int type, float cutoff, float reso) -> float
        {
            // Normalise cutoff to 0..1
            const float fc = juce::jlimit(20.f, nyq * 0.95f, cutoff) / nyq;
            const float r = juce::jlimit(0.f, 1.f, reso);

            if (model == 0)  // KR-106 VCF — 4-pole LP
            {
                // Approximate with a 4-pole Butterworth-shaped response plus
                // resonance bump near cutoff
                const float ratio = (w > 1e-6f) ? (fc / w) : 1000.f;
                const float r2 = ratio * ratio;
                const float base = 1.0f / std::sqrt(1.0f + r2 * r2 *
                    (1.0f + 2.f * r2 + r2 * r2));
                // Resonance peak
                const float q = 0.5f + r * 8.f;
                const float peak = std::exp(-0.5f * std::pow(
                    std::log(ratio + 1e-6f) * q * 0.4f, 2.f));
                return juce::jlimit(0.f, 4.f, base + peak * r * 2.5f);
            }
            if (model == 1)  // Wasp Ladder
            {
                const float ratio = (w > 1e-6f) ? (fc / w) : 1000.f;
                const float k = r * 3.99f;
                // 4-pole with feedback
                const float denom = 1.f + std::pow(ratio, 4.f) *
                    (1.f + k);
                const float base = 1.0f / std::sqrt(denom);
                const float peak = std::exp(-0.5f * std::pow(
                    std::log(ratio + 1e-6f) * 4.0f, 2.f));
                return juce::jlimit(0.f, 4.f, base + peak * r * 3.0f);
            }
            if (model == 2)  // Chamberlin
            {
                const float ratio = (w > 1e-6f) ? (fc / w) : 1000.f;
                const float Q = 0.5f + r * 19.5f;
                float mag = 0.f;
                switch (type)
                {
                case 0: case 1:  // LP
                    mag = 1.0f / std::sqrt(1.0f + ratio * ratio * ratio * ratio);
                    break;
                case 2: case 3:  // HP
                    mag = (w / fc) / std::sqrt(1.0f +
                        std::pow(w / fc, 4.f));
                    break;
                default:  // BP
                    mag = (w / fc) / std::sqrt(1.0f +
                        std::pow((w / fc - fc / juce::jmax(w, 1e-6f)) * Q, 2.f));
                    break;
                }
                return juce::jlimit(0.f, 4.f, mag);
            }
            if (model == 3)  // KR-106 HPF — 4-position
            {
                const int pos = juce::jlimit(0, 3, (int)(r * 3.999f));
                const float hpfHz = getJuno106HPFFreq(pos);
                if (hpfHz < 0.f)  // bass boost
                    return 1.0f + 0.5f / (1.f + std::pow(w * 100.f, 2.f));
                if (hpfHz == 0.f) return 1.0f;  // flat
                const float hfc = hpfHz / nyq;
                return juce::jlimit(0.f, 4.f,
                    (w / hfc) / std::sqrt(1.f + std::pow(w / hfc, 2.f)));
            }
            if (model == 4)  // Bass Boost
            {
                // Low shelf: 1.0 at HF, up to ~2.0 at DC
                return 1.0f + 1.0f / (1.f + std::pow(w * 50.f, 2.f));
            }
            if (model == 5)  // SVF 12dB
            {
                const float ratio = (w > 1e-6f) ? (fc / w) : 1000.f;
                const float Q = 0.5f + r * 19.5f;
                float mag = 0.f;
                switch (type)
                {
                case 0: case 1:  // LP
                    mag = 1.0f / std::sqrt(1.0f + ratio * ratio * ratio * ratio);
                    break;
                case 2: case 3:  // HP
                    mag = (w / fc) / std::sqrt(1.0f +
                        std::pow(w / fc, 4.f));
                    break;
                default:  // BP
                    mag = (w / fc) / std::sqrt(1.0f +
                        std::pow((w / fc - fc / juce::jmax(w, 1e-6f)) * Q, 2.f));
                    break;
                }
                return juce::jlimit(0.f, 4.f, mag);
            }
            if (model == 6)  // Comb
            {
                // Notches at multiples of cutoff
                const float phase = (w / fc) * (float)M_PI;
                const float r2 = juce::jlimit(0.f, 0.95f, r);
                return juce::jlimit(0.f, 2.f,
                    1.f + r2 * r2 - 2.f * r2 * std::cos(phase));
            }
            return 1.0f;
        };

    float mag = 1.0f;
    const int route = snapshot.routing.load();

    const bool f1Active = snapshot.f1On.load() &&
        (route != 3);
    const bool f2Active = snapshot.f2On.load() &&
        (route != 2);

    if (f1Active)
    {
        const float m1 = respFor(snapshot.f1Model.load(),
            snapshot.f1Type.load(),
            snapshot.f1Cutoff.load(),
            snapshot.f1Reso.load());
        if (route == 0 || route == 2)  // Series or F1-only
            mag *= m1;
        else
        {
            // Parallel — rough blend
            const float pm = snapshot.parallelMix.load();
            mag = mag * (1.f - pm) + m1 * pm;
        }
    }

    if (f2Active)
    {
        const float m2 = respFor(snapshot.f2Model.load(),
            snapshot.f2Type.load(),
            snapshot.f2Cutoff.load(),
            snapshot.f2Reso.load());
        if (route == 0 || route == 3)  // Series or F2-only
            mag *= m2;
        else
        {
            const float pm = snapshot.parallelMix.load();
            mag = mag * pm + m2 * (1.f - pm);
        }
    }

    return juce::jlimit(0.f, 4.f, mag);
}

//==============================================================================
// processBlock
//==============================================================================
void FilterForgeProcessor::processBlock(juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    if (!isPrepared) return;

    const int numSamples = buffer.getNumSamples();
    const int numCh = buffer.getNumChannels();
    if (numSamples <= 0 || numCh < 1) return;

    using namespace FilterParams;

    updateSnapshot();

    const bool bypassed = apvts.getRawParameterValue(BYPASS)->load() > 0.5f;

    // Filter 1 params
    const int   f1Model = snapshot.f1Model.load();
    const int   f1Type = snapshot.f1Type.load();
    const float f1Cutoff = snapshot.f1Cutoff.load();
    const float f1Reso = snapshot.f1Reso.load();
    const float f1Drive = apvts.getRawParameterValue(F1_DRIVE)->load();
    const bool  f1On = snapshot.f1On.load();

    // Filter 2 params
    const int   f2Model = snapshot.f2Model.load();
    const int   f2Type = snapshot.f2Type.load();
    const float f2Cutoff = snapshot.f2Cutoff.load();
    const float f2Reso = snapshot.f2Reso.load();
    const float f2Drive = apvts.getRawParameterValue(F2_DRIVE)->load();
    const bool  f2On = snapshot.f2On.load();

    // Routing
    const int   route = snapshot.routing.load();
    const float parallelMix = snapshot.parallelMix.load();

    // LFO
    const bool  lfoOn = apvts.getRawParameterValue(LFO_ON)->load() > 0.5f;
    const int   lfoShape = (int)apvts.getRawParameterValue(LFO_SHAPE)->load();
    const float lfoRate = apvts.getRawParameterValue(LFO_RATE)->load();
    const float lfoAmount = snapshot.lfoAmount.load();
    const int   lfoTarget = (int)apvts.getRawParameterValue(LFO_TARGET)->load();
    const float lfoDelay = apvts.getRawParameterValue(LFO_DELAY)->load();

    // Envelope follower
    const bool  envOn = apvts.getRawParameterValue(ENV_ON)->load() > 0.5f;
    const float envAmt = snapshot.envAmount.load();
    const float envAtk = apvts.getRawParameterValue(ENV_ATTACK)->load();
    const float envRel = apvts.getRawParameterValue(ENV_RELEASE)->load();
    const int   envTarget = (int)apvts.getRawParameterValue(ENV_TARGET)->load();

    // Output
    const float mix = apvts.getRawParameterValue(MIX)->load();
    const float outDb = apvts.getRawParameterValue(OUTPUT)->load();

    smoothedMix.setTargetValue(mix);
    smoothedOutput.setTargetValue(juce::Decibels::decibelsToGain(outDb));

    // Input RMS for meter
    {
        float sumL = 0.f, sumR = 0.f;
        for (int i = 0; i < numSamples; ++i)
        {
            sumL += buffer.getSample(0, i) * buffer.getSample(0, i);
            if (numCh >= 2)
                sumR += buffer.getSample(1, i) * buffer.getSample(1, i);
        }
        levelInRmsL.store(std::sqrt(sumL / (float)numSamples));
        levelInRmsR.store(std::sqrt(sumR / (float)numSamples));
    }

    float* left = buffer.getWritePointer(0);
    float* right = (numCh >= 2) ? buffer.getWritePointer(1) : left;

    // -------------------------------------------------------------------
    // Pick the active filter instances for this block
    //
    // One virtual pointer per channel per filter. All filter instances are
    // owned by the processor and have already been prepared, so switching
    // models mid-stream is safe — the new instance just runs from its
    // current state (or is reset, depending on whether it's the first time).
    // -------------------------------------------------------------------
    auto selectF1 = [&](bool isRight) -> FilterDSP::FilterBase* {
        auto& kr = isRight ? kr106F1R : kr106F1L;
        auto& wa = isRight ? waspF1R : waspF1L;
        auto& ch = isRight ? chamF1R : chamF1L;
        auto& hp = isRight ? hpfF1R : hpfF1L;
        auto& bb = isRight ? bassF1R : bassF1L;
        auto& sv = isRight ? svfF1R : svfF1L;
        auto& co = isRight ? combF1R : combF1L;
        switch (f1Model)
        {
        case 0: return &kr;
        case 1: return &wa;
        case 2: return &ch;
        case 3: return &hp;
        case 4: return &bb;
        case 5: return &sv;
        case 6: return &co;
        default: return &kr;
        }
        };

    auto selectF2 = [&](bool isRight) -> FilterDSP::FilterBase* {
        auto& kr = isRight ? kr106F2R : kr106F2L;
        auto& wa = isRight ? waspF2R : waspF2L;
        auto& ch = isRight ? chamF2R : chamF2L;
        auto& hp = isRight ? hpfF2R : hpfF2L;
        auto& bb = isRight ? bassF2R : bassF2L;
        auto& sv = isRight ? svfF2R : svfF2L;
        auto& co = isRight ? combF2R : combF2L;
        switch (f2Model)
        {
        case 0: return &kr;
        case 1: return &wa;
        case 2: return &ch;
        case 3: return &hp;
        case 4: return &bb;
        case 5: return &sv;
        case 6: return &co;
        default: return &kr;
        }
        };

    FilterDSP::FilterBase* f1L = selectF1(false);
    FilterDSP::FilterBase* f1R = selectF1(true);
    FilterDSP::FilterBase* f2L = selectF2(false);
    FilterDSP::FilterBase* f2R = selectF2(true);

    // Precompute per-sample LFO increment
    const float lfoInc = lfoRate / (float)currentSampleRate;

    // Envelope follower coefficients
    const float envAtkCoeff = 1.f - std::exp(-1.f /
        (0.001f + envAtk * envAtk * 0.5f) / (float)currentSampleRate);
    const float envRelCoeff = 1.f - std::exp(-1.f /
        (0.001f + envRel * envRel * 0.5f) / (float)currentSampleRate);

    // -------------------------------------------------------------------
    // Per-sample loop
    // -------------------------------------------------------------------
    for (int i = 0; i < numSamples; ++i)
    {
        const float dryL = left[i];
        const float dryR = right[i];

        // --- LFO ---
        float lfoVal = 0.f;
        if (lfoOn)
        {
            lfoDelayTimer += 1.f / (float)currentSampleRate;
            float delayGain = 1.f;
            if (lfoDelay > 0.001f)
            {
                const float past = lfoDelayTimer - lfoDelay;
                delayGain = (past <= 0.f) ? 0.f
                    : juce::jlimit(0.f, 1.f, past * (float)currentSampleRate / 480.f);
            }

            switch (lfoShape)
            {
            case 0: lfoVal = std::sin(lfoPhase * juce::MathConstants<float>::twoPi); break;
            case 1: lfoVal = (lfoPhase < 0.5f) ? (lfoPhase * 4.f - 1.f)
                : (3.f - lfoPhase * 4.f); break;
            case 2: lfoVal = 1.f - 2.f * lfoPhase; break;
            case 3: lfoVal = 2.f * lfoPhase - 1.f; break;
            case 4: lfoVal = (lfoPhase < 0.5f) ? 1.f : -1.f; break;
            case 5: {
                static juce::Random rng;
                if (lfoPhase < lfoInc) lfoVal = rng.nextFloat() * 2.f - 1.f;
                else lfoVal = 0.f;
                break;
            }
            default: break;
            }

            lfoPhase += lfoInc;
            if (lfoPhase >= 1.f) lfoPhase -= 1.f;

            lfoVal *= lfoAmount * delayGain;
        }

        // --- Envelope follower ---
        float envVal = 0.f;
        if (envOn)
        {
            const float rectified = std::abs(dryL + dryR) * 0.5f;
            const float coeff = (rectified > envFollower) ? envAtkCoeff : envRelCoeff;
            envFollower += coeff * (rectified - envFollower);
            envVal = envFollower * envAmt;
        }

        // --- Compute per-filter cutoff modifiers ---
        float f1CutoffMod = f1Cutoff;
        float f2CutoffMod = f2Cutoff;

        if (lfoOn)
        {
            if (lfoTarget == 0 || lfoTarget == 2)
                f1CutoffMod *= std::pow(2.f, lfoVal * 4.f);
            if (lfoTarget == 1 || lfoTarget == 2)
                f2CutoffMod *= std::pow(2.f, lfoVal * 4.f);
        }
        if (envOn)
        {
            if (envTarget == 0 || envTarget == 2)
                f1CutoffMod *= std::pow(2.f, envVal * 4.f);
            if (envTarget == 1 || envTarget == 2)
                f2CutoffMod *= std::pow(2.f, envVal * 4.f);
        }

        f1CutoffMod = juce::jlimit(20.f, (float)currentSampleRate * 0.45f, f1CutoffMod);
        f2CutoffMod = juce::jlimit(20.f, (float)currentSampleRate * 0.45f, f2CutoffMod);

        // --- Process ---
        float outL = dryL;
        float outR = dryR;

        if (!bypassed)
        {
            if (route == 0)  // Series
            {
                if (f1On)
                {
                    outL = f1L->process(outL, f1CutoffMod, f1Reso, f1Type, f1Drive);
                    outR = f1R->process(outR, f1CutoffMod, f1Reso, f1Type, f1Drive);
                }
                if (f2On)
                {
                    outL = f2L->process(outL, f2CutoffMod, f2Reso, f2Type, f2Drive);
                    outR = f2R->process(outR, f2CutoffMod, f2Reso, f2Type, f2Drive);
                }
            }
            else if (route == 1)  // Parallel
            {
                float aL = dryL, aR = dryR;
                float bL = dryL, bR = dryR;

                if (f1On)
                {
                    aL = f1L->process(dryL, f1CutoffMod, f1Reso, f1Type, f1Drive);
                    aR = f1R->process(dryR, f1CutoffMod, f1Reso, f1Type, f1Drive);
                }
                if (f2On)
                {
                    bL = f2L->process(dryL, f2CutoffMod, f2Reso, f2Type, f2Drive);
                    bR = f2R->process(dryR, f2CutoffMod, f2Reso, f2Type, f2Drive);
                }

                const float aGain = 1.f - parallelMix;
                const float bGain = parallelMix;
                outL = aL * aGain + bL * bGain;
                outR = aR * aGain + bR * bGain;
            }
            else if (route == 2)  // F1 Only
            {
                if (f1On)
                {
                    outL = f1L->process(dryL, f1CutoffMod, f1Reso, f1Type, f1Drive);
                    outR = f1R->process(dryR, f1CutoffMod, f1Reso, f1Type, f1Drive);
                }
            }
            else  // route == 3 — F2 Only
            {
                if (f2On)
                {
                    outL = f2L->process(dryL, f2CutoffMod, f2Reso, f2Type, f2Drive);
                    outR = f2R->process(dryR, f2CutoffMod, f2Reso, f2Type, f2Drive);
                }
            }

            // Dry/wet mix
            const float m = smoothedMix.getNextValue();
            outL = dryL * (1.f - m) + outL * m;
            outR = dryR * (1.f - m) + outR * m;
        }

        // Output gain
        const float g = smoothedOutput.getNextValue();
        left[i] = outL * g;
        if (numCh >= 2) right[i] = outR * g;
    }

    // Output meter
    float peakL = 0.f, peakR = 0.f;
    for (int i = 0; i < numSamples; ++i)
    {
        peakL = juce::jmax(peakL, std::abs(left[i]));
        if (numCh >= 2) peakR = juce::jmax(peakR, std::abs(right[i]));
    }
    levelPeakL.store(peakL);
    levelPeakR.store(peakR);
}

//==============================================================================
// A/B snapshot
//==============================================================================
void FilterForgeProcessor::captureAState()
{
    aState = apvts.copyState();
    aStateValid = true;
}

void FilterForgeProcessor::swapAB()
{
    if (!aStateValid) return;
    auto current = apvts.copyState();
    apvts.replaceState(aState);
    aState = current;
}

//==============================================================================
// Preset navigation
//==============================================================================
void FilterForgeProcessor::loadPresetByIndex(int index)
{
    if (!presetManager) return;
    if (index < 0 || index >= presetManager->getNumPresets()) return;

    presetManager->loadPreset(index);
    currentPresetIndex = index;
    if (onPresetChanged) onPresetChanged(index);
}

void FilterForgeProcessor::loadPreviousPreset()
{
    if (!presetManager) return;
    int n = presetManager->getNumPresets();
    if (n == 0) return;
    int idx = (currentPresetIndex <= 0) ? n - 1 : currentPresetIndex - 1;
    loadPresetByIndex(idx);
}

void FilterForgeProcessor::loadNextPreset()
{
    if (!presetManager) return;
    int n = presetManager->getNumPresets();
    if (n == 0) return;
    int idx = (currentPresetIndex + 1) % n;
    loadPresetByIndex(idx);
}

//==============================================================================
// State
//==============================================================================
void FilterForgeProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void FilterForgeProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

//==============================================================================
// Editor
//==============================================================================
juce::AudioProcessorEditor* FilterForgeProcessor::createEditor()
{
    return new FilterForgeEditor(*this);
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FilterForgeProcessor();
}