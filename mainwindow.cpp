#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initializeGroupBoxWidgets();
    setGroupBoxWidgetSizePolicy();
    connectConversionButtons();
    connectLineEditInputs();

    connect(ui->leInputValue, &QLineEdit::textChanged, this, &MainWindow::onInputValueChanged);

    // 초기 DEC 모드 설정
    activeSuffix = "Deci";
    resetLabelStyles();
    ui->leDeci->setEnabled(true);
    labelMap["Deci"]->setStyleSheet("background-color: black; color: white;");

    // 탭 아이콘 설정
    PropertySetting("tabFullKeypad", "C:/dev/Git/NumberSystemCalculator/keypad.png");
    PropertySetting("tabBitwiseTransformationKeypad", "C:/dev/Git/NumberSystemCalculator/btkeypad.png");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeGroupBoxWidgets()
{
    labelList = ui->groupBox->findChildren<QLabel*>();
    buttonList = ui->groupBox->findChildren<QPushButton*>();
    lineEditList = ui->groupBox->findChildren<QLineEdit*>();

    for (QLabel* label : labelList) {
        QString name = label->objectName();
        if (name.startsWith("lb")) {
            QString key = name.mid(2);
            labelMap[key] = label;
        }
    }

    for (QLineEdit* edit : lineEditList) {
        QString name = edit->objectName();
        if (name.startsWith("le") && name != "leInputValue") {
            QString key = name.mid(2);
            lineEditMap[key] = edit;
            edit->installEventFilter(this);
        }
    }
}

void MainWindow::setGroupBoxWidgetSizePolicy()
{
    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    for (QLabel* label : labelList) label->setSizePolicy(policy);
    for (QPushButton* button : buttonList) button->setSizePolicy(policy);
    for (QLineEdit* edit : lineEditList) edit->setSizePolicy(policy);
}

void MainWindow::connectConversionButtons()
{
    connect(ui->pbnBin, &QPushButton::clicked, this, &MainWindow::onConversionButtonClicked);
    connect(ui->pbnOct, &QPushButton::clicked, this, &MainWindow::onConversionButtonClicked);
    connect(ui->pbnDeci, &QPushButton::clicked, this, &MainWindow::onConversionButtonClicked);
    connect(ui->pbnHexa, &QPushButton::clicked, this, &MainWindow::onConversionButtonClicked);
}

void MainWindow::connectLineEditInputs()
{
    for (auto it = lineEditMap.begin(); it != lineEditMap.end(); ++it) {
        QLineEdit* edit = it.value();
        QString suffix = it.key();

        connect(edit, &QLineEdit::textChanged, this, [=](const QString &text){
            if (!isValidInput(suffix, text)) return;
            if (suffix != activeSuffix) return;

            ui->leInputValue->setText(text);
            updateConversions(suffix, text);
        });
    }
}

void MainWindow::resetLabelStyles()
{
    for (QLabel* label : labelList) label->setStyleSheet("");
    for (QLineEdit* edit : lineEditMap.values()) edit->setEnabled(false);
}

void MainWindow::onConversionButtonClicked()
{
    QPushButton* senderBtn = qobject_cast<QPushButton*>(sender());
    if (!senderBtn) return;

    QString suffix = senderBtn->objectName().mid(3);
    activeSuffix = suffix;

    resetLabelStyles();

    if (labelMap.contains(suffix)) {
        labelMap[suffix]->setStyleSheet("background-color: black; color: white;");
    }

    if (lineEditMap.contains(suffix)) {
        QLineEdit* targetEdit = lineEditMap[suffix];
        targetEdit->setEnabled(true);
        targetEdit->setFocus();

        QString value = targetEdit->text();
        ui->leInputValue->setText(value);
        updateConversions(suffix, value);
    }
}

void MainWindow::onInputValueChanged(const QString &text)
{
    if (activeSuffix.isEmpty()) return;
    if (!isValidInput(activeSuffix, text)) return;
    if (!lineEditMap.contains(activeSuffix)) return;

    updateConversions(activeSuffix, text);
}

bool MainWindow::isValidInput(const QString &base, const QString &value)
{
    QRegularExpression regex;
    if (base == "Bin") regex.setPattern("^[01]+$");
    else if (base == "Oct") regex.setPattern("^[0-7]+$");
    else if (base == "Deci") regex.setPattern("^[0-9]+$");
    else if (base == "Hexa") regex.setPattern("^[0-9A-F]+$");
    else return false;

    return regex.match(value).hasMatch();
}

void MainWindow::updateConversions(const QString &base, const QString &value)
{
    bool ok = false;
    quint64 number = 0;

    if (base == "Bin") number = value.toULongLong(&ok, 2);
    else if (base == "Oct") number = value.toULongLong(&ok, 8);
    else if (base == "Deci") number = value.toULongLong(&ok, 10);
    else if (base == "Hexa") number = value.toULongLong(&ok, 16);
    else return;

    if (!ok) return;

    if (lineEditMap.contains("Bin")) lineEditMap["Bin"]->setText(QString::number(number, 2));
    if (lineEditMap.contains("Oct")) lineEditMap["Oct"]->setText(QString::number(number, 8));
    if (lineEditMap.contains("Deci")) lineEditMap["Deci"]->setText(QString::number(number, 10));
    if (lineEditMap.contains("Hexa")) lineEditMap["Hexa"]->setText(QString::number(number, 16).toUpper());
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        for (auto it = lineEditMap.begin(); it != lineEditMap.end(); ++it) {
            if (watched == it.value()) {
                QString suffix = it.key();
                QString value = it.value()->text();

                if (!isValidInput(suffix, value)) return false;

                activeSuffix = suffix;
                ui->leInputValue->setText(value);
                updateConversions(suffix, value);

                resetLabelStyles();
                if (labelMap.contains(suffix)) {
                    labelMap[suffix]->setStyleSheet("background-color: black; color: white;");
                }

                if (lineEditMap.contains(suffix)) {
                    lineEditMap[suffix]->setEnabled(true);
                    lineEditMap[suffix]->setFocus();
                }

                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::PropertySetting(const QString &tabObjectName, const QString &iconPath)
{
    for (int i = 0; i < ui->tabWidget->count(); ++i) {
        QWidget* tab = ui->tabWidget->widget(i);
        if (tab && tab->objectName() == tabObjectName) {
            QIcon icon(iconPath);
            ui->tabWidget->setTabIcon(i, icon);
            break;
        }
    }
}
