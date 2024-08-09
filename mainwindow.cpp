#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QString>
#include "zlgcan.h"
#include "canreceive.h"
#include "cansend.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QObject::connect(ui->btncon,SIGNAL(clicked()),this,SLOT(on_btncon_click()));
    QObject::connect(ui->lamba,&QPushButton::clicked,[=]{
        qDebug() << "这是第lamba按钮";
    });
    QObject::connect(ui->point,&QPushButton::clicked,this,&MainWindow::point_click);

    QObject::connect(ui->openButton,SIGNAL(clicked()),this,SLOT(on_openbtn_clicked()));

    //QObject::connect(ui->closeButton,SIGNAL(clicked()),this,SLOT(on_closeButton_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    qDebug() << "这是第一个按钮";
}

void MainWindow::on_btncon_click()
{
    qDebug() << "这是第click按钮";
}

void MainWindow::point_click()
{
    ZCAN_Transmit_Data frame;
    memset(&frame, 0, sizeof(frame));
    frame.frame.can_id = MAKE_CAN_ID(0x642, 0, 0, 0);
    frame.frame.can_dlc = 6;
    frame.transmit_type = 0;
    BYTE data[] = {0x00, 0x8D,0x00,0x00,0x00,0x00};
    memcpy(frame.frame.data, data, sizeof(data));
    if (ZCAN_Transmit(this->canHandle, &frame, 1) != 1)
    {
        qDebug()  << "发送数据失败";
    }

    BYTE data1[] = {0x00, 0x8A,0x00,0x00,0x15,0xBE};
    memcpy(frame.frame.data, data1, sizeof(data1));
    if (ZCAN_Transmit(this->canHandle, &frame, 1) != 1)
    {
        qDebug()  << "发送数据失败";
    }

    BYTE data2[] = {0x00, 0x83};
    frame.frame.can_dlc = 2;
    memcpy(frame.frame.data, data2, sizeof(data2));
    if (ZCAN_Transmit(this->canHandle, &frame, 1) != 1)
    {
        qDebug()  << "发送数据失败";
    }
}

void MainWindow::on_openbtn_clicked()
{
    int deviceType = this->ui->comboBox->currentIndex();
    QString deviceIndex = this->ui->lineEdit->text();
    int deviceChannel = this->ui->comboBox_2->currentIndex();
    qDebug() << deviceType;
    qDebug() << deviceIndex;
    qDebug() << deviceChannel;

    //ZCAN_USBCAN_2E_U 为设备类型, 请根据实际修改
    this->canHandle = ZCAN_OpenDevice(ZCAN_USBCAN_E_U, 0, 0);
    if (INVALID_DEVICE_HANDLE == this->canHandle)
    {
        qDebug()  << "打开设备失败";
        return;
    }
    //CAN 设备设置波特率的 key 为 baud_rate，值 1000000 为 1000kbps, 800000 为 800kbps, 其它请查看
    //属性表
    //若为 CANFD 设备, 设置冲裁域波特率的 key 为 canfd_abit_baud_rate，数据域波特率为
    //canfd_dbit_baud_rate，请注意区分 CAN 和 CANFD 设备设置波特率的区别。
    if (ZCAN_SetValue (this->canHandle , "0/baud_rate", "250000") != STATUS_OK)
    {
        qDebug()  << "设置波特率失败";

    }
    else
    {
        ZCAN_CHANNEL_INIT_CONFIG cfg;
        memset(&cfg, 0, sizeof(cfg));
        cfg.can_type = TYPE_CAN;//CANFD 设备为 TYPE_CANFD
        cfg.can.filter = 0;
        cfg.can.mode = 0; //正常模式, 1 为只听模式
        cfg.can.acc_code = 0;
        cfg.can.acc_mask = 0xffffffff;
        CHANNEL_HANDLE chHandle = ZCAN_InitCAN(this->canHandle, 0, &cfg);
        if (INVALID_CHANNEL_HANDLE == chHandle)
        {
            qDebug()  << "初始化通道失败" ;
        }
        else if (ZCAN_StartCAN(chHandle) != STATUS_OK)
        {
            qDebug()  << "启动通道失败";

        }
        else
        {

            //创建一个接收线程
            //创建子线程
            CANReceive * subThread = new CANReceive;
            subThread->setHandle(chHandle);
            // 启动子线程
            subThread->start();

            ZCAN_Transmit_Data frame;
            memset(&frame, 0, sizeof(frame));
            frame.frame.can_id = MAKE_CAN_ID(0x100, 1, 0, 0);
            frame.frame.can_dlc = 8;
            frame.transmit_type = 0;
            BYTE data[] = {1, 2, 3, 4, 5, 6, 7, 8};
            memcpy(frame.frame.data, data, sizeof(data));
            if (ZCAN_Transmit(this->canHandle, &frame, 1) != 1)
            {
                qDebug()  << "发送数据失败";
            }
            ui->openButton->setText(QString("已打开"));
        }
    }
    //ZCAN_CloseDevice(dhandle);
}

void MainWindow::on_closeButton_clicked()
{
    qDebug() << "点击close 按钮";
    //判断是否已经打开
    if (this->canHandle == NULL)
    {
        qDebug() << "设备没有打开";
        //todo 弹出提示框 设备没有打开
    }
    else{
        qDebug() << "设备已打开,正在关闭";
         ZCAN_CloseDevice(this->canHandle);
         this->canHandle = NULL;
         ui->openButton->setText(QString("打开"));
          qDebug() << "设备已关闭";
    }
}
//上升
void MainWindow::on_pushButton_2_clicked()
{
    qDebug() << "点击,上升";
    canSender cSender;
    uint8_t data[2] = {0x10,0x11};
    cSender.Send(this->canHandle,0x111,data,2);
}
//下降
void MainWindow::on_pushButton_3_clicked()
{
    qDebug() << "点击,下降";
}

//举升停止
void MainWindow::on_pushButton_4_clicked()
{
    qDebug() << "点击,举升停止";
}

//方位 左
void MainWindow::on_pushButton_5_clicked()
{
    qDebug() << "点击,方位 左";
}
//方位 右
void MainWindow::on_pushButton_6_clicked()
{
    qDebug() << "点击,方位 右";
}
// 方位停止
void MainWindow::on_pushButton_7_clicked()
{
    qDebug() << "点击,方位停止";
}
//俯仰 上
void MainWindow::on_pushButton_8_clicked()
{
    qDebug() << "点击,俯仰 上";
}
//俯仰 下
void MainWindow::on_pushButton_9_clicked()
{
    qDebug() << "点击,俯仰 下";
}
//俯仰 停止
void MainWindow::on_pushButton_10_clicked()
{
    qDebug() << "点击,俯仰 停止";
}
//伸缩 伸出
void MainWindow::on_pushButton_11_clicked()
{
    qDebug() << "点击,伸缩 伸出";
}
//伸缩 缩
void MainWindow::on_pushButton_12_clicked()
{
    qDebug() << "点击,伸缩 缩";
}
//伸缩 停
void MainWindow::on_pushButton_13_clicked()
{
    qDebug() << "点击,伸缩 停";
}
//夹住
void MainWindow::on_pushButton_14_clicked()
{
     qDebug() << "点击,夹住";
}
//松开
void MainWindow::on_pushButton_15_clicked()
{
    qDebug() << "点击,松开";
}
//抓手 停止
void MainWindow::on_pushButton_16_clicked()
{
    qDebug() << "点击,抓手 停止";
}
//开始 激震
void MainWindow::on_pushButton_17_clicked()
{
    qDebug() << "点击,开始 激震";
}
//停止 激震
void MainWindow::on_pushButton_18_clicked()
{
    qDebug() << "点击,停止 激震";
}
