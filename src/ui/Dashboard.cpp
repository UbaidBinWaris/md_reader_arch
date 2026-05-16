/**
 * NanoMark — Dashboard.cpp
 * Welcome screen implementation
 */

#include "Dashboard.h"
#include "SettingsService.h"
#include "ThemeManager.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QIcon>
#include <QPixmap>

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
    mainLayout->setContentsMargins(48, 64, 48, 64);
    mainLayout->setSpacing(32);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    // ── Header Section ──
    auto *headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(8);
    headerLayout->setAlignment(Qt::AlignHCenter);

    auto *logoLayout = new QHBoxLayout();
    logoLayout->setAlignment(Qt::AlignHCenter);

    auto *logoLabel = new QLabel(this);
    QPixmap logoPixmap(":/logo/logo.png");
    // Scale down smoothly if needed, e.g., 64x64
    if (!logoPixmap.isNull()) {
        logoLabel->setPixmap(logoPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    logoLayout->addWidget(logoLabel);
    headerLayout->addLayout(logoLayout);

    auto *header = new QLabel("NanoMark", this);
    header->setStyleSheet("font-size: 32px; font-weight: bold; color: #10a37f;");
    header->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(header);

    auto *subHeader = new QLabel("Write. Study. Organize.", this);
    subHeader->setStyleSheet("font-size: 16px; color: #888888;");
    subHeader->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(subHeader);

    mainLayout->addLayout(headerLayout);
    mainLayout->addSpacing(32);

    // ── Content Area ──
    auto *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(48);
    contentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    // Left Column: Quick Start & Tips
    auto *leftCol = new QVBoxLayout();
    leftCol->setSpacing(24);
    leftCol->setAlignment(Qt::AlignTop);

    auto *actionsLayout = new QVBoxLayout();
    actionsLayout->setSpacing(12);

    auto createActionButton = [&](const QString &text, const QString &subText, auto slot) {
        auto *btn = new QPushButton(this);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton {"
            "  background: #1a1a1a;"
            "  border: 1px solid #2e2e2e;"
            "  text-align: left;"
            "  padding: 16px 20px;"
            "  border-radius: 8px;"
            "}"
            "QPushButton:hover {"
            "  background: #252525;"
            "  border-color: #10a37f;"
            "}"
        );
        
        auto *vbox = new QVBoxLayout(btn);
        vbox->setContentsMargins(0, 0, 0, 0);
        vbox->setSpacing(4);
        
        auto *title = new QLabel(text, btn);
        title->setStyleSheet("font-weight: bold; font-size: 14px; background: transparent; color: #e0e0e0;");
        vbox->addWidget(title);
        
        auto *desc = new QLabel(subText, btn);
        desc->setStyleSheet("color: #888888; font-size: 12px; background: transparent;");
        vbox->addWidget(desc);
        
        connect(btn, &QPushButton::clicked, this, slot);
        actionsLayout->addWidget(btn);
    };

    createActionButton("New File", "Create a blank markdown document", [this]() { emit newFileRequested(); });
    createActionButton("Open File...", "Browse your computer for a file", [this]() { emit openFileRequested(""); });
    createActionButton("Open Folder...", "Open a workspace directory", [this]() { emit openFolderRequested(); });

    leftCol->addLayout(actionsLayout);

    // Tips section
    leftCol->addSpacing(16);
    auto *tipsHeader = new QLabel("<b>Tips / Shortcuts</b>", this);
    tipsHeader->setStyleSheet("color: #888888; font-size: 12px; text-transform: uppercase; letter-spacing: 1px;");
    leftCol->addWidget(tipsHeader);

    auto *tipsLayout = new QVBoxLayout();
    tipsLayout->setSpacing(8);
    auto addTip = [&](const QString &shortcut, const QString &desc) {
        auto *lbl = new QLabel(QString("<span style='color:#10a37f; font-family:monospace;'>%1</span> &rarr; %2").arg(shortcut, desc), this);
        lbl->setStyleSheet("color: #aaaaaa; font-size: 13px;");
        tipsLayout->addWidget(lbl);
    };
    addTip("Ctrl+P", "Quick Open");
    addTip("Ctrl+Shift+P", "Command Palette");
    addTip("Ctrl+B", "Toggle Sidebar");
    addTip("Ctrl+E", "Study Mode");
    leftCol->addLayout(tipsLayout);

    // Right Column: Recent & Pinned
    auto *rightCol = new QVBoxLayout();
    rightCol->setSpacing(24);
    rightCol->setAlignment(Qt::AlignTop);

    auto *pinnedHeader = new QLabel("<b>Pinned</b>", this);
    pinnedHeader->setStyleSheet("color: #888888; font-size: 12px; text-transform: uppercase; letter-spacing: 1px;");
    rightCol->addWidget(pinnedHeader);

    auto *pinnedList = new QListWidget(this);
    pinnedList->setStyleSheet(
        "QListWidget { background: transparent; border: none; font-size: 14px; outline: none; }"
        "QListWidget::item { padding: 8px 12px; border-radius: 6px; color: #cccccc; }"
        "QListWidget::item:hover { background: #1a1a1a; color: #ffffff; }"
    );
    pinnedList->setFixedHeight(40); // Just a placeholder
    pinnedList->addItem("📌 Thesis.md");
    rightCol->addWidget(pinnedList);

    auto *recentHeader = new QLabel("<b>Recent Files</b>", this);
    recentHeader->setStyleSheet("color: #888888; font-size: 12px; text-transform: uppercase; letter-spacing: 1px;");
    rightCol->addWidget(recentHeader);

    m_recentFilesList = new QListWidget(this);
    m_recentFilesList->setStyleSheet(
        "QListWidget { background: transparent; border: none; font-size: 14px; outline: none; }"
        "QListWidget::item { padding: 8px 12px; border-radius: 6px; color: #cccccc; }"
        "QListWidget::item:hover { background: #1a1a1a; color: #ffffff; }"
    );
    connect(m_recentFilesList, &QListWidget::itemClicked, this, &Dashboard::onRecentFileClicked);
    rightCol->addWidget(m_recentFilesList);

    contentLayout->addLayout(leftCol, 1);
    contentLayout->addLayout(rightCol, 1);

    // Container for centering
    auto *centerWidget = new QWidget(this);
    centerWidget->setLayout(contentLayout);
    centerWidget->setMaximumWidth(800);
    
    auto *centerLayout = new QHBoxLayout();
    centerLayout->addWidget(centerWidget);
    mainLayout->addLayout(centerLayout);

    mainLayout->addStretch();
}

void Dashboard::updateRecentFiles()
{
    m_recentFilesList->clear();
    auto recent = SettingsService::instance().recentFiles();
    
    for (const auto &path : recent) {
        QFileInfo fi(path);
        auto *item = new QListWidgetItem("📄 " + fi.fileName());
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
