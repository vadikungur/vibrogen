#pragma once

#include <QImage>
#include <QWidget>
#include <QVector>

class OscillogramWidget : public QWidget {
    Q_OBJECT
public:
    explicit OscillogramWidget(QWidget *parent = nullptr);
    void setSamples(const QVector<float> &samples);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<float> m_samples;
};

class SpectrumWidget : public QWidget {
    Q_OBJECT
public:
    explicit SpectrumWidget(QWidget *parent = nullptr);
    void setSamples(const QVector<float> &samples, int sampleRate);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<float> m_bins;
    int m_sampleRate = 48000;
};

class WaterfallWidget : public QWidget {
    Q_OBJECT
public:
    explicit WaterfallWidget(QWidget *parent = nullptr);
    void setSamples(const QVector<float> &samples, int sampleRate);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void ensureImage();
    static QRgb colorFromValue(float value);

    QImage m_image;
    int m_sampleRate = 48000;
};
