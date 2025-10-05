#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initializePbButtons();
    setPbButtonSizePolicy();
    connectPbButtons();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializePbButtons()
{
    pbButtons.clear();
    const QList<lbBin*> allButtons = ui->tabFullKeypad->findChildren<lbBin*>();
    for (auto it = allButtons.constBegin(); it != allButtons.constEnd(); ++it)
    {
        lbBin* btn = *it;
        const QString name = btn->objectName();
        if (name.contains("pb") || name.contains("comboBoxBitwise") || name.contains("lbBitwiseStatus") || name.contains("pbnBitwiseStatus"))
            pbButtons.append(btn);
    }
}

void MainWindow::setPbButtonSizePolicy()
{
    for (auto it = pbButtons.constBegin(); it != pbButtons.constEnd(); ++it)
    {
        (*it)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
}

void MainWindow::connectPbButtons()
{
    for (auto it = pbButtons.constBegin(); it != pbButtons.constEnd(); ++it)
    {
        connect(*it, &lbBin::clicked, this, &MainWindow::onPbButtonClicked);
    }
}

void MainWindow::onPbButtonClicked()
{
    lbBin* senderBtn = qobject_cast<lbBin*>(sender());
    if (senderBtn)
    {
        // 클릭된 버튼에 대한 동작을 여기에 작성하세요.
    }
}
