#include "UI/mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(const QStringList& expirationDates, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), expirationDate(expirationDates.isEmpty() ? QString() : expirationDates.first()) {
    ui->setupUi(this);

    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit_2->setDate(QDate::currentDate());

    comboBox = new QComboBox(this);
    comboBox_2 = new QComboBox(this);
    
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(comboBox);
    layout->addWidget(comboBox_2);

    comboBox->addItems({"1m", "2m", "5m", "15m", "30m", "60m", "90m", "1h", "1d", "5d", "1wk", "1mo", "3mo"});
    comboBox_2->addItems(expirationDates);

    comboBox->setGeometry(180, 230, 111, 22);
    comboBox_2->setGeometry(540, 150, 111, 22);


    // Connect line edits to slots
    // Change the connections
// Connect QLineEdit signals to slots
connect(ui->lineEdit_2, &QLineEdit::textChanged, this, &MainWindow::on_lineEdit_2_textChanged);
connect(ui->lineEdit_11, &QLineEdit::textChanged, this, &MainWindow::on_lineEdit_11_textChanged);

// Connect QComboBox signals to slots
connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onComboBoxIntervalChanged);
connect(comboBox_2, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onComboBoxExpirationChanged);

// Connect QDateEdit signals to slots
connect(ui->dateEdit, &QDateEdit::dateChanged, this, &MainWindow::on_dateEdit_startDateDateChanged);
connect(ui->dateEdit_2, &QDateEdit::dateChanged, this, &MainWindow::on_dateEdit_endDateDateChanged);

// Connect QCheckBox signals to slots
connect(ui->checkBox_3, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_openStateChanged);
connect(ui->checkBox_4, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_highStateChanged);
connect(ui->checkBox_5, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_lowStateChanged);
connect(ui->checkBox_6, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_closeStateChanged);
connect(ui->checkBox_7, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_adjStateChanged);
connect(ui->checkBox_8, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_volumeStateChanged);

// Connect additional QCheckBox signals to slots
connect(ui->checkBox, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_callsStateChanged);
connect(ui->checkBox_2, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_putsStateChanged);


}

MainWindow::~MainWindow() {
    delete ui;
}

// Implement slots to update variables
void MainWindow::on_lineEdit_2_textChanged(const QString &text) {
    ticker = text;
    Q_UNUSED(text);
}

void MainWindow::onComboBoxIntervalChanged(const int& index)
{
    interval = comboBox->itemText(index);
    qDebug() << "Selected Interval: " << interval;
    Q_UNUSED(index);
}

void MainWindow::onComboBoxExpirationChanged(const int& index)
{
    expirationDate = comboBox_2->itemText(index);
    qDebug() << "Selected Interval: " << expirationDate;
    Q_UNUSED(index);
}

void MainWindow::on_lineEdit_11_textChanged(const QString &text) {
    filePath = text;
    Q_UNUSED(text);
}

void MainWindow::setError(const QString& errorBuffer) {
    ui->lineEdit->setText(errorBuffer);
    Q_UNUSED(errorBuffer);
}

// Implement new slots
void MainWindow::on_dateEdit_startDateDateChanged(const QDate &date) {
    endDate = date;
    Q_UNUSED(date);
}

void MainWindow::on_dateEdit_endDateDateChanged(const QDate &date) {
    startDate = date;
    Q_UNUSED(date);
}

void MainWindow::on_checkBox_callsStateChanged(int state) {
    includeCalls = state == Qt::Checked;
    Q_UNUSED(state);
}

void MainWindow::on_checkBox_putsStateChanged(int state) {
    includePuts = state == Qt::Checked;
    Q_UNUSED(state);
}

void MainWindow::on_pushButton_clicked() {
    emit buttonPushed();
}


QString MainWindow::getTicker() const {
    return ticker;
}

void MainWindow::setTicker(const QString &value) {
    ticker = value;
}

QString MainWindow::getFilePath() const {
    return filePath;
}

void MainWindow::setFilePath(const QString &value) {
    filePath = value;
}

// Getters and Setters for QDate variables
QDate MainWindow::getStartDate() const {
    return startDate;
}

void MainWindow::setStartDate(const QDate &value) {
    startDate = value;
}

QDate MainWindow::getEndDate() const {
    return endDate;
}

void MainWindow::setEndDate(const QDate &value) {
    endDate = value;
}

QString MainWindow::getExpirationDate() const {
    return expirationDate;
}

void MainWindow::setExpirationDate(const QString &value) {
    expirationDate = value;
}

// Getters and Setters for bool variables
bool MainWindow::getIncludeCalls() const {
    return includeCalls;
}

void MainWindow::setIncludeCalls(bool value) {
    includeCalls = value;
}

bool MainWindow::getIncludePuts() const {
    return includePuts;
}

void MainWindow::setIncludePuts(bool value) {
    includePuts = value;
}

bool MainWindow::includeOpen() const
{
    return stocksInclude[0];
}

void MainWindow::setIncludeOpen(bool value)
{
    stocksInclude[0] = value;
}

bool MainWindow::includeClose() const
{
    return stocksInclude[1];
}

void MainWindow::setIncludeClose(bool value)
{
    stocksInclude[1] = value;
}

bool MainWindow::includeHigh() const
{
    return stocksInclude[2];
}

void MainWindow::setIncludeHigh(bool value)
{
    stocksInclude[2] = value;
}

bool MainWindow::includeLow() const
{
    return stocksInclude[3];
}

void MainWindow::setIncludeLow(bool value)
{
    stocksInclude[3] = value;
}

bool MainWindow::includeVolume() const
{
    return stocksInclude[4];
}

void MainWindow::setIncludeVolume(bool value)
{
    stocksInclude[4] = value;
}

bool MainWindow::includeAdj() const
{
    return stocksInclude[5];
}

void MainWindow::setIncludeAdj(bool value)
{
    stocksInclude[5] = value;
}

void MainWindow::on_checkBox_openStateChanged(int state)
{
    setIncludeOpen(state == Qt::Checked);
}

void MainWindow::on_checkBox_closeStateChanged(int state)
{
    setIncludeClose(state == Qt::Checked);
}

void MainWindow::on_checkBox_highStateChanged(int state)
{
    setIncludeHigh(state == Qt::Checked);
}

void MainWindow::on_checkBox_lowStateChanged(int state)
{
    setIncludeLow(state == Qt::Checked);
}

void MainWindow::on_checkBox_volumeStateChanged(int state)
{
    setIncludeVolume(state == Qt::Checked);
}

void MainWindow::on_checkBox_adjStateChanged(int state)
{
    setIncludeAdj(state == Qt::Checked);
}

std::string MainWindow::getInterval() const {
    return interval.toStdString();
}
