#pragma once

#include <QDateTime>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTextEdit>
#include <QTimer>
#include <QWidget>
#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <unordered_map>

#include <matchingengine/protocol/trading/add_order.h>
#include <matchingengine/protocol/trading/cancel_order.h>
#include <matchingengine/protocol/trading/modify_order.h>
#include <matchingengine/protocol/trading/trade.h>
#include <matchingengine/protocol/view/get_book.h>
#include <matchingengine/tcp/client.h>
#include <matchingengine/types.h>

QT_BEGIN_NAMESPACE
namespace Ui
{
        class MainWindow;
}
QT_END_NAMESPACE

// Structure to hold all widgets for a trading symbol tab
struct SymbolTabWidgets
{
        QWidget *tabWidget;
        QTableWidget *asksTable;
        QTableWidget *bidsTable;
        QTableWidget *ordersTable;
        QTableWidget *tradesTable;
        QTextEdit *logText;
        QLabel *lastPriceLabel;
        QLabel *spreadLabel;
        QLabel *bboLabel;
        QSpinBox *priceSpinBox;
        QSpinBox *qtySpinBox;
        QPushButton *buyButton;
        QPushButton *sellButton;
        QPushButton *modifyButton;
        QPushButton *cancelButton;
};

class MainWindow final : public QMainWindow
{
        Q_OBJECT

public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow() override;

        static QString formatTimestampMs(const core::Timestamp timestamp)
        {
                const auto dt = QDateTime::fromMSecsSinceEpoch(timestamp, Qt::UTC);
                return dt.toString("dd/MM/yyyy hh:mm:ss.zzz");
        }

        // Helper for converting integral values to QString with locale-aware formatting
        template<typename T>
        static QString qstr(T value)
        {
                static_assert(std::is_integral_v<T>, "qstr only supports integral types");

                const auto locale = QLocale::c();
                if constexpr (std::is_unsigned_v<T>) {
                        return locale.toString(static_cast<qulonglong>(value));
                } else {
                        return locale.toString(static_cast<qlonglong>(value));
                }
        }

private slots:
        // --- UI Action Slots ---
        void onActionConnectTriggered();
        void onActionDisconnectTriggered();
        void onActionAddSymbolTriggered();
        void onAddTabButtonClicked();
        void onTabCloseRequested(int index);
        void onTabChanged(int index);
        void onBuyClicked();
        void onSellClicked();
        void onModifyOrderClicked();
        void onCancelOrderClicked();

        // --- Data Handler Slots (called from network thread) ---
        void handleGetBookResponse(const core::protocol::view::GetBookResponse &msg);
        void handleAddOrderResponse(const core::protocol::trading::AddOrderResponse &msg) const;
        void handleModifyOrderResponse(const core::protocol::trading::ModifyOrderResponse &msg);
        void handleCancelOrderResponse(const core::protocol::trading::CancelOrderResponse &msg);
        void handleTrade(const core::protocol::trading::Trade &msg) const;
        void updateOrderStatusFromTrade(core::OrderId orderId, core::Quantity tradeQuantity) const;
        void appendLog(const QString &message) const;

private:
        std::unique_ptr<Ui::MainWindow> ui; // The UI generated from the .ui file

        // --- Tab Management ---
        // Each symbol tab has its own set of widgets stored in SymbolTabWidgets
        std::unordered_map<QString, SymbolTabWidgets> m_symbolTabs;
        QString m_currentSymbol;

        // --- Networking ---
        std::unique_ptr<boost::asio::io_context> m_ioContext;
        std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> m_workGuard;
        std::thread m_ioThread;
        std::shared_ptr<tcp::Client> m_client;
        QTimer *m_bookTimer = nullptr;

        // --- Helper Methods ---
        void startIoThread();
        void stopIoThread();
        bool ensureConnected();
        void setupTables() const;
        std::string getCurrentSymbol() const;
        SymbolTabWidgets *getCurrentTabWidgets();
        const SymbolTabWidgets *getCurrentTabWidgets() const;

        // --- Tab Management Helpers ---
        void setupConnections();
        QWidget *createSymbolTab(const QString &symbol);
        void addSymbolTab(const QString &symbol);
        void removeSymbolTab(int index);
        void refreshOrderBook() const;

        // --- UI Update Helpers ---
        void updateBidsTable(const std::vector<core::protocol::view::Level> &bids);
        void updateAsksTable(const std::vector<core::protocol::view::Level> &asks);
        void addOrderToTable(core::OrderId orderId, core::Side side, core::Price price, core::Quantity quantity,
                             core::Timestamp timestamp) const;
        void removeOrderFromTable(core::OrderId orderId) const;
        void addTradeToTable(core::TradeId tradeId, core::Price price, core::Quantity quantity,
                             core::OrderId sourceOrder, core::OrderId matchedOrder, core::Timestamp timestamp) const;
        void sendAddOrderRequest(core::Price price, core::Quantity quantity, core::Side side);
        void sendCancelRequest(core::OrderId orderId) const;
};
