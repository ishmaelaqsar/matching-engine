#include <QMessageBox>
#include <QThread>

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <sstream>

#include <core/protocol/trading/add_order.h>
#include <core/protocol/trading/cancel_order.h>
#include <core/protocol/trading/modify_order.h>
#include <core/protocol/view/get_book.h>
#include <tcp/client.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    io_context_(std::make_unique<boost::asio::io_context>()),
    work_guard_(boost::asio::make_work_guard(*io_context_))
{
        ui->setupUi(this);

        // Start io_context in background thread
        io_thread_ = std::thread([this]() {
                try {
                        io_context_->run();
                } catch (const std::exception &e) {
                        appendLog(QString("io_context exception: %1").arg(e.what()));
                } catch (...) {
                        appendLog("io_context: unknown exception");
                }
        });

        connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
        connect(ui->disconnectButton, &QPushButton::clicked, this, &MainWindow::onDisconnectClicked);

        connect(ui->getButton, &QPushButton::clicked, this, &MainWindow::onGetClicked);
        connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddClicked);
        connect(ui->modifyButton, &QPushButton::clicked, this, &MainWindow::onModifyClicked);
        connect(ui->cancelButton, &QPushButton::clicked, this, &MainWindow::onCancelClicked);
}

MainWindow::~MainWindow()
{
        if (client_) {
                client_->disconnect();
        }
        work_guard_.reset();
        io_context_->stop();
        if (io_thread_.joinable()) {
                io_thread_.join();
        }
        delete ui;
}

void MainWindow::appendLog(const QString &msg)
{
        ui->logText->append(msg);
}

void MainWindow::onConnectClicked()
{
        const auto host = ui->hostEdit->text().toStdString();
        const auto port_str = ui->portEdit->text().toStdString();
        const auto port_ul = std::strtoul(port_str.c_str(), nullptr, 10);

        if (port_ul > 65535) {
                appendLog("Port out of range");
                return;
        }

        const unsigned short port = static_cast<unsigned short>(port_ul);

        client_ = std::make_shared<tcp::Client>(
                *io_context_, host, port, [this](const std::shared_ptr<core::protocol::Message> &resp) {
                        QMetaObject::invokeMethod(
                                this,
                                [this, resp]() {
                                        std::ostringstream oss;
                                        oss << *resp;
                                        appendLog(QString("Response: %1").arg(QString::fromStdString(oss.str())));
                                },
                                Qt::QueuedConnection);
                });

        try {
                client_->connect();
                appendLog("Connecting...");
        } catch (const std::exception &e) {
                appendLog(QString("Connect error: %1").arg(e.what()));
        }
}

void MainWindow::onDisconnectClicked()
{
        if (client_) {
                client_->disconnect();
                appendLog("Disconnected");
        }
}

void MainWindow::onGetClicked()
{
        if (!client_) return;
        const auto symbol = ui->symbolEdit->text().toStdString();
        core::protocol::view::GetBookRequest req(symbol);
        client_->request(req);
        appendLog(QString("Sent GetBook for %1").arg(ui->symbolEdit->text()));
}

void MainWindow::onAddClicked()
{
        if (!client_) return;
        const auto symbol = ui->symbolEdit->text().toStdString();
        const auto price = ui->priceEdit->value();
        const auto qty = ui->qtyEdit->value();
        const auto side = ui->sideCombo->currentIndex() == 0 ? core::Side::Buy : core::Side::Sell;
        core::protocol::trading::AddOrderRequest req(symbol, price, qty, side);
        client_->request(req);
        appendLog(QString("Sent AddOrder: %1 %2@%3").arg(ui->symbolEdit->text()).arg(qty).arg(price));
}

void MainWindow::onModifyClicked()
{
        if (!client_) return;
        const auto symbol = ui->symbolEdit->text().toStdString();
        const auto id = ui->orderIdEdit->value();
        const auto price = ui->priceEdit->value();
        const auto qty = ui->qtyEdit->value();
        core::protocol::trading::ModifyOrderRequest req(symbol, id, price, qty);
        client_->request(req);
        appendLog(QString("Sent ModifyOrder id=%1").arg(id));
}

void MainWindow::onCancelClicked()
{
        if (!client_) return;
        const auto symbol = ui->symbolEdit->text().toStdString();
        const auto id = ui->orderIdEdit->value();
        core::protocol::trading::CancelOrderRequest req(symbol, id);
        client_->request(req);
        appendLog(QString("Sent CancelOrder id=%1").arg(id));
}
