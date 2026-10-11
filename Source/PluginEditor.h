#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

//==============================================================================
// Colour palette — "Filter Forge Steel"
//==============================================================================
namespace ForgeColour
{
    inline const juce::Colour BG{ 0xFF0A0C10 };
    inline const juce::Colour Panel{ 0xFF161A22 };
    inline const juce::Colour PanelBdr{ 0xFF242A36 };
    inline const juce::Colour Header{ 0xFF050608 };
    inline const juce::Colour ListBg{ 0xFF0E1116 };
    inline const juce::Colour ListRow{ 0xFF141820 };
    inline const juce::Colour ListRowAlt{ 0xFF181C24 };
    inline const juce::Colour ListRowHover{ 0xFF202634 };
    inline const juce::Colour ListRowSelected{ 0xFF2A3448 };

    // Section accents
    inline const juce::Colour Orange{ 0xFFFFC080 };   // Filter 1
    inline const juce::Colour OrangeDim{ 0xFF6A4A20 };
    inline const juce::Colour Cyan{ 0xFF60C0FF };     // Filter 2
    inline const juce::Colour CyanDim{ 0xFF205070 };
    inline const juce::Colour Purple{ 0xFFC060D0 };   // Routing
    inline const juce::Colour PurpleDim{ 0xFF502060 };
    inline const juce::Colour Yellow{ 0xFFFFD070 };   // Mod / LFO
    inline const juce::Colour YellowDim{ 0xFF6A5820 };
    inline const juce::Colour Green{ 0xFF70D080 };    // Env follower
    inline const juce::Colour GreenDim{ 0xFF2A5028 };
    inline const juce::Colour Steel{ 0xFFA0A8B4 };    // Utility
    inline const juce::Colour Magenta{ 0xFFFF4080 };  // Response curve
    inline const juce::Colour IvoryText{ 0xFFD8DDE8 };
    inline const juce::Colour DimText{ 0xFF6A7080 };
    inline const juce::Colour White{ 0xFFFFFFFF };
}

//==============================================================================
// LookAndFeel
//==============================================================================
class ForgeLAF : public juce::LookAndFeel_V4
{
public:
    ForgeLAF();

    void drawRotarySlider(juce::Graphics&, int x, int y, int w, int h,
        float sliderPos, float startAng, float endAng,
        juce::Slider&) override;

    void drawToggleButton(juce::Graphics&, juce::ToggleButton&,
        bool highlight, bool down) override;

    void drawComboBox(juce::Graphics&, int width, int height, bool isButtonDown,
        int buttonX, int buttonY, int buttonW, int buttonH,
        juce::ComboBox&) override;

    static juce::Font monoFont(float size);
    static juce::Font displayFont(float size);
};

//==============================================================================
// Labelled knob
//==============================================================================
class ForgeKnob : public juce::Component
{
public:
    ForgeKnob(const juce::String& label, const juce::String& paramID,
        juce::AudioProcessorValueTreeState& apvts,
        juce::Colour accent = ForgeColour::Orange);

    void resized() override;
    void paint(juce::Graphics&) override;

    juce::Slider slider;

private:
    juce::Label nameLabel, valueLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    juce::String paramID;
    juce::AudioProcessorValueTreeState& apvtsRef;
    juce::Colour accent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ForgeKnob)
};

//==============================================================================
// Section panel
//==============================================================================
class ForgeSection : public juce::Component
{
public:
    ForgeSection(const juce::String& titleText, juce::Colour accentCol);
    void paint(juce::Graphics&) override;

    juce::String title;
    juce::Colour accent;
};

//==============================================================================
// Frequency response display
//
// Plots the combined filter magnitude response using
// FilterForgeProcessor::evaluateResponse(), plus a per-filter overlay
// showing each filter's own contribution.
//==============================================================================
class ResponseDisplay : public juce::Component,
    private juce::Timer
{
public:
    ResponseDisplay(FilterForgeProcessor& p);
    ~ResponseDisplay() override;

    void paint(juce::Graphics&) override;
    void timerCallback() override;

private:
    FilterForgeProcessor& proc;

    // Frequency → X mapping (log scale, 20 Hz..20 kHz)
    float freqToX(float freq, float width) const;
    float dbToY(float dB, float height) const;
    void  drawGrid(juce::Graphics&);
    void  drawCurve(juce::Graphics&, bool forFilter2, juce::Colour);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResponseDisplay)
};

//==============================================================================
// Level meter
//==============================================================================
class ForgeMeter : public juce::Component,
    private juce::Timer
{
public:
    ForgeMeter(FilterForgeProcessor& p);
    ~ForgeMeter() override;
    void paint(juce::Graphics&) override;
    void timerCallback() override;

private:
    FilterForgeProcessor& proc;
    float dispInL = 0.f, dispInR = 0.f;
    float dispOutL = 0.f, dispOutR = 0.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ForgeMeter)
};

//==============================================================================
// Preset browser — a right-side list panel
//
// Instead of a cramped dropdown, this shows the full preset list with
// category headers, a search field, and Previous/Next/Delete/Save buttons
// that don't collide with anything else.
//==============================================================================
class PresetBrowser : public juce::Component,
    private juce::Timer
{
public:
    PresetBrowser(FilterForgeProcessor& p);
    ~PresetBrowser() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    // Rebuild the list from the manager
    void refresh();

    // Callbacks the editor can hook into
    std::function<void(int)> onPresetSelected;
    std::function<void()>    onSaveRequested;

    // The currently selected preset in the list (-1 = none)
    int getSelectedIndex() const { return selectedIndex; }

    // Select a preset in the list without triggering a callback
    void setSelectedIndexSilent(int index);

private:
    class PresetList;  // forward — defined in the .cpp

    FilterForgeProcessor& proc;
    std::unique_ptr<PresetList> list;
    std::unique_ptr<juce::TextEditor> searchField;
    std::unique_ptr<juce::TextButton> saveButton;
    std::unique_ptr<juce::TextButton> deleteButton;
    std::unique_ptr<juce::TextButton> exportButton;
    std::unique_ptr<juce::TextButton> folderButton;
    std::unique_ptr<juce::Label> headerLabel;

    // Rows tracked for hit-testing
    struct Row
    {
        juce::String display;
        juce::String name;
        juce::String category;
        int presetIndex = -1;   // index into manager list, -1 for a header
        bool isHeader = false;
        bool isUser = false;
        juce::Rectangle<int> bounds;
    };
    std::vector<Row> rows;
    int selectedIndex = -1;
    int lastKnownManagerIndex = -1;

    void rebuildRows(const juce::String& filterText);
    void handleRowClick(int presetIndex);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetBrowser)
};

//==============================================================================
// Main editor
//==============================================================================
class FilterForgeEditor : public juce::AudioProcessorEditor,
    private juce::Timer
{
public:
    FilterForgeEditor(FilterForgeProcessor&);
    ~FilterForgeEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    FilterForgeProcessor& audioProcessor;
    ForgeLAF laf;

    //==========================================================================
    // Display + meter
    //==========================================================================
    std::unique_ptr<ResponseDisplay> responseDisplay;
    std::unique_ptr<ForgeMeter>      meter;

    //==========================================================================
    // Sections
    //==========================================================================
    ForgeSection secF1{ "FILTER 1", ForgeColour::Orange };
    ForgeSection secF2{ "FILTER 2", ForgeColour::Cyan };
    ForgeSection secRouting{ "ROUTING", ForgeColour::Purple };
    ForgeSection secLfo{ "LFO", ForgeColour::Yellow };
    ForgeSection secEnv{ "ENVELOPE FOLLOWER", ForgeColour::Green };
    ForgeSection secOutput{ "OUTPUT", ForgeColour::Steel };

    //==========================================================================
    // Filter 1 controls
    //==========================================================================
    juce::ComboBox f1ModelCombo;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> f1ModelAttach;

    juce::ComboBox f1TypeCombo;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> f1TypeAttach;

    std::unique_ptr<ForgeKnob> f1CutoffKnob;
    std::unique_ptr<ForgeKnob> f1ResoKnob;
    std::unique_ptr<ForgeKnob> f1DriveKnob;

    juce::ToggleButton f1OnBtn{ "ON" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> f1OnAttach;

    //==========================================================================
    // Filter 2 controls
    //==========================================================================
    juce::ComboBox f2ModelCombo;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> f2ModelAttach;

    juce::ComboBox f2TypeCombo;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> f2TypeAttach;

    std::unique_ptr<ForgeKnob> f2CutoffKnob;
    std::unique_ptr<ForgeKnob> f2ResoKnob;
    std::unique_ptr<ForgeKnob> f2DriveKnob;

    juce::ToggleButton f2OnBtn{ "ON" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> f2OnAttach;

    //==========================================================================
    // Routing
    //==========================================================================
    juce::ComboBox routingCombo;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> routingAttach;

    std::unique_ptr<ForgeKnob> parallelMixKnob;

    //==========================================================================
    // LFO
    //==========================================================================
    juce::ToggleButton lfoOnBtn{ "ON" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lfoOnAttach;

    juce::ComboBox lfoShapeCombo;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfoShapeAttach;

    juce::ComboBox lfoTargetCombo;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfoTargetAttach;

    std::unique_ptr<ForgeKnob> lfoRateKnob;
    std::unique_ptr<ForgeKnob> lfoAmountKnob;
    std::unique_ptr<ForgeKnob> lfoDelayKnob;

    //==========================================================================
    // Envelope follower
    //==========================================================================
    juce::ToggleButton envOnBtn{ "ON" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> envOnAttach;

    juce::ComboBox envTargetCombo;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> envTargetAttach;

    std::unique_ptr<ForgeKnob> envAmountKnob;
    std::unique_ptr<ForgeKnob> envAttackKnob;
    std::unique_ptr<ForgeKnob> envReleaseKnob;

    //==========================================================================
    // Output
    //==========================================================================
    std::unique_ptr<ForgeKnob> mixKnob;
    std::unique_ptr<ForgeKnob> outputKnob;

    juce::ToggleButton bypassBtn{ "BYPASS" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttach;

    //==========================================================================
    // A/B
    //==========================================================================
    juce::TextButton abButton{ "A/B" };
    juce::TextButton captureBtn{ "CAP A" };

    //==========================================================================
    // Preset browser (right-side panel)
    //==========================================================================
    std::unique_ptr<PresetBrowser> presetBrowser;

    //==========================================================================
    // Helpers
    //==========================================================================
    void drawHeader(juce::Graphics&, juce::Rectangle<int>);
    void setupCombo(juce::ComboBox&, const juce::StringArray&);

    void onProcessorPresetChanged(int newIndex);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterForgeEditor)
};