#pragma once

#include "models.h"

#include <QAudioFormat>
#include <QAudioSink>
#include <QIODevice>
#include <QMutex>
#include <QRandomGenerator>
#include <QVector>

class SignalGeneratorDevice : public QIODevice {
    Q_OBJECT
public:
    explicit SignalGeneratorDevice(QObject *parent = nullptr);

    void setProjectData(const ProjectData &data);
    void setAudioFormat(const QAudioFormat &format);
    void resetGenerator();
    QVector<float> takeRecentSamples(int maxSamples);

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

private:
    float sampleAtTime(double seconds);
    double scenarioSpeed(double seconds) const;

    ProjectData m_project;
    QAudioFormat m_format;
    double m_timeSec = 0.0;
    QVector<double> m_phase;
    QVector<float> m_recentSamples;
    mutable QMutex m_recentMutex;
};

class SignalEngine : public QObject {
    Q_OBJECT
public:
    explicit SignalEngine(QObject *parent = nullptr);
    ~SignalEngine() override;

    void setProjectData(const ProjectData &data);
    bool start(QString *error);
    void stop();
    bool isRunning() const;
    QVector<float> takeRecentSamples(int maxSamples) const;
    int sampleRate() const;

private:
    QAudioFormat createFormat() const;

    SignalGeneratorDevice *m_device = nullptr;
    QAudioSink *m_sink = nullptr;
    ProjectData m_project;
};
