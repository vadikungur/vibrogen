#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QVector>

enum class SignalType {
    Harmonic,
    WhiteNoise,
    Defect
};

inline QString signalTypeToString(SignalType type)
{
    switch (type) {
    case SignalType::Harmonic:
        return QStringLiteral("Гармонический");
    case SignalType::WhiteNoise:
        return QStringLiteral("Белый шум");
    case SignalType::Defect:
        return QStringLiteral("Типовой дефект");
    }
    return QStringLiteral("Неизвестно");
}

struct SignalEntry {
    SignalType type = SignalType::Harmonic;
    QString name;
    double amplitude = 1.0;
    double frequencyHz = 100.0;
    double bandwidthHz = 50.0;
    double spectralPower = 1.0;
    QString defectKind;

    QJsonObject toJson() const
    {
        return {
            {QStringLiteral("type"), static_cast<int>(type)},
            {QStringLiteral("name"), name},
            {QStringLiteral("amplitude"), amplitude},
            {QStringLiteral("frequencyHz"), frequencyHz},
            {QStringLiteral("bandwidthHz"), bandwidthHz},
            {QStringLiteral("spectralPower"), spectralPower},
            {QStringLiteral("defectKind"), defectKind},
        };
    }

    static SignalEntry fromJson(const QJsonObject &obj)
    {
        SignalEntry signal;
        signal.type = static_cast<SignalType>(obj.value(QStringLiteral("type")).toInt());
        signal.name = obj.value(QStringLiteral("name")).toString();
        signal.amplitude = obj.value(QStringLiteral("amplitude")).toDouble(1.0);
        signal.frequencyHz = obj.value(QStringLiteral("frequencyHz")).toDouble(100.0);
        signal.bandwidthHz = obj.value(QStringLiteral("bandwidthHz")).toDouble(50.0);
        signal.spectralPower = obj.value(QStringLiteral("spectralPower")).toDouble(1.0);
        signal.defectKind = obj.value(QStringLiteral("defectKind")).toString();
        return signal;
    }
};

enum class ScenarioType {
    ConstantSpeed,
    Accelerate,
    Brake,
    Cycle
};

inline QString scenarioTypeToString(ScenarioType type)
{
    switch (type) {
    case ScenarioType::ConstantSpeed:
        return QStringLiteral("Постоянная скорость");
    case ScenarioType::Accelerate:
        return QStringLiteral("Разгон");
    case ScenarioType::Brake:
        return QStringLiteral("Торможение");
    case ScenarioType::Cycle:
        return QStringLiteral("Цикл");
    }
    return QStringLiteral("Неизвестно");
}

struct Scenario {
    ScenarioType type = ScenarioType::ConstantSpeed;
    double startSpeedKmh = 0.0;
    double targetSpeedKmh = 80.0;
    double accelKmhPerSec = 2.0;
    double cycleStopSec = 10.0;
    double cycleCruiseSec = 30.0;

    QJsonObject toJson() const
    {
        return {
            {QStringLiteral("type"), static_cast<int>(type)},
            {QStringLiteral("startSpeedKmh"), startSpeedKmh},
            {QStringLiteral("targetSpeedKmh"), targetSpeedKmh},
            {QStringLiteral("accelKmhPerSec"), accelKmhPerSec},
            {QStringLiteral("cycleStopSec"), cycleStopSec},
            {QStringLiteral("cycleCruiseSec"), cycleCruiseSec},
        };
    }

    static Scenario fromJson(const QJsonObject &obj)
    {
        Scenario scenario;
        scenario.type = static_cast<ScenarioType>(obj.value(QStringLiteral("type")).toInt());
        scenario.startSpeedKmh = obj.value(QStringLiteral("startSpeedKmh")).toDouble(0.0);
        scenario.targetSpeedKmh = obj.value(QStringLiteral("targetSpeedKmh")).toDouble(80.0);
        scenario.accelKmhPerSec = obj.value(QStringLiteral("accelKmhPerSec")).toDouble(2.0);
        scenario.cycleStopSec = obj.value(QStringLiteral("cycleStopSec")).toDouble(10.0);
        scenario.cycleCruiseSec = obj.value(QStringLiteral("cycleCruiseSec")).toDouble(30.0);
        return scenario;
    }
};

struct TrainConfig {
    QString presetName = QStringLiteral("ЭС2Г Ласточка");
    double wheelDiameterMm = 957.0;
    QString axleboxType = QStringLiteral("Кассетный подшипниковый узел");
    QString gearboxType = QStringLiteral("Одноступенчатый тяговый редуктор");
    QString notes;

    QJsonObject toJson() const
    {
        return {
            {QStringLiteral("presetName"), presetName},
            {QStringLiteral("wheelDiameterMm"), wheelDiameterMm},
            {QStringLiteral("axleboxType"), axleboxType},
            {QStringLiteral("gearboxType"), gearboxType},
            {QStringLiteral("notes"), notes},
        };
    }

    static TrainConfig fromJson(const QJsonObject &obj)
    {
        TrainConfig cfg;
        cfg.presetName = obj.value(QStringLiteral("presetName")).toString(cfg.presetName);
        cfg.wheelDiameterMm = obj.value(QStringLiteral("wheelDiameterMm")).toDouble(cfg.wheelDiameterMm);
        cfg.axleboxType = obj.value(QStringLiteral("axleboxType")).toString(cfg.axleboxType);
        cfg.gearboxType = obj.value(QStringLiteral("gearboxType")).toString(cfg.gearboxType);
        cfg.notes = obj.value(QStringLiteral("notes")).toString();
        return cfg;
    }
};

struct ProjectData {
    QVector<SignalEntry> signalEntries;
    Scenario scenario;
    TrainConfig trainConfig;

    QJsonObject toJson() const
    {
        QJsonArray signalArray;
        for (const auto &signal : signalEntries) {
            signalArray.append(signal.toJson());
        }

        return {
            {QStringLiteral("signals"), signalArray},
            {QStringLiteral("scenario"), scenario.toJson()},
            {QStringLiteral("trainConfig"), trainConfig.toJson()},
        };
    }

    static ProjectData fromJson(const QJsonObject &obj)
    {
        ProjectData data;
        const auto signalArray = obj.value(QStringLiteral("signals")).toArray();
        data.signalEntries.reserve(signalArray.size());
        for (const auto &value : signalArray) {
            data.signalEntries.append(SignalEntry::fromJson(value.toObject()));
        }

        data.scenario = Scenario::fromJson(obj.value(QStringLiteral("scenario")).toObject());
        data.trainConfig = TrainConfig::fromJson(obj.value(QStringLiteral("trainConfig")).toObject());
        return data;
    }
};
