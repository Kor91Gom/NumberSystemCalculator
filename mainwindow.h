#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPbButtonClicked();

private:
    Ui::MainWindow *ui;
    QList<lbBin*> pbButtons;

    void initializePbButtons();
    void setPbButtonSizePolicy();
    void connectPbButtons();
};

#endif // MAINWINDOW_H
