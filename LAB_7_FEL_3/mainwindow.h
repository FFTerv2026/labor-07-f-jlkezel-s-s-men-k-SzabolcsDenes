#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void showRandomQuote();

private slots:
    void onNewQuoteClicked();

private:
    void setupUi();
    void loadQuotesFromFile();
    void applyStyles();

    QStringList m_quotes;
    int m_lastIndex;
    QLabel *m_quoteLabel;
    QPushButton *m_newQuoteButton;
};

#endif // MAINWINDOW_H
