#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(std::make_unique<Ui::MainWindow>())
{
        ui->setupUi(this);
        setupTables();
        setupConnections();

        // Initialize with the default BTC/USD tab
        m_currentSymbol = ui->symbolTabs->tabText(0);

        SymbolTabWidgets btcWidgets;
        btcWidgets.tabWidget = ui->btcTab;
        btcWidgets.asksTable = ui->asksTable;
        btcWidgets.bidsTable = ui->bidsTable;
        btcWidgets.ordersTable = ui->ordersTable;
        btcWidgets.tradesTable = ui->tradesTable;
        btcWidgets.logText = ui->logText;
        btcWidgets.lastPriceLabel = ui->lastPriceLabel;
        btcWidgets.spreadLabel = ui->spreadLabel;
        btcWidgets.bboLabel = ui->bboLabel;
        btcWidgets.priceSpinBox = ui->priceSpinBox;
        btcWidgets.qtySpinBox = ui->qtySpinBox;
        btcWidgets.buyButton = ui->buyButton;
        btcWidgets.sellButton = ui->sellButton;

        // Cancel button
        btcWidgets.cancelButton = new QPushButton("Cancel Selected Order", ui->ordersTab);
        ui->ordersTab->layout()->addWidget(btcWidgets.cancelButton);
        connect(btcWidgets.cancelButton, &QPushButton::clicked, this, &MainWindow::onCancelOrderClicked);

        m_symbolTabs[m_currentSymbol] = btcWidgets;
}

MainWindow::~MainWindow()
{
        stopIoThread();
}

// ===================== SETUP =====================

void MainWindow::setupConnections()
{
        connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::onActionConnectTriggered);
        connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::onActionDisconnectTriggered);
        connect(ui->actionAddSymbol, &QAction::triggered, this, &MainWindow::onActionAddSymbolTriggered);

        connect(ui->addTabButton, &QPushButton::clicked, this, &MainWindow::onAddTabButtonClicked);
        connect(ui->symbolTabs, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);
        connect(ui->symbolTabs, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

        connect(ui->buyButton, &QPushButton::clicked, this, &MainWindow::onBuyClicked);
        connect(ui->sellButton, &QPushButton::clicked, this, &MainWindow::onSellClicked);
}

void MainWindow::setupTables() const
{
        const auto tables = {ui->bidsTable, ui->asksTable, ui->ordersTable, ui->tradesTable};
        for (auto *table: tables) {
                table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                table->setSelectionBehavior(QAbstractItemView::SelectRows);
                table->setSelectionMode(QAbstractItemView::SingleSelection);
                table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        }
}

// ===================== CREATE TAB =====================

QWidget *MainWindow::createSymbolTab(const QString &symbol)
{
        auto *tabWidget = new QWidget();
        auto *mainLayout = new QVBoxLayout(tabWidget);

        // Orderbook splitter: bids (left) | asks (right)
        auto *orderBookSplitter = new QSplitter(Qt::Horizontal, tabWidget);

        // Bids
        auto *bidsTable = new QTableWidget(0, 2, tabWidget);
        bidsTable->setHorizontalHeaderLabels({"Price", "Size"});
        bidsTable->setStyleSheet("QTableWidget { color: #2ecc71; }");
        bidsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        bidsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        bidsTable->setSelectionMode(QAbstractItemView::SingleSelection);
        bidsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        orderBookSplitter->addWidget(bidsTable);

        // Asks
        auto *asksTable = new QTableWidget(0, 2, tabWidget);
        asksTable->setHorizontalHeaderLabels({"Price", "Size"});
        asksTable->setStyleSheet("QTableWidget { color: #e74c3c; }");
        asksTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        asksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        asksTable->setSelectionMode(QAbstractItemView::SingleSelection);
        asksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        orderBookSplitter->addWidget(asksTable);

        mainLayout->addWidget(orderBookSplitter);

        // Last Price & Spread
        auto *infoLayout = new QHBoxLayout();
        auto *lastPriceLabel = new QLabel("Last Price: N/A", tabWidget);
        auto *spreadLabel = new QLabel("Spread: N/A", tabWidget);
        auto *bboLabel = new QLabel("BBO: N/A", tabWidget);
        spreadLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        bboLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        infoLayout->addWidget(lastPriceLabel);
        infoLayout->addWidget(spreadLabel);
        infoLayout->addWidget(bboLabel);
        mainLayout->addLayout(infoLayout);

        // Order entry: price, quantity, buy/sell buttons
        auto *orderEntryLayout = new QHBoxLayout();
        auto *priceSpinBox = new QSpinBox(tabWidget);
        priceSpinBox->setMaximum(1000000000);
        auto *qtySpinBox = new QSpinBox(tabWidget);
        qtySpinBox->setMaximum(1000000000);
        auto *buyButton = new QPushButton("BUY", tabWidget);
        buyButton->setStyleSheet("background-color:#2ecc71;color:white;font-weight:bold;padding:8px;");
        auto *sellButton = new QPushButton("SELL", tabWidget);
        sellButton->setStyleSheet("background-color:#e74c3c;color:white;font-weight:bold;padding:8px;");

        orderEntryLayout->addWidget(new QLabel("Price:", tabWidget));
        orderEntryLayout->addWidget(priceSpinBox);
        orderEntryLayout->addWidget(new QLabel("Quantity:", tabWidget));
        orderEntryLayout->addWidget(qtySpinBox);
        orderEntryLayout->addWidget(buyButton);
        orderEntryLayout->addWidget(sellButton);

        mainLayout->addLayout(orderEntryLayout);

        // User data tabs: orders, trades, logs
        auto *userDataTabs = new QTabWidget(tabWidget);

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

        mainLayout->addWidget(userDataTabs);

        // Connect buttons
        connect(buyButton, &QPushButton::clicked, this, &MainWindow::onBuyClicked);
        connect(sellButton, &QPushButton::clicked, this, &MainWindow::onSellClicked);

        // Store widgets
        SymbolTabWidgets widgets;
        widgets.tabWidget = tabWidget;
        widgets.asksTable = asksTable;
        widgets.bidsTable = bidsTable;
        widgets.ordersTable = ordersTable;
        widgets.tradesTable = tradesTable;
        widgets.logText = logText;
        widgets.lastPriceLabel = lastPriceLabel;
        widgets.spreadLabel = spreadLabel;
        widgets.bboLabel = bboLabel;
        widgets.priceSpinBox = priceSpinBox;
        widgets.qtySpinBox = qtySpinBox;
        widgets.buyButton = buyButton;
        widgets.sellButton = sellButton;
        widgets.cancelButton = cancelButton;

        m_symbolTabs[symbol] = widgets;

        return tabWidget;
}

// ===================== TAB MANAGEMENT =====================

void MainWindow::addSymbolTab(const QString &symbol)
{
        if (m_symbolTabs.find(symbol) != m_symbolTabs.end()) {
                // switch to existing
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

// ===================== UI ACTION SLOTS =====================

void MainWindow::onActionAddSymbolTriggered()
{
        onAddTabButtonClicked();
}

void MainWindow::onAddTabButtonClicked()
{
        bool ok;
        const QString symbol = QInputDialog::getText(this, "Add Symbol",
                                                     "Enter symbol (e.g., ETH/USD, AAPL):", QLineEdit::Normal, "", &ok);
        if (ok && !symbol.isEmpty()) addSymbolTab(symbol.toUpper());
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
        bool ok;
        const QString server = QInputDialog::getText(this, "Connect to Server", "Enter Host:Port", QLineEdit::Normal,
                                                     "127.0.0.1:8080", &ok);
        if (!ok || server.isEmpty()) return;

        QStringList parts = server.split(':');
        if (parts.size() != 2) {
                QMessageBox::warning(this, "Invalid Input", "Format must be host:port");
                return;
        }
        std::string host = parts[0].toStdString();
        quint16 port = parts[1].toUShort();

        m_ioContext = std::make_unique<boost::asio::io_context>();
        m_workGuard = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(
                boost::asio::make_work_guard(*m_ioContext));
        m_client = std::make_shared<tcp::Client>(*m_ioContext, host, port);

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

        core::protocol::view::GetBookRequest req{getCurrentSymbol()};
        m_client->request(std::move(req));

        m_bookTimer = new QTimer(this);
        connect(m_bookTimer, &QTimer::timeout, this, &MainWindow::refreshOrderBook);
        m_bookTimer->start(1000);
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
        if (!ensureConnected()) return;
        const auto *widgets = getCurrentTabWidgets();
        if (!widgets) return;

        auto selectedItems = widgets->ordersTable->selectedItems();
        if (selectedItems.isEmpty()) {
                QMessageBox::information(this, "No Order Selected", "Please select an order to cancel.");
                return;
        }

        const int row = selectedItems.first()->row();
        const core::OrderId orderId = widgets->ordersTable->item(row, 1)->text().toULongLong();
        core::protocol::trading::CancelOrderRequest req{getCurrentSymbol(), orderId};
        m_client->request(std::move(req));
        appendLog(QString("Sent CancelOrderRequest for Order ID: %1").arg(orderId));
}

void MainWindow::sendAddOrderRequest(const core::Side side)
{
        if (!ensureConnected()) return;
        const auto *widgets = getCurrentTabWidgets();
        if (!widgets) return;

        const std::string symbol = getCurrentSymbol();
        const core::Price price = static_cast<core::Price>(widgets->priceSpinBox->value());
        const core::Quantity quantity = static_cast<core::Quantity>(widgets->qtySpinBox->value());
        if (quantity == 0) {
                QMessageBox::warning(this, "Invalid Quantity", "Quantity cannot be zero.");
                return;
        }
        core::protocol::trading::AddOrderRequest req{symbol, price, quantity, side};
        m_client->request(std::move(req));
        appendLog(QString("Sent AddOrderRequest: %1 %2 @ %3")
                          .arg(side == core::Side::Buy ? "BUY" : "SELL")
                          .arg(quantity)
                          .arg(price));
}

// ===================== DATA HANDLER SLOTS =====================

void MainWindow::handleGetBookResponse(const core::protocol::view::GetBookResponse &msg)
{
        updateBidsTable(msg.bids());
        updateAsksTable(msg.asks());

        if (const auto *widgets = getCurrentTabWidgets(); widgets) {
                const auto bestBid = msg.bids().empty() ? core::protocol::view::Level{0, 0} : msg.bids().front();
                const auto bestAsk = msg.asks().empty() ? core::protocol::view::Level{0, 0} : msg.asks().front();
                const auto spread = msg.bids().empty() || msg.asks().empty() ? 0 : bestAsk.price - bestBid.price;

                widgets->spreadLabel->setText(QString("Spread: %1").arg(spread));
                widgets->bboLabel->setText(QString("Bid: %1@%2, Offer: %3@%4")
                                                   .arg(bestBid.quantity)
                                                   .arg(bestBid.price)
                                                   .arg(bestAsk.quantity)
                                                   .arg(bestAsk.price));
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

        if (auto *widgets = getCurrentTabWidgets())
                widgets->lastPriceLabel->setText(QString("Last Price: %1").arg(msg.price()));

        updateOrderStatusFromTrade(msg.source_order(), msg.quantity());
        updateOrderStatusFromTrade(msg.matched_order(), msg.quantity());
}

// ===================== UI UPDATE HELPERS =====================

void MainWindow::updateBidsTable(const std::vector<core::protocol::view::Level> &bids)
{
        if (const auto *widgets = getCurrentTabWidgets()) {
                widgets->bidsTable->setRowCount(bids.size());
                for (size_t i = 0; i < bids.size(); ++i) {
                        const auto &l = bids[i];
                        widgets->bidsTable->setItem(i, 0, new QTableWidgetItem(QString::number(l.price)));
                        widgets->bidsTable->setItem(i, 1, new QTableWidgetItem(QString::number(l.quantity)));
                        widgets->bidsTable->setItem(i, 2, new QTableWidgetItem(QString::number(l.price * l.quantity)));
                }
        }
}

void MainWindow::updateAsksTable(const std::vector<core::protocol::view::Level> &asks)
{
        if (const auto *widgets = getCurrentTabWidgets()) {
                widgets->asksTable->setRowCount(asks.size());
                for (size_t i = 0; i < asks.size(); ++i) {
                        const auto &l = asks[i];
                        widgets->asksTable->setItem(i, 0, new QTableWidgetItem(QString::number(l.price)));
                        widgets->asksTable->setItem(i, 1, new QTableWidgetItem(QString::number(l.quantity)));
                        widgets->asksTable->setItem(i, 2, new QTableWidgetItem(QString::number(l.price * l.quantity)));
                }
        }
}

void MainWindow::addOrderToTable(const core::OrderId orderId, const core::Side side, const core::Price price,
                                 const core::Quantity quantity, const core::Timestamp timestamp) const
{
        if (auto *widgets = getCurrentTabWidgets()) {
                const int row = widgets->ordersTable->rowCount();
                widgets->ordersTable->insertRow(row);
                widgets->ordersTable->setItem(row, 0, new QTableWidgetItem(formatTimestampMs(timestamp)));
                widgets->ordersTable->setItem(row, 1, new QTableWidgetItem(QString::number(orderId)));
                widgets->ordersTable->setItem(row, 2, new QTableWidgetItem(side == core::Side::Buy ? "BUY" : "SELL"));
                widgets->ordersTable->setItem(row, 3, new QTableWidgetItem(QString::number(price)));
                widgets->ordersTable->setItem(row, 4, new QTableWidgetItem(QString::number(quantity)));
                widgets->ordersTable->setItem(row, 5, new QTableWidgetItem("OPEN"));
        }
}

void MainWindow::removeOrderFromTable(const core::OrderId orderId) const
{
        if (auto *widgets = getCurrentTabWidgets()) {
                for (int i = 0; i < widgets->ordersTable->rowCount(); ++i) {
                        if (widgets->ordersTable->item(i, 1)->text().toULongLong() == orderId) {
                                widgets->ordersTable->removeRow(i);
                                break;
                        }
                }
        }
}

void MainWindow::addTradeToTable(const core::TradeId tradeId, const core::Price price, const core::Quantity quantity,
                                 const core::OrderId sourceOrder, const core::OrderId matchedOrder,
                                 const core::Timestamp timestamp) const
{
        if (auto *widgets = getCurrentTabWidgets()) {
                const int row = widgets->tradesTable->rowCount();
                widgets->tradesTable->insertRow(row);
                widgets->tradesTable->setItem(row, 0, new QTableWidgetItem(formatTimestampMs(timestamp)));
                widgets->tradesTable->setItem(row, 1, new QTableWidgetItem(QString::number(tradeId)));
                widgets->tradesTable->setItem(row, 2, new QTableWidgetItem(QString::number(price)));
                widgets->tradesTable->setItem(row, 3, new QTableWidgetItem(QString::number(quantity)));
                widgets->tradesTable->setItem(row, 4, new QTableWidgetItem(QString::number(sourceOrder)));
                widgets->tradesTable->setItem(row, 5, new QTableWidgetItem(QString::number(matchedOrder)));
        }
}

void MainWindow::updateOrderStatusFromTrade(const core::OrderId orderId, const core::Quantity tradeQuantity) const
{
        if (auto *widgets = getCurrentTabWidgets()) {
                for (int i = 0; i < widgets->ordersTable->rowCount(); ++i) {
                        if (const auto *item = widgets->ordersTable->item(i, 1);
                            item->text().toULongLong() == orderId) {
                                auto *qtyItem = widgets->ordersTable->item(i, 4);
                                const core::Quantity remaining = qtyItem->text().toULongLong() - tradeQuantity;
                                qtyItem->setText(QString::number(remaining));
                                if (remaining == 0) widgets->ordersTable->item(i, 5)->setText("FILLED");
                                break;
                        }
                }
        }
}

void MainWindow::appendLog(const QString &message) const
{
        if (auto *widgets = getCurrentTabWidgets()) {
                widgets->logText->append(
                        QString("[%1] %2").arg(QDateTime::currentDateTime().toString("HH:mm:ss")).arg(message));
        }
}

// ===================== NETWORK THREAD =====================

void MainWindow::startIoThread()
{
        m_ioThread = std::thread([this] { m_ioContext->run(); });
}

void MainWindow::stopIoThread()
{
        if (m_ioContext) {
                m_ioContext->stop();
        }
        if (m_ioThread.joinable()) m_ioThread.join();
}

bool MainWindow::ensureConnected()
{
        if (!m_client) {
                QMessageBox::warning(this, "Not Connected", "Please connect to the server first.");
                return false;
        }
        return true;
}

std::string MainWindow::getCurrentSymbol() const
{
        return m_currentSymbol.toStdString();
}
