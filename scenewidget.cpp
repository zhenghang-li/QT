#include "scenewidget.h"
#include "moduleitem.h"
#include "connectionitem.h"
#include <QGraphicsScene>
#include <QList>
#include <QPoint>
#include <QBrush>
#include <QColor>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QPen>
#include <QGraphicsPathItem>
#include <QFont>
#include <QDebug>

SceneWidget::SceneWidget(QWidget *parent) : QGraphicsView(parent)
{
    QGraphicsScene* scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 1800, 1200);  // 扩大场景尺寸
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);

    // ============== 创建关键颜色定义 ==============
    const QColor cpuColor(211, 211, 211);    // CPU灰色
    const QColor l1Color(255, 215, 0);       // L1黄金色
    const QColor l2Color(30, 144, 255);      // L2道奇蓝
    const QColor l3Color(50, 205, 50);       // L3酸橙绿
    const QColor memColor(255, 99, 71);      // 内存番茄红
    const QColor routerColor(240, 248, 255); // 路由器爱丽丝蓝
    const QColor busyPathColor(220, 20, 60); // 高负载路径深红

    // ============== 创建路由器节点 ==============
    QList<ModuleItem*> routers;
    // 优化布局：路由器按环形排列，避免遮挡
    const QList<QPointF> routerPositions = {
        QPointF(700, 300),   // Router0 (中心位置)
        QPointF(900, 300),   // Router1 (右侧)
        QPointF(900, 500),   // Router2 (右下)
        QPointF(700, 500),   // Router3 (左下)
        QPointF(500, 400),   // Router4 (左侧)
        QPointF(1100, 400)   // Router5 (右侧空闲节点)
    };

    for (int i = 0; i < routerPositions.size(); ++i) {
        QString name = QString("Router%1").arg(i);
        if (i == 5) name += " (空闲)";

        ModuleItem* router = new ModuleItem(name, routerPositions[i].x(),
                                            routerPositions[i].y(), 120, 70);
        router->setBrush(routerColor);

        // 添加多个端口：左、右、上、下
        router->addPort(ModuleItem::Left);
        router->addPort(ModuleItem::Right);
        router->addPort(ModuleItem::Top);
        router->addPort(ModuleItem::Bottom);

        scene->addItem(router);
        routers.append(router);
        if (i == 0) {
            QString routerInfo = "<b>Router0 详细信息:</b><br>"
                                 "• 类型: 中央路由器<br>"
                                 "• 端口数: 4 (上/下/左/右)<br>"
                                 "• 连接数: 5<br>"
                                 "• 平均延迟: 3.2ns<br>"
                                 "• 最大带宽: 20GB/s<br>"
                                 "<b>端口映射:</b><br>"
                                 "• Port1: L3Cache0<br>"
                                 "• Port2: CPU0 子系统";
            router->setInfoText(routerInfo);
        }

        // 标注端口映射关系 (根据文档2要求)
        if (i == 0) {
            // Router0: Port1 -> L3Cache0
            QGraphicsTextItem* portLabel = new QGraphicsTextItem("Port1: L3Cache0");
            portLabel->setPos(routerPositions[i].x() + 130, routerPositions[i].y() + 10);
            portLabel->setDefaultTextColor(Qt::darkBlue);
            scene->addItem(portLabel);
        }
    }

    // ============== 创建CPU + L1 + L2缓存子系统 ==============
    QList<ModuleItem*> cpus;
    QList<ModuleItem*> l1Caches;
    QList<ModuleItem*> l2Caches;

    // 优化布局：CPU/L1/L2垂直排列在左侧
    const QList<QPointF> cpuPositions = {
        QPointF(200, 200),   // CPU0
        QPointF(200, 400),   // CPU1
        QPointF(200, 600),   // CPU2
        QPointF(200, 800)    // CPU3
    };

    // L1缓存位置：位于CPU右侧
    const QList<QPointF> l1Positions = {
        QPointF(350, 200),   // L1d0 + L1i0
        QPointF(350, 400),   // L1d1 + L1i1
        QPointF(350, 600),   // L1d2 + L1i2
        QPointF(350, 800)    // L1d3 + L1i3
    };

    // L2缓存位置：位于L1右侧
    const QList<QPointF> l2Positions = {
        QPointF(500, 200),   // L2Cache0
        QPointF(500, 400),   // L2Cache1
        QPointF(500, 600),   // L2Cache2
        QPointF(500, 800)    // L2Cache3
    };

    for (int i = 0; i < 4; ++i) {
        // 创建CPU模块
        ModuleItem* cpu = new ModuleItem(QString("CPU%1").arg(i),
                                         cpuPositions[i].x(), cpuPositions[i].y(),
                                         120, 60);
        cpu->setBrush(cpuColor);
        cpu->addPort(ModuleItem::Right); // 右侧端口连接L1
        scene->addItem(cpu);
        cpus.append(cpu);
        QString cpuInfo = QString("<b>CPU%1 详细信息:</b><br>").arg(i)
                          + "• 架构: ARMv8.2<br>"
                            "• 频率: 2.5GHz<br>"
                            "• 核心数: 4<br>"
                            "• 缓存:<br>"
                            "  - L1i: 32KB<br>"
                            "  - L1d: 32KB<br>"
                            "• 功耗: 15W<br>"
                            "• 指令数: " + QString::number(i * 20000 + 30000) + "/s";
        cpu->setInfoText(cpuInfo);

        // 创建L1缓存模块 (分为L1d和L1i)
        ModuleItem* l1Cache = new ModuleItem(QString("L1%1").arg(i),
                                             l1Positions[i].x(), l1Positions[i].y(),
                                             100, 60);
        l1Cache->setBrush(l1Color);
        l1Cache->addPort(ModuleItem::Left);  // 左侧连接CPU
        l1Cache->addPort(ModuleItem::Right); // 右侧连接L2
        scene->addItem(l1Cache);
        l1Caches.append(l1Cache);
        QString l1Info = QString("<b>L1%1 缓存:</b><br>").arg(i)
                         + "• 类型: 一级缓存<br>"
                           "• 大小: 64KB (32KB指令+32KB数据)<br>"
                           "• 延迟: 0.8ns<br>"
                           "• 关联性: 4路<br>"
                           "• 命中率: " + QString::number(96.0 + i) + "%";
        l1Cache->setInfoText(l1Info);

        // 创建L2缓存模块 (显式标注)
        ModuleItem* l2Cache = new ModuleItem(QString("L2Cache%1").arg(i),
                                             l2Positions[i].x(), l2Positions[i].y(),
                                             120, 50);
        l2Cache->setBrush(l2Color);
        l2Cache->addPort(ModuleItem::Left);  // 左侧连接L1
        l2Cache->addPort(ModuleItem::Right); // 右侧连接路由器
        scene->addItem(l2Cache);
        l2Caches.append(l2Cache);
        QString l2Info = QString("<b>L2%1 缓存:</b><br>").arg(i)
                         + "• 类型: 二级缓存<br>"
                           "• 总线端口: %2<br>"
                           "• L2路数: 8<br>"
                           "• L2组数: 128<br>"
                           "• MSHR数: 8<br>"
                           "• 索引延迟: 4周期<br>"
                           "• 命中率: " + QString::number(96.0 + i) + "%";
        l2Cache->setInfoText(l2Info);

        // 连接CPU到L1缓存
        scene->addItem(new ConnectionItem(
            cpu->getPortPos(0),
            l1Cache->getPortPos(0),
            QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap)
            ));

        // 连接L1到L2缓存
        scene->addItem(new ConnectionItem(
            l1Cache->getPortPos(1),
            l2Cache->getPortPos(0),
            QPen(Qt::darkGray, 2, Qt::SolidLine, Qt::RoundCap)
            ));

        // 连接L2缓存到路由器 (根据文档2的端口映射)
        int routerIndex = i; // 默认映射
        if (i == 2) routerIndex = 3; // CPU2 -> Router3
        if (i == 3) routerIndex = 4; // CPU3 -> Router4

        scene->addItem(new ConnectionItem(
            l2Cache->getPortPos(1),
            routers[routerIndex]->getPortPos(ModuleItem::Left), // 使用Left端口连接
            QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap)
            ));
    }

    // ============== 创建L3缓存模块 ==============
    QList<ModuleItem*> l3Caches;
    // L3缓存位置：位于路由器下方
    const QList<QPointF> l3Positions = {
        QPointF(700, 600),   // L3Cache0 (Router0下方)
        QPointF(900, 600),   // L3Cache1 (Router1下方)
        QPointF(900, 700),   // L3Cache2 (Router2下方)
        QPointF(700, 700)    // L3Cache3 (Router3下方)
    };

    for (int i = 0; i < 4; ++i) {
        ModuleItem* l3Cache = new ModuleItem(QString("L3Cache%1").arg(i),
                                             l3Positions[i].x(), l3Positions[i].y(),
                                             150, 60);
        l3Cache->setBrush(l3Color);
        l3Cache->addPort(ModuleItem::Top); // 顶部端口连接路由器
        scene->addItem(l3Cache);
        l3Caches.append(l3Cache);
        QString l3Info = QString("<b>L3%1 缓存:</b><br>").arg(i)
                         +      "• 类型: 共享三级缓存<br>"
                                "• 总线端口: %2<br>"
                                "• 路数: 8<br>"
                                "• 组数: 512<br>"
                                "• MSHR数: 8<br>"
                                "• 索引延迟: 10周期<br>"
                                "• NUCA索引: %3/4<br>"
                                "• 命中率: " + QString::number(96.0 + i) + "%";
        l3Cache->setInfoText(l3Info);


        // 连接到路由器 (根据文档2的端口映射)
        int routerIndex = i; // 默认映射
        if (i == 2) routerIndex = 2; // L3Cache2 -> Router2
        if (i == 3) routerIndex = 3; // L3Cache3 -> Router3

        scene->addItem(new ConnectionItem(
            l3Cache->getPortPos(0),
            routers[routerIndex]->getPortPos(ModuleItem::Bottom), // 使用Bottom端口连接
            QPen(Qt::darkGreen, 2, Qt::SolidLine, Qt::RoundCap)
            ));
    }

    // ============== 创建内存模块 ==============
    ModuleItem* memory = new ModuleItem("Memory", 500, 700, 200, 80);
    memory->setBrush(memColor);
    memory->addPort(ModuleItem::Top); // 顶部端口连接路由器
    scene->addItem(memory);
    QString memInfo = "<b>内存子系统:</b><br>"
                      "• 容量: 16GB DDR4<br>"
                      "• 频率: 3200MHz<br>"
                      "• 通道数: 2<br>"
                      "• 最大带宽: 45GB/s<br>"
                      "• 平均延迟: 85ns<br>"
                      "• 当前使用率: 68%";
    memory->setInfoText(memInfo);

    // 连接到Router2 (根据文档2要求)
    scene->addItem(new ConnectionItem(
        memory->getPortPos(0),
        routers[2]->getPortPos(ModuleItem::Bottom), // 使用Bottom端口连接
        QPen(Qt::darkRed, 3, Qt::SolidLine, Qt::RoundCap)
        ));

    // ============== 连接路由器节点 (修复拓扑) ==============
    struct RouterConnection {
        int from;
        int to;
        double usage;
        ModuleItem::PortPosition fromPort;
        ModuleItem::PortPosition toPort;
    };

    // 根据文档2要求修复拓扑连接
    const QList<RouterConnection> routerLinks = {
        {0, 1, 0.0181, ModuleItem::Right, ModuleItem::Left},   // Router0->Router1
        {0, 5, 0.0052, ModuleItem::Top, ModuleItem::Bottom},   // Router0->Router5 (新增)
        {1, 0, 0.0154, ModuleItem::Left, ModuleItem::Right},   // Router1->Router0
        {1, 2, 0.0161, ModuleItem::Bottom, ModuleItem::Top},   // Router1->Router2
        {2, 1, 0.0163, ModuleItem::Top, ModuleItem::Bottom},   // Router2->Router1
        {2, 3, 0.0145, ModuleItem::Left, ModuleItem::Right},   // Router2->Router3
        {3, 2, 0.0066, ModuleItem::Right, ModuleItem::Left},   // Router3->Router2
        {3, 4, 0.0082, ModuleItem::Bottom, ModuleItem::Top},   // Router3->Router4
        {4, 3, 0.0034, ModuleItem::Top, ModuleItem::Bottom},   // Router4->Router3
        {4, 5, 0.0071, ModuleItem::Right, ModuleItem::Left},   // Router4->Router5
        {5, 0, 0.0071, ModuleItem::Bottom, ModuleItem::Top},   // Router5->Router0 (修复缺失连接)
        {5, 4, 0.0052, ModuleItem::Left, ModuleItem::Right}    // Router5->Router4
    };

    for (const auto& link : routerLinks) {
        QPointF fromPos = routers[link.from]->getPortPos(link.fromPort);
        QPointF toPos = routers[link.to]->getPortPos(link.toPort);

        // 设置不同颜色表示使用率
        QPen pen(Qt::darkBlue, 2);
        if (link.usage > 0.01) {
            pen = QPen(busyPathColor, 4);
        }

        scene->addItem(new ConnectionItem(fromPos, toPos, pen));

        // 添加使用率标签
        if (link.usage > 0) {
            QGraphicsTextItem* usageLabel = new QGraphicsTextItem(
                QString("%1%").arg(link.usage * 100, 0, 'f', 2)
                );
            QPointF midPoint = (fromPos + toPos) / 2;
            usageLabel->setPos(midPoint.x() - 20, midPoint.y() - 15);
            usageLabel->setDefaultTextColor(link.usage > 0.01 ? Qt::red : Qt::darkBlue);
            QFont labelFont("Arial", 8, QFont::Bold);
            usageLabel->setFont(labelFont);
            scene->addItem(usageLabel);
        }
    }

    // ============== 标注关键瓶颈路径 ==============
    // Router0->Router1 (高负载路径)
    QGraphicsRectItem* busyPathMarker = new QGraphicsRectItem(
        routers[0]->getPortPos(ModuleItem::Right).x() - 5,
        routers[0]->getPortPos(ModuleItem::Right).y() - 5,
        10, 10
        );
    busyPathMarker->setBrush(busyPathColor);
    scene->addItem(busyPathMarker);



    // ============== 添加NUCA访问路径 ==============
    // CPU0 → Router0 → Router1 → L3Cache1
    QPolygonF numaPath;
    numaPath << cpus[0]->getPortPos(0)
             << l1Caches[0]->getPortPos(0)
             << l2Caches[0]->getPortPos(1)
             << routers[0]->getPortPos(ModuleItem::Right)
             << routers[1]->getPortPos(ModuleItem::Left)
             << l3Caches[1]->getPortPos(0);

    QGraphicsPathItem* numaPathIndicator = new QGraphicsPathItem();
    QPainterPath numaPathPainter;
    numaPathPainter.moveTo(numaPath[0]);
    for (int i = 1; i < numaPath.size(); ++i) {
        numaPathPainter.lineTo(numaPath[i]);
    }
    numaPathIndicator->setPath(numaPathPainter);
    numaPathIndicator->setPen(QPen(QColor(106, 90, 205), 2, Qt::DotLine)); // 紫罗兰色
    scene->addItem(numaPathIndicator);

    QGraphicsTextItem* numaLabel = new QGraphicsTextItem("跨节点NUCA访问");
    numaLabel->setPos(numaPath[3].x(), numaPath[3].y() - 20);
    numaLabel->setDefaultTextColor(QColor(106, 90, 205));
    scene->addItem(numaLabel);

    // ============== 添加性能数据标签 ==============
    // CPU性能数据
    QList<QString> cpuPerf = {
        "指令数: 31189\n周期: 85339\nL1命中率: 97.5%",
        "指令数: 34298\n周期: 87306\nL1命中率: 95.0%",
        "指令数: 10333\n周期: 28999\nL1命中率: 98.7%",
        "指令数: 9921\n周期: 27904\nL1命中率: 98.8%"
    };

    for (int i = 0; i < 4; ++i) {
        QGraphicsTextItem* perfLabel = new QGraphicsTextItem(cpuPerf[i]);
        perfLabel->setPos(cpuPositions[i].x() - 50, cpuPositions[i].y() + 70);
        perfLabel->setFont(QFont("Arial", 8));
        scene->addItem(perfLabel);
    }

    // L2缓存命中率
    QList<double> l2HitRates = {0.165, 0.092, 0.056, 0.058}; // 根据文档1计算
    for (int i = 0; i < 4; ++i) {
        QGraphicsTextItem* hitLabel = new QGraphicsTextItem(
            QString("L2命中率: %1%").arg(l2HitRates[i] * 100, 0, 'f', 1)
            );
        hitLabel->setPos(l2Positions[i].x() + 130, l2Positions[i].y() + 10);
        scene->addItem(hitLabel);
    }

    // L3缓存命中率
    QList<double> l3HitRates = {0.362, 0.385, 0.400, 0.395}; // 根据文档1计算
    for (int i = 0; i < 4; ++i) {
        QGraphicsTextItem* hitLabel = new QGraphicsTextItem(
            QString("L3命中率: %1%").arg(l3HitRates[i] * 100, 0, 'f', 1)
            );
        hitLabel->setPos(l3Positions[i].x() + 160, l3Positions[i].y() + 10);
        scene->addItem(hitLabel);
    }

    // 内存使用率
    QGraphicsTextItem* memUsage = new QGraphicsTextItem("内存使用率: 0.8%");
    memUsage->setPos(500, 790);
    scene->addItem(memUsage);

    // ============== 添加图例 ==============
    QGraphicsRectItem* legendBg = new QGraphicsRectItem(50, 50, 300, 200);
    legendBg->setBrush(QBrush(QColor(240, 240, 240, 220)));
    scene->addItem(legendBg);

    QList<QPair<QString, QColor>> legendItems = {
        {"CPU", cpuColor},
        {"L1缓存", l1Color},
        {"L2缓存", l2Color},
        {"L3缓存", l3Color},
        {"路由器", routerColor},
        {"内存", memColor},
        {"高负载路径", busyPathColor},
        {"NUCA访问", QColor(106, 90, 205)}
    };

    for (int i = 0; i < legendItems.size(); ++i) {
        QGraphicsRectItem* colorIcon = new QGraphicsRectItem(70, 80 + i * 25, 20, 15);
        colorIcon->setBrush(legendItems[i].second);
        scene->addItem(colorIcon);

        QGraphicsTextItem* legendLabel = new QGraphicsTextItem(legendItems[i].first);
        legendLabel->setPos(100, 80 + i * 25 - 5);
        scene->addItem(legendLabel);
    }

    // ============== 添加全局标题 ==============
    QGraphicsTextItem* title = new QGraphicsTextItem("三级缓存NUCA架构拓扑图3_2.");
    title->setPos(650, 50);
    QFont titleFont("Arial", 18, QFont::Bold);
    title->setFont(titleFont);
    scene->addItem(title);

    // 确保正确缩放视图
    QTimer::singleShot(100, [this]() {
        fitInView(sceneRect(), Qt::KeepAspectRatio);
        scale(2.8, 2.8);  // 适当缩放确保可见性
    });
}
