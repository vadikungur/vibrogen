#include "signalengine.h"

#include <QtMath>
#include <cstring>

namespace {
constexpr int kSampleRate = 48000;
constexpr int kChannels = 1;
constexpr int kBytesPerSample = 2;
constexpr double kPi2 = 6.283185307179586;
constexpr double kMaxTrainSpeedKmh = 150.0;
}

SignalGeneratorDevice::SignalGeneratorDevice(QObject *parent)
    : QIODevice(parent)
{
}

void SignalGeneratorDevice::setProjectData(const ProjectData &data)
{
    m_project = data;
    m_phase = QVector<double>(m_project.signalEntries.size(), 0.0);
}

void SignalGeneratorDevice::setAudioFormat(const QAudioFormat &format)
{
    m_format = format;
}

void SignalGeneratorDevice::resetGenerator()
{
    m_timeSec = 0.0;
    m_phase.fill(0.0);
}

qint64 SignalGeneratorDevice::readData(char *data, qint64 maxlen)
{
    if (m_format.sampleRate() <= 0 || m_project.signalEntries.isEmpty()) {
        memset(data, 0, maxlen);
        return maxlen;
    }

    const int sampleBytes = m_format.bytesPerSample();
    const int channels = m_format.channelCount();
    const int frames = static_cast<int>(maxlen / (sampleBytes * channels));
    auto *out = reinterpret_cast<qint16 *>(data);

    for (int frame = 0; frame < frames; ++frame) {
        const float sample = sampleAtTime(m_timeSec);
        const qint16 s16 = static_cast<qint16>(qBound(-1.0f, sample, 1.0f) * 32767);
        for (int ch = 0; ch < channels; ++ch) {
            *out++ = s16;
        }
        m_timeSec += 1.0 / m_format.sampleRate();
    }

    return frames * sampleBytes * channels;
}

qint64 SignalGeneratorDevice::writeData(const char *, qint64)
{
    return 0;
}

float SignalGeneratorDevice::sampleAtTime(double seconds)
{
    if (m_project.signalEntries.isEmpty()) {
        return 0.0f;
    }

    const double speedFactor = qMax(0.0, scenarioSpeed(seconds) / kMaxTrainSpeedKmh);
    double sum = 0.0;

    for (int i = 0; i < m_project.signalEntries.size(); ++i) {
        const auto &signal = m_project.signalEntries.at(i);
        const double amplitude = signal.amplitude;

        if (signal.type == SignalType::Harmonic) {
            const double value = qSin(kPi2 * signal.frequencyHz * seconds);
            sum += amplitude * value;
        } else if (signal.type == SignalType::WhiteNoise) {
            const double noise = (QRandomGenerator::global()->generateDouble() * 2.0 - 1.0);
            sum += noise * signal.spectralPower;
        } else {
            const double defectFrequency = signal.frequencyHz * (0.2 + speedFactor);
            const double mod = 0.5 + 0.5 * qSin(kPi2 * 2.0 * seconds);
            sum += amplitude * mod * qSin(kPi2 * defectFrequency * seconds);
        }
    }

    return static_cast<float>(sum / m_project.signalEntries.size());
}

double SignalGeneratorDevice::scenarioSpeed(double seconds) const
{
    const auto &scenario = m_project.scenario;
    switch (scenario.type) {
    case ScenarioType::ConstantSpeed:
        return scenario.targetSpeedKmh;
    case ScenarioType::Accelerate:
        return qMin(scenario.targetSpeedKmh, scenario.startSpeedKmh + scenario.accelKmhPerSec * seconds);
    case ScenarioType::Brake:
        return qMax(0.0, scenario.startSpeedKmh - scenario.accelKmhPerSec * seconds);
    case ScenarioType::Cycle: {
        const double accelSec = scenario.accelKmhPerSec > 0.0
            ? (scenario.targetSpeedKmh / scenario.accelKmhPerSec)
            : 1.0;
        const double brakeSec = accelSec;
        const double cycleSec = scenario.cycleStopSec + accelSec + scenario.cycleCruiseSec + brakeSec;
        const double t = std::fmod(seconds, cycleSec);

        if (t < scenario.cycleStopSec) {
            return 0.0;
        }

        const double afterStop = t - scenario.cycleStopSec;
        if (afterStop < accelSec) {
            return qMin(scenario.targetSpeedKmh, afterStop * scenario.accelKmhPerSec);
        }

        if (afterStop < accelSec + scenario.cycleCruiseSec) {
            return scenario.targetSpeedKmh;
        }

        const double brakeT = afterStop - accelSec - scenario.cycleCruiseSec;
        return qMax(0.0, scenario.targetSpeedKmh - brakeT * scenario.accelKmhPerSec);
    }
    }
    return scenario.targetSpeedKmh;
}

SignalEngine::SignalEngine(QObject *parent)
    : QObject(parent)
    , m_device(new SignalGeneratorDevice(this))
{
}

SignalEngine::~SignalEngine()
{
    stop();
}

void SignalEngine::setProjectData(const ProjectData &data)
{
    m_project = data;
    m_device->setProjectData(m_project);
}

bool SignalEngine::start(QString *error)
{
    if (m_sink) {
        return true;
    }

    const auto format = createFormat();
    m_device->setAudioFormat(format);
    m_device->resetGenerator();

    m_sink = new QAudioSink(format, this);
    m_device->open(QIODevice::ReadOnly);
    m_sink->start(m_device);

    if (m_sink->error() != QAudio::NoError) {
        if (error) {
            *error = QStringLiteral("Ошибка аудиовыхода: %1").arg(static_cast<int>(m_sink->error()));
        }
        stop();
        return false;
    }

    return true;
}

void SignalEngine::stop()
{
    if (!m_sink) {
        return;
    }

    m_sink->stop();
    m_device->close();
    m_sink->deleteLater();
    m_sink = nullptr;
}

bool SignalEngine::isRunning() const
{
    return m_sink != nullptr;
}

QAudioFormat SignalEngine::createFormat() const
{
    QAudioFormat format;
    format.setSampleRate(kSampleRate);
    format.setChannelCount(kChannels);
    format.setSampleFormat(QAudioFormat::Int16);
    return format;
}
