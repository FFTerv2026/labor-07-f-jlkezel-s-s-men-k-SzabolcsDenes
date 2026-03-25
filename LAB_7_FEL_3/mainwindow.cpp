#include "mainwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QFont>
#include <QPalette>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_lastIndex(-1)
{
    setupUi();
    loadQuotesFromFile();
    applyStyles();
    showRandomQuote();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    // Központi widget és elrendezés
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setSpacing(20);
    layout->setContentsMargins(30, 30, 30, 30);

    // Idézet címkéje
    m_quoteLabel = new QLabel(this);
    m_quoteLabel->setWordWrap(true);
    m_quoteLabel->setAlignment(Qt::AlignCenter);
    m_quoteLabel->setMinimumHeight(200);
    layout->addWidget(m_quoteLabel);

    // Gomb
    m_newQuoteButton = new QPushButton("Új idézet", this);
    m_newQuoteButton->setCursor(Qt::PointingHandCursor);
    m_newQuoteButton->setFixedSize(150, 40);
    layout->addWidget(m_newQuoteButton, 0, Qt::AlignCenter);

    // Összekötés
    connect(m_newQuoteButton, &QPushButton::clicked, this, &MainWindow::onNewQuoteClicked);

    setWindowTitle("Motivációs idézetek");
    resize(500, 300);
}

void MainWindow::loadQuotesFromFile()
{
    QFile file("quotes.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Figyelmeztetés",
                             "A quotes.txt fájl nem található.\n"
                             "Az alkalmazás alapértelmezett idézetekkel működik tovább.");
        // Alapértelmezett idézetek (néhány a teljes listából)
        m_quotes = {
            "If you want to achieve greatness stop asking for permission. ~Anonymous",
            "Things work out best for those who make the best of how things work out. ~John Wooden",
            "To live a creative life, we must lose our fear of being wrong. ~Anonymous",
            "If you are not willing to risk the usual you will have to settle for the ordinary. ~Jim Rohn",
            "Trust because you are willing to accept the risk, not because it's safe or certain. ~Anonymous"
        };
        return;
    }

    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();
        if (!line.isEmpty()) {
            m_quotes.append(line);
        }
    }
    file.close();

    if (m_quotes.isEmpty()) {
        m_quotes.append("Nincsenek idézetek. ~Rendszer");
    }
}

void MainWindow::showRandomQuote()
{
    if (m_quotes.isEmpty()) {
        m_quoteLabel->setText("Nincs elérhető idézet.");
        return;
    }

    int count = m_quotes.size();
    int newIndex;

    if (count == 1) {
        newIndex = 0; // csak egy van, nem tudunk másikat választani
    } else {
        do {
            newIndex = QRandomGenerator::global()->bounded(count);
        } while (newIndex == m_lastIndex);
    }

    m_lastIndex = newIndex;
    m_quoteLabel->setText(m_quotes.at(newIndex));
}

void MainWindow::onNewQuoteClicked()
{
    showRandomQuote();
}

void MainWindow::applyStyles()
{
    // Ablak stílusa
    setStyleSheet(
        "QMainWindow {"
        "   background-color: #f5f5f5;"
        "}"
        "QLabel {"
        "   background-color: white;"
        "   border: 2px solid #cccccc;"
        "   border-radius: 15px;"
        "   padding: 20px;"
        "   font-size: 18px;"
        "   font-family: 'Segoe UI', Arial, sans-serif;"
        "   color: #333333;"
        "   qproperty-alignment: AlignCenter;"
        "}"
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   padding: 8px 16px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #3d8b40;"
        "}"
        );
}
