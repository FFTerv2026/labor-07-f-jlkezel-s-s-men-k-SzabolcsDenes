#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressDialog>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QTableWidget;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void browseDirectory();
    void startSearch();

private:
    void setupUi();
    qint64 countFiles(const QString &dirPath) const;

    QLineEdit *dirPathEdit;
    QPushButton *browseButton;
    QLineEdit *searchTextEdit;
    QPushButton *searchButton;
    QTableWidget *resultTable;
    QProgressDialog *progressDialog;
};

#endif // MAINWINDOW_H
