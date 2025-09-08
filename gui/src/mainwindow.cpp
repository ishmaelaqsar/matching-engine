#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(std::make_unique<Ui::MainWindow>())
{
        ui->setupUi(this);
        setupTables();
        setupConnections();

        // Initialize with the default BTC/USD tab
        m_currentSymbol = ui->symbolTabs->tabText(0);

        // Store the initial tab widgets
        SymbolTabWidgets btcWidgets;
        btcWidgets.tabWidget = ui->btcTab;
        btcWidgets.asksTable = ui->asksTable;
        btcWidgets.bidsTable = ui->bidsTable;
        btcWidgets.ordersTable = ui->ordersTable;
        btcWidgets.tradesTable = ui->tradesTable;
        btcWidgets.logText = ui->logText;
        btcWidgets.lastPriceLabel = ui->lastPriceLabel;
        btcWidgets.spreadLabel = ui->spreadLabel;
        btcWidgets.priceSpinBox = ui->priceSpinBox;
        btcWidgets.qtySpinBox = ui->qtySpinBox;
        btcWidgets.buyButton = ui->buyButton;
        btcWidgets.sellButton = ui->sellButton;

        // Add cancel button to the initial tab
        btcWidgets.cancelButton = new QPushButton("Cancel Selected Order", ui->ordersTab);
        ui->ordersTab->layout()->addWidget(btcWidgets.cancelButton);
        connect(btcWidgets.cancelButton, &QPushButton::clicked, this, &MainWindow::onCancelOrderClicked);

        m_symbolTabs[m_currentSymbol] = btcWidgets;
}

MainWindow::~MainWindow()
{
        stopIoThread();
}

void MainWindow::setupConnections()
{
        // --- Connect Actions from Toolbar ---
        connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::onActionConnectTriggered);
        connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::onActionDisconnectTriggered);
        connect(ui->actionAddSymbol, &QAction::triggered, this, &MainWindow::onActionAddSymbolTriggered);

        // --- Connect Add Tab Button ---
        connect(ui->addTabButton, &QPushButton::clicked, this, &MainWindow::onAddTabButtonClicked);

        // --- Connect Tab Widget Signals ---
        connect(ui->symbolTabs, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);
        connect(ui->symbolTabs, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

        // --- Connect Order Entry Buttons for initial tab ---
        connect(ui->buyButton, &QPushButton::clicked, this, &MainWindow::onBuyClicked);
        connect(ui->sellButton, &QPushButton::clicked, this, &MainWindow::onSellClicked);
}

void MainWindow::setupTables() const
{
        // Configure tables to stretch columns and enable row selection
        const auto tables = {ui->bidsTable, ui->asksTable, ui->ordersTable, ui->tradesTable};
        for (auto *table: tables) {
                table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                table->setSelectionBehavior(QAbstractItemView::SelectRows);
                table->setSelectionMode(QAbstractItemView::SingleSelection);
                table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        }
}

QWidget *MainWindow::createSymbolTab(const QString &symbol)
{
        // Create main tab widget
        auto *tabWidget = new QWidget();
        auto *mainLayout = new QVBoxLayout(tabWidget);

        // Create main splitter (vertical)
        auto *mainSplitter = new QSplitter(Qt::Vertical, tabWidget);

        // Create top splitter (horizontal)
        auto *topSplitter = new QSplitter(Qt::Horizontal, mainSplitter);

        // === ORDER BOOK GROUP ===
        auto *orderBookGroup = new QGroupBox("Order Book", topSplitter);
        auto *orderBookLayout = new QVBoxLayout(orderBookGroup);

        // Asks table
        auto *asksTable = new QTableWidget(0, 3, orderBookGroup);
        asksTable->setHorizontalHeaderLabels({"Price", "Size", "Total"});
        asksTable->setStyleSheet("QTableWidget { color: #e74c3c; }");
        asksTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        asksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        asksTable->setSelectionMode(QAbstractItemView::SingleSelection);
        asksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        orderBookLayout->addWidget(asksTable);

        // Spread group
        auto *spreadGroup = new QGroupBox("Spread", orderBookGroup);
        auto *spreadLayout = new QHBoxLayout(spreadGroup);
        auto *lastPriceLabel = new QLabel("Last Price: N/A", spreadGroup);
        auto *spreadLabel = new QLabel("Spread: N/A", spreadGroup);
        spreadLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        spreadLayout->addWidget(lastPriceLabel);
        spreadLayout->addWidget(spreadLabel);
        orderBookLayout->addWidget(spreadGroup);

        // Bids table
        auto *bidsTable = new QTableWidget(0, 3, orderBookGroup);
        bidsTable->setHorizontalHeaderLabels({"Price", "Size", "Total"});
        bidsTable->setStyleSheet("QTableWidget { color: #2ecc71; }");
        bidsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        bidsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        bidsTable->setSelectionMode(QAbstractItemView::SingleSelection);
        bidsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        orderBookLayout->addWidget(bidsTable);

        topSplitter->addWidget(orderBookGroup);

        // === ORDER ENTRY GROUP ===
        auto *orderEntryGroup = new QGroupBox("Order Entry", topSplitter);
        auto *formLayout = new QFormLayout(orderEntryGroup);

        auto *priceSpinBox = new QDoubleSpinBox(orderEntryGroup);
        priceSpinBox->setDecimals(2);
        priceSpinBox->setMaximum(99999999.99);
        formLayout->addRow("Price:", priceSpinBox);

        auto *qtySpinBox = new QSpinBox(orderEntryGroup);
        qtySpinBox->setMaximum(1000000000);
        formLayout->addRow("Quantity:", qtySpinBox);

        auto *buttonLayout = new QHBoxLayout();
        auto *buyButton = new QPushButton("BUY", orderEntryGroup);
        buyButton->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold; padding: 8px;");
        auto *sellButton = new QPushButton("SELL", orderEntryGroup);
        sellButton->setStyleSheet("background-color: #e74c3c; color: white; font-weight: bold; padding: 8px;");
        buttonLayout->addWidget(buyButton);
        buttonLayout->addWidget(sellButton);
        formLayout->addRow(buttonLayout);

        topSplitter->addWidget(orderEntryGroup);
        mainSplitter->addWidget(topSplitter);

        // === USER DATA TABS ===
        auto *userDataTabs = new QTabWidget(mainSplitter);

        // Orders tab
        auto *ordersTab = new QWidget();
        auto *ordersLayout = new QVBoxLayout(ordersTab);
        auto *ordersTable = new QTableWidget(0, 6, ordersTab);
        ordersTable->setHorizontalHeaderLabels({"Timestamp", "Order ID", "Side", "Price", "Quantity", "Status"});
        ordersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        ordersTable->setSelectionMode(QAbstractItemView::SingleSelection);
        ordersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ordersLayout->addWidget(ordersTable);

        auto *cancelButton = new QPushButton("Cancel Selected Order", ordersTab);
        ordersLayout->addWidget(cancelButton);
        connect(cancelButton, &QPushButton::clicked, this, &MainWindow::onCancelOrderClicked);

        userDataTabs->addTab(ordersTab, "My Open Orders");

        // Trades tab
        auto *tradesTab = new QWidget();
        auto *tradesLayout = new QVBoxLayout(tradesTab);
        auto *tradesTable = new QTableWidget(0, 6, tradesTab);
        tradesTable->setHorizontalHeaderLabels(
                {"Timestamp", "Trade ID", "Price", "Quantity", "Source Order ID", "Matched Order ID"});
        tradesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tradesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        tradesTable->setSelectionMode(QAbstractItemView::SingleSelection);
        tradesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tradesLayout->addWidget(tradesTable);
        userDataTabs->addTab(tradesTab, "Trade History");

        // Logs tab
        auto *logTab = new QWidget();
        auto *logLayout = new QVBoxLayout(logTab);
        auto *logText = new QTextEdit(logTab);
        logText->setReadOnly(true);
        logLayout->addWidget(logText);
        userDataTabs->addTab(logTab, "Logs");

        mainSplitter->addWidget(userDataTabs);
        mainLayout->addWidget(mainSplitter);

        // Connect buy/sell buttons for this tab
        connect(buyButton, &QPushButton::clicked, this, &MainWindow::onBuyClicked);
        connect(sellButton, &QPushButton::clicked, this, &MainWindow::onSellClicked);

        // Store widgets in the symbol tabs map
        SymbolTabWidgets widgets;
        widgets.tabWidget = tabWidget;
        widgets.asksTable = asksTable;
        widgets.bidsTable = bidsTable;
        widgets.ordersTable = ordersTable;
        widgets.tradesTable = tradesTable;
        widgets.logText = logText;
        widgets.lastPriceLabel = lastPriceLabel;
        widgets.spreadLabel = spreadLabel;
        widgets.priceSpinBox = priceSpinBox;
        widgets.qtySpinBox = qtySpinBox;
        widgets.buyButton = buyButton;
        widgets.sellButton = sellButton;
        widgets.cancelButton = cancelButton;

        m_symbolTabs[symbol] = widgets;

        return tabWidget;
}

void MainWindow::addSymbolTab(const QString &symbol)
{
        if (const auto it = m_symbolTabs.find(symbol); it != m_symbolTabs.end()) {
                // Tab already exists, switch to it
                for (int i = 0; i < ui->symbolTabs->count(); ++i) {
                        if (ui->symbolTabs->tabText(i) == symbol) {
                                ui->symbolTabs->setCurrentIndex(i);
                                return;
                        }
                }
        }

        QWidget *tabWidget = createSymbolTab(symbol);
        const int index = ui->symbolTabs->addTab(tabWidget, symbol);
        ui->symbolTabs->setCurrentIndex(index);

        // Request order book for the new symbol if connected
        if (m_client) {
                core::protocol::view::GetBookRequest req{symbol.toStdString()};
                m_client->request(std::move(req));
        }
}

void MainWindow::removeSymbolTab(const int index)
{
        if (ui->symbolTabs->count() <= 1) {
                QMessageBox::information(this, "Cannot Close", "At least one symbol tab must remain open.");
                return;
        }

        const QString symbol = ui->symbolTabs->tabText(index);
        ui->symbolTabs->removeTab(index);
        m_symbolTabs.erase(symbol);

        // Update current symbol
        m_currentSymbol = ui->symbolTabs->tabText(ui->symbolTabs->currentIndex());
}

SymbolTabWidgets *MainWindow::getCurrentTabWidgets()
{
        const auto it = m_symbolTabs.find(m_currentSymbol);
        return it != m_symbolTabs.end() ? &it->second : nullptr;
}

const SymbolTabWidgets *MainWindow::getCurrentTabWidgets() const
{
        const auto it = m_symbolTabs.find(m_currentSymbol);
        return it != m_symbolTabs.end() ? &it->second : nullptr;
}

void MainWindow::refreshOrderBook() const
{
        if (m_client) {
                core::protocol::view::GetBookRequest req{getCurrentSymbol()};
                m_client->request(std::move(req));
        }
}

// ===================================================================================
//  UI Action Slots
// ===================================================================================

void MainWindow::onActionAddSymbolTriggered()
{
        onAddTabButtonClicked();
}

void MainWindow::onAddTabButtonClicked()
{
        bool ok;
        const QString symbol = QInputDialog::getText(
                this, "Add Symbol", "Enter symbol (e.g., ETH/USD, AAPL, etc.):", QLineEdit::Normal, "", &ok);
        if (ok && !symbol.isEmpty()) {
                addSymbolTab(symbol.toUpper());
        }
}

void MainWindow::onTabCloseRequested(const int index)
{
        removeSymbolTab(index);
}

void MainWindow::onTabChanged(const int index)
{
        if (index >= 0 && index < ui->symbolTabs->count()) {
                m_currentSymbol = ui->symbolTabs->tabText(index);
                refreshOrderBook();
        }
}

void MainWindow::onActionConnectTriggered()
{
        if (m_client) {
                appendLog("Already connected.");
                return;
        }

        // Get connection details from the user
        bool ok;
        const QString server = QInputDialog::getText(this, "Connect to Server", "Enter Host:Port", QLineEdit::Normal,
                                                     "127.0.0.1:8080", &ok);
        if (!ok || server.isEmpty()) {
                return;
        }
        QStringList parts = server.split(':');
        if (parts.size() != 2) {
                QMessageBox::warning(this, "Invalid Input", "Format must be host:port");
                return;
        }
        const std::string host = parts[0].toStdString();
        const quint16 port = parts[1].toUShort();

        // --- Initialize Networking ---
        m_ioContext = std::make_unique<boost::asio::io_context>();
        m_workGuard = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(
                boost::asio::make_work_guard(*m_ioContext));
        m_client = std::make_shared<tcp::Client>(*m_ioContext, host, port);

        // --- Register Response Handlers (Thread-Safe) ---
        auto invoke = [this](auto function) { QMetaObject::invokeMethod(this, function, Qt::QueuedConnection); };

        m_client->register_handler<core::protocol::view::GetBookResponse>(
                [invoke, this](const auto &msg) { invoke([this, msg] { handleGetBookResponse(msg); }); });
        m_client->register_handler<core::protocol::trading::AddOrderResponse>(
                [invoke, this](const auto &msg) { invoke([this, msg] { handleAddOrderResponse(msg); }); });
        m_client->register_handler<core::protocol::trading::CancelOrderResponse>(
                [invoke, this](const auto &msg) { invoke([this, msg] { handleCancelOrderResponse(msg); }); });
        m_client->register_handler<core::protocol::trading::Trade>(
                [invoke, this](const auto &msg) { invoke([this, msg] { handleTrade(msg); }); });

        m_client->connect();
        startIoThread();
        appendLog(QString("Connecting to %1:%2...").arg(QString::fromStdString(host)).arg(port));
        ui->statusbar->showMessage(QString("Connected to %1:%2").arg(QString::fromStdString(host)).arg(port), 5000);

        // Automatically request the order book for the current symbol on connect
        core::protocol::view::GetBookRequest req{getCurrentSymbol()};
        m_client->request(std::move(req));

        // --- Periodic order book request ---
        m_bookTimer = new QTimer(this);
        connect(m_bookTimer, &QTimer::timeout, this, &MainWindow::refreshOrderBook);
        m_bookTimer->start(1000); // every 1000 ms (1 second)
}

void MainWindow::onActionDisconnectTriggered()
{
        if (!m_client) return;
        if (m_bookTimer) {
                m_bookTimer->stop();
                m_bookTimer->deleteLater();
                m_bookTimer = nullptr;
        }
        m_client->disconnect();
        stopIoThread();
        m_client.reset();
        appendLog("Disconnected.");
        ui->statusbar->showMessage("Disconnected", 5000);
}

void MainWindow::onBuyClicked()
{
        sendAddOrderRequest(core::Side::Buy);
}

void MainWindow::onSellClicked()
{
        sendAddOrderRequest(core::Side::Sell);
}

void MainWindow::onCancelOrderClicked()
{
        if (!ensureConnected()) {
                return;
        }
        const auto *widgets = getCurrentTabWidgets();
        if (!widgets) return;

        auto selectedItems = widgets->ordersTable->selectedItems();
        if (selectedItems.isEmpty()) {
                QMessageBox::information(this, "No Order Selected",
                                         "Please select an order from the 'My Open Orders' table to cancel.");
                return;
        }

        const int row = selectedItems.first()->row();
        const core::OrderId orderId = widgets->ordersTable->item(row, 0)->text().toULongLong();

        core::protocol::trading::CancelOrderRequest req{getCurrentSymbol(), orderId};
        m_client->request(std::move(req));
        appendLog(QString("Sent CancelOrderRequest for Order ID: %1").arg(orderId));
}

void MainWindow::sendAddOrderRequest(const core::Side side)
{
        if (!ensureConnected()) {
                return;
        }
        const auto *widgets = getCurrentTabWidgets();
        if (!widgets) return;

        auto symbol = getCurrentSymbol();
        const auto price = static_cast<core::Price>(widgets->priceSpinBox->value());
        const auto quantity = static_cast<core::Quantity>(widgets->qtySpinBox->value());

        if (quantity == 0) {
                QMessageBox::warning(this, "Invalid Quantity", "Quantity cannot be zero.");
                return;
        }

        core::protocol::trading::AddOrderRequest req{std::move(symbol), price, quantity, side};
        m_client->request(std::move(req));
        appendLog(QString("Sent AddOrderRequest: %1 %2 @ %3")
                          .arg(side == core::Side::Buy ? "BUY" : "SELL")
                          .arg(quantity)
                          .arg(price));
}

// ===================================================================================
//  Data Handler Slots
// ===================================================================================

void MainWindow::handleGetBookResponse(const core::protocol::view::GetBookResponse &msg)
{
        updateBidsTable(msg.bids());
        updateAsksTable(msg.asks());

        if (const auto *widgets = getCurrentTabWidgets(); widgets && !msg.bids().empty() && !msg.asks().empty()) {
                const double spread = msg.asks().front().price - msg.bids().front().price;
                widgets->spreadLabel->setText(QString("Spread: %1").arg(spread));
        }
}

void MainWindow::handleAddOrderResponse(const core::protocol::trading::AddOrderResponse &msg) const
{
        appendLog(QString("AddOrderRequest ACCEPTED. Order ID: %1").arg(msg.order_id()));
        addOrderToTable(msg.order_id(), msg.side(), msg.price(), msg.quantity(), msg.timestamp());
}

void MainWindow::handleCancelOrderResponse(const core::protocol::trading::CancelOrderResponse &msg)
{
        if (msg.success()) {
                appendLog(QString("CancelOrderRequest ACCEPTED for Order ID: %1").arg(msg.order_id()));
                removeOrderFromTable(msg.order_id());
        } else {
                appendLog(QString("CancelOrderRequest REJECTED for Order ID: %1").arg(msg.order_id()));
                QMessageBox::warning(this, "Cancel Rejected", "The order could not be canceled.");
        }
}

void MainWindow::handleTrade(const core::protocol::trading::Trade &msg) const
{
        appendLog(QString("Received Trade Notification. Price: %1, Quantity: %2").arg(msg.price()).arg(msg.quantity()));
        addTradeToTable(msg.id(), msg.price(), msg.quantity(), msg.source_order(), msg.matched_order(),
                        msg.timestamp());

        if (auto *widgets = getCurrentTabWidgets()) {
                widgets->lastPriceLabel->setText(QString("Last Price: %1").arg(msg.price()));
        }

        // Update order statuses
        updateOrderStatusFromTrade(msg.source_order(), msg.quantity());
        updateOrderStatusFromTrade(msg.matched_order(), msg.quantity());
}

void MainWindow::updateOrderStatusFromTrade(const core::OrderId orderId, core::Quantity tradeQuantity) const
{
        auto *widgets = getCurrentTabWidgets();
        if (!widgets) return;

        for (int row = 0; row < widgets->ordersTable->rowCount(); ++row) {
                auto *idItem = widgets->ordersTable->item(row, 1); // Order ID column
                if (!idItem) continue;

                if (idItem->text().toULongLong() == orderId) {
                        auto *qtyItem = widgets->ordersTable->item(row, 4); // Quantity column
                        if (!qtyItem) continue;

                        // Get remaining quantity
                        qulonglong remaining = qtyItem->text().replace(',', "").toULongLong();
                        if (tradeQuantity > remaining) tradeQuantity = remaining; // safety
                        remaining -= tradeQuantity;
                        qtyItem->setText(qstr(remaining));

                        // Update status
                        auto *statusItem = widgets->ordersTable->item(row, 5); // Status column
                        if (!statusItem) continue;
                        statusItem->setText(remaining == 0 ? "FILLED" : "ACTIVE");

                        break; // found the order, no need to continue
                }
        }
}

void MainWindow::appendLog(const QString &message) const
{
        if (auto *widgets = getCurrentTabWidgets()) {
                widgets->logText->append(QString("[%1] %2")
                                                 .arg(QDateTime::currentDateTime().toString("dd/MM/yy hh:mm:ss.zzz"))
                                                 .arg(message));
        }
}

// ===================================================================================
//  UI Update Helpers
// ===================================================================================

void MainWindow::updateBidsTable(const std::vector<core::protocol::view::Level> &bids)
{
        const auto *widgets = getCurrentTabWidgets();
        if (!widgets) return;

        widgets->bidsTable->setRowCount(0);
        for (const auto &level: bids) {
                const int row = widgets->bidsTable->rowCount();
                widgets->bidsTable->insertRow(row);
                widgets->bidsTable->setItem(row, 0, new QTableWidgetItem(qstr(level.price)));
                widgets->bidsTable->setItem(row, 1, new QTableWidgetItem(qstr(level.quantity)));
        }
}

void MainWindow::updateAsksTable(const std::vector<core::protocol::view::Level> &asks)
{
        const auto *widgets = getCurrentTabWidgets();
        if (!widgets) return;

        widgets->asksTable->setRowCount(0);
        // Asks are typically shown with the lowest price at the bottom, closest to the spread
        for (int i = asks.size() - 1; i >= 0; --i) {
                const auto &level = asks[i];
                const int row = widgets->asksTable->rowCount();
                widgets->asksTable->insertRow(row);
                widgets->asksTable->setItem(row, 0, new QTableWidgetItem(qstr(level.price)));
                widgets->asksTable->setItem(row, 1, new QTableWidgetItem(qstr(level.quantity)));
        }
}

void MainWindow::addOrderToTable(const core::OrderId orderId, const core::Side side, const core::Price price,
                                 const core::Quantity quantity, const core::Timestamp timestamp) const
{
        auto *widgets = getCurrentTabWidgets();
        if (!widgets) return;

        const int row = widgets->ordersTable->rowCount();
        widgets->ordersTable->insertRow(row);
        widgets->ordersTable->setItem(
                row, 0,
                new QTableWidgetItem(QDateTime::fromMSecsSinceEpoch(timestamp).toString("dd/MM/yy hh:mm:ss.zzz")));
        widgets->ordersTable->setItem(row, 1, new QTableWidgetItem(qstr(orderId)));
        widgets->ordersTable->setItem(row, 2, new QTableWidgetItem(side == core::Side::Buy ? "BUY" : "SELL"));
        widgets->ordersTable->setItem(row, 3, new QTableWidgetItem(qstr(price)));
        widgets->ordersTable->setItem(row, 4, new QTableWidgetItem(qstr(quantity)));
        widgets->ordersTable->setItem(row, 5, new QTableWidgetItem("Active"));
}

void MainWindow::removeOrderFromTable(const core::OrderId orderId) const
{
        auto *widgets = getCurrentTabWidgets();
        if (!widgets) return;

        for (int i = 0; i < widgets->ordersTable->rowCount(); ++i) {
                if (widgets->ordersTable->item(i, 0)->text().toULongLong() == orderId) {
                        widgets->ordersTable->removeRow(i);
                        return;
                }
        }
}

void MainWindow::addTradeToTable(const core::TradeId tradeId, const core::Price price, const core::Quantity quantity,
                                 const core::OrderId sourceOrder, const core::OrderId matchedOrder,
                                 const core::Timestamp timestamp) const
{
        auto *widgets = getCurrentTabWidgets();
        if (!widgets) return;

        widgets->tradesTable->insertRow(0); // Insert at the top for most recent trades first
        widgets->tradesTable->setItem(
                0, 0,
                new QTableWidgetItem(QDateTime::fromMSecsSinceEpoch(timestamp).toString("dd/MM/yy hh:mm:ss.zzz")));
        widgets->tradesTable->setItem(0, 1, new QTableWidgetItem(qstr(tradeId)));
        widgets->tradesTable->setItem(0, 2, new QTableWidgetItem(qstr(price)));
        widgets->tradesTable->setItem(0, 3, new QTableWidgetItem(qstr(quantity)));
        widgets->tradesTable->setItem(0, 4, new QTableWidgetItem(qstr(sourceOrder)));
        widgets->tradesTable->setItem(0, 5, new QTableWidgetItem(qstr(matchedOrder)));
}

// ===================================================================================
//  Networking & Threading
// ===================================================================================

std::string MainWindow::getCurrentSymbol() const
{
        return ui->symbolTabs->tabText(ui->symbolTabs->currentIndex()).toStdString();
}

void MainWindow::startIoThread()
{
        m_ioThread = std::thread([this] {
                try {
                        m_ioContext->run();
                } catch (const std::exception &e) {
                        QMetaObject::invokeMethod(
                                this, [this, e] { appendLog(QString("IO thread exception: %1").arg(e.what())); });
                }
        });
}

void MainWindow::stopIoThread()
{
        if (m_ioContext) {
                m_workGuard.reset();
                m_ioContext->stop();
        }
        if (m_ioThread.joinable()) {
                m_ioThread.join();
        }
}

bool MainWindow::ensureConnected()
{
        if (!m_client) {
                QMessageBox::warning(this, "Error", "Not connected to server.");
                return false;
        }
        return true;
}
