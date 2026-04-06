#include "signalplotwidgets.h"

#include <QPainter>
#include <QResizeEvent>
#include <QtMath>

#include <algorithm>
#include <complex>
#include <cstring>

namespace {
constexpr double kPi = 3.14159265358979323846;

QVector<float> downsampleForView(const QVector<float> &samples, int maxCount)
{
    if (samples.size() <= maxCount || maxCount <= 0) {
        return samples;
    }

    QVector<float> result;
    result.reserve(maxCount);
    const double step = static_cast<double>(samples.size()) / maxCount;
    for (int i = 0; i < maxCount; ++i) {
        const int idx = qBound(0, static_cast<int>(i * step), samples.size() - 1);
        result.push_back(samples.at(idx));
    }
    return result;
}

QVector<float> magnitudeSpectrum(const QVector<float> &samples, int binCount)
{
    if (samples.isEmpty() || binCount <= 0) {
        return {};
    }

    const QVector<float> windowed = downsampleForView(samples, 1024);
    const int n = windowed.size();
    const int usefulBins = std::min(binCount, n / 2);
    QVector<float> bins(usefulBins, 0.0f);

    for (int k = 0; k < usefulBins; ++k) {
        std::complex<double> acc(0.0, 0.0);
        for (int t = 0; t < n; ++t) {
            const double angle = -2.0 * kPi * k * t / n;
            const std::complex<double> w(std::cos(angle), std::sin(angle));
            acc += static_cast<double>(windowed.at(t)) * w;
        }
        bins[k] = static_cast<float>(std::abs(acc) / n);
    }

    return bins;
}
}

OscillogramWidget::OscillogramWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(220);
}

void OscillogramWidget::setSamples(const QVector<float> &samples)
{
    m_samples = downsampleForView(samples, width());
    update();
}

void OscillogramWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(14, 18, 24));
    p.setRenderHint(QPainter::Antialiasing, true);

    const int midY = height() / 2;
    p.setPen(QPen(QColor(60, 70, 90), 1));
    p.drawLine(0, midY, width(), midY);

    if (m_samples.isEmpty()) {
        p.setPen(QColor(170, 180, 200));
        p.drawText(rect(), Qt::AlignCenter, QStringLiteral("Нет данных"));
        return;
    }

    QPainterPath path;
    path.moveTo(0, midY);
    for (int i = 0; i < m_samples.size(); ++i) {
        const qreal x = (m_samples.size() == 1)
            ? 0
            : static_cast<qreal>(i) / (m_samples.size() - 1) * (width() - 1);
        const qreal y = midY - m_samples.at(i) * (height() * 0.45);
        path.lineTo(x, y);
    }

    p.setPen(QPen(QColor(60, 200, 160), 1.5));
    p.drawPath(path);
}

SpectrumWidget::SpectrumWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(220);
}

void SpectrumWidget::setSamples(const QVector<float> &samples, int sampleRate)
{
    m_sampleRate = sampleRate;
    m_bins = magnitudeSpectrum(samples, 256);
    update();
}

void SpectrumWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(14, 18, 24));

    if (m_bins.isEmpty()) {
        p.setPen(QColor(170, 180, 200));
        p.drawText(rect(), Qt::AlignCenter, QStringLiteral("Нет данных"));
        return;
    }

    const float maxValue = std::max(0.0001f, *std::max_element(m_bins.cbegin(), m_bins.cend()));
    const qreal barW = static_cast<qreal>(width()) / m_bins.size();

    p.setPen(Qt::NoPen);
    p.setBrush(QColor(120, 150, 255));
    for (int i = 0; i < m_bins.size(); ++i) {
        const qreal norm = m_bins.at(i) / maxValue;
        const qreal h = norm * (height() - 20);
        p.drawRect(QRectF(i * barW, height() - h, qMax(1.0, barW - 1.0), h));
    }

    p.setPen(QColor(170, 180, 200));
    p.drawText(8, 16, QStringLiteral("0 Гц"));
    p.drawText(width() - 80, 16, QStringLiteral("%1 Гц").arg(m_sampleRate / 2));
}

WaterfallWidget::WaterfallWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(220);
}

void WaterfallWidget::setSamples(const QVector<float> &samples, int sampleRate)
{
    m_sampleRate = sampleRate;
    ensureImage();

    if (m_image.isNull()) {
        return;
    }

    const QVector<float> bins = magnitudeSpectrum(samples, m_image.width());
    if (bins.isEmpty()) {
        return;
    }

    const float maxValue = std::max(0.0001f, *std::max_element(bins.cbegin(), bins.cend()));

    for (int y = m_image.height() - 1; y > 0; --y) {
        memcpy(m_image.scanLine(y), m_image.scanLine(y - 1), static_cast<size_t>(m_image.bytesPerLine()));
    }

    for (int x = 0; x < m_image.width(); ++x) {
        const int idx = qBound(0, x, bins.size() - 1);
        const float norm = bins.at(idx) / maxValue;
        m_image.setPixel(x, 0, colorFromValue(norm));
    }

    update();
}

void WaterfallWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(14, 18, 24));
    ensureImage();

    if (!m_image.isNull()) {
        p.drawImage(rect(), m_image);
    }

    p.setPen(QColor(170, 180, 200));
    p.drawText(8, 16, QStringLiteral("0 Гц"));
    p.drawText(width() - 80, 16, QStringLiteral("%1 Гц").arg(m_sampleRate / 2));
}

void WaterfallWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    ensureImage();
}

void WaterfallWidget::ensureImage()
{
    const QSize s = size().boundedTo(QSize(1024, 768));
    if (s.isEmpty()) {
        return;
    }

    if (m_image.size() == s) {
        return;
    }

    QImage next(s, QImage::Format_RGB32);
    next.fill(QColor(10, 12, 18));
    if (!m_image.isNull()) {
        QPainter p(&next);
        p.drawImage(next.rect(), m_image);
    }
    m_image = next;
}

QRgb WaterfallWidget::colorFromValue(float value)
{
    const float v = qBound(0.0f, value, 1.0f);
    const int r = static_cast<int>(qBound(0.0f, 255.0f * v * 1.4f, 255.0f));
    const int g = static_cast<int>(qBound(0.0f, 255.0f * (1.0f - std::abs(v - 0.5f) * 2.0f), 255.0f));
    const int b = static_cast<int>(qBound(0.0f, 255.0f * (1.0f - v), 255.0f));
    return qRgb(r, g, b);
}
