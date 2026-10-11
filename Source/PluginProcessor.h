#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <JuceHeader.h>
#include <atomic>
#include <array>
#include "PresetManager.h"

//==============================================================================
// FilterForge
// Multi-model filter: KR-106 VCF/HPF + Wasp Ladder + Chamberlin + Bass Boost
// + SVF + Comb, dual filter routing, LFO and envelope follower
// AlphaAudio by William Ashley Music
//==============================================================================

//==============================================================================
// Filter model enum
//==============================================================================
enum class FilterModel
{
    KRF106 = 0,   // Juno 106 4-pole TPT cascade
    WaspLadder,   // Moog-style ladder with CMOS
    Chamberlin,   // SVF with internal 2x oversampler
    KRHPF,        // Juno high-pass 4-position
    BassBoost,    // Juno bass boost shelf
    SVF12,        // Clean 2-pole SVF
    Comb          // Resonant comb
};

enum class FilterType
{
    LP12 = 0,
    LP24,
    HP12,
    HP24,
    BP12,
    BP24,
    Notch
};

//==============================================================================
// DSP primitives
//==============================================================================
namespace FilterDSP
{
    //==========================================================================
    // FilterBase — virtual interface implemented by every filter model
    //==========================================================================
    class FilterBase
    {
    public:
        virtual ~FilterBase() = default;

        virtual void prepare(double sampleRate) = 0;
        virtual void reset() = 0;

        // Process one sample. cutoffHz is cutoff frequency in Hz,
        // reso01 is resonance 0..1, type is the FilterType enum value,
        // drive01 is drive/saturation amount 0..1.
        virtual float process(float input, float cutoffHz, float reso01,
            int type, float drive01) = 0;
    };

    //==========================================================================
    // Shared nonlinearities
    //==========================================================================
    inline float OTASat(float x)
    {
        if (x > 3.f) return 1.f;
        if (x < -3.f) return -1.f;
        float x2 = x * x;
        return x * (27.f + x2) / (27.f + 9.f * x2);
    }

    inline float OTASatDeriv(float x)
    {
        if (x > 3.f || x < -3.f) return 0.f;
        float x2 = x * x;
        float d = 27.f + 9.f * x2;
        return 27.f * (27.f - 3.f * x2) / (d * d);
    }

    inline float ResK_J6(float res)
    {
        return 0.811f * (std::exp(2.128f * res) - 1.f);
    }

    inline float ResK_J106(float res)
    {
        float r = res, r2 = r * r, r3 = r2 * r, r4 = r2 * r2;
        float k = 4.6265f * r + 4.3866f * r2 - 8.9869f * r3 + 3.3902f * r4;
        if (r > 0.7f)
        {
            float knee = (r - 0.7f) / 0.3f;
            k += std::min(knee * knee * 1.2f, 0.35f);
        }
        float boost = std::max(0.f, (r - 0.898f) * (0.17f / 0.102f));
        return k * (1.f + boost);
    }

    inline float SoftClipK(float k)
    {
        if (k > 3.0f)
        {
            float ex = k - 3.0f;
            k = 3.0f + ex / (1.0f + ex * 0.2f);
        }
        return std::min(k, 6.6f);
    }

    inline float getJuno106HPFFreq(int position)
    {
        switch (position)
        {
        case 0: return -1.f;  // Bass boost
        case 1: return 0.f;   // Flat
        case 2: return 236.f;
        case 3: return 754.f;
        }
        return 0.f;
    }

    //==========================================================================
    // KR-106 VCF — 4-pole TPT cascade with OTA saturation
    //==========================================================================
    class KRF106Filter : public FilterBase
    {
    public:
        float mS[4] = {};
        bool mJ106Res = true;
        float mSampleRate = 44100.f;
        float mDrive = 1.0f;

        struct Coeffs
        {
            float k, g, g1, g1_2, g1_3, G;
            float invDen;
        } mC;

        float mCacheFrq = -1.f;
        float mCacheRes = -1.f;

        void prepare(double sr) override
        {
            mSampleRate = (float)sr;
            invalidate();
        }

        void reset() override
        {
            mS[0] = mS[1] = mS[2] = mS[3] = 0.f;
            invalidate();
        }

        void invalidate() { mCacheFrq = -1.f; }

        // FilterBase::process — cutoff in Hz, uses drive to scale OTA input
        float process(float input, float cutoffHz, float reso01,
            int /*type*/, float drive01) override
        {
            const float cutoffNorm = juce::jlimit(0.001f, 0.85f,
                cutoffHz / (mSampleRate * 0.5f));
            mDrive = 1.0f + drive01 * 3.0f;
            return processSample(input, cutoffNorm, reso01);
        }

        // Low-level process, cutoff as normalised 0..1 (for editor response)
        float processSample(float input, float frqNorm, float res)
        {
            updateCoeffs(frqNorm, res);

            constexpr float kInputRatio = 0.065f;
            constexpr float kDirectGain = 0.142f;

            float driven = input * mDrive;

            float S = (1.f - mC.g1) *
                (mS[0] * mC.g1_3 + mS[1] * mC.g1_2 + mS[2] * mC.g1 + mS[3]);

            float otaDiff = driven * kInputRatio - S;
            float otaOut = OTASat(otaDiff * 4.20f) / 4.20f;
            float u = (driven * kDirectGain + mC.k * otaOut) * mC.invDen;

            float lp1 = NLStage(mS[0], u, mC.g, mC.g1);
            float lp2 = NLStage(mS[1], lp1, mC.g, mC.g1);
            float lp3 = NLStage(mS[2], lp2, mC.g, mC.g1);
            float lp4 = NLStage(mS[3], lp3, mC.g, mC.g1);

            return lp4 * 8.59f / mDrive;
        }

    private:
        static float NLStage(float& s, float x, float g, float g1)
        {
            constexpr float otaScale = 0.35f;
            float y = s + g1 * (x - s);
            float diff = x - y;
            float sd = diff * otaScale;
            float t = OTASat(sd) / otaScale;
            float f = y - s - g * t;
            float df = 1.f + g * OTASatDeriv(sd);
            y -= f / df;
            s = 2.f * y - s;
            return y;
        }

        void updateCoeffs(float frq, float res)
        {
            if (frq == mCacheFrq && res == mCacheRes) return;
            mCacheFrq = frq;
            mCacheRes = res;

            float k = mJ106Res ? ResK_J106(res) : ResK_J6(res);
            if (!mJ106Res) k = SoftClipK(k);

            float frqOver = juce::jlimit(0.001f, 0.85f, frq);
            float g = std::tan(frqOver * (float)M_PI * 0.5f);

            float kFit = juce::jlimit(0.f, 5.0f, k);
            float logM = 0.20646f + kFit * (-0.04140f + kFit * (0.00602f - kFit * 0.00127f));
            if (k < 1.0f) logM *= std::max(k, 0.f);
            if (k > 3.7f)
            {
                float fade = std::max(0.f, (4.4f - k) / 0.7f);
                fade = std::min(1.f, fade);
                logM *= fade;
            }
            g *= std::exp(logM);
            g = std::min(g, 10.f);

            const float g1 = g / (1.f + g);
            const float g1_2 = g1 * g1;
            const float g1_3 = g1_2 * g1;
            const float G = g1_2 * g1_2;

            mC.k = k;
            mC.g = g;
            mC.g1 = g1;
            mC.g1_2 = g1_2;
            mC.g1_3 = g1_3;
            mC.G = G;
            mC.invDen = 1.f / (1.f + k * G);
        }
    };

    //==========================================================================
    // Wasp Ladder — Moog-style 4-pole with CMOS option
    //==========================================================================
    class WaspLadderFilter : public FilterBase
    {
    public:
        double s[4] = { 0.0, 0.0, 0.0, 0.0 };
        double x0_ = 0.0;
        double sampleRate = 44100.0;

        void prepare(double sr) override { sampleRate = sr; }

        void reset() override
        {
            for (auto& x : s) x = 0.0;
            x0_ = 0.0;
        }

        float process(float input, float cutoffHz, float reso01,
            int type, float drive01) override
        {
            const bool cmos = drive01 > 0.5f;

            // Map FilterType to wasp's internal codes:
            // 0 = LP12, 1 = LP24, 2 = HP12, 3 = BP12
            int waspType = 0;
            switch (type)
            {
            case 0: waspType = 0; break;
            case 1: waspType = 1; break;
            case 2: waspType = 2; break;
            case 3: waspType = 2; break;
            case 4: waspType = 3; break;
            case 5: waspType = 3; break;
            case 6: waspType = 3; break;
            default: waspType = 0;
            }

            x0_ = (double)input;
            return processInternal(input, cutoffHz, reso01, waspType, cmos);
        }

    private:
        float processInternal(float input, float cutoffHz, float resonance,
            int type, bool cmos)
        {
            if (!std::isfinite(input)) input = 0.0f;
            double f = 2.0 * juce::MathConstants<double>::pi *
                (double)cutoffHz / sampleRate;
            f = juce::jlimit(0.001, 0.499, f);
            double t1 = f * 0.9892;

            double y0, y1, y2, y3;

            if (cmos)
            {
                double kCmos = (double)juce::jlimit(0.0f, 1.0f, resonance) * 1.8;
                double xC = (double)input - kCmos * s[3];
                auto cmosClip = [](double v) -> double {
                    return v / (1.0 + std::abs(v));
                    };
                y0 = cmosClip(s[0] + t1 * (xC - s[0]));
                y1 = cmosClip(s[1] + t1 * (y0 - s[1]));
                y2 = cmosClip(s[2] + t1 * (y1 - s[2]));
                y3 = cmosClip(s[3] + t1 * (y2 - s[3]));
            }
            else
            {
                double k = (double)juce::jlimit(0.0f, 1.0f, resonance) * 3.99;
                double x = (double)input - k * s[3];
                y0 = x * t1 + s[0] * (1.0 - t1);
                y1 = y0 * t1 + s[1] * (1.0 - t1);
                y2 = y1 * t1 + s[2] * (1.0 - t1);
                y3 = y2 * t1 + s[3] * (1.0 - t1);
            }

            if (!std::isfinite(y0)) y0 = 0.0;
            if (!std::isfinite(y1)) y1 = 0.0;
            if (!std::isfinite(y2)) y2 = 0.0;
            if (!std::isfinite(y3)) y3 = 0.0;
            s[0] = y0; s[1] = y1; s[2] = y2; s[3] = y3;

            float out;
            switch (type)
            {
            case 1: out = (float)y3; break;
            case 2: out = (float)(x0_ - y2); break;
            case 3: out = (float)(y1 - y3); break;
            default: out = (float)y1; break;
            }
            if (cmos) out *= 1.4f;
            if (!std::isfinite(out)) out = 0.0f;
            return out;
        }
    };

    //==========================================================================
    // Chamberlin SVF — 2-pole state-variable
    //==========================================================================
    class ChamberlinFilter : public FilterBase
    {
    public:
        double low = 0.0, band = 0.0;
        double dcX1 = 0.0, dcY1 = 0.0;
        float mSampleRate = 44100.f;

        void prepare(double sr) override { mSampleRate = (float)sr; }

        void reset() override
        {
            low = band = 0.0;
            dcX1 = dcY1 = 0.0;
        }

        float process(float input, float cutoffHz, float reso01,
            int type, float /*drive01*/) override
        {
            // Map FilterType to chamberlin codes: 0=LP, 1=BP, 2=HP
            int chamberType = 0;
            switch (type)
            {
            case 0: case 1: chamberType = 0; break;
            case 2: case 3: chamberType = 2; break;
            case 4: case 5: chamberType = 1; break;
            case 6: chamberType = 1; break;
            default: chamberType = 0;
            }
            return processInternal(input, cutoffHz, reso01, chamberType);
        }

    private:
        float processInternal(float input, float cutoffHz, float resonance,
            int type)
        {
            if (!std::isfinite(input)) input = 0.0f;

            const double Q = 0.5 + juce::jlimit(0.0, 1.0,
                (double)resonance) * 19.5;
            double maxNorm = (Q > 1.0) ? juce::jmin(1.8 / Q, 0.40) : 0.40;
            maxNorm = juce::jmax(maxNorm, 0.001);
            const double norm = juce::jlimit(0.0005, maxNorm,
                (double)cutoffHz / mSampleRate);
            const double f = 2.0 * std::sin(juce::MathConstants<double>::pi *
                norm);
            double Q1 = 1.0 / Q;
            Q1 = juce::jlimit(0.05, 2.0, Q1);

            double L = low, B = band;
            double H = (double)input - L - Q1 * B;
            B = f * H + B;
            L = f * B + L;

            if (!std::isfinite(L) || !std::isfinite(B) || !std::isfinite(H))
            {
                low = band = 0.0;
                dcX1 = dcY1 = 0.0;
                return 0.0f;
            }

            low = L;
            band = B;

            float out;
            switch (type)
            {
            case 1: out = (float)B; break;
            case 2: out = (float)H; break;
            default: out = (float)L; break;
            }

            if (!std::isfinite(out)) { out = 0.0f; low = band = 0.0; }
            return out;
        }
    };

    //==========================================================================
    // Bass Boost — Juno low-shelf
    //==========================================================================
    class BassBoostFilter : public FilterBase
    {
    public:
        static constexpr float kR1 = 47e3f, kC1 = 0.047e-6f, kCA = 0.01e-6f;
        static constexpr float kRg = 10e3f, kRf = 100e3f, kCf = 0.022e-6f;
        static constexpr float kR43 = 47e3f, kR44 = 220e3f, kR45 = 47e3f;

        float b0 = 1.f, b1 = 0.f, b2 = 0.f;
        float a1 = 0.f, a2 = 0.f;
        double z1 = 0.0, z2 = 0.0;

        void prepare(double sr) override { init((float)sr); }

        void init(float sampleRate)
        {
            const float tau_1z = kR1 * kC1;
            const float tau_1p = kR1 * (kC1 + kCA);
            const float tau_2z = (kRg * kRf / (kRg + kRf)) * kCf;
            const float tau_2p = kRf * kCf;
            const float G2_dc = 1.f + kRf / kRg;
            const float alpha = kR45 / kR44;
            const float direct = kR45 / kR43;

            const float D0 = 1.f;
            const float D1 = tau_1p + tau_2p;
            const float D2 = tau_1p * tau_2p;
            const float Nb0 = 1.f;
            const float Nb1 = tau_1z + tau_2z;
            const float Nb2 = tau_1z * tau_2z;
            const float ag = alpha * G2_dc;
            const float N0 = direct * D0 + ag * Nb0;
            const float N1 = direct * D1 + ag * Nb1;
            const float N2 = direct * D2 + ag * Nb2;

            const float K = 2.f * sampleRate;
            const float K2 = K * K;
            const float a0 = D0 + D1 * K + D2 * K2;
            b0 = (N0 + N1 * K + N2 * K2) / a0;
            b1 = 2.f * (N0 - N2 * K2) / a0;
            b2 = (N0 - N1 * K + N2 * K2) / a0;
            a1 = 2.f * (D0 - D2 * K2) / a0;
            a2 = (D0 - D1 * K + D2 * K2) / a0;
            reset();
        }

        void reset() override { z1 = z2 = 0.f; }

        // FilterBase::process — ignores cutoff, reso, type, drive
        float process(float x, float /*cutoffHz*/, float /*reso01*/,
            int /*type*/, float /*drive01*/) override
        {
            return processInternal(x);
        }

        // Public so sibling classes (KRF106HPF) can call it without the
        // virtual dispatch overhead
        float processInternal(float x)
        {
            float y = b0 * x + (float)z1;
            z1 = b1 * x - a1 * y + z2;
            z2 = b2 * x - a2 * y;
            return y;
        }
    };

    //==========================================================================
    // KR-106 HPF — 4-position with click-free switching + bass boost
    //==========================================================================
    class KRF106HPF : public FilterBase
    {
    public:
        static constexpr float kDCBlockHz = 0.35f;
        static constexpr int kXfadeSamples = 64;

        int   mMode = 1;
        float mFreqHz = 0.f;
        float mSampleRate = 44100.f;
        float mG = 0.f;
        float mHpS = 0.f;
        float mDcG = 0.f;
        float mDcS = 0.f;
        BassBoostFilter mBassBoost;

        float mPrevFreqHz = 0.f;
        float mPrevG = 0.f;
        float mPrevHpS = 0.f;
        float mPrevDcS = 0.f;
        BassBoostFilter mPrevBassBoost;
        int   mXfadeCount = 0;

        void prepare(double sr) override { init((float)sr); }

        void init(float sr)
        {
            mSampleRate = sr;
            mHpS = mDcS = 0.f;
            mXfadeCount = 0;
            mBassBoost.init(sr);
            mPrevBassBoost.init(sr);
            recalc();
        }

        void setMode(int mode)
        {
            int newMode = juce::jlimit(0, 3, mode);
            float newFreqHz = getJuno106HPFFreq(newMode);

            if (newFreqHz != mFreqHz)
            {
                mPrevFreqHz = mFreqHz;
                mPrevG = mG;
                mPrevHpS = mHpS;
                mPrevDcS = mDcS;
                mPrevBassBoost = mBassBoost;
                mXfadeCount = kXfadeSamples;
            }
            mMode = newMode;
            mFreqHz = newFreqHz;
            recalc();
        }

        void recalc()
        {
            float dcFrq = juce::jlimit(kDCBlockHz / (mSampleRate * 0.5f),
                0.f, 0.9f);
            mDcG = std::tan(dcFrq * (float)M_PI * 0.5f);
            if (mFreqHz <= 0.f) { mG = 0.f; return; }
            float frq = juce::jlimit(mFreqHz / (mSampleRate * 0.5f),
                0.001f, 0.9f);
            mG = std::tan(frq * (float)M_PI * 0.5f);
        }

        void reset() override
        {
            mHpS = mDcS = mPrevHpS = mPrevDcS = 0.f;
            mBassBoost.reset();
            mPrevBassBoost.reset();
            mXfadeCount = 0;
        }

        // FilterBase::process — the reso knob acts as the position selector
        float process(float input, float /*cutoffHz*/, float reso01,
            int /*type*/, float /*drive01*/) override
        {
            int pos = juce::jlimit(0, 3, (int)(reso01 * 3.999f));
            setMode(pos);
            return processInternal(input);
        }

    private:
        float dcBlock(float input, float& dcState)
        {
            float v = (input - dcState) * mDcG / (1.f + mDcG);
            float lp = dcState + v;
            dcState = lp + v;
            return input - lp;
        }

        float processWith(float input, float freqHz, float g, float& hpState,
            float& dcState, BassBoostFilter& boost)
        {
            float dcBlocked;
            if (freqHz < 0.f)
                dcBlocked = dcBlock(boost.processInternal(input), dcState);
            else
                dcBlocked = dcBlock(input, dcState);
            if (freqHz <= 0.f) return dcBlocked;
            float v = (dcBlocked - hpState) * g / (1.f + g);
            float lp = hpState + v;
            hpState = lp + v;
            return dcBlocked - lp;
        }

        float processInternal(float input)
        {
            float out = processWith(input, mFreqHz, mG, mHpS, mDcS, mBassBoost);
            if (mXfadeCount > 0)
            {
                float prev = processWith(input, mPrevFreqHz, mPrevG, mPrevHpS,
                    mPrevDcS, mPrevBassBoost);
                float t = (float)mXfadeCount / (float)kXfadeSamples;
                out = out * (1.f - t) + prev * t;
                mXfadeCount--;
            }
            return out;
        }
    };

    //==========================================================================
    // Clean 2-pole SVF
    //==========================================================================
    class SVF12 : public FilterBase
    {
    public:
        double ic1 = 0.0, ic2 = 0.0;
        float mSampleRate = 44100.f;

        void prepare(double sr) override { mSampleRate = (float)sr; }
        void reset() override { ic1 = ic2 = 0.0; }

        float process(float input, float cutoffHz, float reso01,
            int type, float /*drive01*/) override
        {
            // Map FilterType to svf codes: 0=LP, 1=BP, 2=HP
            int svfType = 0;
            switch (type)
            {
            case 0: case 1: svfType = 0; break;
            case 2: case 3: svfType = 2; break;
            case 4: case 5: svfType = 1; break;
            case 6: svfType = 1; break;
            default: svfType = 0;
            }
            return processInternal(input, cutoffHz, reso01, svfType);
        }

    private:
        float processInternal(float input, float cutoffHz, float resonance,
            int type)
        {
            const double g = std::tan(juce::MathConstants<double>::pi *
                juce::jlimit(20.0, (double)mSampleRate * 0.45,
                    (double)cutoffHz) / mSampleRate);
            const double k = 1.0 / (0.5 + juce::jlimit(0.0, 1.0,
                (double)resonance) * 19.5);

            const double a1 = 1.0 / (1.0 + g * (g + k));
            const double a2 = g * a1;
            const double a3 = g * a2;

            const double v3 = (double)input - ic2;
            const double v1 = a1 * ic1 + a2 * v3;
            const double v2 = ic2 + a2 * ic1 + a3 * v3;

            ic1 = 2.0 * v1 - ic1;
            ic2 = 2.0 * v2 - ic2;

            switch (type)
            {
            case 1: return (float)v1;                     // BP
            case 2: return (float)(input - k * v1 - v2);  // HP
            default: return (float)v2;                    // LP
            }
        }
    };

    //==========================================================================
    // Comb filter — short delay with feedback
    //==========================================================================
    class CombFilter : public FilterBase
    {
    public:
        std::vector<float> buf;
        int writePos = 0;
        int delaySamples = 100;
        float sampleRate = 44100.f;

        void prepare(double sr) override { init((float)sr); }

        void init(float sr)
        {
            sampleRate = sr;
            int maxDelay = (int)(sr / 20.f) + 4;
            buf.assign((size_t)maxDelay, 0.f);
            writePos = 0;
        }

        void reset() override
        {
            std::fill(buf.begin(), buf.end(), 0.f);
            writePos = 0;
        }

        void setFrequency(float hz)
        {
            if (hz < 20.f) hz = 20.f;
            delaySamples = (int)(sampleRate / hz);
            if (delaySamples < 2) delaySamples = 2;
            if (delaySamples >= (int)buf.size())
                delaySamples = (int)buf.size() - 2;
        }

        float process(float input, float cutoffHz, float reso01,
            int /*type*/, float /*drive01*/) override
        {
            setFrequency(cutoffHz);
            return processInternal(input, reso01 * 0.95f);
        }

    private:
        float processInternal(float input, float feedback)
        {
            if (buf.empty()) return input;
            int readPos = writePos - delaySamples;
            if (readPos < 0) readPos += (int)buf.size();
            float delayed = buf[(size_t)readPos];
            buf[(size_t)writePos] = input + delayed * feedback;
            writePos = (writePos + 1) % (int)buf.size();
            return (input + delayed) * 0.5f;
        }
    };

} // namespace FilterDSP

//==============================================================================
// Parameter IDs
//==============================================================================
namespace FilterParams
{
    // --- Filter 1 ---
    static constexpr const char* F1_MODEL = "f1_model";
    static constexpr const char* F1_TYPE = "f1_type";
    static constexpr const char* F1_CUTOFF = "f1_cutoff";
    static constexpr const char* F1_RESO = "f1_reso";
    static constexpr const char* F1_DRIVE = "f1_drive";
    static constexpr const char* F1_KEYTRACK = "f1_keytrack";
    static constexpr const char* F1_ON = "f1_on";

    // --- Filter 2 ---
    static constexpr const char* F2_MODEL = "f2_model";
    static constexpr const char* F2_TYPE = "f2_type";
    static constexpr const char* F2_CUTOFF = "f2_cutoff";
    static constexpr const char* F2_RESO = "f2_reso";
    static constexpr const char* F2_DRIVE = "f2_drive";
    static constexpr const char* F2_KEYTRACK = "f2_keytrack";
    static constexpr const char* F2_ON = "f2_on";

    // --- Routing ---
    static constexpr const char* ROUTING = "routing";
    static constexpr const char* PARALLEL_MIX = "parallel_mix";

    // --- LFO ---
    static constexpr const char* LFO_ON = "lfo_on";
    static constexpr const char* LFO_SHAPE = "lfo_shape";
    static constexpr const char* LFO_RATE = "lfo_rate";
    static constexpr const char* LFO_AMOUNT = "lfo_amount";
    static constexpr const char* LFO_TARGET = "lfo_target";
    static constexpr const char* LFO_DELAY = "lfo_delay";

    // --- Envelope follower ---
    static constexpr const char* ENV_ON = "env_on";
    static constexpr const char* ENV_AMOUNT = "env_amount";
    static constexpr const char* ENV_ATTACK = "env_attack";
    static constexpr const char* ENV_RELEASE = "env_release";
    static constexpr const char* ENV_TARGET = "env_target";

    // --- Output ---
    static constexpr const char* MIX = "mix";
    static constexpr const char* OUTPUT = "output";
    static constexpr const char* BYPASS = "bypass";
}

//==============================================================================
// Processor
//==============================================================================
class FilterForgeProcessor : public juce::AudioProcessor,
    public juce::AudioProcessorValueTreeState::Listener
{
public:
    FilterForgeProcessor();
    ~FilterForgeProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "FilterForge"; }
    bool acceptsMidi()  const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void parameterChanged(const juce::String& paramID, float newValue) override;

    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Preset manager
    PresetManager& getPresetManager() { return *presetManager; }
    const PresetManager& getPresetManager() const { return *presetManager; }

    void loadPreviousPreset();
    void loadNextPreset();
    void loadPresetByIndex(int index);
    int  getCurrentPresetIndex() const { return currentPresetIndex; }

    std::function<void(int)> onPresetChanged;

    // A/B snapshot
    void captureAState();
    void swapAB();
    bool hasAState() const { return aStateValid; }

    // Output meters
    std::atomic<float> levelPeakL{ 0.0f };
    std::atomic<float> levelPeakR{ 0.0f };
    std::atomic<float> levelInRmsL{ 0.0f };
    std::atomic<float> levelInRmsR{ 0.0f };

    //==========================================================================
    // Live snapshot for the editor display
    //==========================================================================
    struct FilterSnapshot
    {
        std::atomic<int>   f1Model{ 0 };
        std::atomic<int>   f1Type{ 0 };
        std::atomic<float> f1Cutoff{ 1000.f };
        std::atomic<float> f1Reso{ 0.f };
        std::atomic<bool>  f1On{ true };

        std::atomic<int>   f2Model{ 0 };
        std::atomic<int>   f2Type{ 0 };
        std::atomic<float> f2Cutoff{ 1000.f };
        std::atomic<float> f2Reso{ 0.f };
        std::atomic<bool>  f2On{ false };

        std::atomic<int>   routing{ 0 };
        std::atomic<float> parallelMix{ 0.5f };

        std::atomic<float> lfoAmount{ 0.f };
        std::atomic<float> envAmount{ 0.f };

        std::atomic<float> sampleRate{ 44100.f };
    };
    FilterSnapshot snapshot;

    float evaluateResponse(float freqHz) const;

private:
    double currentSampleRate = 44100.0;
    int    currentBlockSize = 512;
    bool   isPrepared = false;

    // --- Per-channel filter state (filter 1) ---
    FilterDSP::KRF106Filter     kr106F1L, kr106F1R;
    FilterDSP::WaspLadderFilter waspF1L, waspF1R;
    FilterDSP::ChamberlinFilter chamF1L, chamF1R;
    FilterDSP::KRF106HPF        hpfF1L, hpfF1R;
    FilterDSP::BassBoostFilter  bassF1L, bassF1R;
    FilterDSP::SVF12            svfF1L, svfF1R;
    FilterDSP::CombFilter       combF1L, combF1R;

    // --- Per-channel filter state (filter 2) ---
    FilterDSP::KRF106Filter     kr106F2L, kr106F2R;
    FilterDSP::WaspLadderFilter waspF2L, waspF2R;
    FilterDSP::ChamberlinFilter chamF2L, chamF2R;
    FilterDSP::KRF106HPF        hpfF2L, hpfF2R;
    FilterDSP::BassBoostFilter  bassF2L, bassF2R;
    FilterDSP::SVF12            svfF2L, svfF2R;
    FilterDSP::CombFilter       combF2L, combF2R;

    // --- LFO state ---
    float lfoPhase = 0.f;
    float lfoDelayTimer = 0.f;

    // --- Envelope follower state ---
    float envFollower = 0.f;

    // --- Smoothed values ---
    juce::SmoothedValue<float> smoothedOutput;
    juce::SmoothedValue<float> smoothedMix;

    // --- A/B snapshot ---
    juce::ValueTree aState;
    bool aStateValid = false;

    // --- Preset manager ---
    std::unique_ptr<PresetManager> presetManager;
    int currentPresetIndex = -1;

    void updateSnapshot();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterForgeProcessor)
};