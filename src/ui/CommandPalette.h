#pragma once

/**
 * NanoMark — CommandPalette.h
 * Frameless overlay for Command Palette and Quick Open
 */

#include <QWidget>
#include <QStringList>
#include <QListWidget>
#include <QLineEdit>

namespace NanoMark {

class CommandPalette : public QWidget
{
    Q_OBJECT

public:
    enum Mode {
        CommandMode,
        QuickOpenMode
    };

    struct ActionItem {
        QString text;
        QString subText;
        QVariant data; // Can be a function pointer, string path, etc.
    };

    explicit CommandPalette(QWidget *parent = nullptr);

    /** Initialize the overlay with items and mode */
    void showOverlay(Mode mode, const QList<ActionItem> &items);

signals:
    /** Emitted when the user selects an item */
    void itemSelected(const QVariant &data);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onSearchTextChanged(const QString &text);
    void onItemSelected(QListWidgetItem *item);

private:
    void setupUI();
    void centerOnParent();

    Mode m_mode;
    QList<ActionItem> m_allItems;
    
    QLineEdit *m_searchBox;
    QListWidget *m_listWidget;
};

} // namespace NanoMark
