/**
 * NanoMark — CommandPalette.cpp
 * Implementation of the Command Palette overlay
 */

#include "CommandPalette.h"

#include <QVBoxLayout>
#include <QKeyEvent>
#include <QPainter>
#include <QGraphicsDropShadowEffect>

namespace NanoMark {

CommandPalette::CommandPalette(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    // Hide initially
    hide();

    setupUI();

    // Install event filter on parent to reposition on resize
    if (parent) {
        parent->installEventFilter(this);
    }
}

void CommandPalette::setupUI()
{
    // Fixed size for the palette
    setFixedSize(600, 450);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    auto *container = new QWidget(this);
    container->setObjectName("CommandContainer");
    container->setStyleSheet(
        "#CommandContainer {"
        "  background-color: #1e1e1e;"
        "  border: 1px solid #333333;"
        "  border-radius: 8px;"
        "}"
    );

    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 160));
    shadow->setOffset(0, 8);
    container->setGraphicsEffect(shadow);

    auto *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);

    m_searchBox = new QLineEdit(container);
    m_searchBox->setPlaceholderText("Search commands...");
    m_searchBox->setStyleSheet(
        "QLineEdit {"
        "  background: transparent;"
        "  border: none;"
        "  padding: 16px;"
        "  font-size: 16px;"
        "  color: #ffffff;"
        "  border-bottom: 1px solid #333333;"
        "}"
    );
    connect(m_searchBox, &QLineEdit::textChanged, this, &CommandPalette::onSearchTextChanged);

    m_listWidget = new QListWidget(container);
    m_listWidget->setStyleSheet(
        "QListWidget {"
        "  background: transparent;"
        "  border: none;"
        "  outline: none;"
        "  padding: 8px;"
        "}"
        "QListWidget::item {"
        "  padding: 8px 12px;"
        "  border-radius: 4px;"
        "  color: #cccccc;"
        "}"
        "QListWidget::item:hover, QListWidget::item:selected {"
        "  background: #2a2d2e;"
        "  color: #ffffff;"
        "}"
    );
    m_listWidget->setFocusPolicy(Qt::NoFocus); // Keep focus on search box
    connect(m_listWidget, &QListWidget::itemClicked, this, &CommandPalette::onItemSelected);

    containerLayout->addWidget(m_searchBox);
    containerLayout->addWidget(m_listWidget);
    mainLayout->addWidget(container);
}

void CommandPalette::showOverlay(Mode mode, const QList<ActionItem> &items)
{
    m_mode = mode;
    m_allItems = items;

    if (mode == CommandMode) {
        m_searchBox->setPlaceholderText("Search commands (e.g. 'theme', 'export')...");
    } else {
        m_searchBox->setPlaceholderText("Search files by name...");
    }

    m_searchBox->clear();
    onSearchTextChanged(""); // populate full list initially
    
    centerOnParent();
    raise();
    show();
    m_searchBox->setFocus();
}

void CommandPalette::onSearchTextChanged(const QString &text)
{
    m_listWidget->clear();
    QString query = text.toLower();

    for (const auto &item : m_allItems) {
        // Simple fuzzy match check
        if (query.isEmpty() || item.text.toLower().contains(query) || item.subText.toLower().contains(query)) {
            auto *listItem = new QListWidgetItem();
            listItem->setText(item.text);
            listItem->setData(Qt::UserRole, item.data);
            m_listWidget->addItem(listItem);
        }
    }

    if (m_listWidget->count() > 0) {
        m_listWidget->setCurrentRow(0);
    }
}

void CommandPalette::onItemSelected(QListWidgetItem *item)
{
    if (!item) return;
    QVariant data = item->data(Qt::UserRole);
    hide();
    emit itemSelected(data);
}

void CommandPalette::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        hide();
        return;
    }

    if (event->key() == Qt::Key_Down) {
        int row = m_listWidget->currentRow();
        if (row < m_listWidget->count() - 1) {
            m_listWidget->setCurrentRow(row + 1);
        }
        return;
    }

    if (event->key() == Qt::Key_Up) {
        int row = m_listWidget->currentRow();
        if (row > 0) {
            m_listWidget->setCurrentRow(row - 1);
        }
        return;
    }

    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        onItemSelected(m_listWidget->currentItem());
        return;
    }

    QWidget::keyPressEvent(event);
}

bool CommandPalette::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == parent() && event->type() == QEvent::Resize) {
        centerOnParent();
    }
    return QWidget::eventFilter(obj, event);
}

void CommandPalette::paintEvent(QPaintEvent *event)
{
    // Draw semi-transparent dark overlay over the parent
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 0, 100)); // Soft background dim
    QWidget::paintEvent(event);
}

void CommandPalette::centerOnParent()
{
    if (!parentWidget()) return;
    resize(parentWidget()->size());
    
    // The container (the actual palette) is centered by the QVBoxLayout automatically
}

} // namespace NanoMark
