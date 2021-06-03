#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow( QWidget *parent = nullptr );

    ~MainWindow();

private:

    Ui::MainWindow *_ui;
};

#endif // MAINWINDOW_H
