#include "PrintOptionsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>

PrintOptionsDialog::PrintOptionsDialog(QWidget *parent)
    : QDialog(parent)
    , m_target(TargetCancel)
{
    setupUi();
    applyPremiumStyles();
}

void PrintOptionsDialog::setupUi()
{
    setWindowTitle(tr("NanoMark Print & PDF Options"));
    resize(480, 360);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Header Title
    QLabel *headerLabel = new QLabel(tr("Configure PDF & Print Settings"), this);
    QFont headerFont = headerLabel->font();
    headerFont.setPointSize(14);
    headerFont.setBold(true);
    headerLabel->setFont(headerFont);
    mainLayout->addWidget(headerLabel);

    // Group Box for page settings
    QGroupBox *pageGroup = new QGroupBox(tr("Page Layout"), this);
    QGridLayout *gridLayout = new QGridLayout(pageGroup);
    gridLayout->setSpacing(12);

    // 1. Paper Size (Default A4)
    gridLayout->addWidget(new QLabel(tr("Paper Size:"), this), 0, 0);
    m_paperSizeCombo = new QComboBox(this);
    m_paperSizeCombo->addItems(QStringList() << "A4" << "Letter" << "A3" << "A5" << "Legal");
    m_paperSizeCombo->setCurrentText("A4");
    gridLayout->addWidget(m_paperSizeCombo, 0, 1);

    // 2. Orientation
    gridLayout->addWidget(new QLabel(tr("Orientation:"), this), 1, 0);
    m_orientationCombo = new QComboBox(this);
    m_orientationCombo->addItems(QStringList() << tr("Portrait") << tr("Landscape"));
    m_orientationCombo->setCurrentIndex(0);
    gridLayout->addWidget(m_orientationCombo, 1, 1);

    // 3. Margins
    gridLayout->addWidget(new QLabel(tr("Margins:"), this), 2, 0);
    m_marginsCombo = new QComboBox(this);
    m_marginsCombo->addItems(QStringList() << tr("Normal") << tr("Narrow") << tr("Wide"));
    m_marginsCombo->setCurrentIndex(0);
    gridLayout->addWidget(m_marginsCombo, 2, 1);

    // 4. Color Mode
    gridLayout->addWidget(new QLabel(tr("Color Mode:"), this), 3, 0);
    m_colorModeCombo = new QComboBox(this);
    m_colorModeCombo->addItems(QStringList() << tr("Full Color") << tr("Grayscale") << tr("Black & White"));
    m_colorModeCombo->setCurrentIndex(0);
    gridLayout->addWidget(m_colorModeCombo, 3, 1);

    mainLayout->addWidget(pageGroup);

    // Bottom Action Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);

    m_pdfBtn = new QPushButton(tr("Export PDF"), this);
    m_pdfBtn->setObjectName("pdfBtn");
    connect(m_pdfBtn, &QPushButton::clicked, this, &PrintOptionsDialog::onExportPDFTriggered);

    m_htmlBtn = new QPushButton(tr("Export HTML"), this);
    connect(m_htmlBtn, &QPushButton::clicked, this, &PrintOptionsDialog::onExportHTMLTriggered);

    m_printBtn = new QPushButton(tr("Direct Print"), this);
    m_printBtn->setObjectName("printBtn");
    connect(m_printBtn, &QPushButton::clicked, this, &PrintOptionsDialog::onDirectPrintTriggered);

    m_cancelBtn = new QPushButton(tr("Cancel"), this);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    btnLayout->addWidget(m_pdfBtn);
    btnLayout->addWidget(m_htmlBtn);
    btnLayout->addWidget(m_printBtn);
    btnLayout->addWidget(m_cancelBtn);

    mainLayout->addLayout(btnLayout);
}

void PrintOptionsDialog::applyPremiumStyles()
{
    // Premium, professional Dark Mode stylesheet to wow the user
    setStyleSheet(
        "QDialog {"
        "    background-color: #181818;"
        "    color: #e0e0e0;"
        "}"
        "QGroupBox {"
        "    border: 1px solid #2e2e2e;"
        "    border-radius: 6px;"
        "    margin-top: 15px;"
        "    padding-top: 15px;"
        "    color: #e0e0e0;"
        "    font-weight: bold;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top left;"
        "    left: 10px;"
        "    padding: 0 5px;"
        "}"
        "QLabel {"
        "    color: #a0a0a0;"
        "    font-weight: 500;"
        "}"
        "QComboBox {"
        "    background-color: #242424;"
        "    color: #ffffff;"
        "    border: 1px solid #3a3a3a;"
        "    border-radius: 4px;"
        "    padding: 6px 10px;"
        "    min-width: 150px;"
        "}"
        "QComboBox:hover {"
        "    border-color: #555555;"
        "}"
        "QComboBox QAbstractItemView {"
        "    background-color: #242424;"
        "    color: #ffffff;"
        "    selection-background-color: #388e3c;"
        "    selection-color: #ffffff;"
        "}"
        "QPushButton {"
        "    background-color: #2e2e2e;"
        "    color: #ffffff;"
        "    border: 1px solid #3a3a3a;"
        "    border-radius: 4px;"
        "    padding: 8px 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #3e3e3e;"
        "    border-color: #555555;"
        "}"
        "QPushButton#pdfBtn {"
        "    background-color: #1b5e20;"
        "    border-color: #1b5e20;"
        "}"
        "QPushButton#pdfBtn:hover {"
        "    background-color: #2e7d32;"
        "}"
        "QPushButton#printBtn {"
        "    background-color: #0d47a1;"
        "    border-color: #0d47a1;"
        "}"
        "QPushButton#printBtn:hover {"
        "    background-color: #1565c0;"
        "}"
    );
}

PrintOptions PrintOptionsDialog::options() const
{
    PrintOptions opts;
    opts.paperSize = m_paperSizeCombo->currentText();
    opts.orientation = m_orientationCombo->currentText();
    opts.margins = m_marginsCombo->currentText();
    opts.colorMode = m_colorModeCombo->currentText();
    return opts;
}

void PrintOptionsDialog::onExportPDFTriggered()
{
    m_target = TargetPDF;
    accept();
}

void PrintOptionsDialog::onExportHTMLTriggered()
{
    m_target = TargetHTML;
    accept();
}

void PrintOptionsDialog::onDirectPrintTriggered()
{
    m_target = TargetPrint;
    accept();
}
