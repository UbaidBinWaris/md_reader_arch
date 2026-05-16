/**
 * NanoMark — Dashboard.cpp
 * Welcome screen implementation
 */

#include "Dashboard.h"
#include "SettingsService.h"
#include "ThemeManager.h"

#include <QHBoxLayout>
#include <QFileInfo>
#include <QIcon>

namespace NanoMark {

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    updateRecentFiles();
}

void Dashboard::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 60, 40, 60);
    mainLayout->setSpacing(30);

    // Header
    auto *header = new QLabel("NanoMark", this);
    header->setStyleSheet("font-size: 42px; font-weight: bold; color: #10a37f;");
    header->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(header);

    auto *subHeader = new QLabel("Professional Markdown & Study Workspace", this);
    subHeader->setStyleSheet("font-size: 16px; color: #888;");
    subHeader->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(subHeader);

    // Content Area
    auto *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(50);
    mainLayout->addLayout(contentLayout);

    // Left Side: Recent Files
    auto *recentLayout = new QVBoxLayout();
    recentLayout->addWidget(new QLabel("<b>Recent Documents</b>"));
    m_recentFilesList = new QListWidget(this);
    m_recentFilesList->setStyleSheet(
        "QListWidget { background: transparent; border: none; font-size: 14px; }"
        "QListWidget::item { padding: 8px; border-radius: 4px; }"
        "QListWidget::item:hover { background: #1a1a1a; }"
    );
    connect(m_recentFilesList, &QListWidget::itemClicked, this, &Dashboard::onRecentFileClicked);
    recentLayout->addWidget(m_recentFilesList);
    contentLayout->addLayout(recentLayout, 2);

    // Right Side: Quick Actions
    auto *actionsLayout = new QVBoxLayout();
    actionsLayout->addWidget(new QLabel("<b>Quick Start</b>"));

    auto createActionButton = [&](const QString &text, const QString &subText, auto slot) {
        auto *btn = new QPushButton(this);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton { background: #1a1a1a; border: 1px solid #2e2e2e; text-align: left; padding: 15px; border-radius: 8px; }"
            "QPushButton:hover { background: #252525; border-color: #10a37f; }"
        );
        
        auto *vbox = new QVBoxLayout(btn);
        vbox->setContentsMargins(0, 0, 0, 0);
        vbox->setSpacing(2);
        
        auto *title = new QLabel(text, btn);
        title->setStyleSheet("font-weight: bold; font-size: 14px; background: transparent;");
        vbox->addWidget(title);
        
        auto *desc = new QLabel(subText, btn);
        desc->setStyleSheet("color: #777; font-size: 12px; background: transparent;");
        vbox->addWidget(desc);
        
        connect(btn, &QPushButton::clicked, this, slot);
        actionsLayout->addWidget(btn);
    };

    createActionButton("New File", "Create a blank markdown document", [this]() { emit newFileRequested(); });
    createActionButton("Open File...", "Browse your computer for a file", [this]() { emit openFileRequested(""); });
    createActionButton("Open Folder...", "Open a workspace directory", [this]() { emit openFolderRequested(); });

    actionsLayout->addStretch();
    contentLayout->addLayout(actionsLayout, 1);

    mainLayout->addStretch();
}

void Dashboard::updateRecentFiles()
{
    m_recentFilesList->clear();
    auto recent = SettingsService::instance().recentFiles();
    
    for (const auto &path : recent) {
        QFileInfo fi(path);
        auto *item = new QListWidgetItem(fi.fileName());
        item->setToolTip(path);
        item->setData(Qt::UserRole, path);
        m_recentFilesList->addItem(item);
    }
}

void Dashboard::onRecentFileClicked(QListWidgetItem *item)
{
    QString path = item->data(Qt::UserRole).toString();
    emit openFileRequested(path);
}

} // namespace NanoMark
