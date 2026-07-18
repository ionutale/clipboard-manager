#ifndef POPUPWIDGET_H
#define POPUPWIDGET_H

#include <QWidget>

class QSystemTrayIcon;
class QTabWidget;
class HistoryTab;
class NotesTab;
class ClipboardMonitor;
class Database;

class PopupWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PopupWidget(QWidget *parent = nullptr);
    ~PopupWidget();

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void changeEvent(QEvent *event) override;

private slots:
    void toggleVisibility();

private:
    void positionPopup();
    void hidePopup();

    QSystemTrayIcon *m_trayIcon;
    QTabWidget *m_tabWidget;
    HistoryTab *m_historyTab;
    NotesTab *m_notesTab;
    ClipboardMonitor *m_monitor;
    Database *m_db;
    bool m_visible = false;
};

#endif
