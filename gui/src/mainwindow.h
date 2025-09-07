#pragma once

#include <QMainWindow>
#include <boost/asio.hpp>
#include <memory>
#include <thread>

#include <matchingengine/protocol/trading/add_order.h>
#include <matchingengine/protocol/trading/cancel_order.h>
#include <matchingengine/protocol/trading/modify_order.h>
#include <matchingengine/protocol/view/get_book.h>
#include <matchingengine/tcp/client.h>

QT_BEGIN_NAMESPACE
namespace Ui
{
        class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
        Q_OBJECT

public:
        using AnyRequest =
                std::variant<core::protocol::view::GetBookRequest, core::protocol::trading::AddOrderRequest,
                             core::protocol::trading::ModifyOrderRequest, core::protocol::trading::CancelOrderRequest>;

        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow() override;

private slots:
        void onConnectClicked();
        void onDisconnectClicked();
        void onGetClicked();
        void onAddClicked();
        void onModifyClicked();
        void onCancelClicked();
        void appendLog(const QString &line) const;
        void appendResponse(const QString &line) const;

private:
        std::unique_ptr<Ui::MainWindow> ui; // generated from .ui

        // Networking
        std::unique_ptr<boost::asio::io_context> f_io;
        std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> f_work;
        std::thread f_io_thread;
        std::shared_ptr<tcp::Client> f_client;

        // Helpers
        void startIoThread();
        void stopIoThread();
        void ensureConnected();
        AnyRequest buildMessageFromForm();
};
