#include "mainwindow.h"
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QProgressDialog>
#include <QFileDialog>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    // Fő widget és layout
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // --- Könyvtárválasztó sor ---
    QHBoxLayout *dirLayout = new QHBoxLayout();
    dirLayout->addWidget(new QLabel("Könyvtár:"));
    dirPathEdit = new QLineEdit(QDir::currentPath());
    dirPathEdit->setReadOnly(true);
    dirLayout->addWidget(dirPathEdit);
    browseButton = new QPushButton("Tallózás");
    dirLayout->addWidget(browseButton);
    mainLayout->addLayout(dirLayout);

    // --- Keresőmező sor ---
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->addWidget(new QLabel("Keresés:"));
    searchTextEdit = new QLineEdit();
    searchTextEdit->setPlaceholderText("írd be a keresett szöveget (üresen minden fájl)");
    searchLayout->addWidget(searchTextEdit);
    searchButton = new QPushButton("Keresés");
    searchLayout->addWidget(searchButton);
    mainLayout->addLayout(searchLayout);

    // --- Eredménytáblázat ---
    resultTable = new QTableWidget(0, 2);
    resultTable->setHorizontalHeaderLabels(QStringList() << "Fájlnév" << "Méret (byte)");
    resultTable->horizontalHeader()->setStretchLastSection(true);
    resultTable->setAlternatingRowColors(true);
    mainLayout->addWidget(resultTable);

    // --- Progress dialógus (rejtett, csak kereséskor jelenik meg) ---
    progressDialog = new QProgressDialog(this);
    progressDialog->setWindowTitle("Keresés folyamatban");
    progressDialog->setLabelText("Fájlok feldolgozása...");
    progressDialog->setCancelButtonText("Mégse");
    progressDialog->setMinimum(0);
    progressDialog->setMaximum(0);
    progressDialog->reset(); // rejtett állapot

    // Összekötések
    connect(browseButton, &QPushButton::clicked, this, &MainWindow::browseDirectory);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::startSearch);
}

void MainWindow::browseDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    "Válasszon könyvtárat",
                                                    dirPathEdit->text());
    if (!dir.isEmpty()) {
        dirPathEdit->setText(dir);
    }
}

qint64 MainWindow::countFiles(const QString &dirPath) const
{
    qint64 count = 0;
    QDirIterator it(dirPath, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        ++count;
    }
    return count;
}

void MainWindow::startSearch()
{
    QString directory = dirPathEdit->text();
    QString searchText = searchTextEdit->text();

    // Ellenőrizzük, hogy a könyvtár létezik-e
    if (!QDir(directory).exists()) {
        // Hibajelzés egyszerűen: a progress dialogon keresztül
        progressDialog->setLabelText("A megadott könyvtár nem létezik!");
        progressDialog->setRange(0, 1);
        progressDialog->setValue(1);
        progressDialog->exec(); // rövid ideig mutatjuk
        return;
    }

    // 1. Fájlok megszámlálása a progress dialog maximumának beállításához
    qint64 totalFiles = countFiles(directory);
    if (totalFiles == 0) {
        progressDialog->setLabelText("Nincs fájl a könyvtárban.");
        progressDialog->setRange(0, 1);
        progressDialog->setValue(1);
        progressDialog->exec();
        return;
    }

    // 2. Progress dialog előkészítése
    progressDialog->setRange(0, static_cast<int>(totalFiles));
    progressDialog->setValue(0);
    progressDialog->setLabelText(QString("Fájlok feldolgozása... (%1 találat)").arg(0));
    progressDialog->show();

    // 3. Táblázat törlése
    resultTable->setRowCount(0);

    // 4. Iteráció a fájlokon
    qint64 processed = 0;
    QDirIterator it(directory, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (it.hasNext() && !progressDialog->wasCanceled()) {
        it.next();
        QString filePath = it.filePath();
        QFileInfo fileInfo = it.fileInfo();

        // Ha nincs keresőszöveg, minden fájl számít
        bool match = searchText.isEmpty();
        if (!match) {
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream stream(&file);
                QString content = stream.readAll();
                if (content.contains(searchText, Qt::CaseSensitive)) {
                    match = true;
                }
                file.close();
            } // ha nem lehet megnyitni, akkor nem találat
        }

        if (match) {
            int row = resultTable->rowCount();
            resultTable->insertRow(row);
            resultTable->setItem(row, 0, new QTableWidgetItem(fileInfo.fileName()));
            resultTable->setItem(row, 1, new QTableWidgetItem(QString::number(fileInfo.size())));
        }

        // Progress frissítése
        ++processed;
        progressDialog->setValue(static_cast<int>(processed));
        progressDialog->setLabelText(QString("Fájlok feldolgozása... (%1 találat)")
                                         .arg(resultTable->rowCount()));
    }

    progressDialog->reset(); // elrejti a dialógust
}
