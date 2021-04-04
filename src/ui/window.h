#ifndef _MANDELBROT_UI_WINDOW_H_
#define _MANDELBROT_UI_WINDOW_H_

#include <QMainWindow>

namespace Ui {
    class Window;
}

class Window : public QMainWindow
{
    Q_OBJECT

public:
    /// Window constructor
    explicit Window(QWidget *parent = nullptr);

    /// Window destructor
    ~Window();

private Q_SLOTS:
    void openSaveDialog();
    void openIterationDialog();
    void openColorIntensityDialog();

    void setupSmoothColorSpecificAction();

private:
    /// UI items from .ui file
    Ui::Window *ui;

    /// Action to adjust the color intensity, only available when the color strategy is set to \ref mandelbrot::ColorStrategySmooth
    QAction *m_actionColorIntensity;
};

#endif // _MANDELBROT_UI_WINDOW_H_

