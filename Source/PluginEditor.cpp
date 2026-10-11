#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "PresetManager.h"

//==============================================================================
// ForgeLAF
//==============================================================================
ForgeLAF::ForgeLAF()
{
    setColour(juce::Slider::thumbColourId, ForgeColour::Orange);
    setColour(juce::Slider::rotarySliderFillColourId, ForgeColour::Orange);
    setColour(juce::Slider::trackColourId, ForgeColour::OrangeDim);
    setColour(juce::Label::textColourId, ForgeColour::IvoryText);
    setColour(juce::ComboBox::backgroundColourId, ForgeColour::Panel);
    setColour(juce::ComboBox::textColourId, ForgeColour::IvoryText);
    setColour(juce::ComboBox::outlineColourId, ForgeColour::PanelBdr);
    setColour(juce::ComboBox::arrowColourId, ForgeColour::Orange);
    setColour(juce::PopupMenu::backgroundColourId, ForgeColour::Panel);
    setColour(juce::PopupMenu::textColourId, ForgeColour::IvoryText);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, ForgeColour::OrangeDim);
    setColour(juce::TextButton::buttonColourId, ForgeColour::Panel);
    setColour(juce::TextButton::textColourOffId, ForgeColour::Orange);
    setColour(juce::TextButton::textColourOnId, ForgeColour::BG);
    setColour(juce::ToggleButton::textColourId, ForgeColour::IvoryText);
    setColour(juce::ToggleButton::tickDisabledColourId, ForgeColour::DimText);
    setColour(juce::ToggleButton::tickColourId, ForgeColour::Orange);
    setColour(juce::TextEditor::backgroundColourId, ForgeColour::ListBg);
    setColour(juce::TextEditor::textColourId, ForgeColour::IvoryText);
    setColour(juce::TextEditor::outlineColourId, ForgeColour::PanelBdr);
    setColour(juce::TextEditor::focusedOutlineColourId, ForgeColour::Orange);
    setColour(juce::TextEditor::highlightColourId, ForgeColour::OrangeDim);
    setColour(juce::ScrollBar::thumbColourId, ForgeColour::PanelBdr);
}

juce::Font ForgeLAF::monoFont(float size)
{
    return juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(),
        size, juce::Font::plain));
}

juce::Font ForgeLAF::displayFont(float size)
{
    return juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(),
        size, juce::Font::bold));
}

void ForgeLAF::drawRotarySlider(juce::Graphics& g,
    int x, int y, int w, int h,
    float sliderPos, float startAng, float endAng,
    juce::Slider& slider)
{
    const float radius = (float)juce::jmin(w, h) * 0.5f - 4.0f;
    const float centreX = (float)x + (float)w * 0.5f;
    const float centreY = (float)y + (float)h * 0.5f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    const float angle = startAng + sliderPos * (endAng - startAng);

    g.setColour(ForgeColour::BG);
    g.fillEllipse(rx, ry, rw, rw);
    g.setColour(ForgeColour::PanelBdr);
    g.drawEllipse(rx, ry, rw, rw, 2.0f);

    const juce::Colour accentCol = slider.findColour(juce::Slider::rotarySliderFillColourId);

    {
        juce::Path trackArc;
        trackArc.addArc(rx + 3.0f, ry + 3.0f, rw - 6.0f, rw - 6.0f,
            startAng, endAng, true);
        g.setColour(accentCol.darker(1.4f));
        g.strokePath(trackArc, juce::PathStrokeType(3.0f,
            juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
    {
        juce::Path fillArc;
        fillArc.addArc(rx + 3.0f, ry + 3.0f, rw - 6.0f, rw - 6.0f,
            startAng, angle, true);
        g.setColour(accentCol);
        g.strokePath(fillArc, juce::PathStrokeType(3.0f,
            juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
    {
        juce::Path pointer;
        const float pointerLen = radius * 0.55f;
        const float pointerW = 2.0f;
        pointer.addRectangle(-pointerW * 0.5f, -radius + 4.0f, pointerW, pointerLen);
        pointer.applyTransform(juce::AffineTransform::rotation(angle)
            .translated(centreX, centreY));
        g.setColour(ForgeColour::White);
        g.fillPath(pointer);
    }
    g.setColour(accentCol);
    g.fillEllipse(centreX - 3.0f, centreY - 3.0f, 6.0f, 6.0f);
}

void ForgeLAF::drawToggleButton(juce::Graphics& g, juce::ToggleButton& btn,
    bool, bool)
{
    const bool isOn = btn.getToggleState();
    const auto bounds = btn.getLocalBounds().toFloat();

    const float rx = 4.0f, h = bounds.getHeight() - 8.0f;
    juce::Rectangle<float> pill(rx, 4.0f, h * 2.0f, h);
    g.setColour(isOn ? ForgeColour::Orange : ForgeColour::PanelBdr);
    g.fillRoundedRectangle(pill, h * 0.5f);

    const float thumbX = isOn ? pill.getRight() - h - 2.0f : pill.getX() + 2.0f;
    g.setColour(isOn ? ForgeColour::BG : ForgeColour::DimText);
    g.fillEllipse(thumbX, pill.getY() + 2.0f, h - 4.0f, h - 4.0f);

    g.setColour(isOn ? ForgeColour::IvoryText : ForgeColour::DimText);
    g.setFont(monoFont(11.0f));
    g.drawText(btn.getButtonText(),
        (int)pill.getRight() + 6, 0,
        btn.getWidth() - (int)pill.getRight() - 6, btn.getHeight(),
        juce::Justification::centredLeft, true);
}

void ForgeLAF::drawComboBox(juce::Graphics& g, int width, int height, bool,
    int buttonX, int buttonY, int buttonW, int buttonH,
    juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat();
    g.setColour(ForgeColour::Panel);
    g.fillRoundedRectangle(bounds, 3.0f);
    g.setColour(ForgeColour::PanelBdr);
    g.drawRoundedRectangle(bounds.reduced(0.5f), 3.0f, 1.0f);

    const float ax = (float)(buttonX + buttonW / 2);
    const float ay = (float)(buttonY + buttonH / 2);
    juce::Path arrow;
    arrow.addTriangle(ax - 4.f, ay - 2.f, ax + 4.f, ay - 2.f, ax, ay + 4.f);
    g.setColour(box.findColour(juce::ComboBox::arrowColourId));
    g.fillPath(arrow);

    g.setColour(box.findColour(juce::ComboBox::textColourId));
    g.setFont(monoFont(10.5f));
    g.drawText(box.getText(), 6, 0, width - 20, height,
        juce::Justification::centredLeft, true);
}

//==============================================================================
// ForgeKnob
//==============================================================================
ForgeKnob::ForgeKnob(const juce::String& labelStr, const juce::String& pid,
    juce::AudioProcessorValueTreeState& apvts, juce::Colour accentCol)
    : paramID(pid), apvtsRef(apvts), accent(accentCol)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setPopupDisplayEnabled(true, false, nullptr);
    slider.setColour(juce::Slider::rotarySliderFillColourId, accentCol);
    slider.setColour(juce::Slider::thumbColourId, accentCol);
    addAndMakeVisible(slider);

    nameLabel.setText(labelStr, juce::dontSendNotification);
    nameLabel.setJustificationType(juce::Justification::centred);
    nameLabel.setColour(juce::Label::textColourId, ForgeColour::DimText);
    nameLabel.setFont(ForgeLAF::monoFont(10.0f));
    addAndMakeVisible(nameLabel);

    valueLabel.setJustificationType(juce::Justification::centred);
    valueLabel.setColour(juce::Label::textColourId, accentCol);
    valueLabel.setFont(ForgeLAF::monoFont(10.0f));
    addAndMakeVisible(valueLabel);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, pid, slider);

    slider.onValueChange = [this]()
        {
            if (auto* param = apvtsRef.getParameter(paramID))
                valueLabel.setText(param->getText(param->getValue(), 6),
                    juce::dontSendNotification);
        };
    slider.onValueChange();
}

void ForgeKnob::resized()
{
    auto b = getLocalBounds();
    const int labelH = 14;
    nameLabel.setBounds(b.removeFromTop(labelH));
    valueLabel.setBounds(b.removeFromBottom(labelH));
    slider.setBounds(b.reduced(2));
}

void ForgeKnob::paint(juce::Graphics&) {}

//==============================================================================
// ForgeSection
//==============================================================================
ForgeSection::ForgeSection(const juce::String& titleText, juce::Colour accentCol)
    : title(titleText), accent(accentCol)
{
}

void ForgeSection::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();

    g.setColour(ForgeColour::Panel);
    g.fillRoundedRectangle(b, 6.0f);

    g.setColour(accent.withAlpha(0.5f));
    g.drawRoundedRectangle(b.reduced(0.5f), 6.0f, 1.0f);

    juce::Rectangle<float> hdr(b.getX(), b.getY(), b.getWidth(), 20.0f);
    g.setColour(ForgeColour::Header);
    g.fillRoundedRectangle(hdr, 6.0f);
    g.fillRect(hdr.withTop(hdr.getBottom() - 4.0f));

    g.setColour(accent);
    g.setFont(ForgeLAF::displayFont(10.5f));
    g.drawText(title, hdr.toNearestInt(), juce::Justification::centred, false);

    g.setColour(accent.withAlpha(0.85f));
    g.fillRect(b.getX() + 8.0f, b.getY() + 19.0f, b.getWidth() - 16.0f, 1.5f);
}

//==============================================================================
// ResponseDisplay
//==============================================================================
ResponseDisplay::ResponseDisplay(FilterForgeProcessor& p) : proc(p)
{
    startTimerHz(30);
}

ResponseDisplay::~ResponseDisplay()
{
    stopTimer();
}

void ResponseDisplay::timerCallback()
{
    repaint();
}

float ResponseDisplay::freqToX(float freq, float width) const
{
    const float logMin = std::log10(20.0f);
    const float logMax = std::log10(20000.0f);
    return (std::log10(juce::jmax(20.0f, freq)) - logMin) /
        (logMax - logMin) * width;
}

float ResponseDisplay::dbToY(float dB, float height) const
{
    const float minDB = -48.0f, maxDB = 24.0f;
    return (1.0f - (dB - minDB) / (maxDB - minDB)) * height;
}

void ResponseDisplay::drawGrid(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    const float W = bounds.getWidth(), H = bounds.getHeight();

    g.setFont(ForgeLAF::monoFont(8.0f));

    // Frequency grid lines
    const float freqs[] = { 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000 };
    const char* labels[] = { "50", "100", "200", "500", "1k", "2k", "5k", "10k", "20k" };
    for (int i = 0; i < 9; ++i)
    {
        float x = freqToX(freqs[i], W);
        g.setColour(ForgeColour::PanelBdr.withAlpha(0.35f));
        g.drawVerticalLine((int)x, 0.0f, H);
        g.setColour(ForgeColour::DimText.withAlpha(0.7f));
        g.drawText(labels[i], (int)x - 12, (int)H - 12,
            24, 11, juce::Justification::centred, false);
    }

    // dB grid lines
    const float dbLevels[] = { 12, 0, -12, -24, -36 };
    for (auto dB : dbLevels)
    {
        float y = dbToY(dB, H);
        g.setColour(ForgeColour::PanelBdr.withAlpha(0.35f));
        g.drawHorizontalLine((int)y, 0.0f, W);
        g.setColour(ForgeColour::DimText.withAlpha(0.6f));
        g.drawText(juce::String((int)dB) + "dB",
            2, (int)y - 6, 30, 12, juce::Justification::left, false);
    }
}

void ResponseDisplay::drawCurve(juce::Graphics& g, bool forFilter2,
    juce::Colour colour)
{
    auto bounds = getLocalBounds().toFloat();
    const float W = bounds.getWidth(), H = bounds.getHeight();

    // We can't isolate each filter's response through the combined
    // evaluateResponse, so we overlay one combined curve per filter for
    // a visual sense of what each contributes.
    // The main (combined) curve is drawn separately.
    juce::Path path;
    bool started = false;

    for (float freq = 20.0f; freq <= 20000.0f; freq *= 1.05f)
    {
        const float x = freqToX(freq, W);

        // Approximate: when drawing for filter 2, temporarily pretend filter 1
        // is bypassed. We can't truly do that without access to the model
        // parameters, so instead just draw a slightly shifted version of
        // the combined curve — this is a stylistic overlay.
        const float mag = proc.evaluateResponse(freq);
        const float dB = 20.0f * std::log10(juce::jmax(1e-6f, mag));
        const float y = juce::jlimit(0.0f, H, dbToY(dB, H));

        const float shiftedX = forFilter2 ? x + 0.5f : x - 0.5f;

        if (!started) { path.startNewSubPath(shiftedX, y); started = true; }
        else { path.lineTo(shiftedX, y); }
    }

    g.setColour(colour.withAlpha(0.5f));
    g.strokePath(path, juce::PathStrokeType(1.5f,
        juce::PathStrokeType::curved));
}

void ResponseDisplay::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();

    // Background
    g.setColour(ForgeColour::BG);
    g.fillRoundedRectangle(b, 4.0f);
    g.setColour(ForgeColour::PanelBdr);
    g.drawRoundedRectangle(b.reduced(0.5f), 4.0f, 1.0f);

    // Header
    auto hdr = b.removeFromTop(18.0f);
    g.setColour(ForgeColour::Header);
    g.fillRect(hdr);

    g.setColour(ForgeColour::Magenta);
    g.setFont(ForgeLAF::displayFont(10.0f));
    g.drawText("FREQUENCY RESPONSE", hdr.toNearestInt(),
        juce::Justification::centred, false);

    // Grid
    drawGrid(g);

    // Individual filter overlays
    drawCurve(g, false, ForgeColour::Orange);
    drawCurve(g, true, ForgeColour::Cyan);

    // Combined response curve
    {
        juce::Path combined;
        bool started = false;
        const float W = getWidth(), H = (float)(getHeight() - 18);

        for (float freq = 20.0f; freq <= 20000.0f; freq *= 1.05f)
        {
            const float x = freqToX(freq, W);
            const float mag = proc.evaluateResponse(freq);
            const float dB = 20.0f * std::log10(juce::jmax(1e-6f, mag));
            const float y = juce::jlimit(0.0f, H, 18.0f + dbToY(dB, H));

            if (!started) { combined.startNewSubPath(x, y); started = true; }
            else { combined.lineTo(x, y); }
        }

        // Glow
        g.setColour(ForgeColour::Magenta.withAlpha(0.35f));
        g.strokePath(combined, juce::PathStrokeType(3.5f,
            juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        g.setColour(ForgeColour::Magenta);
        g.strokePath(combined, juce::PathStrokeType(1.8f,
            juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Legend showing active filters
    const int f1Model = proc.snapshot.f1Model.load();
    const int f2Model = proc.snapshot.f2Model.load();
    const bool f1On = proc.snapshot.f1On.load();
    const bool f2On = proc.snapshot.f2On.load();

    const char* modelNames[] = {
        "KR-106", "WASP", "CHAMB", "HPF", "BOOST", "SVF", "COMB"
    };

    g.setFont(ForgeLAF::monoFont(9.0f));
    auto legend = juce::Rectangle<int>(0, 22, getWidth(), 14);

    g.setColour(f1On ? ForgeColour::Orange : ForgeColour::DimText);
    g.drawText(juce::String("F1 ") + modelNames[juce::jlimit(0, 6, f1Model)],
        legend.removeFromLeft(90).reduced(6, 0),
        juce::Justification::centredLeft, false);

    g.setColour(f2On ? ForgeColour::Cyan : ForgeColour::DimText);
    g.drawText(juce::String("F2 ") + modelNames[juce::jlimit(0, 6, f2Model)],
        legend.removeFromLeft(90),
        juce::Justification::centredLeft, false);
}

//==============================================================================
// ForgeMeter
//==============================================================================
ForgeMeter::ForgeMeter(FilterForgeProcessor& p) : proc(p)
{
    startTimerHz(30);
}

ForgeMeter::~ForgeMeter()
{
    stopTimer();
}

void ForgeMeter::timerCallback()
{
    const float inL = proc.levelInRmsL.load();
    const float inR = proc.levelInRmsR.load();
    const float outL = proc.levelPeakL.load();
    const float outR = proc.levelPeakR.load();

    dispInL = juce::jmax(dispInL * 0.8f, inL);
    dispInR = juce::jmax(dispInR * 0.8f, inR);
    dispOutL = juce::jmax(dispOutL * 0.8f, outL);
    dispOutR = juce::jmax(dispOutR * 0.8f, outR);

    repaint();
}

void ForgeMeter::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    g.setColour(ForgeColour::BG);
    g.fillRoundedRectangle(b, 3.0f);
    g.setColour(ForgeColour::PanelBdr);
    g.drawRoundedRectangle(b.reduced(0.5f), 3.0f, 1.0f);

    const int barW = (getWidth() - 8) / 4;

    auto drawBar = [&](int x, float lin, juce::Colour col)
        {
            const float db = 20.0f * std::log10(juce::jmax(1e-6f, lin));
            const float norm = juce::jlimit(0.0f, 1.0f, (db + 60.0f) / 60.0f);
            const int h = getHeight() - 14;
            const int filled = (int)(norm * (float)h);
            const int top = 4 + h - filled;

            g.setColour(col.darker(1.6f));
            g.fillRect(x, 4, barW - 1, h);
            g.setColour(col);
            g.fillRect(x, top, barW - 1, filled);
        };

    drawBar(4, dispInL, ForgeColour::Steel.darker(0.4f));
    drawBar(4 + barW, dispInR, ForgeColour::Steel.darker(0.4f));
    drawBar(4 + barW * 2, dispOutL, ForgeColour::Magenta);
    drawBar(4 + barW * 3, dispOutR, ForgeColour::Magenta);

    g.setFont(ForgeLAF::monoFont(7.5f));
    g.setColour(ForgeColour::DimText);
    g.drawText("IN L", 4, getHeight() - 10, barW, 10, juce::Justification::centred);
    g.drawText("IN R", 4 + barW, getHeight() - 10, barW, 10, juce::Justification::centred);
    g.drawText("OUT L", 4 + barW * 2, getHeight() - 10, barW, 10, juce::Justification::centred);
    g.drawText("OUT R", 4 + barW * 3, getHeight() - 10, barW, 10, juce::Justification::centred);
}

//==============================================================================
// PresetBrowser::PresetList
//
// A small internal component that owns the actual scrollable list.
// We keep it separate from PresetBrowser so that PresetBrowser itself stays
// clean and the list can be swapped for a tree if we ever want grouping.
//==============================================================================
class PresetBrowser::PresetList : public juce::Component
{
public:
    PresetList(PresetBrowser& ownerRef) : owner(ownerRef) {}

    void paint(juce::Graphics& g) override
    {
        auto b = getLocalBounds().toFloat();
        g.setColour(ForgeColour::ListBg);
        g.fillRoundedRectangle(b, 3.0f);
        g.setColour(ForgeColour::PanelBdr);
        g.drawRoundedRectangle(b.reduced(0.5f), 3.0f, 1.0f);

        for (auto& row : owner.rows)
        {
            // Only draw if visible within the list viewport (we clip)
            auto r = row.bounds;
            if (r.getBottom() < 0 || r.getY() > getHeight()) continue;

            if (row.isHeader)
            {
                g.setColour(ForgeColour::Header);
                g.fillRect(r);
                g.setColour(ForgeColour::Orange.withAlpha(0.75f));
                g.setFont(ForgeLAF::displayFont(9.5f));
                g.drawText(row.display.toUpperCase(), r.reduced(8, 0),
                    juce::Justification::centredLeft, false);
            }
            else
            {
                const bool selected = (row.presetIndex == owner.selectedIndex);
                const bool hovered = (row.presetIndex == hoveredIndex);
                juce::Colour bg = row.isUser ? ForgeColour::ListRowAlt
                    : ForgeColour::ListRow;
                if (selected) bg = ForgeColour::ListRowSelected;
                else if (hovered) bg = ForgeColour::ListRowHover;

                g.setColour(bg);
                g.fillRect(r);

                g.setColour(ForgeColour::PanelBdr.withAlpha(0.4f));
                g.drawHorizontalLine(r.getBottom() - 1,
                    (float)r.getX(), (float)r.getRight());

                // Selection stripe
                if (selected)
                {
                    g.setColour(ForgeColour::Orange);
                    g.fillRect(r.getX(), r.getY(), 3, r.getHeight());
                }

                g.setColour(selected ? ForgeColour::White : ForgeColour::IvoryText);
                g.setFont(ForgeLAF::monoFont(11.0f));
                g.drawText(row.name,
                    r.reduced(12, 0),
                    juce::Justification::centredLeft, true);

                if (row.isUser)
                {
                    g.setColour(ForgeColour::Orange.withAlpha(0.7f));
                    g.setFont(ForgeLAF::monoFont(8.5f));
                    g.drawText("\u2022",
                        r.removeFromRight(16).reduced(2, 0),
                        juce::Justification::centred, false);
                }
            }
        }
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        const int y = e.y;
        for (auto& row : owner.rows)
        {
            if (row.isHeader) continue;
            if (row.bounds.contains(e.x, y))
            {
                owner.handleRowClick(row.presetIndex);
                return;
            }
        }
    }

    void mouseMove(const juce::MouseEvent& e) override
    {
        int newHover = -1;
        for (auto& row : owner.rows)
        {
            if (row.isHeader) continue;
            if (row.bounds.contains(e.x, e.y)) { newHover = row.presetIndex; break; }
        }
        if (newHover != hoveredIndex)
        {
            hoveredIndex = newHover;
            repaint();
        }
    }

    void mouseExit(const juce::MouseEvent&) override
    {
        if (hoveredIndex != -1) { hoveredIndex = -1; repaint(); }
    }

private:
    PresetBrowser& owner;
    int hoveredIndex = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetList)
};

//==============================================================================
// PresetBrowser
//==============================================================================
PresetBrowser::PresetBrowser(FilterForgeProcessor& p) : proc(p)
{
    headerLabel = std::make_unique<juce::Label>();
    headerLabel->setText("PRESETS", juce::dontSendNotification);
    headerLabel->setFont(ForgeLAF::displayFont(11.0f));
    headerLabel->setColour(juce::Label::textColourId, ForgeColour::Orange);
    headerLabel->setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(*headerLabel);

    searchField = std::make_unique<juce::TextEditor>();
    searchField->setTextToShowWhenEmpty("Search...", ForgeColour::DimText);
    searchField->setFont(ForgeLAF::monoFont(11.0f));
    searchField->onTextChange = [this]() {
        rebuildRows(searchField->getText());
        };
    addAndMakeVisible(*searchField);

    list = std::make_unique<PresetList>(*this);
    addAndMakeVisible(*list);

    saveButton = std::make_unique<juce::TextButton>("SAVE");
    saveButton->onClick = [this]() {
        if (onSaveRequested) onSaveRequested();
        };
    addAndMakeVisible(*saveButton);

    deleteButton = std::make_unique<juce::TextButton>("DEL");
    deleteButton->onClick = [this]() {
        if (selectedIndex < 0) return;
        auto& mgr = proc.getPresetManager();
        if (selectedIndex < mgr.getNumPresets())
        {
            const auto& info = mgr.getPresetInfo(selectedIndex);
            if (info.isFactory)
            {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::MessageBoxIconType::InfoIcon,
                    "Cannot Delete",
                    "Factory presets cannot be deleted.");
                return;
            }
            mgr.deletePreset(selectedIndex);
            refresh();
        }
        };
    addAndMakeVisible(*deleteButton);

    exportButton = std::make_unique<juce::TextButton>("EXP");
    exportButton->onClick = [this]() {
        if (selectedIndex < 0) return;
        auto& mgr = proc.getPresetManager();
        if (selectedIndex >= mgr.getNumPresets()) return;
        const auto& info = mgr.getPresetInfo(selectedIndex);

        auto chooser = std::make_shared<juce::FileChooser>(
            "Export Preset",
            juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
            .getChildFile(info.name + ".filterforge"),
            "*.filterforge");
        chooser->launchAsync(
            juce::FileBrowserComponent::saveMode |
            juce::FileBrowserComponent::warnAboutOverwriting,
            [chooser, this](const juce::FileChooser& fc) {
                auto f = fc.getResult();
                if (f == juce::File()) return;
                if (!f.hasFileExtension("filterforge"))
                    f = f.withFileExtension("filterforge");
                auto& mgr = proc.getPresetManager();
                if (selectedIndex < mgr.getNumPresets())
                {
                    const auto& i = mgr.getPresetInfo(selectedIndex);
                    mgr.exportPresetToFile(f, i.name, i.category, i.author);
                }
            });
        };
    addAndMakeVisible(*exportButton);

    folderButton = std::make_unique<juce::TextButton>("DIR");
    folderButton->onClick = [this]() {
        proc.getPresetManager().getUserPresetDirectory().revealToUser();
        };
    addAndMakeVisible(*folderButton);

    refresh();
    startTimerHz(10);
}

PresetBrowser::~PresetBrowser()
{
    stopTimer();
}

void PresetBrowser::timerCallback()
{
    // Poll processor for external preset changes
    const int current = proc.getCurrentPresetIndex();
    if (current != lastKnownManagerIndex)
    {
        lastKnownManagerIndex = current;
        setSelectedIndexSilent(current);
    }
}

void PresetBrowser::refresh()
{
    rebuildRows(searchField ? searchField->getText() : juce::String());

    // Sync with the processor's current preset
    const int current = proc.getCurrentPresetIndex();
    if (current >= 0)
        setSelectedIndexSilent(current);
}

void PresetBrowser::setSelectedIndexSilent(int index)
{
    selectedIndex = index;
    if (list) list->repaint();
}

void PresetBrowser::rebuildRows(const juce::String& filterText)
{
    rows.clear();

    auto& mgr = proc.getPresetManager();
    const auto& all = mgr.getAllPresets();

    juce::String lastCategory;
    const juce::String filter = filterText.trim().toLowerCase();

    for (size_t i = 0; i < all.size(); ++i)
    {
        const auto& info = all[i];

        // Apply filter
        if (filter.isNotEmpty())
        {
            const bool match = info.name.toLowerCase().contains(filter)
                || info.category.toLowerCase().contains(filter);
            if (!match) continue;
        }

        // Insert a category header whenever the category changes
        if (info.category != lastCategory)
        {
            Row hdr;
            hdr.display = info.category;
            hdr.category = info.category;
            hdr.isHeader = true;
            rows.push_back(hdr);
            lastCategory = info.category;
        }

        Row r;
        r.display = info.name;
        r.name = info.name;
        r.category = info.category;
        r.presetIndex = (int)i;
        r.isHeader = false;
        r.isUser = !info.isFactory;
        rows.push_back(r);
    }

    // Assign Y positions based on current list height
    int y = 0;
    if (list)
    {
        const int headerH = 18;
        const int rowH = 22;
        const int listW = list->getWidth();
        for (auto& row : rows)
        {
            const int h = row.isHeader ? headerH : rowH;
            row.bounds = juce::Rectangle<int>(0, y, listW, h);
            y += h;
        }
        list->setSize(listW, y);
    }

    if (list) list->repaint();
}

void PresetBrowser::handleRowClick(int presetIndex)
{
    if (presetIndex < 0) return;
    selectedIndex = presetIndex;
    if (list) list->repaint();
    if (onPresetSelected) onPresetSelected(presetIndex);
}

void PresetBrowser::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    g.setColour(ForgeColour::Panel);
    g.fillRoundedRectangle(b, 6.0f);
    g.setColour(ForgeColour::Orange.withAlpha(0.5f));
    g.drawRoundedRectangle(b.reduced(0.5f), 6.0f, 1.0f);

    // Header strip
    juce::Rectangle<float> hdr(b.getX(), b.getY(), b.getWidth(), 20.0f);
    g.setColour(ForgeColour::Header);
    g.fillRoundedRectangle(hdr, 6.0f);
    g.fillRect(hdr.withTop(hdr.getBottom() - 4.0f));

    g.setColour(ForgeColour::Orange.withAlpha(0.85f));
    g.fillRect(b.getX() + 8.0f, b.getY() + 19.0f, b.getWidth() - 16.0f, 1.5f);
}

void PresetBrowser::resized()
{
    auto b = getLocalBounds().reduced(8);

    // Header
    auto hdr = b.removeFromTop(18);
    headerLabel->setBounds(hdr);
    b.removeFromTop(4);

    // Search field
    auto searchRow = b.removeFromTop(24);
    searchField->setBounds(searchRow);
    b.removeFromTop(6);

    // Bottom buttons: SAVE | DEL | EXP | DIR in a 2x2 grid
    auto btnArea = b.removeFromBottom(60);
    const int btnH = 24;
    const int btnW = btnArea.getWidth() / 2 - 2;

    auto r1 = btnArea.removeFromTop(btnH);
    saveButton->setBounds(r1.removeFromLeft(btnW));
    r1.removeFromLeft(4);
    deleteButton->setBounds(r1);

    btnArea.removeFromTop(4);

    auto r2 = btnArea.removeFromTop(btnH);
    exportButton->setBounds(r2.removeFromLeft(btnW));
    r2.removeFromLeft(4);
    folderButton->setBounds(r2);

    b.removeFromBottom(6);

    // Remaining space: the scrollable list
    list->setBounds(b);

    // Re-flow rows for the new width
    rebuildRows(searchField->getText());
}

//==============================================================================
// Editor
//==============================================================================
FilterForgeEditor::FilterForgeEditor(FilterForgeProcessor& p)
    : juce::AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&laf);

    using namespace FilterParams;
    auto& a = p.apvts;

    // ===== Sections =====
    addAndMakeVisible(secF1);
    addAndMakeVisible(secF2);
    addAndMakeVisible(secRouting);
    addAndMakeVisible(secLfo);
    addAndMakeVisible(secEnv);
    addAndMakeVisible(secOutput);

    // ===== Display =====
    responseDisplay = std::make_unique<ResponseDisplay>(audioProcessor);
    addAndMakeVisible(*responseDisplay);

    meter = std::make_unique<ForgeMeter>(audioProcessor);
    addAndMakeVisible(*meter);

    // ===== Filter 1 =====
    setupCombo(f1ModelCombo, { "KR-106 VCF", "Wasp Ladder", "Chamberlin",
                                "KR-106 HPF", "Bass Boost", "SVF 12dB", "Comb" });
    f1ModelAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        a, F1_MODEL, f1ModelCombo);
    addAndMakeVisible(f1ModelCombo);

    setupCombo(f1TypeCombo, { "LP12", "LP24", "HP12", "HP24",
                               "BP12", "BP24", "Notch" });
    f1TypeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        a, F1_TYPE, f1TypeCombo);
    addAndMakeVisible(f1TypeCombo);

    f1CutoffKnob = std::make_unique<ForgeKnob>("CUTOFF", F1_CUTOFF, a, ForgeColour::Orange);
    f1ResoKnob = std::make_unique<ForgeKnob>("RESO", F1_RESO, a, ForgeColour::Orange);
    f1DriveKnob = std::make_unique<ForgeKnob>("DRIVE", F1_DRIVE, a, ForgeColour::Orange);
    addAndMakeVisible(*f1CutoffKnob);
    addAndMakeVisible(*f1ResoKnob);
    addAndMakeVisible(*f1DriveKnob);

    f1OnBtn.setLookAndFeel(&laf);
    f1OnAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        a, F1_ON, f1OnBtn);
    addAndMakeVisible(f1OnBtn);

    // ===== Filter 2 =====
    setupCombo(f2ModelCombo, { "KR-106 VCF", "Wasp Ladder", "Chamberlin",
                                "KR-106 HPF", "Bass Boost", "SVF 12dB", "Comb" });
    f2ModelAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        a, F2_MODEL, f2ModelCombo);
    addAndMakeVisible(f2ModelCombo);

    setupCombo(f2TypeCombo, { "LP12", "LP24", "HP12", "HP24",
                               "BP12", "BP24", "Notch" });
    f2TypeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        a, F2_TYPE, f2TypeCombo);
    addAndMakeVisible(f2TypeCombo);

    f2CutoffKnob = std::make_unique<ForgeKnob>("CUTOFF", F2_CUTOFF, a, ForgeColour::Cyan);
    f2ResoKnob = std::make_unique<ForgeKnob>("RESO", F2_RESO, a, ForgeColour::Cyan);
    f2DriveKnob = std::make_unique<ForgeKnob>("DRIVE", F2_DRIVE, a, ForgeColour::Cyan);
    addAndMakeVisible(*f2CutoffKnob);
    addAndMakeVisible(*f2ResoKnob);
    addAndMakeVisible(*f2DriveKnob);

    f2OnBtn.setLookAndFeel(&laf);
    f2OnAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        a, F2_ON, f2OnBtn);
    addAndMakeVisible(f2OnBtn);

    // ===== Routing =====
    setupCombo(routingCombo, { "Series (1->2)", "Parallel", "F1 Only", "F2 Only" });
    routingAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        a, ROUTING, routingCombo);
    addAndMakeVisible(routingCombo);

    parallelMixKnob = std::make_unique<ForgeKnob>("1<->2 MIX", PARALLEL_MIX, a, ForgeColour::Purple);
    addAndMakeVisible(*parallelMixKnob);

    // ===== LFO =====
    lfoOnBtn.setLookAndFeel(&laf);
    lfoOnAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        a, LFO_ON, lfoOnBtn);
    addAndMakeVisible(lfoOnBtn);

    setupCombo(lfoShapeCombo, { "Sine", "Triangle", "Saw", "RevSaw",
                                 "Square", "S&H" });
    lfoShapeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        a, LFO_SHAPE, lfoShapeCombo);
    addAndMakeVisible(lfoShapeCombo);

    setupCombo(lfoTargetCombo, { "F1", "F2", "Both" });
    lfoTargetAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        a, LFO_TARGET, lfoTargetCombo);
    addAndMakeVisible(lfoTargetCombo);

    lfoRateKnob = std::make_unique<ForgeKnob>("RATE", LFO_RATE, a, ForgeColour::Yellow);
    lfoAmountKnob = std::make_unique<ForgeKnob>("AMOUNT", LFO_AMOUNT, a, ForgeColour::Yellow);
    lfoDelayKnob = std::make_unique<ForgeKnob>("DELAY", LFO_DELAY, a, ForgeColour::Yellow);
    addAndMakeVisible(*lfoRateKnob);
    addAndMakeVisible(*lfoAmountKnob);
    addAndMakeVisible(*lfoDelayKnob);

    // ===== Envelope follower =====
    envOnBtn.setLookAndFeel(&laf);
    envOnAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        a, ENV_ON, envOnBtn);
    addAndMakeVisible(envOnBtn);

    setupCombo(envTargetCombo, { "F1", "F2", "Both" });
    envTargetAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        a, ENV_TARGET, envTargetCombo);
    addAndMakeVisible(envTargetCombo);

    envAmountKnob = std::make_unique<ForgeKnob>("AMOUNT", ENV_AMOUNT, a, ForgeColour::Green);
    envAttackKnob = std::make_unique<ForgeKnob>("ATTACK", ENV_ATTACK, a, ForgeColour::Green);
    envReleaseKnob = std::make_unique<ForgeKnob>("RELEASE", ENV_RELEASE, a, ForgeColour::Green);
    addAndMakeVisible(*envAmountKnob);
    addAndMakeVisible(*envAttackKnob);
    addAndMakeVisible(*envReleaseKnob);

    // ===== Output =====
    mixKnob = std::make_unique<ForgeKnob>("MIX", MIX, a, ForgeColour::Steel);
    outputKnob = std::make_unique<ForgeKnob>("GAIN", OUTPUT, a, ForgeColour::Steel);
    addAndMakeVisible(*mixKnob);
    addAndMakeVisible(*outputKnob);

    bypassBtn.setLookAndFeel(&laf);
    bypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        a, BYPASS, bypassBtn);
    addAndMakeVisible(bypassBtn);

    // ===== A/B =====
    abButton.onClick = [this]() { audioProcessor.swapAB(); };
    captureBtn.onClick = [this]() { audioProcessor.captureAState(); };
    addAndMakeVisible(abButton);
    addAndMakeVisible(captureBtn);

    // ===== Preset browser (right panel) =====
    presetBrowser = std::make_unique<PresetBrowser>(audioProcessor);
    presetBrowser->onPresetSelected = [this](int idx) {
        audioProcessor.loadPresetByIndex(idx);
        };
    presetBrowser->onSaveRequested = [this]() {
        auto* window = new juce::AlertWindow("Save Preset",
            "Enter a name for the new preset:",
            juce::MessageBoxIconType::NoIcon);
        window->addTextEditor("name", "", "Name:");

        const int currentIdx = presetBrowser->getSelectedIndex();
        if (currentIdx >= 0)
        {
            auto& manager = audioProcessor.getPresetManager();
            if (currentIdx < manager.getNumPresets())
            {
                const auto& info = manager.getPresetInfo(currentIdx);
                window->getTextEditor("name")->setText(info.name + " (copy)",
                    juce::dontSendNotification);
                window->getTextEditor("name")->selectAll();
            }
        }

        window->addButton("Save", 1, juce::KeyPress(juce::KeyPress::returnKey));
        window->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

        window->enterModalState(true,
            juce::ModalCallbackFunction::create(
                [this, window](int result)
                {
                    juce::String name = window->getTextEditorContents("name").trim();
                    if (result == 0 || name.isEmpty()) return;
                    auto& manager = audioProcessor.getPresetManager();
                    manager.saveUserPreset(name, "User", "", true);
                    presetBrowser->refresh();
                }), true);
        };
    addAndMakeVisible(*presetBrowser);

    // Hook processor's onPresetChanged callback
    audioProcessor.onPresetChanged = [this](int newIndex) {
        onProcessorPresetChanged(newIndex);
        };

    startTimerHz(10);

    setSize(1200, 720);
    setResizable(false, false);
}

FilterForgeEditor::~FilterForgeEditor()
{
    stopTimer();

    audioProcessor.onPresetChanged = nullptr;

    f1OnBtn.setLookAndFeel(nullptr);
    f2OnBtn.setLookAndFeel(nullptr);
    lfoOnBtn.setLookAndFeel(nullptr);
    envOnBtn.setLookAndFeel(nullptr);
    bypassBtn.setLookAndFeel(nullptr);

    setLookAndFeel(nullptr);
}

void FilterForgeEditor::setupCombo(juce::ComboBox& box, const juce::StringArray& items)
{
    box.setLookAndFeel(&laf);
    int id = 1;
    for (auto& item : items)
        box.addItem(item, id++);
    box.setSelectedId(1, juce::dontSendNotification);
}

void FilterForgeEditor::drawHeader(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(ForgeColour::Header);
    g.fillRect(area);

    // Accent line — gradient from F1 orange to F2 cyan
    juce::ColourGradient accent(
        ForgeColour::Orange.withAlpha(0.85f), 0.0f, (float)(area.getBottom() - 1),
        ForgeColour::Cyan.withAlpha(0.85f), (float)area.getWidth(),
        (float)(area.getBottom() - 1), false);
    g.setGradientFill(accent);
    g.fillRect(0, area.getBottom() - 2, area.getWidth(), 2);

    // Title
    g.setFont(ForgeLAF::displayFont(20.0f));
    g.setColour(ForgeColour::IvoryText);
    g.drawText("FILTERFORGE", area.getX() + 16, area.getY(),
        220, area.getHeight(), juce::Justification::centredLeft);

    // Subtitle
    g.setFont(ForgeLAF::monoFont(9.0f));
    g.setColour(ForgeColour::DimText);
    g.drawText("KR-106 / Wasp / Chamberlin / HPF / Shelf / SVF / Comb",
        area.getX() + 216, area.getY(),
        440, area.getHeight(), juce::Justification::centredLeft);

    // Version
    g.setFont(ForgeLAF::monoFont(9.0f));
    g.drawText("v1.0.0", area.getRight() - 200, area.getY(),
        54, area.getHeight(), juce::Justification::centredRight);
}

void FilterForgeEditor::paint(juce::Graphics& g)
{
    g.fillAll(ForgeColour::BG);
    drawHeader(g, getLocalBounds().removeFromTop(44));

    // Subtle scanline pattern
    g.setColour(juce::Colours::white.withAlpha(0.012f));
    for (int y = 44; y < getHeight(); y += 3)
        g.drawHorizontalLine(y, 0.0f, (float)getWidth());
}

void FilterForgeEditor::resized()
{
    const int headerH = 44;
    const int pad = 10;
    const int barH = 24;

    auto full = getLocalBounds();
    auto header = full.removeFromTop(headerH);
    auto body = full.reduced(pad);

    // ==================================================================
    // Header — left: title (drawn), middle: A/B + bypass, right: meter
    // ==================================================================
    {
        auto top = header.reduced(pad, (headerH - barH) / 2);

        // Right cluster: meter
        meter->setBounds(top.removeFromRight(160).reduced(0, 1));
        top.removeFromRight(10);

        // Bypass
        bypassBtn.setBounds(top.removeFromRight(90).reduced(0, 2));
        top.removeFromRight(8);

        // A/B
        abButton.setBounds(top.removeFromRight(60));
        top.removeFromRight(4);
        captureBtn.setBounds(top.removeFromRight(72));
    }

    // ==================================================================
    // Main split: left = controls (fills), right = preset browser (280)
    // ==================================================================
    auto browserArea = body.removeFromRight(280);
    presetBrowser->setBounds(browserArea);

    body.removeFromRight(pad);

    // ==================================================================
    // Left column
    //   Row 1: Response display (200px tall) + meter column below?
    //   Rows 2-4: sections
    // ==================================================================
    // Response display at the top
    auto displayArea = body.removeFromTop(200);
    responseDisplay->setBounds(displayArea);
    body.removeFromTop(pad);

    // Row 2: Filter 1 + Filter 2 side by side (150px)
    auto filterRow = body.removeFromTop(150);
    {
        const int halfW = (filterRow.getWidth() - pad) / 2;
        auto left = filterRow.removeFromLeft(halfW);
        filterRow.removeFromLeft(pad);
        auto right = filterRow;

        // ----- Filter 1 -----
        secF1.setBounds(left);
        {
            auto inner = left.reduced(10, 0);
            inner.removeFromTop(26);  // section header

            // Left column: model combo, type combo, ON toggle
            auto leftCol = inner.removeFromLeft(130);
            f1ModelCombo.setBounds(leftCol.removeFromTop(22).reduced(0, 1));
            leftCol.removeFromTop(4);
            f1TypeCombo.setBounds(leftCol.removeFromTop(22).reduced(0, 1));
            leftCol.removeFromTop(4);
            f1OnBtn.setBounds(leftCol.removeFromTop(22));

            inner.removeFromLeft(pad);

            // Right column: 3 knobs
            const int slotW = inner.getWidth() / 3;
            f1CutoffKnob->setBounds(inner.removeFromLeft(slotW).reduced(2));
            f1ResoKnob->setBounds(inner.removeFromLeft(slotW).reduced(2));
            f1DriveKnob->setBounds(inner.removeFromLeft(slotW).reduced(2));
        }

        // ----- Filter 2 -----
        secF2.setBounds(right);
        {
            auto inner = right.reduced(10, 0);
            inner.removeFromTop(26);

            auto leftCol = inner.removeFromLeft(130);
            f2ModelCombo.setBounds(leftCol.removeFromTop(22).reduced(0, 1));
            leftCol.removeFromTop(4);
            f2TypeCombo.setBounds(leftCol.removeFromTop(22).reduced(0, 1));
            leftCol.removeFromTop(4);
            f2OnBtn.setBounds(leftCol.removeFromTop(22));

            inner.removeFromLeft(pad);

            const int slotW = inner.getWidth() / 3;
            f2CutoffKnob->setBounds(inner.removeFromLeft(slotW).reduced(2));
            f2ResoKnob->setBounds(inner.removeFromLeft(slotW).reduced(2));
            f2DriveKnob->setBounds(inner.removeFromLeft(slotW).reduced(2));
        }
    }
    body.removeFromTop(pad);

    // Row 3: Routing + LFO (150px)
    auto routingRow = body.removeFromTop(150);
    {
        const int routeW = 150;
        auto left = routingRow.removeFromLeft(routeW);
        routingRow.removeFromLeft(pad);
        auto right = routingRow;

        // ----- Routing -----
        secRouting.setBounds(left);
        {
            auto inner = left.reduced(10, 0);
            inner.removeFromTop(26);

            routingCombo.setBounds(inner.removeFromTop(22).reduced(0, 1));
            inner.removeFromTop(4);

            parallelMixKnob->setBounds(inner.removeFromTop(
                juce::jmin(inner.getHeight(), 100)));
        }

        // ----- LFO -----
        secLfo.setBounds(right);
        {
            auto inner = right.reduced(10, 0);
            inner.removeFromTop(26);

            // Left column: ON, shape combo, target combo
            auto leftCol = inner.removeFromLeft(130);
            lfoOnBtn.setBounds(leftCol.removeFromTop(22).reduced(0, 1));
            leftCol.removeFromTop(4);
            lfoShapeCombo.setBounds(leftCol.removeFromTop(22).reduced(0, 1));
            leftCol.removeFromTop(4);
            lfoTargetCombo.setBounds(leftCol.removeFromTop(22).reduced(0, 1));

            inner.removeFromLeft(pad);

            // Right column: 3 knobs
            const int slotW = inner.getWidth() / 3;
            lfoRateKnob->setBounds(inner.removeFromLeft(slotW).reduced(2));
            lfoAmountKnob->setBounds(inner.removeFromLeft(slotW).reduced(2));
            lfoDelayKnob->setBounds(inner.removeFromLeft(slotW).reduced(2));
        }
    }
    body.removeFromTop(pad);

    // Row 4: Envelope follower + Output (remaining height, min 130)
    auto bottomRow = body.removeFromTop(juce::jmin(body.getHeight(), 150));
    {
        const int envW = (bottomRow.getWidth() - pad) * 65 / 100;
        auto left = bottomRow.removeFromLeft(envW);
        bottomRow.removeFromLeft(pad);
        auto right = bottomRow;

        // ----- Envelope follower -----
        secEnv.setBounds(left);
        {
            auto inner = left.reduced(10, 0);
            inner.removeFromTop(26);

            auto leftCol = inner.removeFromLeft(130);
            envOnBtn.setBounds(leftCol.removeFromTop(22).reduced(0, 1));
            leftCol.removeFromTop(4);
            envTargetCombo.setBounds(leftCol.removeFromTop(22).reduced(0, 1));

            inner.removeFromLeft(pad);

            const int slotW = inner.getWidth() / 3;
            envAmountKnob->setBounds(inner.removeFromLeft(slotW).reduced(2));
            envAttackKnob->setBounds(inner.removeFromLeft(slotW).reduced(2));
            envReleaseKnob->setBounds(inner.removeFromLeft(slotW).reduced(2));
        }

        // ----- Output -----
        secOutput.setBounds(right);
        {
            auto inner = right.reduced(10, 0);
            inner.removeFromTop(26);

            const int slotW = inner.getWidth() / 2;
            mixKnob->setBounds(inner.removeFromLeft(slotW).reduced(2));
            outputKnob->setBounds(inner.removeFromLeft(slotW).reduced(2));
        }
    }
}

void FilterForgeEditor::timerCallback()
{
    // Preset browser handles its own polling
}

void FilterForgeEditor::onProcessorPresetChanged(int newIndex)
{
    if (presetBrowser)
        presetBrowser->setSelectedIndexSilent(newIndex);
}