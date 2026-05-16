#pragma once

/**
 * NanoMark — Dashboard.h
 * Welcome screen shown when no documents are open
 */

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>

namespace NanoMark {

class Dashboard : public QWidget
{
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);

signals:
    void openFileRequested(const QString &filePath);
    void newFileRequested();
    void openFolderRequested();

private slots:
    void onRecentFileClicked(QListWidgetItem *item);

private:
    void setupUI();
    void updateRecentFiles();

    QListWidget *m_recentFilesList = nullptr;
};

} // namespace NanoMark
