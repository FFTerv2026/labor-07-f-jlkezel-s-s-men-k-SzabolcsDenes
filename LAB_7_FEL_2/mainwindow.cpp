#include "mainwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Táblázat létrehozása és beállítása
    tableWidget = new QTableWidget(this);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // nem szerkeszthető
    tableWidget->setAlternatingRowColors(true);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    setCentralWidget(tableWidget);

    // Menü elkészítése
    setupMenu();

    setWindowTitle("CSV megjelenítő");
    resize(800, 600);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupMenu()
{
    // Menüsáv létrehozása
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // File menü
    QMenu *fileMenu = menuBar->addMenu("File");

    // Open File... akció
    QAction *openAction = new QAction("Open File...", this);
    openAction->setShortcut(QKeySequence::Open);
    fileMenu->addAction(openAction);

    // Kapcsolódás a slot-hoz
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
}

void MainWindow::openFile()
{
    // Fájlválasztó dialógus megjelenítése CSV szűrővel
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Válasszon CSV fájlt",
                                                    QString(),
                                                    "CSV files (*.csv);;All files (*.*)");
    if (fileName.isEmpty())
        return; // a felhasználó mégsem választott fájlt

    // CSV fájl betöltése
    loadCsv(fileName);
}

void MainWindow::loadCsv(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Hiba", "A fájl megnyitása sikertelen.");
        return;
    }

    QTextStream stream(&file);
    // Táblázat alaphelyzetbe állítása
    tableWidget->clear();
    tableWidget->setRowCount(0);

    // Első sor beolvasása (oszlopfejlécek)
    if (stream.atEnd()) {
        file.close();
        return; // üres fájl
    }
    QString headerLine = stream.readLine();
    QStringList headers = headerLine.split(','); // egyszerű szétválasztás vessző mentén

    // Oszlopok számának beállítása és fejlécek beírása
    tableWidget->setColumnCount(headers.size());
    tableWidget->setHorizontalHeaderLabels(headers);

    // Többi sor beolvasása
    int row = 0;
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        if (line.trimmed().isEmpty())
            continue; // üres sorok kihagyása

        QStringList fields = line.split(',');

        // Új sor beszúrása
        tableWidget->insertRow(row);

        // Cellák kitöltése
        for (int col = 0; col < fields.size() && col < tableWidget->columnCount(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(fields.at(col));
            // Az item alapértelmezés szerint nem szerkeszthető, de a táblázat beállítása miatt is az
            tableWidget->setItem(row, col, item);
        }

        // Ha a sorban kevesebb mező van, mint oszlop, a maradék cellák üresek maradnak
        // (azaz nem hozunk létre itemet, így automatikusan üresek)

        ++row;
    }

    file.close();

    // Oszlopok szélességének automatikus igazítása
    tableWidget->resizeColumnsToContents();
}
