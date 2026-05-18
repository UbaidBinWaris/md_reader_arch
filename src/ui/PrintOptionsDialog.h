#ifndef PRINTOPTIONSDIALOG_H
#define PRINTOPTIONSDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QRadioButton>
#include <QPushButton>
#include <QLabel>
#include <QButtonGroup>

struct PrintOptions {
    QString paperSize;    // "A4", "Letter", "A3", "A5", "Legal"
    QString orientation;  // "Portrait", "Landscape"
    QString margins;      // "Normal", "Narrow", "Wide"
    QString colorMode;    // "Color", "Grayscale", "B&W"
};

enum PrintTarget {
    TargetPDF,
    TargetHTML,
    TargetPrint,
    TargetCancel
};

class PrintOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrintOptionsDialog(QWidget *parent = nullptr);
    
    PrintOptions options() const;
    PrintTarget selectedTarget() const { return m_target; }

private slots:
    void onExportPDFTriggered();
    void onExportHTMLTriggered();
    void onDirectPrintTriggered();

private:
    void setupUi();
    void applyPremiumStyles();

    QComboBox *m_paperSizeCombo;
    QComboBox *m_orientationCombo;
    QComboBox *m_marginsCombo;
    QComboBox *m_colorModeCombo;

    QPushButton *m_pdfBtn;
    QPushButton *m_htmlBtn;
    QPushButton *m_printBtn;
    QPushButton *m_cancelBtn;

    PrintTarget m_target;
};

#endif // PRINTOPTIONSDIALOG_H
