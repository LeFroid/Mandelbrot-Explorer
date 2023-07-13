#include "window.h"
#include "ui_window.h"

#include <QActionGroup>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>

Window::Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Window),
    m_actionColorIntensity(nullptr),
    m_statusLabel(new QLabel)
{
    ui->setupUi(this);

    connect(ui->actionSave_As, &QAction::triggered, this, &Window::openSaveDialog);
    connect(ui->actionQuit,    &QAction::triggered, this, &Window::close);

    // Add Edit -> Color -> (Color Strategy A, Color Strategy B, ...) items to an exclusive action group
    QActionGroup *colorStrategyGroup = new QActionGroup(this);
    colorStrategyGroup->addAction(ui->actionSmooth);
    colorStrategyGroup->addAction(ui->actionPolynomial);
    colorStrategyGroup->addAction(ui->actionSin_Wave);
    
    ui->actionSmooth->setChecked(true);
    colorStrategyGroup->setExclusive(true);

    setupSmoothColorSpecificAction();
    connect(colorStrategyGroup, &QActionGroup::triggered, [this](QAction *action){
        if (action != ui->actionSmooth && m_actionColorIntensity != nullptr)
        {
            ui->menuEdit->removeAction(m_actionColorIntensity);
            delete m_actionColorIntensity;
            m_actionColorIntensity = nullptr;
        }
        else if (action == ui->actionSmooth && m_actionColorIntensity == nullptr)
        {
            setupSmoothColorSpecificAction();
        }
    });

    connect(ui->actionSmooth,     &QAction::toggled, ui->mandelbrotWidget, &MandelbrotView::setColorStrategySmooth);
    connect(ui->actionPolynomial, &QAction::toggled, ui->mandelbrotWidget, &MandelbrotView::setColorStrategyIter);
    connect(ui->actionSin_Wave,   &QAction::toggled, ui->mandelbrotWidget, &MandelbrotView::setColorStrategyWave);

    connect(ui->actionIteration_Count, &QAction::triggered, this, &Window::openIterationDialog);

    m_statusLabel->setAlignment(Qt::AlignRight);
    ui->statusBar->addWidget(m_statusLabel, 1);
    connect(ui->mandelbrotWidget, &MandelbrotView::displayUpdated, this, &Window::updateStatusBar);

    updateStatusBar();
}

Window::~Window()
{
    if (m_actionColorIntensity != nullptr)
        m_actionColorIntensity->deleteLater();

    delete ui;

    delete m_statusLabel;
}

void Window::openSaveDialog()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    QDir::home().absoluteFilePath(QLatin1String("mandelbrot.bmp")),
                                                    tr("BMP Image (*.bmp)"));
    if (!fileName.isEmpty())
    {
        int colorStrategy = 0;
        if (ui->actionSmooth->isChecked())
            colorStrategy = 0;
        else if (ui->actionPolynomial->isChecked())
            colorStrategy = 1;
        else if (ui->actionSin_Wave->isChecked())
            colorStrategy = 2;

        ui->mandelbrotWidget->saveToFile(fileName, colorStrategy);
    }
}

void Window::openIterationDialog()
{
    bool ok = false;
    int result = QInputDialog::getInt(this, tr("Set Max Iterations"), tr("Maximum Iterations:"),
                                      ui->mandelbrotWidget->getMaxIterations(), 1, 2147483647,
                                      1, &ok);
    if (ok)
        ui->mandelbrotWidget->setMaxIterations(result);
}

void Window::openColorIntensityDialog()
{
    bool ok = false;
    double result = QInputDialog::getDouble(this, tr("Set Color Intensity"), tr("Color Factor:"),
                                      ui->mandelbrotWidget->getColorIntensity(), -2147483647.0, 2147483647.0,
                                      8, &ok);

    if (ok)
        ui->mandelbrotWidget->setColorIntensity(result);
}

void Window::setupSmoothColorSpecificAction()
{
    m_actionColorIntensity = ui->menuEdit->addAction(tr("Color Intensity"));
    connect(m_actionColorIntensity, &QAction::triggered, this, &Window::openColorIntensityDialog);
}

void Window::updateStatusBar()
{
    m_statusLabel->setText(QString("Iterations: %1 | Scale: %2 | Center: (%3, %4)")
        .arg(ui->mandelbrotWidget->getMaxIterations())
        .arg(ui->mandelbrotWidget->getScale())
        .arg(ui->mandelbrotWidget->getCenterX())
        .arg(ui->mandelbrotWidget->getCenterY()));
}
