#include <QMessageBox>
#include <QThread>

#include "mainwindow.h"
#include "matchingengine/protocol/trading/trade.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(std::make_unique<Ui::MainWindow>())
{
        ui->setupUi(this);

        // Hook up UI actions
        connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
        connect(ui->disconnectButton, &QPushButton::clicked, this, &MainWindow::onDisconnectClicked);
        connect(ui->getButton, &QPushButton::clicked, this, &MainWindow::onGetClicked);
        connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddClicked);
        connect(ui->modifyButton, &QPushButton::clicked, this, &MainWindow::onModifyClicked);
        connect(ui->cancelButton, &QPushButton::clicked, this, &MainWindow::onCancelClicked);
}

MainWindow::~MainWindow()
{
        stopIoThread();
}

// --- Slots ---

void MainWindow::onConnectClicked()
{
        const QString host = ui->hostEdit->text();
        const quint16 port = ui->portEdit->text().toUShort();

        if (f_client) {
                appendLog("Already connected");
                return;
        }

        f_io = std::make_unique<boost::asio::io_context>();
        f_work = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(
                boost::asio::make_work_guard(*f_io));

        f_client = std::make_shared<tcp::Client>(*f_io, host.toStdString(), port);

        // Register response handlers
        f_client->register_handler<core::protocol::trading::Trade>(
                [this](auto &&msg) -> void { appendResponse(QString::fromStdString(msg->to_string())); });
        f_client->register_handler<core::protocol::trading::AddOrderResponse>(
                [this](auto &&msg) -> void { appendResponse(QString::fromStdString(msg->to_string())); });
        f_client->register_handler<core::protocol::trading::ModifyOrderResponse>(
                [this](auto &&msg) { appendResponse(QString::fromStdString(msg->to_string())); });
        f_client->register_handler<core::protocol::trading::CancelOrderResponse>(
                [this](auto &&msg) { appendResponse(QString::fromStdString(msg->to_string())); });
        f_client->register_handler<core::protocol::view::GetBookResponse>(
                [this](auto &&msg) { appendResponse(QString::fromStdString(msg->to_string())); });

        f_client->connect();
        startIoThread();

        appendLog(QString("Connecting to %1:%2").arg(host).arg(port));
}

void MainWindow::onDisconnectClicked()
{
        if (!f_client) return;
        f_client->disconnect();
        stopIoThread();
        f_client.reset();
        appendLog("Disconnected");
}

void MainWindow::onGetClicked()
{
        ensureConnected();
        auto symbol = ui->symbolEdit->text().toStdString();
        core::protocol::view::GetBookRequest req{std::move(symbol)};
        f_client->request(std::move(req));
        appendLog("Sent GetBookRequest");
}

void MainWindow::onAddClicked()
{
        ensureConnected();
        auto symbol = ui->symbolEdit->text().toStdString();
        auto price = static_cast<core::Price>(ui->priceEdit->value());
        auto quantity = static_cast<core::Quantity>(ui->qtyEdit->value());
        auto side = ui->sideCombo->currentText() == "Buy" ? core::Side::Buy : core::Side::Sell;
        core::protocol::trading::AddOrderRequest req{std::move(symbol), price, quantity, side};
        f_client->request(std::move(req));
        appendLog("Sent AddOrderRequest");
}

void MainWindow::onModifyClicked()
{
        ensureConnected();
        auto symbol = ui->symbolEdit->text().toStdString();
        auto order_id = static_cast<core::OrderId>(ui->orderIdEdit->value());
        auto price = static_cast<core::Price>(ui->priceEdit->value());
        auto quantity = static_cast<core::Quantity>(ui->qtyEdit->value());
        core::protocol::trading::ModifyOrderRequest req{std::move(symbol), order_id, price, quantity};
        f_client->request(std::move(req));
        appendLog("Sent ModifyOrderRequest");
}

void MainWindow::onCancelClicked()
{
        ensureConnected();
        auto symbol = ui->symbolEdit->text().toStdString();
        auto order_id = static_cast<core::OrderId>(ui->orderIdEdit->value());
        core::protocol::trading::CancelOrderRequest req{std::move(symbol), order_id};
        f_client->request(std::move(req));
        appendLog("Sent CancelOrderRequest");
}

// --- Helpers ---

void MainWindow::appendLog(const QString &line) const
{
        ui->logText->append(line);
}

void MainWindow::appendResponse(const QString &line) const
{
        ui->logText->append("Response: " + line);
}

void MainWindow::startIoThread()
{
        f_io_thread = std::thread([this] {
                try {
                        f_io->run();
                } catch (const std::exception &e) {
                        QMetaObject::invokeMethod(
                                this, [this, e] { appendLog(QString("IO thread exception: %1").arg(e.what())); });
                }
        });
}

void MainWindow::stopIoThread()
{
        if (f_io) {
                f_work.reset();
                f_io->stop();
        }
        if (f_io_thread.joinable()) {
                f_io_thread.join();
        }
}

void MainWindow::ensureConnected()
{
        if (!f_client) {
                QMessageBox::warning(this, "Error", "Not connected to server.");
                throw std::runtime_error("Not connected");
        }
}
