
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QPainter>
#include <QPainterPath>
#include <QFrame>
#include <QDialog> 
#include <QList>
#include <QIcon> 
#include <QPixmap>
#include <QScrollArea>
#include <QScrollBar>  
#include <QMouseEvent> 
#include "nitro_backend.h"

#define ICON_PATH ":/nitro.png"


class NitroInfoDialog : public QDialog {
public:
    NitroInfoDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Info NitroLinux");
        setFixedSize(600, 450); 
        setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint); 
        
      
        setStyleSheet(
            "QDialog { background-color: #1a1a1a; border: 2px solid #d30000; border-radius: 10px; }"
            "QLabel { color: #e0e0e0; font-family: Arial; background: transparent; }"
            "QPushButton { background-color: #d30000; color: white; border: none; border-radius: 4px; font-weight: bold; font-size: 14px; padding: 8px; }"
            "QPushButton:hover { background-color: #ff3333; }"
            "QScrollArea { border: none; background: transparent; }"
            "QScrollBar:vertical { border: none; background: #111; width: 10px; margin: 0px; border-radius: 5px; }"
            "QScrollBar::handle:vertical { background: #d30000; min-height: 20px; border-radius: 5px; }"
            "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
        );

        
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(10, 10, 10, 10);
        mainLayout->setSpacing(10);

        
        QScrollArea *scrollArea = new QScrollArea();
        scrollArea->setWidgetResizable(true);
        
        
        QWidget *contentWidget = new QWidget();
        contentWidget->setStyleSheet("background-color: transparent;"); 
        QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
        contentLayout->setContentsMargins(20, 20, 20, 20);
        contentLayout->setSpacing(15);

        
        QLabel *imgLogo = new QLabel();
        QPixmap pixmap(ICON_PATH);
        imgLogo->setPixmap(pixmap.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imgLogo->setAlignment(Qt::AlignCenter);
        contentLayout->addWidget(imgLogo);

        
        QLabel *title = new QLabel("NITRO LINUX");
        title->setStyleSheet("color: #d30000; font-size: 24px; font-weight: bold; letter-spacing: 2px;");
        title->setAlignment(Qt::AlignCenter);
        contentLayout->addWidget(title);

        QLabel *ver = new QLabel("Version 1.0");
        ver->setStyleSheet("color: #888; font-size: 14px; margin-bottom: 10px;");
        ver->setAlignment(Qt::AlignCenter);
        contentLayout->addWidget(ver);

     
        QFrame *line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setStyleSheet("color: #444;");
        contentLayout->addWidget(line);

    
        QLabel *text = new QLabel();
        text->setWordWrap(true);
        text->setTextFormat(Qt::RichText);
        text->setStyleSheet("font-size: 14px; line-height: 140%;");
        text->setText(
            "<p align='center'><b>Controllo Totale per Acer Nitro su Linux</b></p>"
            "<p>Questo software accede direttamente al chip <i>Embedded Controller (EC)</i> "
            "per bypassare le limitazioni del BIOS e sbloccare la massima potenza delle ventole.</p>"
            "<p>Include monitoraggio termico di precisione (x86_pkg & Nvidia) e gestione Power Plan.</p>"
            "<br>"
            "<h3>🛠️ Caratteristiche Tecniche</h3>"
            "<ul>"
            "<li><b>EC Hacking:</b> Scrittura diretta registri 0x37/0x3A.</li>"
            "<li><b>Heartbeat:</b> Sistema anti-override del BIOS ogni 1000ms.</li>"
            "<li><b>Smart Sensors:</b> Rilevamento automatico percorso termico x86.</li>"
            "<li><b>Power Profiles:</b> Integrazione con powerprofilesctl.</li>"
            "<li><b>UI:</b> Interfaccia Qt5 Custom Rendered.</li>"
            "</ul>"
            "<hr>"
            "<p align='center'><b>Sviluppato da:</b> Christian Panetta(mcpf2001.github.io)<br>"
            "<p align='center'><b>Sostieni i miei progetti:</b> Paypal: christianpanetta64@gmail.com <br>"

            "<span style='color:#888'>Powered by Christian Panetta</span></p>"
            "<br>" 
        );
        contentLayout->addWidget(text);
        contentLayout->addStretch();

        
        scrollArea->setWidget(contentWidget);
        mainLayout->addWidget(scrollArea);

      
        QPushButton *btnClose = new QPushButton("CHIUDI");
        connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
        mainLayout->addWidget(btnClose);
    }

    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            dragPosition = event->globalPos() - frameGeometry().topLeft();
            event->accept();
        }
    }
    void mouseMoveEvent(QMouseEvent *event) override {
        if (event->buttons() & Qt::LeftButton) {
            move(event->globalPos() - dragPosition);
            event->accept();
        }
    }
private:
    QPoint dragPosition;
};



class NitroFanGauge : public QWidget {
    Q_OBJECT
public:
    int rpm = 0; QString label; int maxRpm = 6122;
    NitroFanGauge(QString name, QWidget *parent = nullptr) : QWidget(parent), label(name) { setMinimumSize(160, 160); }
    void setRpm(int val) { rpm = val; update(); }
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
        int side = qMin(width(), height()); p.translate(width() / 2, height() / 2); p.scale(side / 200.0, side / 200.0);
        QPen bgPen(QColor("#333333")); bgPen.setWidth(15); p.setPen(bgPen); p.drawEllipse(-80, -80, 160, 160);
        QPen redPen(QColor("#d30000")); redPen.setWidth(15); redPen.setCapStyle(Qt::FlatCap); p.setPen(redPen);
        double percent = (double)rpm / maxRpm; if (percent > 1.0) percent = 1.0;
        int spanAngle = -270 * percent; p.drawArc(-80, -80, 160, 160, 225 * 16, spanAngle * 16);
        p.setPen(Qt::white); QFont fontRpm("Arial", 24, QFont::Bold); p.setFont(fontRpm); p.drawText(QRect(-100, -30, 200, 40), Qt::AlignCenter, QString::number(rpm));
        QFont fontUnit("Arial", 9); p.setFont(fontUnit); p.setPen(QColor("#aaaaaa")); p.drawText(QRect(-100, 10, 200, 20), Qt::AlignCenter, "RPM");
        p.setFont(QFont("Arial", 12, QFont::Bold)); p.setPen(Qt::white); p.drawText(QRect(-100, 45, 200, 30), Qt::AlignCenter, label);
        p.setPen(QColor("#444")); for (int i = 0; i < 12; ++i) { p.rotate(30); p.drawLine(60, 0, 70, 0); }
    }
};


class DualGraph : public QWidget {
    Q_OBJECT
    QList<int> historyTemp; QList<int> historyUsage; int maxPoints = 60;
public:
    DualGraph(QWidget *parent = nullptr) : QWidget(parent) {
        setMinimumHeight(100); setStyleSheet("background-color: #151515; border: 1px solid #333; border-radius: 4px;");
        for(int i=0; i<maxPoints; i++) { historyTemp.append(0); historyUsage.append(0); }
    }
    void addValues(int temp, int usage) { 
        historyTemp.append(temp); if (historyTemp.size() > maxPoints) historyTemp.removeFirst();
        historyUsage.append(usage); if (historyUsage.size() > maxPoints) historyUsage.removeFirst();
        update(); 
    }
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this); p.setRenderHint(QPainter::Antialiasing); int w = width(); int h = height();
        p.setPen(QPen(QColor("#222"), 1, Qt::DotLine)); for(int i=1; i<4; i++) { int y = h - (i * h / 4); p.drawLine(0, y, w, y); }
        double xStep = (double)w / (maxPoints - 1);
        if (!historyTemp.isEmpty()) {
            QPainterPath pathT; pathT.moveTo(0, h);
            for (int i = 0; i < historyTemp.size(); ++i) pathT.lineTo(i * xStep, h - ((double)historyTemp[i] / 100.0 * h));
            pathT.lineTo(w, h); pathT.lineTo(0, h);
            QLinearGradient grad(0, 0, 0, h); grad.setColorAt(0, QColor(200, 0, 0, 100)); grad.setColorAt(1, QColor(200, 0, 0, 10));
            p.fillPath(pathT, grad);
            p.setPen(QPen(QColor("#d30000"), 2)); 
            for (int i = 0; i < historyTemp.size() - 1; ++i) p.drawLine(i*xStep, h - (historyTemp[i]/100.0*h), (i+1)*xStep, h - (historyTemp[i+1]/100.0*h));
        }
        if (!historyUsage.isEmpty()) {
            p.setPen(QPen(QColor("#00ffff"), 2)); 
            for (int i = 0; i < historyUsage.size() - 1; ++i) p.drawLine(i*xStep, h - (historyUsage[i]/100.0*h), (i+1)*xStep, h - (historyUsage[i+1]/100.0*h));
        }
    }
};


class NitroButton : public QPushButton {
public:
    NitroButton(QString text, bool isRed = false) : QPushButton(text) {
        setCheckable(true); setMinimumHeight(45); setFont(QFont("Arial", 10, QFont::Bold)); updateStyle(isRed);
    }
    void updateStyle(bool active) {
        if (active) setStyleSheet("QPushButton { background-color: #2b2b2b; color: #ff0000; border: 2px solid #ff0000; border-radius: 4px; padding-left: 15px; text-align: left; }");
        else setStyleSheet("QPushButton { background-color: #1c1c1c; color: #aaaaaa; border: 1px solid #3e3e3e; border-radius: 4px; padding-left: 15px; text-align: left; } QPushButton:hover { background-color: #252525; }");
    }
};


class NitroWindow : public QWidget {
    Q_OBJECT 

public:
    NitroFanGauge *gaugeCPU; NitroFanGauge *gaugeGPU;
    DualGraph *graphCPU; DualGraph *graphGPU; 
    QLabel *lblStatsCPU; QLabel *lblStatsGPU; 
    NitroButton *btnAuto; NitroButton *btnMax; NitroButton *btnCustom;
    QWidget *customPanel; QSlider *sliderCustom; QLabel *lblSliderVal;
    NitroButton *btnSaver; NitroButton *btnBalanced; NitroButton *btnPerf;

    int currentMode = 0; 
    int currentSpeed = 75;

    NitroWindow() {
        setWindowTitle("NitroLinux");
        setWindowIcon(QIcon(ICON_PATH));
        resize(1000, 700); 
        setStyleSheet("background-color: #121212; font-family: Arial;");

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0,0,0,0); mainLayout->setSpacing(0);

        // HEADER
        QFrame *header = new QFrame(); header->setStyleSheet("background-color: #000; border-bottom: 2px solid #d30000;"); header->setFixedHeight(65);
        QHBoxLayout *headerLay = new QHBoxLayout(header); headerLay->setContentsMargins(15, 5, 15, 5);
        
        QLabel *imgLogo = new QLabel(); QPixmap pixmap(ICON_PATH); imgLogo->setPixmap(pixmap.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        headerLay->addWidget(imgLogo);
        QLabel *logoText = new QLabel("NITRO LINUX MONITOR"); logoText->setStyleSheet("color: white; font-size: 20px; font-weight: bold; letter-spacing: 2px; padding-left: 10px;");
        headerLay->addWidget(logoText); 
        headerLay->addStretch();
        
        // Pulsante Info
        QPushButton *btnInfo = new QPushButton("ℹ️ INFO");
        btnInfo->setFixedSize(90, 35);
        btnInfo->setCursor(Qt::PointingHandCursor);
        btnInfo->setStyleSheet("QPushButton { background-color: #333; color: white; border: 1px solid #555; border-radius: 4px; font-weight: bold; } QPushButton:hover { background-color: #555; border-color: #888; }");
        connect(btnInfo, &QPushButton::clicked, this, &NitroWindow::showInfo);
        headerLay->addWidget(btnInfo);

        mainLayout->addWidget(header);

        // CORPO
        QHBoxLayout *bodyLayout = new QHBoxLayout(); bodyLayout->setContentsMargins(20, 20, 20, 20); bodyLayout->setSpacing(20);
        QFrame *leftPanel = createPanel(); leftPanel->setFixedWidth(280); QVBoxLayout *leftLay = new QVBoxLayout(leftPanel); leftLay->setSpacing(10);
        
        QLabel *lblTitleFan = new QLabel("Fan Control"); lblTitleFan->setStyleSheet("color: #888; font-weight: bold;"); leftLay->addWidget(lblTitleFan);
        btnAuto = new NitroButton("Auto", true); btnMax = new NitroButton("Max", false); btnCustom = new NitroButton("Custom", false);
        leftLay->addWidget(btnAuto); leftLay->addWidget(btnMax); leftLay->addWidget(btnCustom);
        customPanel = new QWidget(); QVBoxLayout *customLay = new QVBoxLayout(customPanel); customLay->setContentsMargins(0,10,0,0);
        QLabel *lblSlide = new QLabel("Manual Speed:"); lblSlide->setStyleSheet("color: #888;");
        sliderCustom = new QSlider(Qt::Horizontal); sliderCustom->setRange(0, 100); sliderCustom->setValue(75);
        sliderCustom->setStyleSheet("QSlider::groove:horizontal { background: #333; height: 6px; border-radius: 3px; } QSlider::handle:horizontal { background: #d30000; width: 16px; margin: -5px 0; border-radius: 8px; } QSlider::sub-page:horizontal { background: #d30000; border-radius: 3px; }");
        lblSliderVal = new QLabel("75%"); lblSliderVal->setAlignment(Qt::AlignRight); lblSliderVal->setStyleSheet("color: white; font-weight: bold;");
        customLay->addWidget(lblSlide); customLay->addWidget(sliderCustom); customLay->addWidget(lblSliderVal);
        leftLay->addWidget(customPanel); customPanel->hide(); 

        leftLay->addSpacing(20);
        QLabel *lblTitlePwr = new QLabel("Power Plan"); lblTitlePwr->setStyleSheet("color: #888; font-weight: bold;"); leftLay->addWidget(lblTitlePwr);
        btnSaver = new NitroButton("Power Saver"); btnBalanced = new NitroButton("Balanced"); btnPerf = new NitroButton("Performance");
        leftLay->addWidget(btnSaver); leftLay->addWidget(btnBalanced); leftLay->addWidget(btnPerf);
        leftLay->addStretch();

        QVBoxLayout *rightCol = new QVBoxLayout(); rightCol->setSpacing(15);
        QFrame *fanPanel = createPanel(); QHBoxLayout *fanLay = new QHBoxLayout(fanPanel);
        gaugeCPU = new NitroFanGauge("CPU Fan"); gaugeGPU = new NitroFanGauge("GPU Fan");
        fanLay->addStretch(); fanLay->addWidget(gaugeCPU); fanLay->addStretch(); fanLay->addWidget(gaugeGPU); fanLay->addStretch();
        rightCol->addWidget(fanPanel, 1);
        QFrame *cpuPanel = createPanel(); QVBoxLayout *cpuLay = new QVBoxLayout(cpuPanel);
        lblStatsCPU = new QLabel("CPU: --°C | Usage: --%"); lblStatsCPU->setStyleSheet("color: white; font-size: 14px; font-weight: bold;");
        cpuLay->addWidget(lblStatsCPU); graphCPU = new DualGraph(); cpuLay->addWidget(graphCPU);
        rightCol->addWidget(cpuPanel, 1);
        QFrame *gpuPanel = createPanel(); QVBoxLayout *gpuLay = new QVBoxLayout(gpuPanel);
        lblStatsGPU = new QLabel("GPU: --°C | Usage: --%"); lblStatsGPU->setStyleSheet("color: white; font-size: 14px; font-weight: bold;");
        gpuLay->addWidget(lblStatsGPU); graphGPU = new DualGraph(); gpuLay->addWidget(graphGPU);
        rightCol->addWidget(gpuPanel, 1);

        bodyLayout->addWidget(leftPanel); bodyLayout->addLayout(rightCol); mainLayout->addLayout(bodyLayout);

        connect(btnAuto, &QPushButton::clicked, this, &NitroWindow::onAutoClicked);
        connect(btnMax, &QPushButton::clicked, this, &NitroWindow::onMaxClicked);
        connect(btnCustom, &QPushButton::clicked, this, &NitroWindow::onCustomClicked);
        connect(sliderCustom, &QSlider::valueChanged, this, &NitroWindow::onSliderChange);
        connect(sliderCustom, &QSlider::sliderReleased, this, &NitroWindow::onSliderRelease);
        connect(btnSaver, &QPushButton::clicked, this, [=](){ setPower(0); });
        connect(btnBalanced, &QPushButton::clicked, this, [=](){ setPower(1); });
        connect(btnPerf, &QPushButton::clicked, this, [=](){ setPower(2); });

        init_backend();
        updatePowerButtons(get_current_profile());
        QTimer *timer = new QTimer(this); connect(timer, &QTimer::timeout, this, &NitroWindow::updateStats); timer->start(1000); 
    }
    ~NitroWindow() { close_backend(); }
    QFrame* createPanel() { QFrame *p = new QFrame(); p->setStyleSheet("background-color: #1a1a1a; border: 1px solid #333; border-radius: 8px;"); return p; }

public slots:
    void showInfo() {
        NitroInfoDialog dlg(this);
        dlg.exec(); 
    }

    void updateStats() {
        int f1 = get_fan_rpm(0); int f2 = get_fan_rpm(1);
        double cpuTemp = get_cpu_temp(); double cpuUsage = get_cpu_usage();
        double gpuTemp = 0; int gpuUsage = 0; get_gpu_stats(&gpuTemp, &gpuUsage);

        gaugeCPU->setRpm(f1); gaugeGPU->setRpm(f2);
        lblStatsCPU->setText("CPU: " + QString::number(cpuTemp, 'f', 0) + "°C  |  Usage: " + QString::number(cpuUsage, 'f', 1) + "%");
        graphCPU->addValues((int)cpuTemp, (int)cpuUsage);
        lblStatsGPU->setText("GPU: " + QString::number(gpuTemp, 'f', 0) + "°C  |  Usage: " + QString::number(gpuUsage) + "%");
        graphGPU->addValues((int)gpuTemp, gpuUsage);

        if (currentMode == 1) set_fan_mode(1);
        else if (currentMode == 2) set_fixed_speed(currentSpeed);
    }

    void resetButtons() { btnAuto->updateStyle(false); btnMax->updateStyle(false); btnCustom->updateStyle(false); customPanel->hide(); }
    void onAutoClicked() { resetButtons(); btnAuto->updateStyle(true); currentMode = 0; set_fan_mode(0); }
    void onMaxClicked() { resetButtons(); btnMax->updateStyle(true); currentMode = 1; set_fan_mode(1); }
    void onCustomClicked() { resetButtons(); btnCustom->updateStyle(true); customPanel->show(); currentMode = 2; onSliderRelease(); }
    void onSliderChange(int val) { lblSliderVal->setText(QString::number(val) + "%"); }
    void onSliderRelease() { currentSpeed = sliderCustom->value(); set_fixed_speed(currentSpeed); }
    void setPower(int mode) { set_power_profile(mode); updatePowerButtons(mode); }
    void updatePowerButtons(int mode) {
        btnSaver->updateStyle(false); btnBalanced->updateStyle(false); btnPerf->updateStyle(false);
        if (mode == 0) btnSaver->updateStyle(true); else if (mode == 1) btnBalanced->updateStyle(true); else if (mode == 2) btnPerf->updateStyle(true);
    }
};

#include "main_gui.moc"
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    

    app.setApplicationName("nitrolinux"); 
    app.setDesktopFileName("nitrolinux.desktop"); 
    app.setWindowIcon(QIcon(ICON_PATH));
    
    NitroWindow window; 
    window.show(); 
    return app.exec();
}
