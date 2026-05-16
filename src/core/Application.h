#pragma once

/**
 * NanoMark — Application.h
 * Main application controller
 */

#include <QObject>
#include <QStringList>
#include <memory>

namespace NanoMark {

class Application : public QObject
{
    Q_OBJECT

public:
    explicit Application(QObject *parent = nullptr);
    ~Application() override;

    /**
     * Initialize the application.
     * @param args Command-line arguments
     */
    void initialize(const QStringList &args);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace NanoMark
