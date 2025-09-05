#pragma once

#include <QMainWindow>
#include <QMutex>
#include <QPointer>
#include <QString>

#include <memory>
#include <thread>
#include <boost/asio.hpp>
#include <core/protocol/message.h>

#include <core/protocol/trading/add_order.h>
#include <core/protocol/trading/cancel_order.h>
#include <core/protocol/trading/modify_order.h>
#include <core/protocol/view/get_book.h>
#include <tcp/client.h>

class QLineEdit;
class QPushButton;
class QPlainTextEdit;
class QComboBox;
class QStackedWidget;
class QSpinBox;
class QDoubleSpinBox;

class MainWindow : public QMainWindow
{
        Q_OBJECT
public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow() override;

private slots:
        void onConnectClicked();
        void onDisconnectClicked();
        void onSendClicked();
        void appendLog(const QString &line);
        void appendResponse(const QString &line);

private:
        // UI
        QLineEdit *hostEdit_;
        QLineEdit *portEdit_;
        QPushButton *connectBtn_;
        QPushButton *disconnectBtn_;
        QComboBox *typeBox_;
        QStackedWidget *formStack_;
        QPushButton *sendBtn_;
        QPlainTextEdit *logView_;

        // Per-request widgets
        // get: symbol
        QLineEdit *get_symbol_;

        // add: symbol, price, quantity, side
        QLineEdit *add_symbol_;
        QDoubleSpinBox *add_price_;
        QSpinBox *add_qty_;
        QComboBox *add_side_;

        // modify: symbol, order_id, price, quantity
        QLineEdit *mod_symbol_;
        QSpinBox *mod_order_id_;
        QDoubleSpinBox *mod_price_;
        QSpinBox *mod_qty_;

        // cancel: symbol, order_id
        QLineEdit *can_symbol_;
        QSpinBox *can_order_id_;

        // Networking
        std::unique_ptr<boost::asio::io_context> io_;
        std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> work_;
        std::thread ioThread_;
        std::shared_ptr<tcp::Client> client_;

        // Helpers
        void startIoThread();
        void stopIoThread();
        void ensureConnected();
        std::unique_ptr<core::protocol::Message> buildMessageFromForm();
};
