#pragma once

#include <QWidget>
#include <QVector>
#include <QImage>

namespace AetherSDR {

// Panadapter / spectrum display widget.
//
// Layout (top to bottom):
//   ~40% — spectrum line plot (current FFT frame, smoothed)
//   ~60% — waterfall (scrolling heat-map history)
//   20px — absolute frequency scale bar
//
// Overlays (drawn on top of spectrum + waterfall):
//   - Filter passband: semi-transparent band from filterLow to filterHigh Hz
//   - Slice marker: vertical orange line at slice center frequency
//
// Click anywhere in the spectrum/waterfall area to emit frequencyClicked().
class SpectrumWidget : public QWidget {
    Q_OBJECT

public:
    explicit SpectrumWidget(QWidget* parent = nullptr);

    QSize sizeHint() const override { return {800, 300}; }

    // Set the frequency range covered by this panadapter.
    void setFrequencyRange(double centerMhz, double bandwidthMhz);

    // Feed a new FFT frame. bins are scaled dBm values.
    void updateSpectrum(const QVector<float>& binsDbm);

    // Update the dBm range used for the waterfall colour map and spectrum Y axis.
    void setDbmRange(float minDbm, float maxDbm);

    // Set the active slice center frequency (draws the orange marker).
    void setSliceFrequency(double freqMhz);

    // Set the active slice filter edges (Hz offsets from slice center).
    void setSliceFilter(int lowHz, int highHz);

signals:
    // Emitted when the user clicks in the panadapter area.
    void frequencyClicked(double mhz);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void drawGrid(QPainter& p, const QRect& r);
    void drawSpectrum(QPainter& p, const QRect& r);
    void drawSliceOverlay(QPainter& p, const QRect& specRect, const QRect& wfRect);
    void drawWaterfall(QPainter& p, const QRect& r);
    void drawFreqScale(QPainter& p, const QRect& r);

    void pushWaterfallRow(const QVector<float>& bins, int destWidth);
    QRgb dbmToRgb(float dbm) const;

    // Pixel x coordinate for a given frequency in MHz (0 = left edge).
    int mhzToX(double mhz) const;

    QVector<float> m_bins;       // raw FFT frame (dBm)
    QVector<float> m_smoothed;   // exponential-smoothed for visual stability

    double m_centerMhz{14.225};
    double m_bandwidthMhz{0.200};
    double m_sliceFreqMhz{14.225};
    int    m_filterLowHz{-1500};   // Hz below slice center
    int    m_filterHighHz{1500};   // Hz above slice center

    float m_refLevel{0.0f};         // top of display (dBm)
    float m_dynamicRange{120.0f};   // dB range shown in spectrum

    // Waterfall colour range (dBm)
    float m_wfMinDbm{-130.0f};
    float m_wfMaxDbm{-20.0f};

    // Scrolling waterfall image (Format_RGB32)
    QImage m_waterfall;

    static constexpr float SMOOTH_ALPHA    = 0.35f;
    // Fraction of the panadapter area (above freq scale) used for spectrum
    static constexpr float SPECTRUM_FRAC   = 0.40f;
    // Height of the frequency scale bar at the bottom
    static constexpr int   FREQ_SCALE_H    = 20;
};

} // namespace AetherSDR
